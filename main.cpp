#include <iostream>
#include <vector>

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

    bool ifCoordValid(int x, int y) const;
    bool nodeRules(int x, int y);
    bool blackRules(int x, int y);
    bool whiteRules(int x, int y);
    void saveState();
    void restoreState();
    void applyRules();
    bool backtrack();
    bool ifLoopComplete() const;
    bool ifValid() const;

public:
    CourseworkMasyu(int x, int y);
    void setCircleType(int x,  int y, CircleType type);
    void setLineStatus(int x, int y, Direction dir, LineStat status);

    //Qt
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    CircleType getCircleType(int x, int y) const;
    LineStat getLine(int  x, int y, Direction dir) const;

    bool solve();
    void printToConsole() const;
};

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

void CourseworkMasyu::setCircleType(int x, int y, CircleType type) {
    grid[y][x].circle = type;
}

void CourseworkMasyu::printToConsole() const {
    cout << std::string(53, '-') << endl;
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
    cout << std::string(53, '-') << endl;
}

bool CourseworkMasyu::ifCoordValid(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void CourseworkMasyu::setLineStatus(int x, int y, Direction dir, LineStat status) {
    if (!ifCoordValid(x, y)) return;

    switch (dir) {
        case Direction::UP: grid[y][x].up = status; break;
        case Direction::DOWN: grid[y][x].down = status; break;
        case Direction::RIGHT: grid[y][x].right = status; break;
        case Direction::LEFT: grid[y][x].left = status; break;
        default: break;
    }

    int nx = x, ny = y;
    Direction oppositeDirection;

    switch (dir) {
        case Direction::UP: ny = y - 1; oppositeDirection = Direction::DOWN; break;
        case Direction::DOWN: ny = y + 1; oppositeDirection = Direction::UP; break;
        case Direction::LEFT: nx = x - 1; oppositeDirection = Direction::RIGHT; break;
        case Direction::RIGHT: nx = x + 1; oppositeDirection = Direction::LEFT; break;
        default: break;
    }

    if (ifCoordValid(nx, ny)) {
        switch (oppositeDirection) {
            case Direction::UP: grid[ny][nx].up = status; break;
            case Direction::DOWN: grid[ny][nx].down = status; break;
            case Direction::LEFT: grid[ny][nx].left = status; break;
            case Direction::RIGHT: grid[ny][nx].right = status; break;
            default: break;
        }
    }
}

bool CourseworkMasyu::nodeRules(int x, int y) {
    int lines = 0;
    int unknowns = 0;
    bool changed = false;

    switch (grid[y][x].up) {
        case LineStat::LINE: lines++; break;
        case LineStat::UNKNOWN: unknowns++; break;
    }
    switch (grid[y][x].right) {
        case LineStat::LINE: lines++; break;
        case LineStat::UNKNOWN: unknowns++; break;
    }
    switch (grid[y][x].down) {
        case LineStat::LINE: lines++; break;
        case LineStat::UNKNOWN: unknowns++; break;
    }
    switch (grid[y][x].left) {
        case LineStat::LINE: lines++; break;
        case LineStat::UNKNOWN: unknowns++; break;
    }

    if (lines == 2 && unknowns > 0) {
        if (grid[y][x].up == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
        if (grid[y][x].right == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }
        if (grid[y][x].down == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
        if (grid[y][x].left == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
    }
    if (lines == 1 && unknowns == 1) {
        if (grid[y][x].up == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::UP, LineStat::LINE); changed = true; }
        if (grid[y][x].right == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::RIGHT, LineStat::LINE); changed = true; }
        if (grid[y][x].down == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::DOWN, LineStat::LINE); changed = true; }
        if (grid[y][x].left == LineStat::UNKNOWN) {  setLineStatus(x, y, Direction::LEFT, LineStat::LINE); changed = true; }
    }
    if (lines == 0 && unknowns == 1) {
        if (grid[y][x].up == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
        if (grid[y][x].right == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }
        if (grid[y][x].down == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
        if (grid[y][x].left == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
    }
    if (grid[y][x].circle != CircleType::EMPTY) {
        if (lines + unknowns == 2 && unknowns > 0) {
            if (grid[y][x].up == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::UP, LineStat::LINE); changed = true; }
            if (grid[y][x].right == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::RIGHT, LineStat::LINE); changed = true; }
            if (grid[y][x].down == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::DOWN, LineStat::LINE); changed = true; }
            if (grid[y][x].left == LineStat::UNKNOWN) {  setLineStatus(x, y, Direction::LEFT, LineStat::LINE); changed = true; }
        }
    }

    return changed;
}

bool CourseworkMasyu::blackRules(int x, int y) {
    bool changed = false;
    if (grid[y][x].up == LineStat::UNKNOWN) {
        if (!ifCoordValid(x, y-1) || grid[y-1][x].up == LineStat::EMPTY) {
            setLineStatus(x, y, Direction::UP, LineStat::EMPTY);
            changed = true;
        }
    }
    if (grid[y][x].right == LineStat::UNKNOWN) {
        if (!ifCoordValid(x+1, y) || grid[y][x+1].right == LineStat::EMPTY) {
            setLineStatus(x, y, Direction::RIGHT, LineStat::EMPTY);
            changed = true;
        }
    }
    if (grid[y][x].down == LineStat::UNKNOWN) {
        if (!ifCoordValid(x, y+1) || grid[y+1][x].down == LineStat::EMPTY) {
            setLineStatus(x, y, Direction::DOWN, LineStat::EMPTY);
            changed = true;
        }
    }
    if (grid[y][x].left == LineStat::UNKNOWN) {
        if (!ifCoordValid(x-1, y) || grid[y][x-1].left == LineStat::EMPTY) {
            setLineStatus(x, y, Direction::LEFT, LineStat::EMPTY);
            changed = true;
        }
    }
    if (grid[y][x].up == LineStat::LINE && grid[y][x].down == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
    if (grid[y][x].right == LineStat::LINE && grid[y][x].left == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
    if (grid[y][x].down == LineStat::LINE && grid[y][x].up == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
    if (grid[y][x].left == LineStat::LINE && grid[y][x].right == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }

    if (grid[y][x].up == LineStat::LINE && ifCoordValid(x, y-1)) {
        if (grid[y-1][x].up == LineStat::UNKNOWN) {
            setLineStatus(x, y-1, Direction::UP, LineStat::LINE);
            changed = true;
        }
    }
    if (grid[y][x].right == LineStat::LINE && ifCoordValid(x+1, y)) {
        if (grid[y][x+1].right == LineStat::UNKNOWN) {
            setLineStatus(x+1, y, Direction::RIGHT, LineStat::LINE);
            changed = true;
        }
    }
    if (grid[y][x].down == LineStat::LINE && ifCoordValid(x, y+1)) {
        if (grid[y+1][x].down == LineStat::UNKNOWN) {
            setLineStatus(x, y+1, Direction::DOWN, LineStat::LINE);
            changed = true;
        }
    }
    if (grid[y][x].left == LineStat::LINE && ifCoordValid(x-1, y)) {
        if (grid[y][x-1].left == LineStat::UNKNOWN) {
            setLineStatus(x-1, y, Direction::LEFT, LineStat::LINE);
            changed = true;
        }
    }

    if (grid[y][x].up ==  LineStat::EMPTY && grid[y][x].down == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::DOWN, LineStat::LINE); changed = true; }
    if (grid[y][x].right ==  LineStat::EMPTY && grid[y][x].left == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::LEFT, LineStat::LINE); changed = true; }
    if (grid[y][x].down ==  LineStat::EMPTY && grid[y][x].up == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::UP, LineStat::LINE); changed = true; }
    if (grid[y][x].left ==  LineStat::EMPTY && grid[y][x].right == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::RIGHT, LineStat::LINE); changed = true; }

    return changed;
}

bool CourseworkMasyu::whiteRules(int x, int y) {
    bool changed = false;
    if (grid[y][x].up == LineStat::LINE || grid[y][x].down == LineStat::LINE) {
        if (grid[y][x].up == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::UP, LineStat::LINE); changed = true; }
        if (grid[y][x].right == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }
        if (grid[y][x].down == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::DOWN, LineStat::LINE); changed = true; }
        if (grid[y][x].left == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
    } else if (grid[y][x].left == LineStat::LINE || grid[y][x].right == LineStat::LINE) {
        if (grid[y][x].up == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
        if (grid[y][x].right == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::RIGHT, LineStat::LINE); changed = true; }
        if (grid[y][x].down == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
        if (grid[y][x].left == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::LEFT, LineStat::LINE); changed = true; }
    }
    if (grid[y][x].up == LineStat::EMPTY || grid[y][x].down == LineStat::EMPTY) {
        if (grid[y][x].up == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::UP, LineStat::EMPTY); changed = true; }
        if (grid[y][x].down == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::DOWN, LineStat::EMPTY); changed = true; }
        if (grid[y][x].right == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::RIGHT, LineStat::LINE); changed = true; }
        if (grid[y][x].left == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::LEFT, LineStat::LINE); changed = true; }
    }
    if (grid[y][x].left == LineStat::EMPTY || grid[y][x].right == LineStat::EMPTY) {
        if (grid[y][x].up == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::UP, LineStat::LINE); changed = true; }
        if (grid[y][x].down == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::DOWN, LineStat::LINE); changed = true; }
        if (grid[y][x].left == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::LEFT, LineStat::EMPTY); changed = true; }
        if (grid[y][x].right == LineStat::UNKNOWN) { setLineStatus(x, y, Direction::RIGHT, LineStat::EMPTY); changed = true; }
    }
    if (grid[y][x].left == LineStat::LINE && grid[y][x].right == LineStat::LINE) {
        if (ifCoordValid(x-1, y) && grid[y][x-1].left == LineStat::LINE) {
            if (ifCoordValid(x+1, y) && grid[y][x+1].right == LineStat::UNKNOWN) {
                setLineStatus(x+1, y, Direction::RIGHT, LineStat::EMPTY);
                changed = true;
            }
        }
        if (ifCoordValid(x+1, y) && grid[y][x+1].right == LineStat::LINE) {
            if (ifCoordValid(x-1, y) && grid[y][x-1].left == LineStat::UNKNOWN) {
                setLineStatus(x-1, y, Direction::LEFT, LineStat::EMPTY);
                changed = true;
            }
        }
    }
    if (grid[y][x].up == LineStat::LINE && grid[y][x].down == LineStat::LINE) {
        if (ifCoordValid(x, y-1) && grid[y-1][x].up == LineStat::LINE) {
            if (ifCoordValid(x, y+1) && grid[y+1][x].down == LineStat::UNKNOWN) {
                setLineStatus(x, y+1, Direction::DOWN, LineStat::EMPTY);
                changed = true;
            }
        }
        if (ifCoordValid(x, y+1) && grid[y+1][x].down == LineStat::LINE) {
            if (ifCoordValid(x, y-1) && grid[y-1][x].up == LineStat::UNKNOWN) {
                setLineStatus(x, y-1, Direction::UP, LineStat::EMPTY);
                changed = true;
            }
        }
    }
    return changed;
}

void CourseworkMasyu::applyRules() {
    bool changed = true;
    while (changed) {
        changed = false;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (nodeRules(x, y)) changed = true;

                if (grid[y][x].circle == CircleType::WHITE) {
                    if (whiteRules(x, y)) changed = true;
                } else if (grid[y][x].circle == CircleType::BLACK) {
                    if (blackRules(x, y)) changed = true;
                }
            }
        }
    }
}

bool CourseworkMasyu::solve() {
    return backtrack();
}

bool CourseworkMasyu::ifValid() const {
    //int totEnds = 0;
    //int totLines = 0;

    for (int  y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int l = 0;
            int u = 0;

            if (grid[y][x].up == LineStat::LINE) {
                l++;
            } else if (grid[y][x].up == LineStat::UNKNOWN) {
                u++;
            }
            if (grid[y][x].right == LineStat::LINE) {
                l++;
            } else if (grid[y][x].right == LineStat::UNKNOWN) {
                u++;
            }
            if (grid[y][x].down == LineStat::LINE) {
                l++;
            } else if (grid[y][x].down == LineStat::UNKNOWN) {
                u++;
            }
            if (grid[y][x].left == LineStat::LINE) {
                l++;
            } else if (grid[y][x].left == LineStat::UNKNOWN) {
                u++;
            }

            if (l > 2) return false;
            if (l == 1 && u == 0) return false;
            if (grid[y][x].circle != CircleType::EMPTY) {
                if (l + u < 2) return false;
            }

            if (l==2) {
                bool strt = (grid[y][x].up == LineStat::LINE && grid[y][x].down == LineStat::LINE) || (grid[y][x].left == LineStat::LINE && grid[y][x].right == LineStat::LINE);
                if (grid[y][x].circle == CircleType::WHITE && !strt) return false;
                if (grid[y][x].circle == CircleType::BLACK && strt) return false;
            }
            //if (l == 1) totEnds++;
            //if (l > 0) totLines++;
        }
    }
    //if (totLines > 0 && totEnds == 0) {
    //    if (!ifLoopComplete()) return false;
    //}

    return true;
}

void CourseworkMasyu::saveState() {
    history.push_back(grid);
}

void CourseworkMasyu::restoreState() {
    grid = history.back();
    history.pop_back();
}

bool CourseworkMasyu::ifLoopComplete() const {
    int cir = 0;
    int cellWithLine = 0;
    int sX = -1;
    int sY = -1;
    int currX = -1;
    int currY = -1;
    int pathLen = 0;
    int cirCount = 0;
    bool found = false;
    bool ifFirstStep = true;
    Direction cameFrom = Direction::NONE;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x].circle == CircleType::WHITE || grid[y][x].circle == CircleType::BLACK) cir++;
            if (grid[y][x].up == LineStat::LINE || grid[y][x].down == LineStat::LINE || grid[y][x].left == LineStat::LINE || grid[y][x].right == LineStat::LINE) cellWithLine++;
        }
    }
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x].up == LineStat::LINE || grid[y][x].down == LineStat::LINE || grid[y][x].left == LineStat::LINE || grid[y][x].right == LineStat::LINE) {
                sX = x;
                sY = y;
                found = true;
                break;
            }
        }
        if (found) break;
    }
    if (sX == -1 || sY == -1) return false;

    currX = sX;
    currY = sY;
    while (true) {
        pathLen++;
        if (grid[currY][currX].circle != CircleType::EMPTY) cirCount++;
        if (grid[currY][currX].up == LineStat::LINE && (ifFirstStep || cameFrom != Direction::UP)) {
            currY = currY-1;
            cameFrom = Direction::DOWN;
        } else if (grid[currY][currX].down == LineStat::LINE && (ifFirstStep || cameFrom != Direction::DOWN)) {
            currY = currY+1;
            cameFrom = Direction::UP;
        } else if (grid[currY][currX].right == LineStat::LINE && (ifFirstStep || cameFrom != Direction::RIGHT)) {
            currX = currX+1;
            cameFrom = Direction::LEFT;
        } else if (grid[currY][currX].left == LineStat::LINE && (ifFirstStep || cameFrom != Direction::LEFT)) {
            currX = currX-1;
            cameFrom = Direction::RIGHT;
        } else {
            return false;
        }
        ifFirstStep = false;
        if (currX == sX && currY == sY) break;
    }
    if (pathLen != cellWithLine || cirCount != cir) return false;
    return true;
}

bool CourseworkMasyu::backtrack() {
    applyRules();
    if (!ifValid()) return false;
    if (ifLoopComplete()) return true;

    int gX = -1;
    int gY = -1;
    Direction gDir = Direction::NONE;
    int minSc = 1000;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int u = 0;
            int l = 0;

            if (grid[y][x].up == LineStat::UNKNOWN) u++;
            else if (grid[y][x].up == LineStat::LINE) l++;
            if (grid[y][x].right == LineStat::UNKNOWN) u++;
            else if (grid[y][x].right == LineStat::LINE) l++;
            if (grid[y][x].down == LineStat::UNKNOWN) u++;
            else if (grid[y][x].down == LineStat::LINE) l++;
            if (grid[y][x].left == LineStat::UNKNOWN) u++;
            else if (grid[y][x].left == LineStat::LINE) l++;

            if (u > 0) {
                int score = u;
                if (grid[y][x].circle != CircleType::EMPTY) {
                    score -= 10;
                } else if (l > 0) {
                        score -= 5;
                }
                if (score < minSc) {
                    minSc = score;
                    gX = x; gY = y;

                    if (grid[y][x].up == LineStat::UNKNOWN) gDir = Direction::UP;
                    else if (grid[y][x].right == LineStat::UNKNOWN) gDir = Direction::RIGHT;
                    else if (grid[y][x].down == LineStat::UNKNOWN) gDir = Direction::DOWN;
                    else if (grid[y][x].left == LineStat::UNKNOWN) gDir = Direction::LEFT;
                }
            }
        }
    }

    if (gX == -1 || gY == -1) return false;
    saveState();
    setLineStatus(gX, gY, gDir, LineStat::LINE);
    if (backtrack()) return true;
    restoreState();

    saveState();
    setLineStatus(gX, gY, gDir, LineStat::EMPTY);
    if (backtrack()) return true;
    restoreState();

    return false;
}

int main() {
    int w = 14;
    int h = 10;
    CourseworkMasyu field1(w, h);
    CourseworkMasyu field2(w, h);
    CourseworkMasyu field3(w, h);
    //Ex. 1
    field1.setCircleType(4, 0, CircleType::WHITE);
    field1.setCircleType(7, 0, CircleType::BLACK);
    field1.setCircleType(2, 2, CircleType::WHITE);
    field1.setCircleType(4, 2, CircleType::BLACK);
    field1.setCircleType(7, 2, CircleType::WHITE);
    field1.setCircleType(10, 2, CircleType::WHITE);
    field1.setCircleType(12, 2, CircleType::BLACK);
    field1.setCircleType(2, 4, CircleType::BLACK);
    field1.setCircleType(5, 4, CircleType::BLACK);
    field1.setCircleType(9, 4, CircleType::BLACK);
    field1.setCircleType(12, 4, CircleType::WHITE);
    field1.setCircleType(3, 5, CircleType::BLACK);
    field1.setCircleType(7, 5, CircleType::WHITE);
    field1.setCircleType(11, 5, CircleType::WHITE);
    field1.setCircleType(1, 6, CircleType::WHITE);
    field1.setCircleType(7, 6, CircleType::BLACK);
    field1.setCircleType(9, 6, CircleType::WHITE);
    field1.setCircleType(12, 6, CircleType::BLACK);
    field1.setCircleType(6, 7, CircleType::WHITE);
    field1.setCircleType(10, 7, CircleType::BLACK);
    field1.setCircleType(4, 8, CircleType::BLACK);
    field1.setCircleType(7, 8, CircleType::BLACK);
    field1.setCircleType(9, 8, CircleType::WHITE);
    field1.setCircleType(2, 9, CircleType::WHITE);
    field1.setCircleType(11, 9, CircleType::WHITE);

    cout << "Ex. #1:" << endl;
    field1.solve();
    field1.printToConsole();
    cout << "\n" << endl;

    //Ex. 2
    field2.setCircleType(2, 0, CircleType::WHITE);
    field2.setCircleType(4, 0, CircleType::BLACK);
    field2.setCircleType(8, 0, CircleType::BLACK);
    field2.setCircleType(10, 0, CircleType::WHITE);
    field2.setCircleType(1, 1, CircleType::WHITE);
    field2.setCircleType(6, 1, CircleType::WHITE);
    field2.setCircleType(11, 1, CircleType::BLACK);
    field2.setCircleType(12, 1, CircleType::WHITE);
    field2.setCircleType(1, 2, CircleType::BLACK);
    field2.setCircleType(6, 2, CircleType::WHITE);
    field2.setCircleType(8, 2, CircleType::BLACK);
    field2.setCircleType(5, 3, CircleType::WHITE);
    field2.setCircleType(7, 3, CircleType::WHITE);
    field2.setCircleType(8, 3, CircleType::BLACK);
    field2.setCircleType(10, 3, CircleType::WHITE);
    field2.setCircleType(11, 3, CircleType::BLACK);
    field2.setCircleType(0, 4, CircleType::WHITE);
    field2.setCircleType(1, 4, CircleType::WHITE);
    field2.setCircleType(2, 4, CircleType::WHITE);
    field2.setCircleType(5, 4, CircleType::WHITE);
    field2.setCircleType(6, 4, CircleType::WHITE);
    field2.setCircleType(2, 5, CircleType::WHITE);
    field2.setCircleType(7, 5, CircleType::WHITE);
    field2.setCircleType(8, 5, CircleType::WHITE);
    field2.setCircleType(11, 5, CircleType::WHITE);
    field2.setCircleType(13, 5, CircleType::BLACK);
    field2.setCircleType(1, 6, CircleType::WHITE);
    field2.setCircleType(3, 6, CircleType::WHITE);
    field2.setCircleType(4, 6, CircleType::WHITE);
    field2.setCircleType(7, 6, CircleType::WHITE);
    field2.setCircleType(8, 6, CircleType::BLACK);
    field2.setCircleType(11, 6, CircleType::WHITE);
    field2.setCircleType(13, 6, CircleType::WHITE);
    field2.setCircleType(2, 7, CircleType::WHITE);
    field2.setCircleType(10, 7, CircleType::WHITE);
    field2.setCircleType(12, 7, CircleType::WHITE);
    field2.setCircleType(2, 8, CircleType::WHITE);
    field2.setCircleType(3, 8, CircleType::WHITE);
    field2.setCircleType(5, 9, CircleType::WHITE);
    field2.setCircleType(6, 9, CircleType::BLACK);
    field2.setCircleType(9, 9, CircleType::BLACK);
    field2.setCircleType(12, 9, CircleType::WHITE);

    cout << "Ex. #2:" << endl;
    field2.solve();
    field2.printToConsole();
    cout << "\n" << endl;

    //Ex. 3
    field3.setCircleType(5, 0, CircleType::WHITE);
    field3.setCircleType(10, 0, CircleType::BLACK);
    field3.setCircleType(0, 1, CircleType::WHITE);
    field3.setCircleType(1, 1, CircleType::WHITE);
    field3.setCircleType(5, 1, CircleType::WHITE);
    field3.setCircleType(8, 1, CircleType::WHITE);
    field3.setCircleType(9, 1, CircleType::WHITE);
    field3.setCircleType(2, 2, CircleType::WHITE);
    field3.setCircleType(11, 2, CircleType::WHITE);
    field3.setCircleType(13, 2, CircleType::WHITE);
    field3.setCircleType(2, 3, CircleType::WHITE);
    field3.setCircleType(5, 3, CircleType::WHITE);
    field3.setCircleType(6, 3, CircleType::WHITE);
    field3.setCircleType(9, 3, CircleType::WHITE);
    field3.setCircleType(0, 4, CircleType::WHITE);
    field3.setCircleType(2, 4, CircleType::WHITE);
    field3.setCircleType(11, 4, CircleType::BLACK);
    field3.setCircleType(9, 5, CircleType::BLACK);
    field3.setCircleType(10, 5, CircleType::BLACK);
    field3.setCircleType(2, 6, CircleType::WHITE);
    field3.setCircleType(4, 6, CircleType::BLACK);
    field3.setCircleType(6, 6, CircleType::BLACK);
    field3.setCircleType(7, 6, CircleType::WHITE);
    field3.setCircleType(1, 7, CircleType::WHITE);
    field3.setCircleType(4, 7, CircleType::BLACK);
    field3.setCircleType(2, 8, CircleType::WHITE);
    field3.setCircleType(12, 8, CircleType::BLACK);
    field3.setCircleType(0, 9, CircleType::BLACK);
    field3.setCircleType(6, 9, CircleType::WHITE);
    field3.setCircleType(8, 9, CircleType::BLACK);
    field3.setCircleType(11, 9, CircleType::WHITE);

    cout << "Ex. #3:" << endl;
    field3.solve();
    field3.printToConsole();

    return 0;
}
