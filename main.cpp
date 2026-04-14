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
}

// could be useful -> std::vector<Grid> history;
int main() {
    int w, h;
    std::cout << "Please enter the width and height (\"w h\"): ";
    std::cin >> w >> h;
    MasyuSolverCore field(w, h);

    return 0;
}
