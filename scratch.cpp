#include <iostream>
int16_t align8(int16_t v) { return (int16_t)((v / 8) * 8); }
int main() {
    int16_t y = align8(32);
    int16_t yEnd = 32 + 128;
    int16_t h = (int16_t)(align8((int16_t)(yEnd + 7)) - y);
    std::cout << "y=" << y << " yEnd=" << yEnd << " h=" << h << std::endl;
    return 0;
}
