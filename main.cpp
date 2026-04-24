#include <iostream>
#include <vector>

enum class Direction {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

enum class CircleType {
    BLANK,
    WHITE,
    BLACK
};

enum class LineStatus {
    UNKNOWN,
    LINE,
    EMPTY
};

struct Node {
    CircleType circle = CircleType::BLANK;

    LineStatus up = LineStatus::UNKNOWN;
    LineStatus down = LineStatus::UNKNOWN;
    LineStatus right = LineStatus::UNKNOWN;
    LineStatus left = LineStatus::UNKNOWN;
};

class MasyuSolverCore {
private:
    int width, height;
    std::vector<std::vector<Node>> grid;
    std::vector<std::vector<std::vector<Node>>> history;

    bool ifCoordIsValid(int x, int y) const;
    void saveState();
    void restoreState();

    void applyRules();
    bool backtrack();
    bool ifLoopIsComplete() const;

public:
    MasyuSolverCore(int x, int y);

    void setCircleType(int x,  int y, CircleType type);
    void setLineStatus(int x, int y, Direction dir, LineStatus status);

    //for Qt
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    CircleType getCircleType(int x, int y) const;
    LineStatus getLine(int  x, int y, Direction dir) const;

    bool solve();
    void printToConsole() const;
};

MasyuSolverCore::MasyuSolverCore(int x, int y) {
    width = x;
    height = y;

    grid.resize(height, std::vector<Node>(width));
}

void MasyuSolverCore::setCircleType(int x, int y, CircleType type) {
    grid[y][x].circle = type;
}

void MasyuSolverCore::printToConsole() const {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x].circle == CircleType::WHITE) {
                std::cout << "0";
            } else if (grid[y][x].circle == CircleType::BLACK) {
                std::cout << "#";
            } else {
                std::cout << "+";
            }

            if (x < width - 1) {
                if (grid[y][x].right == LineStatus::LINE /*|| grid[y][x].left == LineStatus::LINE*/) {
                    std::cout << "---";
                } else {
                    std::cout << "   ";
                }
            }
        }
        std::cout << std::endl;

        if (y < height - 1) {
            for (int x = 0; x < width; x++) {
                if (grid[y][x].down == LineStatus::LINE /*|| grid[y][x].up == LineStatus::LINE*/) {
                    std::cout << "|   ";
                } else {
                  std::cout << "    ";
                }
            }
            std::cout << std::endl;
        }
    }
}

bool MasyuSolverCore::ifCoordIsValid(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void MasyuSolverCore::setLineStatus(int x, int y, Direction dir, LineStatus status) {
    if (!ifCoordIsValid(x, y)) return;

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

    if (ifCoordIsValid(nx, ny)) {
        switch (oppositeDirection) {
            case Direction::UP: grid[ny][nx].up = status; break;
            case Direction::DOWN: grid[ny][nx].down = status; break;
            case Direction::LEFT: grid[ny][nx].left = status; break;
            case Direction::RIGHT: grid[ny][nx].right = status; break;
            default: break;
        }
    }
}

// could be useful -> std::vector<Grid> history;
int main() {
    int w = 14;
    int h = 10;
    MasyuSolverCore field1(w, h);
    // EXAMPLE #1
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

    field1.setLineStatus(4, 0, Direction::RIGHT, LineStatus::LINE); //test
    field1.setLineStatus(3, 5, Direction::DOWN, LineStatus::LINE); // test
    field1.setLineStatus(5, 1, Direction::LEFT, LineStatus::LINE); //test
    field1.setLineStatus(1, 7, Direction::UP, LineStatus::LINE); //test

    std::cout << "Current grid  of Ex. #1 (unsolved):" << std::endl;

    field1.printToConsole();

    return 0;
}
