#ifndef CHIP8_EMULATOR_CONSTANTS_H
#define CHIP8_EMULATOR_CONSTANTS_H

// Program Constants
// Grid Size == 1
constexpr int PIXEL_SIZE{30}; // This should always be an even number for compatibility with SChip and XOChip display
constexpr int WIDTH{64};
constexpr int HEIGHT{32};
constexpr int HIRES_WIDTH{128};
constexpr int HIRES_HEIGHT{64};
constexpr int SCREEN_WIDTH{PIXEL_SIZE * WIDTH + (WIDTH - 1)};
constexpr int SCREEN_HEIGHT{PIXEL_SIZE * HEIGHT + (HEIGHT - 1)};
constexpr int HIRES_SCREEN_WIDTH{PIXEL_SIZE / 2 * HIRES_WIDTH + (HIRES_WIDTH - 1)};
constexpr int HIRES_SCREEN_HEIGHT{PIXEL_SIZE / 2 * HIRES_HEIGHT + (HIRES_HEIGHT - 1)};
constexpr int RAM_SIZE{4096};
constexpr int INSTRUCTION_PER_SECOND{1000};
constexpr int AUDIO_SAMPLE_RATE{44100};

constexpr uint32_t GRID_COLOR{0xFF101010};

constexpr uint8_t FONT[]{
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// Schip did not have font for the letters ABCDEF but the spec includes it, so I'll also include it
constexpr uint8_t SCHIP_FONT[]{
        0x3C, 0x7E, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C, // 0
        0x18, 0x38, 0x58, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, // 1
        0x3E, 0x7F, 0xC3, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xFF, 0xFF, // 2
        0x3C, 0x7E, 0xC3, 0x03, 0x0E, 0x0E, 0x03, 0xC3, 0x7E, 0x3C, // 3
        0x06, 0x0E, 0x1E, 0x36, 0x66, 0xC6, 0xFF, 0xFF, 0x06, 0x06, // 4
        0xFF, 0xFF, 0xC0, 0xC0, 0xFC, 0xFE, 0x03, 0xC3, 0x7E, 0x3C, // 5
        0x3E, 0x7C, 0xC0, 0xC0, 0xFC, 0xFE, 0xC3, 0xC3, 0x7E, 0x3C, // 6
        0xFF, 0xFF, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x60, 0x60, // 7
        0x3C, 0x7E, 0xC3, 0xC3, 0x7E, 0x7E, 0xC3, 0xC3, 0x7E, 0x3C, // 8
        0x3C, 0x7E, 0xC3, 0xC3, 0x7F, 0x3F, 0x03, 0x03, 0x1E, 0x3C, // 9
        0x3C, 0x7E, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, 0xC3, // A
        0xFC, 0xFE, 0xC3, 0xC3, 0xFE, 0xFE, 0xC3, 0xC3, 0xFE, 0xFC, // B

        0x3E, 0x7F, 0xE3, 0xC0, 0xC0, 0xC0, 0xC0, 0xE3, 0x7F, 0x3E, // C

        0xFC, 0xFE, 0xC7, 0xC3, 0xC3, 0xC3, 0xC3, 0xC7, 0xFE, 0xFC, // D
        0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, // E
        0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0, // F
};

#endif