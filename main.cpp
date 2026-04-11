#include <iostream>
#include <vector>

enum Direction {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

enum CircleType {
    BLANK,
    WHITE,
    BLACK
};

enum LineStatus {
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

class MasyuCore {
private:
    int width, height;
    std::vector<std::vector<Node>> grid;

public:
    MasyuCore(int width, int height);
};

int main() {

    return 0;
}
