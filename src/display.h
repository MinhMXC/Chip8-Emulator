#ifndef CHIP8_EMULATOR_DISPLAY_H
#define CHIP8_EMULATOR_DISPLAY_H

class Display {
public:
    virtual ~Display() = default;
    virtual void drawPixel(int x, int y, bool isWhite) {}
    virtual bool flipPixel(int x, int y) { return false; }
    virtual void updateWindowSurface() {}
    virtual void clearScreen() {}
    virtual void run() {}
};

#endif