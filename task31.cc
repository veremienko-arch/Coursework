/* ----------------------------------------------------------------<Header>-
Name: task31.cc
Title: Masyu solver
Group: TV-51
Student: Veremiienko R. R.
Written: 2026-05-09
Revised: 2026-05-11
Description: This program solves Masyu puzzle with some addition
rules: line must pass trough all empty cells and it doesn't have to
collect all circles. But, if line passes though a circle it
must follow the rules.
------------------------------------------------------------------</Header>-*/

#include <iostream>
#include <vector>
#include <limits>

using std::cout;
using std::endl;

enum class Direction {
    UP, DOWN, RIGHT, LEFT, NONE
};

enum class CircleType {
    EMPTY, WHITE, BLACK
};

enum class LineStat {
    UNKNOWN, LINE, EMPTY
};

struct Node {
    CircleType circle = CircleType::EMPTY;
    LineStat up = LineStat::UNKNOWN;
    LineStat down = LineStat::UNKNOWN;
    LineStat right = LineStat::UNKNOWN;
    LineStat left = LineStat::UNKNOWN;
};

class CourseworkMasyu {
private:
    int width, height;
    std::vector<std::vector<Node>> grid;
    std::vector<std::vector<std::vector<Node>>> history;
    bool if_coord_valid(int x, int y) const;
    bool node_rules(int x, int y);
    bool black_rules(int x, int y);
    bool white_rules(int x, int y);
    void save_state();
    void restore_state();
    void apply_rules();
    bool backtrack();
    bool if_loop_complete() const;
    bool if_valid() const;

public:
    CourseworkMasyu(int x, int y);
    void set_circle_type(int x,  int y, CircleType type);
    void set_line_stat(int x, int y, Direction dir, LineStat status);

    bool solve();
    void print_to_console() const;
};

/* ---------------------------------------------------------------------[<]-
Function: CourseworkMasyu
Synopsis: Constructor for a grid size
---------------------------------------------------------------------[>]-*/
CourseworkMasyu::CourseworkMasyu(int x, int y) {
    width = x;
    height = y;
    grid.resize(height, std::vector<Node>(width));

    for (int rows = 0; rows < height; rows++) {
        grid[rows][0].left = LineStat::EMPTY;
        grid[rows][width-1].right = LineStat::EMPTY;
    }
    for (int cols = 0; cols < width; cols++) {
        grid[0][cols].up = LineStat::EMPTY;
        grid[height-1][cols].down = LineStat::EMPTY;
    }
}

/* ---------------------------------------------------------------------[<]-
Function: set_circle_type
Synopsis: Correctly and safely sets circle type
---------------------------------------------------------------------[>]-*/
void CourseworkMasyu::set_circle_type(int x, int y, CircleType type) {
    grid[y][x].circle = type;
}

/* ---------------------------------------------------------------------[<]-
Function: print_to_console
Synopsis: Prints grid to a console
---------------------------------------------------------------------[>]-*/
void CourseworkMasyu::print_to_console() const {
    cout << std::string(width * 4 - 3, '-') << endl;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            switch (grid[y][x].circle) {
                case CircleType::WHITE: cout << '0'; break;
                case CircleType::BLACK: cout << '#'; break;
                default: cout << "+"; break;
            }
            if (x < width - 1) {
                if (grid[y][x].right == LineStat::LINE) {
                    cout << "---";
                } else {
                    cout << "   ";
                }
            }
        }
        cout << endl;
        if (y < height - 1) {
            for (int x = 0; x < width; x++) {
                if (grid[y][x].down == LineStat::LINE) {
                    cout << "|   ";
                } else {
                  cout << "    ";
                }
            }
            cout << endl;
        }
    }
    cout << std::string(width * 4 - 3, '-') << endl;
}

/* ---------------------------------------------------------------------[<]-
Function: if_coord_valid
Synopsis: Checks if coords is valid
---------------------------------------------------------------------[>]-*/
bool CourseworkMasyu::if_coord_valid(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

/* ---------------------------------------------------------------------[<]-
Function: set_line_stat
Synopsis: Correctly and safely sets line status
---------------------------------------------------------------------[>]-*/
void CourseworkMasyu::set_line_stat(int x, int y, Direction dir, LineStat status) {
    if (!if_coord_valid(x, y)) return;

    switch (dir) {
        case Direction::UP: grid[y][x].up = status; break;
        case Direction::DOWN: grid[y][x].down = status; break;
        case Direction::RIGHT: grid[y][x].right = status; break;
        case Direction::LEFT: grid[y][x].left = status; break;
        default: break;
    }

    int nx = x, ny = y;
    Direction opposite_dir = Direction::NONE;

    switch (dir) {
        case Direction::UP: ny = y - 1; opposite_dir = Direction::DOWN; break;
        case Direction::DOWN: ny = y + 1; opposite_dir = Direction::UP; break;
        case Direction::LEFT: nx = x - 1; opposite_dir = Direction::RIGHT; break;
        case Direction::RIGHT: nx = x + 1; opposite_dir = Direction::LEFT; break;
        default: break;
    }

    if (if_coord_valid(nx, ny)) {
        switch (opposite_dir) {
            case Direction::UP: grid[ny][nx].up = status; break;
            case Direction::DOWN: grid[ny][nx].down = status; break;
            case Direction::LEFT: grid[ny][nx].left = status; break;
            case Direction::RIGHT: grid[ny][nx].right = status; break;
            default: break;
        }
    }
}

/* ---------------------------------------------------------------------[<]-
Function: node_rules
Synopsis: Sets rules for the grid and lines
---------------------------------------------------------------------[>]-*/
bool CourseworkMasyu::node_rules(int x, int y) {
    int l = 0;
    int u = 0;
    bool changed = false;

    if (grid[y][x].up == LineStat::LINE) l++;
    else if (grid[y][x].up == LineStat::UNKNOWN) u++;
    else {}
    if (grid[y][x].right == LineStat::LINE) l++;
    else if (grid[y][x].right == LineStat::UNKNOWN) u++;
    else {}
    if (grid[y][x].down == LineStat::LINE) l++;
    else if (grid[y][x].down == LineStat::UNKNOWN) u++;
    else {}
    if (grid[y][x].left == LineStat::LINE) l++;
    else if (grid[y][x].left == LineStat::UNKNOWN) u++;
    else {}

    if (l == 2 && u > 0) {
        if (grid[y][x].up == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
        if (grid[y][x].right == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }
        if (grid[y][x].down == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
        if (grid[y][x].left == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
    }
    if (l == 1 && u == 1) {
        if (grid[y][x].up == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::UP, LineStat::LINE); changed = true; }
        if (grid[y][x].right == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::RIGHT, LineStat::LINE); changed = true; }
        if (grid[y][x].down == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::DOWN, LineStat::LINE); changed = true; }
        if (grid[y][x].left == LineStat::UNKNOWN) {  set_line_stat(x, y, Direction::LEFT, LineStat::LINE); changed = true; }
    }
    if (l == 0 && u == 1) {
        if (grid[y][x].up == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
        if (grid[y][x].right == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }
        if (grid[y][x].down == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
        if (grid[y][x].left == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
    }
    if (grid[y][x].circle == CircleType::EMPTY) {
        if (l + u == 2 && u > 0) {
            if (grid[y][x].up == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::UP, LineStat::LINE); changed = true; }
            if (grid[y][x].right == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::RIGHT, LineStat::LINE); changed = true; }
            if (grid[y][x].down == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::DOWN, LineStat::LINE); changed = true; }
            if (grid[y][x].left == LineStat::UNKNOWN) {  set_line_stat(x, y, Direction::LEFT, LineStat::LINE); changed = true; }
        }
    }

    return changed;
}

/* ---------------------------------------------------------------------[<]-
Function: black_rules
Synopsis: Sets rules for black circles
---------------------------------------------------------------------[>]-*/
bool CourseworkMasyu::black_rules(int x, int y) {
    bool changed = false;

    if (grid[y][x].up == LineStat::UNKNOWN) {
        if (!if_coord_valid(x, y-1) || grid[y-1][x].up == LineStat::EMPTY) { set_line_stat(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
    }
    if (grid[y][x].right == LineStat::UNKNOWN) {
        if (!if_coord_valid(x+1, y) || grid[y][x+1].right == LineStat::EMPTY) { set_line_stat(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }
    }
    if (grid[y][x].down == LineStat::UNKNOWN) {
        if (!if_coord_valid(x, y+1) || grid[y+1][x].down == LineStat::EMPTY) { set_line_stat(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
    }
    if (grid[y][x].left == LineStat::UNKNOWN) {
        if (!if_coord_valid(x-1, y) || grid[y][x-1].left == LineStat::EMPTY) { set_line_stat(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
    }

    int l = (grid[y][x].up == LineStat::LINE) + (grid[y][x].down == LineStat::LINE) + (grid[y][x].left == LineStat::LINE) + (grid[y][x].right == LineStat::LINE);

    if (l > 0) {
        if (grid[y][x].up == LineStat::LINE && grid[y][x].down == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
        if (grid[y][x].right == LineStat::LINE && grid[y][x].left == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
        if (grid[y][x].down == LineStat::LINE && grid[y][x].up == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
        if (grid[y][x].left == LineStat::LINE && grid[y][x].right == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }

        if (grid[y][x].up == LineStat::LINE) {
            if (if_coord_valid(x, y-1) && grid[y-1][x].up == LineStat::UNKNOWN) { set_line_stat(x, y-1, Direction::UP, LineStat::LINE); changed = true; }
        }
        if (grid[y][x].right == LineStat::LINE) {
            if (if_coord_valid(x+1, y) && grid[y][x+1].right == LineStat::UNKNOWN) { set_line_stat(x+1, y, Direction::RIGHT, LineStat::LINE); changed = true; }
        }
        if (grid[y][x].down == LineStat::LINE) {
            if (if_coord_valid(x, y+1) && grid[y+1][x].down == LineStat::UNKNOWN) { set_line_stat(x, y+1, Direction::DOWN, LineStat::LINE); changed = true; }
        }
        if (grid[y][x].left == LineStat::LINE) {
            if (if_coord_valid(x-1, y) && grid[y][x-1].left == LineStat::UNKNOWN) { set_line_stat(x-1, y, Direction::LEFT, LineStat::LINE); changed = true; }
        }
    } else {
        bool if_vertical = (grid[y][x].up != LineStat::EMPTY || grid[y][x].down != LineStat::EMPTY);
        bool if_horizontal = (grid[y][x].left != LineStat::EMPTY || grid[y][x].right != LineStat::EMPTY);
        if (!if_vertical || !if_horizontal) {
            if (grid[y][x].up == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
            if (grid[y][x].down == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
            if (grid[y][x].left == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
            if (grid[y][x].right == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }
        }
    }
    return changed;
}

/* ---------------------------------------------------------------------[<]-
Function: white_rules
Synopsis: Sets rules for white circles
---------------------------------------------------------------------[>]-*/
bool CourseworkMasyu::white_rules(int x, int y) {
    bool changed = false;
    int l = (grid[y][x].up == LineStat::LINE) + (grid[y][x].down == LineStat::LINE) + (grid[y][x].left == LineStat::LINE) + (grid[y][x].right == LineStat::LINE);

    if (l > 0) {
        if (grid[y][x].up == LineStat::LINE || grid[y][x].down == LineStat::LINE) {
            if (grid[y][x].up == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::UP, LineStat::LINE); changed = true; }
            if (grid[y][x].down == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::DOWN, LineStat::LINE); changed = true; }
            if (grid[y][x].right == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }
            if (grid[y][x].left == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
        } else if (grid[y][x].left == LineStat::LINE || grid[y][x].right == LineStat::LINE) {
            if (grid[y][x].up == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
            if (grid[y][x].right == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::RIGHT, LineStat::LINE); changed = true; }
            if (grid[y][x].down == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
            if (grid[y][x].left == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::LEFT, LineStat::LINE); changed = true; }
        } else {}
        if (grid[y][x].up == LineStat::LINE && grid[y][x].down == LineStat::LINE) {
            if (if_coord_valid(x, y-1) && grid[y-1][x].up == LineStat::LINE) {
                if (if_coord_valid(x, y+1) && grid[y+1][x].down == LineStat::UNKNOWN) { set_line_stat(x, y+1, Direction::DOWN, LineStat::EMPTY); changed = true; }
            }
            if (if_coord_valid(x, y+1) && grid[y+1][x].down == LineStat::LINE) {
                if (if_coord_valid(x, y-1) && grid[y-1][x].up == LineStat::UNKNOWN) { set_line_stat(x, y-1, Direction::UP, LineStat::EMPTY); changed = true; }
            }
        }
        if (grid[y][x].left == LineStat::LINE && grid[y][x].right == LineStat::LINE) {
            if (if_coord_valid(x-1, y) && grid[y][x-1].left == LineStat::LINE) {
                if (if_coord_valid(x+1, y) && grid[y][x+1].right == LineStat::UNKNOWN) { set_line_stat(x+1, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }
            }
            if (if_coord_valid(x+1, y) && grid[y][x+1].right == LineStat::LINE) {
                if (if_coord_valid(x-1, y) && grid[y][x-1].left == LineStat::UNKNOWN) { set_line_stat(x-1, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
            }
        }
    } else {
        bool if_vertical = (grid[y][x].up != LineStat::EMPTY && grid[y][x].down != LineStat::EMPTY);
        bool if_horizontal = (grid[y][x].left != LineStat::EMPTY && grid[y][x].right != LineStat::EMPTY);

        if (!if_vertical) {
            if (grid[y][x].up == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
            if (grid[y][x].down == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
        }
        if (!if_horizontal) {
            if (grid[y][x].left == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
            if (grid[y][x].right == LineStat::UNKNOWN) { set_line_stat(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }
        }
    }
    return changed;
}

/* ---------------------------------------------------------------------[<]-
Function: apply_rules
Synopsis: Applying all grid rules
---------------------------------------------------------------------[>]-*/
void CourseworkMasyu::apply_rules() {
    bool changed = true;
    while (changed) {
        changed = false;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (node_rules(x, y)) changed = true;

                if (grid[y][x].circle == CircleType::WHITE) {
                    if (white_rules(x, y)) changed = true;
                } else if (grid[y][x].circle == CircleType::BLACK) {
                    if (black_rules(x, y)) changed = true;
                } else {}
            }
        }
    }
}

/* ---------------------------------------------------------------------[<]-
Function: solve
Synopsis: "Start button"
---------------------------------------------------------------------[>]-*/
bool CourseworkMasyu::solve() {
    return backtrack();
}

/* ---------------------------------------------------------------------[<]-
Function: if_valid
Synopsis: Checks if grid is follows current rules and if it's solvable
---------------------------------------------------------------------[>]-*/
bool CourseworkMasyu::if_valid() const {
    for (int  y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int l = 0, u = 0;
            if (grid[y][x].up == LineStat::LINE) l++;
            else if (grid[y][x].up == LineStat::UNKNOWN) u++;
            else {}
            if (grid[y][x].right == LineStat::LINE) l++;
            else if (grid[y][x].right == LineStat::UNKNOWN) u++;
            else {}
            if (grid[y][x].down == LineStat::LINE) l++;
            else if (grid[y][x].down == LineStat::UNKNOWN) u++;
            else {}
            if (grid[y][x].left == LineStat::LINE) l++;
            else if (grid[y][x].left == LineStat::UNKNOWN) u++;
            else {}

            if (l > 2) return false;
            if (l == 1 && u == 0) return false;
            if (grid[y][x].circle == CircleType::EMPTY) {
                if (l + u < 2) return false;
            }
            if (l == 2) {
                bool strt = (grid[y][x].up == LineStat::LINE && grid[y][x].down == LineStat::LINE) || (grid[y][x].left == LineStat::LINE && grid[y][x].right == LineStat::LINE);
                if (grid[y][x].circle == CircleType::WHITE && !strt) return false;
                if (grid[y][x].circle == CircleType::BLACK && strt) return false;
                if (grid[y][x].circle == CircleType::BLACK) {
                    if (grid[y][x].up == LineStat::LINE && (!if_coord_valid(x, y-1) || grid[y-1][x].up == LineStat::EMPTY)) return false;
                    if (grid[y][x].right == LineStat::LINE && (!if_coord_valid(x+1, y) || grid[y][x+1].right == LineStat::EMPTY)) return false;
                    if (grid[y][x].down == LineStat::LINE && (!if_coord_valid(x, y+1) || grid[y+1][x].down == LineStat::EMPTY)) return false;
                    if (grid[y][x].left == LineStat::LINE && (!if_coord_valid(x-1, y) || grid[y][x-1].left == LineStat::EMPTY)) return false;
                }
            }
        }
    }
    return true;
}

/* ---------------------------------------------------------------------[<]-
Function: save_state
Synopsis: CSaves progress in
          std::vector<std::vector<std::vector<Node>>> history;
          (part of backtrack)
---------------------------------------------------------------------[>]-*/
void CourseworkMasyu::save_state() {
    history.push_back(grid);
}

/* ---------------------------------------------------------------------[<]-
Function: restore_state
Synopsis: Restores progress (part of backtrack)
---------------------------------------------------------------------[>]-*/
void CourseworkMasyu::restore_state() {
    grid = history.back();
    history.pop_back();
}

/* ---------------------------------------------------------------------[<]-
Function: if_loop_complete
Synopsis: Checking if loop is complete
---------------------------------------------------------------------[>]-*/
bool CourseworkMasyu::if_loop_complete() const {
    int empties = 0;
    int cell_with_line = 0;
    int s_x = -1, s_y = -1;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x].circle == CircleType::EMPTY) empties++;
            if (grid[y][x].up == LineStat::LINE || grid[y][x].down == LineStat::LINE || grid[y][x].left == LineStat::LINE || grid[y][x].right == LineStat::LINE) {
                cell_with_line++;
                if (s_x == -1) { s_x = x; s_y = y; }
            }
        }
    }
    if (s_x == -1 || s_y == -1) return false;

    int curr_x = s_x, curr_y = s_y;
    int path_len = 0;
    int empties_visited = 0;
    bool if_first_step = true;
    Direction came_from = Direction::NONE;

    while (true) {
        path_len++;
        if (grid[curr_y][curr_x].circle == CircleType::EMPTY) empties_visited++;

        if (grid[curr_y][curr_x].up == LineStat::LINE && (if_first_step || came_from != Direction::UP)) {
            curr_y = curr_y-1; came_from = Direction::DOWN;
        } else if (grid[curr_y][curr_x].down == LineStat::LINE && (if_first_step || came_from != Direction::DOWN)) {
            curr_y = curr_y+1; came_from = Direction::UP;
        } else if (grid[curr_y][curr_x].right == LineStat::LINE && (if_first_step || came_from != Direction::RIGHT)) {
            curr_x = curr_x+1; came_from = Direction::LEFT;
        } else if (grid[curr_y][curr_x].left == LineStat::LINE && (if_first_step || came_from != Direction::LEFT)) {
            curr_x = curr_x-1; came_from = Direction::RIGHT;
        } else {
            return false;
        }

        if_first_step = false;
        if (curr_x == s_x && curr_y == s_y) break;
    }
    if (path_len != cell_with_line || empties_visited != empties) return false;

    return true;
}

/* ---------------------------------------------------------------------[<]-
Function: backtrack
Synopsis: Finding correct path (backtracking)
---------------------------------------------------------------------[>]-*/
bool CourseworkMasyu::backtrack() {
    apply_rules();
    if (!if_valid()) return false;
    if (if_loop_complete()) return true;

    int g_x = -1;
    int g_y = -1;
    Direction g_dir = Direction::NONE;
    int min_sc = 1000;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int u = 0;
            int l = 0;

            if (grid[y][x].up == LineStat::UNKNOWN) u++;
            else if (grid[y][x].up == LineStat::LINE) l++;
            else {}
            if (grid[y][x].right == LineStat::UNKNOWN) u++;
            else if (grid[y][x].right == LineStat::LINE) l++;
            else {}
            if (grid[y][x].down == LineStat::UNKNOWN) u++;
            else if (grid[y][x].down == LineStat::LINE) l++;
            else {}
            if (grid[y][x].left == LineStat::UNKNOWN) u++;
            else if (grid[y][x].left == LineStat::LINE) l++;
            else {}

            if (u > 0) {
                int score = u;
                if (grid[y][x].circle != CircleType::EMPTY) {
                    score -= 10;
                } else if (l > 0) {
                        score -= 5;
                } else {}
                if (score < min_sc) {
                    min_sc = score;
                    g_x = x; g_y = y;

                    if (grid[y][x].up == LineStat::UNKNOWN) g_dir = Direction::UP;
                    else if (grid[y][x].right == LineStat::UNKNOWN) g_dir = Direction::RIGHT;
                    else if (grid[y][x].down == LineStat::UNKNOWN) g_dir = Direction::DOWN;
                    else if (grid[y][x].left == LineStat::UNKNOWN) g_dir = Direction::LEFT;
                    else {}
                }
            }
        }
    }

    if (g_x == -1 || g_y == -1) return false;
    save_state();
    set_line_stat(g_x, g_y, g_dir, LineStat::LINE);
    if (backtrack()) return true;
    restore_state();

    save_state();
    set_line_stat(g_x, g_y, g_dir, LineStat::EMPTY);
    if (backtrack()) return true;
    restore_state();

    return false;
}

/*---------------------------------------------------------------------[<]-
The main method runs all tests functions.
---------------------------------------------------------------------[>]-*/
int main() {
    int w = 14, h = 10;
    CourseworkMasyu field1(w, h);

    cout << "Welcome to the Masyu crossword solver!" << endl;
    field1.set_circle_type(4, 0, CircleType::WHITE);
    field1.set_circle_type(7, 0, CircleType::BLACK);
    field1.set_circle_type(2, 2, CircleType::WHITE);
    field1.set_circle_type(4, 2, CircleType::BLACK);
    field1.set_circle_type(7, 2, CircleType::WHITE);
    field1.set_circle_type(10, 2, CircleType::WHITE);
    field1.set_circle_type(12, 2, CircleType::BLACK);
    field1.set_circle_type(2, 4, CircleType::BLACK);
    field1.set_circle_type(5, 4, CircleType::BLACK);
    field1.set_circle_type(9, 4, CircleType::BLACK);
    field1.set_circle_type(12, 4, CircleType::WHITE);
    field1.set_circle_type(3, 5, CircleType::BLACK);
    field1.set_circle_type(7, 5, CircleType::WHITE);
    field1.set_circle_type(11, 5, CircleType::WHITE);
    field1.set_circle_type(1, 6, CircleType::WHITE);
    field1.set_circle_type(7, 6, CircleType::BLACK);
    field1.set_circle_type(9, 6, CircleType::WHITE);
    field1.set_circle_type(12, 6, CircleType::BLACK);
    field1.set_circle_type(6, 7, CircleType::WHITE);
    field1.set_circle_type(10, 7, CircleType::BLACK);
    field1.set_circle_type(4, 8, CircleType::BLACK);
    field1.set_circle_type(7, 8, CircleType::BLACK);
    field1.set_circle_type(9, 8, CircleType::WHITE);
    field1.set_circle_type(2, 9, CircleType::WHITE);
    field1.set_circle_type(11, 9, CircleType::WHITE);

    w = 7;
    h = 7;
    CourseworkMasyu field2(w, h);
    field2.set_circle_type(3, 5, CircleType::WHITE);
    field2.set_circle_type(6, 0, CircleType::BLACK);
    field2.set_circle_type(6, 6, CircleType::BLACK);
    field2.set_circle_type(0, 6, CircleType::BLACK);
    field2.set_circle_type(6, 3, CircleType::WHITE);
    field2.set_circle_type(3, 6, CircleType::WHITE);
    field2.set_circle_type(5, 5, CircleType::BLACK);
    field2.set_circle_type(3, 3, CircleType::BLACK);
    field2.set_circle_type(3, 0, CircleType::WHITE);
    field2.set_circle_type(0, 4, CircleType::WHITE);
    field2.set_circle_type(3, 1, CircleType::WHITE);

    w = 10;
    h = 10;
    CourseworkMasyu field3(w, h);
    field3.set_circle_type(9, 0, CircleType::BLACK);
    field3.set_circle_type(4, 3, CircleType::WHITE);
    field3.set_circle_type(9, 9, CircleType::BLACK);
    field3.set_circle_type(5, 1, CircleType::WHITE);
    field3.set_circle_type(8, 5, CircleType::WHITE);
    field3.set_circle_type(5, 8, CircleType::WHITE);
    field3.set_circle_type(0, 9, CircleType::BLACK);
    field3.set_circle_type(8, 1, CircleType::BLACK);
    field3.set_circle_type(5, 2, CircleType::WHITE);
    field3.set_circle_type(7, 5, CircleType::WHITE);
    field3.set_circle_type(8, 8, CircleType::BLACK);
    field3.set_circle_type(1, 8, CircleType::BLACK);
    field3.set_circle_type(5, 0, CircleType::WHITE);
    field3.set_circle_type(9, 5, CircleType::WHITE);
    field3.set_circle_type(5, 9, CircleType::WHITE);
    field3.set_circle_type(0, 0, CircleType::BLACK);
    field3.set_circle_type(2, 2, CircleType::WHITE);
    field3.set_circle_type(1, 5, CircleType::WHITE);
    field3.set_circle_type(3, 1, CircleType::WHITE);
    field3.set_circle_type(2, 7, CircleType::WHITE);

    cout << "Select field:" << endl;
    int ch;
    while (true) {
        cout << "1. Field #1\n2. Field #2\n3. Field #3\n4. Show all.\n5.Exit." << endl;
        cout << "Enter: ";
        std::cin >> ch;
        if (std::cin.fail() || std::cin.peek() != '\n') {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Please! Enter a single number!\n" << endl;
            continue;
        }
        switch (ch) {
            case 1:
                cout << "Solving field #1..." << endl;
                cout << "Field #1:" << endl;
                field1.solve();
                field1.print_to_console();
                break;
            case 2:
                cout << "Solving field #2..." << endl;
                cout << "Field #2:" << endl;
                field2.solve();
                field2.print_to_console();
                break;
            case 3:
                cout << "Solving field #3..." << endl;
                cout << "Field #3:" << endl;
                field3.solve();
                field3.print_to_console();
                break;
            case 4:
                cout << "Solving field #1..." << endl;
                cout << "Field #1:" << endl;
                field1.solve();
                field1.print_to_console();
                cout << "\n" << endl;
                cout << "Solving field #2..." << endl;
                cout << "Field #2:" << endl;
                field2.solve();
                field2.print_to_console();
                cout << "\n" << endl;
                cout << "Solving field #3..." << endl;
                cout << "Field #3:" << endl;
                field3.solve();
                field3.print_to_console();
                break;
            case 5:
                cout << "Thanks for using! Good luck." << endl;
                return 0;
            default:
                cout << "Error! Choose between 1, 2, 3, 4 and 5!\n" << endl;
                break;
        }
    }
}