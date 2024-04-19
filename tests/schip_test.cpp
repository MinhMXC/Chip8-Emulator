#include <catch2/catch_test_macros.hpp>
#include "../src/emulators/schip.h"
#include "../src/constants.h"

// All members in these classes are public for convenient testing

class TestInputHandler : public InputHandler {
public:
    TestInputHandler(): InputHandler() {}

    void setKey(uint8_t key) {
        keys[key] = true;
    }
};

class SChipTest : public SChip {
public:
    explicit SChipTest(AdvancedDisplay& display, TestInputHandler& handler): SChip(display, handler) {
    }

    void decodeTest(uint16_t ins) {
        SChip::decode(ins);
    }

    uint8_t* getMemory() {
        return memory;
    }

    uint16_t getPC() {
        return program_counter;
    }

    uint16_t getIndex() {
        return index_register;
    }

    uint8_t getDelayTimer() {
        return delay_timer;
    }

    uint8_t getSoundTimer() {
        return sound_timer;
    }

    std::vector<uint8_t>& getRegisters() {
        return registers;
    }

    std::vector<uint8_t>& getFlags() {
        return flags;
    }
};

TEST_CASE("SChip Font Loaded Correctly") {
    TestInputHandler inputHandler{};
    AdvancedDisplay display{};
    SChipTest schip{display, inputHandler};

    uint8_t* mem{schip.getMemory()};
    int count{};

    for (int i{0x50}; i <= 0x9F; i++) {
        REQUIRE(mem[i] == FONT[i - 0x50]);
        count++;
    }

    for (int i{0xA0}; i < 0x140; i++) {
        REQUIRE(mem[i] == SCHIP_FONT[i - 0xA0]);
        count++;
        printf("%d\n", count);
    }

    REQUIRE(count == 240);
}

TEST_CASE("SChip Correct Helper Function") {
    TestInputHandler inputHandler{};
    AdvancedDisplay display{};
    SChipTest schip{display, inputHandler};

    SECTION("Correct Small Sprite Decode") {
        std::vector<bool> v(8);
        schip.decodeTest(0xA050); // Point index_register to 0
        schip.decodeSmallSprite(schip.getIndex(), v);

        for (int i{}; i < 4; i++) {
            REQUIRE(v[i]);
        }

        for (int i{4}; i < 8; i++) {
            REQUIRE(!v[i]);
        }
    }

    SECTION("Correct Big Sprite Decode") {
        std::vector<bool> v(16);
        schip.getMemory()[0] = 0xFF;
        schip.getMemory()[1] = 0xFF;
        schip.decodeBigSprite(schip.getIndex(), v);
        for (int i{}; i < 16; i++) {
            REQUIRE(v[i]);
        }
    }
}

TEST_CASE("SChip Registers Do Not Overflow") {
    TestInputHandler inputHandler{};
    AdvancedDisplay display{};
    SChipTest schip{display, inputHandler};

    schip.decodeTest(0x60FF);
    schip.decodeTest(0x61FF);

    schip.decodeTest(0x7001);
    schip.decodeTest(0x7101);

    REQUIRE(schip.getRegisters()[0] == 0);
    REQUIRE(schip.getRegisters()[1] == 0);
}

TEST_CASE("SChip Correct Arithmetic Instruction")  {
    TestInputHandler inputHandler{};
    AdvancedDisplay display{};
    SChipTest schip{display, inputHandler};

    schip.decodeTest(0x6023);
    schip.decodeTest(0x6169);

    SECTION("Correct Set") {
        schip.decodeTest(0x8010);
        REQUIRE(schip.getRegisters()[0] == 0x0069);
        REQUIRE(schip.getRegisters()[1] == 0x0069);
        REQUIRE(schip.getRegisters()[15] == 0);
    }

    SECTION("Correct Binary OR") {
        uint8_t result = schip.getRegisters()[0] | schip.getRegisters()[1];
        schip.decodeTest(0x6F01);
        schip.decodeTest(0x8011);
        REQUIRE(schip.getRegisters()[0] == result);
        REQUIRE(schip.getRegisters()[1] == 0x0069);
        REQUIRE(schip.getRegisters()[15] == 1);
    }

    SECTION("Correct Binary AND") {
        uint8_t result = schip.getRegisters()[0] & schip.getRegisters()[1];
        schip.decodeTest(0x6F01);
        schip.decodeTest(0x8012);
        REQUIRE(schip.getRegisters()[0] == result);
        REQUIRE(schip.getRegisters()[1] == 0x0069);
        REQUIRE(schip.getRegisters()[15] == 1);
    }

    SECTION("Correct Binary XOR") {
        uint8_t result = schip.getRegisters()[0] ^ schip.getRegisters()[1];
        schip.decodeTest(0x6F01);
        schip.decodeTest(0x8013);
        REQUIRE(schip.getRegisters()[0] == result);
        REQUIRE(schip.getRegisters()[1] == 0x0069);
        REQUIRE(schip.getRegisters()[15] == 1);
    }

    SECTION("Correct Add") {
        schip.decodeTest(0x60FF);
        uint8_t result = schip.getRegisters()[0] + schip.getRegisters()[1];
        schip.decodeTest(0x8014);
        REQUIRE(schip.getRegisters()[0] == result);
        REQUIRE(schip.getRegisters()[15] == 1);
        REQUIRE(schip.getRegisters()[1] == 0x0069);
    }

    SECTION("Correct Subtract X - Y") {
        uint8_t result = schip.getRegisters()[0] - schip.getRegisters()[1];
        schip.decodeTest(0x8015);
        REQUIRE(schip.getRegisters()[0] == result);
        REQUIRE(schip.getRegisters()[15] == 0);
        REQUIRE(schip.getRegisters()[1] == 0x0069);
    }

    SECTION("Correct Subtract Y - X") {
        uint8_t result = schip.getRegisters()[1] - schip.getRegisters()[0];
        schip.decodeTest(0x8017);
        REQUIRE(schip.getRegisters()[0] == result);
        REQUIRE(schip.getRegisters()[15] == 1);
        REQUIRE(schip.getRegisters()[1] == 0x0069);
    }

    SECTION("Correct Shift Right") {
        schip.decodeTest(0x8016);
        REQUIRE(schip.getRegisters()[0] == (0x0023 >> 1));
        REQUIRE(schip.getRegisters()[1] == 0x69);
        REQUIRE(schip.getRegisters()[15] == 1);
    }

    SECTION("Correct Shift Left") {
        schip.decodeTest(0x801E);
        REQUIRE(schip.getRegisters()[0] == 70);
        REQUIRE(schip.getRegisters()[1] == 0x69);
        REQUIRE(schip.getRegisters()[15] == 0);
    }
}

TEST_CASE("SChip Correct 0 Instructions") {
    TestInputHandler inputHandler{};
    AdvancedDisplay display{};
    SChipTest schip{display, inputHandler};

    SECTION("Correct 00E0 Instruction") {
        // LORES
        // Fill all pixel white
        for (int i{}; i < WIDTH; i++) {
            for (int j{}; j < HEIGHT; j++) {
                display.drawPixel(i, j, true);
            }
        }

        // Check if all pixel are white
        for (int i{}; i < WIDTH; i++) {
            for (int j{}; j < HEIGHT; j++) {
                REQUIRE(display.getDisplay()[i][j] == 1);
            }
        }

        schip.decodeTest(0x00E0);

        // Check if all pixel are black (e.g. display is cleared)
        for (int i{}; i < WIDTH; i++) {
            for (int j{}; j < HEIGHT; j++) {
                REQUIRE(display.getDisplay()[i][j] == 0);
            }
        }

        schip.decodeTest(0x00FF);

        // HIRES
        // Fill all pixel white
        for (int i{}; i < 128; i++) {
            for (int j{}; j < 64; j++) {
                display.drawPixel(i, j, true);
            }
        }

        // Check if all pixel are white
        for (int i{}; i < 128; i++) {
            for (int j{}; j < 64; j++) {
                REQUIRE(display.getDisplay()[i][j] == 1);
            }
        }

        schip.decodeTest(0x00E0);

        // Check if all pixel are black (e.g. display is cleared)
        for (int i{}; i < 128; i++) {
            for (int j{}; j < 64; j++) {
                REQUIRE(display.getDisplay()[i][j] == 0);
            }
        }
    }

    SECTION("Correct 00CN Instruction") {
        // HIRES
        schip.decodeTest(0x00FF);
        for (int i{}; i < 16; i++) {
            display.drawPixel(0, i, true);
        }
        schip.decodeTest(0x00CF);
        REQUIRE(display.getDisplay()[0][14] == 0);
        for (int i{15}; i < 30; i++) {
            REQUIRE(display.getDisplay()[0][i] == 1);
        }

        display.clearScreen();

        // LORES
        schip.decodeTest(0x00FE);
        for (int i{}; i < 5; i++) {
            display.drawPixel(0, i, true);
        }
        schip.decodeTest(0x00C5);
        REQUIRE(display.getDisplay()[0][1] == 0);
        for (int i{2}; i < 7; i++) {
            REQUIRE(display.getDisplay()[0][i] == 1);
        }
    }

    SECTION("Correct 00FB Instruction") {
        // HIRES
        schip.decodeTest(0x00FF);
        for (int i{}; i < 5; i++) {
            display.drawPixel(i, 0, true);
        }
        schip.decodeTest(0x00FB);
        REQUIRE(display.getDisplay()[3][0] == 0);
        REQUIRE(display.getDisplay()[4][0] == 1);

        // LORES
        schip.decodeTest(0x00FE);
        for (int i{}; i < 3; i++) {
            display.drawPixel(i, 0, true);
        }
        schip.decodeTest(0x00FB);
        REQUIRE(display.getDisplay()[1][0] == 0);
        REQUIRE(display.getDisplay()[2][0] == 1);
    }

    SECTION("Correct 00FC Instruction") {
        // HIRES
        schip.decodeTest(0x00FF);
        for (int i{}; i < 5; i++) {
            display.drawPixel(i, 0, true);
        }
        schip.decodeTest(0x00FC);
        REQUIRE(display.getDisplay()[0][0] == 1);
        REQUIRE(display.getDisplay()[1][0] == 0);

        // LORES
        schip.decodeTest(0x00FE);
        for (int i{}; i < 3; i++) {
            display.drawPixel(i, 0, true);
        }
        schip.decodeTest(0x00FC);
        REQUIRE(display.getDisplay()[0][0] == 1);
        REQUIRE(display.getDisplay()[1][0] == 0);
    }

    // 00FD, 00FE, 00FF will be tested in operation
}

TEST_CASE("Correct F Instructions") {
    TestInputHandler inputHandler{};
    AdvancedDisplay advancedDisplay{};
    SChipTest schip{advancedDisplay, inputHandler};

    // Seed values in registers 0 and 1
    schip.decodeTest(0x60F0);
    schip.decodeTest(0x61FF);

    SECTION("Correct FX07 Instruction") {
        schip.decodeTest(0xF007);
        REQUIRE(schip.getRegisters()[0] == 0x00);
    }

    // FX0A not tested

    SECTION("Correct FX15 Instruction") {
        schip.decodeTest(0xF015);
        REQUIRE(schip.getDelayTimer() == 0xF0);
    }

    SECTION("Correct FX18 Instruction") {
        schip.decodeTest(0xF018);
        REQUIRE(schip.getSoundTimer() == 0xF0);
    }

    SECTION("Correct FX1E Instruction") {
        schip.decodeTest(0xF01E);
        REQUIRE(schip.getIndex() == 0xF0);
    }

    SECTION("Correct FX29 Instruction") {
        schip.decodeTest(0xF129);
        REQUIRE(schip.getIndex() == 0x9B);
    }

    SECTION("Correct FX30 Instruction") {
        schip.decodeTest(0xF130);
        REQUIRE(schip.getIndex() == 0x136);
    }

    SECTION("Correct FX33 Instruction") {
        schip.decodeTest(0xF133);
        REQUIRE(schip.getMemory()[schip.getIndex()] == 2);
        REQUIRE(schip.getMemory()[schip.getIndex() + 1] == 5);
        REQUIRE(schip.getMemory()[schip.getIndex() + 2] == 5);
    }

    SECTION("Correct FX55 Instruction") {
        schip.decodeTest(0xF155);
        REQUIRE(schip.getRegisters()[0] == 0xF0);
        REQUIRE(schip.getRegisters()[1] == 0xFF);
        REQUIRE(schip.getIndex() == 0);
        REQUIRE(schip.getMemory()[0] == 0xF0);
        REQUIRE(schip.getMemory()[1] == 0xFF);
    }

    SECTION("Correct FX65 Instruction") {
        schip.decodeTest(0xF165);
        REQUIRE(schip.getRegisters()[0] == 0);
        REQUIRE(schip.getRegisters()[1] == 0);
        REQUIRE(schip.getIndex() == 0);
    }

    SECTION("Correct FX75 Instruction") {
        schip.decodeTest(0xF175);
        REQUIRE(schip.getFlags()[0] == 0xF0);
        REQUIRE(schip.getFlags()[1] == 0xFF);
    }

    SECTION("Correct FX85 Instruction") {
        schip.decodeTest(0xF185);
        REQUIRE(schip.getRegisters()[0] == 0x00);
        REQUIRE(schip.getRegisters()[1] == 0x00);
    }
}

TEST_CASE("SChip Correct Other Instruction Decode", "") {
    TestInputHandler inputHandler{};
    AdvancedDisplay display{};
    SChipTest schip{display, inputHandler};

    SECTION("Correct Subroutines Instructions") {
        // Namely 00EE and 2NNN

        schip.decodeTest(0x2123);
        REQUIRE(schip.getPC() == 0x0123);

        schip.decodeTest(0x2696);
        REQUIRE(schip.getPC() == 0x0696);

        schip.decodeTest(0x00EE);
        REQUIRE(schip.getPC() == 0x0123);

        schip.decodeTest(0x00EE);
        REQUIRE(schip.getPC() == 0x0200);
    }

    SECTION("Correct 1 Instructions") {
        schip.decodeTest(0x1234);
        REQUIRE(schip.getPC() == 0x0234);

        schip.decodeTest(0x1696);
        REQUIRE(schip.getPC() == 0x0696);
    }

    SECTION("Correct 3 Instructions") {
        uint16_t original{schip.getPC()};

        schip.decodeTest(0x3000);
        REQUIRE(schip.getPC() == original + 2);

        schip.decodeTest(0x3001);
        REQUIRE(schip.getPC() == original + 2);
    }

    SECTION("Correct 4 Instruction") {
        uint16_t original{schip.getPC()};

        schip.decodeTest(0x4000);
        REQUIRE(schip.getPC() == original);

        schip.decodeTest(0x4001);
        REQUIRE(schip.getPC() == original + 2);
    }

    SECTION("Correct 5 Instruction") {
        uint16_t original{schip.getPC()};

        schip.decodeTest(0x5010);
        REQUIRE(schip.getPC() == original + 2);

        schip.decodeTest(0x6101);

        schip.decodeTest(0x5010);
        REQUIRE(schip.getPC() == original + 2);
    }

    SECTION("Correct 6 Instruction") {
        for (int i{}; i < 16; i++) {
            schip.decodeTest(0x6069 + 256 * i);
        }

        for (int i{}; i < 16; i++) {
            REQUIRE(schip.getRegisters()[i] == 0x0069);
        }

        for (int i{}; i < 16; i++) {
            schip.decodeTest(0x6096 + 256 * i);
        }

        for (int i{}; i < 16; i++) {
            REQUIRE(schip.getRegisters()[i] == 0x0096);
        }
    }

    SECTION("Correct 7 Instruction") {
        for (int i{}; i < 16; i++) {
            schip.decodeTest(0x7069 + 256 * i);
        }

        for (int i{}; i < 16; i++) {
            REQUIRE(schip.getRegisters()[i] == 0x0069);
        }

        // Incrementing existing values
        for (int i{}; i < 16; i++) {
            schip.decodeTest(0x7001 + 256 * i);
        }

        for (int i{}; i < 16; i++) {
            REQUIRE(schip.getRegisters()[i] == 0x006A);
        }
    }

    SECTION("Correct 9 Instruction") {
        uint16_t original{schip.getPC()};

        schip.decodeTest(0x9010);
        REQUIRE(schip.getPC() == original);

        schip.decodeTest(0x6101);

        schip.decodeTest(0x9010);
        REQUIRE(schip.getPC() == original + 2);
    }

    SECTION("Correct A Instruction") {
        schip.decodeTest(0xA123);
        REQUIRE(schip.getIndex() == 0x0123);
    }

    SECTION("Correct B Instruction") {
        schip.decodeTest(0xB123);
        REQUIRE(schip.getPC() == 0x0123);

        schip.decodeTest(0x6134);
        schip.decodeTest(0xB123);
        REQUIRE(schip.getPC() == 0x0157);
    }

    SECTION("Correct C Instruction") {
        schip.decodeTest(0xC012);
        REQUIRE(schip.getRegisters()[0] != 0x0012);
    }

//    SECTION("Correct D Instruction") {
//        // x = register 0; y = register 1
//
//
//        // CASE 1: Wrapping and basic functionality
//
//        // Point to 0 in font
//        schip.decodeTest(0xA050);
//
//        // Set x = 64; y = 32
//        // x will wrap down to 0; y will wrap down to 0
//        schip.decodeTest(0x6040);
//        schip.decodeTest(0x6120);
//
//        // Draw 0 at x = 0; y = 0
//        schip.decodeTest(0xD015);
//
//
//        // Checking 0 has been drawn correctly
//        for (int i{}; i < 4; i++) {
//            REQUIRE(display.getDisplay()[0][i] == 1);
//        }
//
//        for (int i{1}; i < 4; i++) {
//            REQUIRE(display.getDisplay()[i][0] == 1);
//            REQUIRE(display.getDisplay()[i][1] == 0);
//            REQUIRE(display.getDisplay()[i][2] == 0);
//            REQUIRE(display.getDisplay()[i][3] == 1);
//        }
//
//        for (int i{}; i < 4; i++) {
//            REQUIRE(display.getDisplay()[4][i] == 1);
//        }
//
//        for (int i{0}; i < 4; i++) {
//            for (int j{}; j < 5; j++) {
//                REQUIRE(display.getDisplay()[j][i + 4] == 0);
//            }
//        }
//
//
//
//        // CASE 2: Clipping and incomplete sprite length
//
//        // Set x = 62; y = 0;
//        schip.decodeTest(0x603E);
//
//        // Draw 0 at x = 62; y = 0
//        // Half of the 0 will be clipped and only 3 lines of 0 will be drawn
//        schip.decodeTest(0xD013);
//
//        // Checking 0 has been drawn correctly
//        REQUIRE(display.getDisplay()[0][62] == 1);
//        REQUIRE(display.getDisplay()[0][63] == 1);
//
//        REQUIRE(display.getDisplay()[1][62] == 1);
//        REQUIRE(display.getDisplay()[1][63] == 0);
//
//        REQUIRE(display.getDisplay()[2][62] == 1);
//        REQUIRE(display.getDisplay()[2][63] == 0);
//
//        REQUIRE(display.getDisplay()[3][62] == 0);
//        REQUIRE(display.getDisplay()[3][63] == 0);
//
//        REQUIRE(display.getDisplay()[4][62] == 0);
//        REQUIRE(display.getDisplay()[4][63] == 0);
//
//
//
//        // CASE 3: VF Flag is set and start 0 is flipped back to blank
//
//        // Set x = 0; y = 0
//        schip.decodeTest(0x6000);
//        schip.decodeTest(0x6100);
//
//        // Draw 0 at x = 0; y = 0
//        schip.decodeTest(0xD015);
//
//        // Checking 0 is flipped back to blank
//        for (int i{}; i < 8; i++) {
//            for (int j{}; j < 5; j++) {
//                REQUIRE(display.getDisplay()[j][i] == 0);
//            }
//        }
//
//        // Checking Flag is set
//        REQUIRE(schip.getRegisters()[15] == 1);
//    }

    SECTION("Correct E Instruction") {
        schip.decodeTest(0xE09E);
        REQUIRE(schip.getPC() == 0x200);

        inputHandler.setKey(0);
        schip.decodeTest(0xE09E);
        REQUIRE(schip.getPC() == 0x202);

        schip.decodeTest(0x6101);
        schip.decodeTest(0xE191);
        REQUIRE(schip.getPC() == 0x204);

        inputHandler.setKey(1);
        schip.decodeTest(0xE191);
        REQUIRE(schip.getPC() == 0x204);
    }
}