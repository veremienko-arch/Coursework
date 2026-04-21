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

    //bool ifCoordIsValid(int x, int y) const; seems useless now
    void setLineStatus(int x, int y, Direction dir, LineStatus status);
    void saveState();
    void restoreState();

    void applyRules();
    bool backtrack();
    bool ifLoopIsComplete() const;

public:
    MasyuSolverCore(int x, int y);

    void setCircleType(int x,  int y, CircleType type);

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
            std::cout << "   ";
        }
        std::cout << std::endl;
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

    std::cout << "Current grid  of Ex. #1 (unsolved):" << std::endl;

    field1.printToConsole();

    return 0;
}
