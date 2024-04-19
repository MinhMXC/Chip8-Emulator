#include <catch2/catch_test_macros.hpp>
#include "../src/displays/simple_display.h"
#include "../src/emulators/chip8.h"

// All members in these classes are public for convenient testing

class TestInputHandler : public InputHandler {
public:
    TestInputHandler(): InputHandler() {}

    void setKey(uint8_t key) {
        keys[key] = true;
    }
};

class Chip8Test : public Chip8 {
public:
    explicit Chip8Test(SimpleDisplay& display, TestInputHandler& handler): Chip8(display, handler, true) {
    }

    void decodeTest(uint16_t ins) {
        Chip8::decode(ins);
    }

    void decodeSpriteDataTest(uint16_t position, std::vector<bool>& v) {
        Chip8::decodeSpriteData(position, v);
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
};

TEST_CASE("Font Loaded Correctly", "") {
    TestInputHandler inputHandler{};
    SimpleDisplay display{};
    Chip8Test chip8{display, inputHandler};

    uint8_t* mem{chip8.getMemory()};
    int count{};

    for (int i{0x50}; i <= 0x9F; i++) {
        REQUIRE(mem[i] == FONT[i - 0x50]);
        count++;
    }

    REQUIRE(count == 80);
}

TEST_CASE("Correct Helper Function", "") {
    TestInputHandler inputHandler{};
    SimpleDisplay display{};
    Chip8Test chip8{display, inputHandler};

    std::vector<bool> v(8);

    // Point index_register to 0
    chip8.decodeTest(0xA050);

    chip8.decodeSpriteDataTest(chip8.getIndex(), v);

    for (int i{}; i < 4; i++) {
        REQUIRE(v[i]);
    }

    for (int i{4}; i < 8; i++) {
        REQUIRE(!v[i]);
    }
}

TEST_CASE("Registers Do Not Overflow") {
    TestInputHandler inputHandler{};
    SimpleDisplay display{};
    Chip8Test chip8{display, inputHandler};

    chip8.decodeTest(0x60FF);
    chip8.decodeTest(0x61FF);

    chip8.decodeTest(0x7001);
    chip8.decodeTest(0x7101);

    REQUIRE(chip8.getRegisters()[0] == 0);
    REQUIRE(chip8.getRegisters()[1] == 0);
}

TEST_CASE("Correct Arithmetic Instruction") {
    TestInputHandler inputHandler{};
    SimpleDisplay display{};
    Chip8Test chip8{display, inputHandler};

    chip8.decodeTest(0x6023);
    chip8.decodeTest(0x6169);

    SECTION("Correct Set") {
        chip8.decodeTest(0x8010);
        REQUIRE(chip8.getRegisters()[0] == 0x0069);
        REQUIRE(chip8.getRegisters()[1] == 0x0069);
        REQUIRE(chip8.getRegisters()[15] == 0);
    }

    SECTION("Correct Binary OR") {
        uint8_t result = chip8.getRegisters()[0] | chip8.getRegisters()[1];
        chip8.decodeTest(0x8011);
        REQUIRE(chip8.getRegisters()[0] == result);
        REQUIRE(chip8.getRegisters()[1] == 0x0069);
        REQUIRE(chip8.getRegisters()[15] == 0);
    }

    SECTION("Correct Binary AND") {
        uint8_t result = chip8.getRegisters()[0] & chip8.getRegisters()[1];
        chip8.decodeTest(0x8012);
        REQUIRE(chip8.getRegisters()[0] == result);
        REQUIRE(chip8.getRegisters()[1] == 0x0069);
        REQUIRE(chip8.getRegisters()[15] == 0);
    }

    SECTION("Correct Binary XOR") {
        uint8_t result = chip8.getRegisters()[0] ^ chip8.getRegisters()[1];
        chip8.decodeTest(0x8013);
        REQUIRE(chip8.getRegisters()[0] == result);
        REQUIRE(chip8.getRegisters()[1] == 0x0069);
        REQUIRE(chip8.getRegisters()[15] == 0);
    }

    SECTION("Correct Add") {
        chip8.decodeTest(0x60FF);
        uint8_t result = chip8.getRegisters()[0] + chip8.getRegisters()[1];
        chip8.decodeTest(0x8014);
        REQUIRE(chip8.getRegisters()[0] == result);
        REQUIRE(chip8.getRegisters()[15] == 1);
        REQUIRE(chip8.getRegisters()[1] == 0x0069);
    }

    SECTION("Correct Subtract X - Y") {
        uint8_t result = chip8.getRegisters()[0] - chip8.getRegisters()[1];
        chip8.decodeTest(0x8015);
        REQUIRE(chip8.getRegisters()[0] == result);
        REQUIRE(chip8.getRegisters()[15] == 0);
        REQUIRE(chip8.getRegisters()[1] == 0x0069);
    }

    SECTION("Correct Subtract Y - X") {
        uint8_t result = chip8.getRegisters()[1] - chip8.getRegisters()[0];
        chip8.decodeTest(0x8017);
        REQUIRE(chip8.getRegisters()[0] == result);
        REQUIRE(chip8.getRegisters()[15] == 1);
        REQUIRE(chip8.getRegisters()[1] == 0x0069);
    }

    SECTION("Correct Shift Right") {
        chip8.decodeTest(0x8016);
        REQUIRE(chip8.getRegisters()[1] == 0x69);
        REQUIRE(chip8.getRegisters()[0] == 52);
        REQUIRE(chip8.getRegisters()[15] == 1);
    }

    SECTION("Correct Shift Left") {
        chip8.decodeTest(0x801E);
        REQUIRE(chip8.getRegisters()[1] == 0x69);
        REQUIRE( (int) chip8.getRegisters()[0] == 210);
        REQUIRE(chip8.getRegisters()[15] == 0);
    }
}

TEST_CASE("Correct Instruction Decode", "") {
    TestInputHandler inputHandler{};
    SimpleDisplay display{};
    Chip8Test chip8{display, inputHandler};

    SECTION("Correct 0 Instructions") {
        // Fill all pixel white
        for (int i{}; i < WIDTH; i++) {
            for (int j{}; j < HEIGHT; j++) {
                display.drawPixel(i, j, true);
            }
        }

        // Check if all pixel are white
        for (int i{}; i < WIDTH; i++) {
            for (int j{}; j < HEIGHT; j++) {
                REQUIRE(display.getArrayDisplay()[i][j] == 1);
            }
        }

        chip8.decodeTest(0x00E0);

        // Check if all pixel are black (e.g. screen is cleared)
        for (int i{}; i < WIDTH; i++) {
            for (int j{}; j < HEIGHT; j++) {
                REQUIRE(display.getArrayDisplay()[i][j] == 0);
            }
        }
    }

    SECTION("Correct Subroutines Instructions") {
        // Namely 00EE and 2NNN

        chip8.decodeTest(0x2123);
        REQUIRE(chip8.getPC() == 0x0123);

        chip8.decodeTest(0x2696);
        REQUIRE(chip8.getPC() == 0x0696);

        chip8.decodeTest(0x00EE);
        REQUIRE(chip8.getPC() == 0x0123);

        chip8.decodeTest(0x00EE);
        REQUIRE(chip8.getPC() == 0x0200);
    }

    SECTION("Correct 1 Instructions") {
        chip8.decodeTest(0x1234);
        REQUIRE(chip8.getPC() == 0x0234);

        chip8.decodeTest(0x1696);
        REQUIRE(chip8.getPC() == 0x0696);
    }

    SECTION("Correct 3 Instructions") {
        uint16_t original{chip8.getPC()};

        chip8.decodeTest(0x3000);
        REQUIRE(chip8.getPC() == original + 2);

        chip8.decodeTest(0x3001);
        REQUIRE(chip8.getPC() == original + 2);
    }

    SECTION("Correct 4 Instruction") {
        uint16_t original{chip8.getPC()};

        chip8.decodeTest(0x4000);
        REQUIRE(chip8.getPC() == original);

        chip8.decodeTest(0x4001);
        REQUIRE(chip8.getPC() == original + 2);
    }

    SECTION("Correct 5 Instruction") {
        uint16_t original{chip8.getPC()};

        chip8.decodeTest(0x5010);
        REQUIRE(chip8.getPC() == original + 2);

        chip8.decodeTest(0x6101);

        chip8.decodeTest(0x5010);
        REQUIRE(chip8.getPC() == original + 2);
    }

    SECTION("Correct 6 Instruction") {
        for (int i{}; i < 16; i++) {
            chip8.decodeTest(0x6069 + 256 * i);
        }

        for (int i{}; i < 16; i++) {
            REQUIRE(chip8.getRegisters()[i] == 0x0069);
        }

        for (int i{}; i < 16; i++) {
            chip8.decodeTest(0x6096 + 256 * i);
        }

        for (int i{}; i < 16; i++) {
            REQUIRE(chip8.getRegisters()[i] == 0x0096);
        }
    }

    SECTION("Correct 7 Instruction") {
        for (int i{}; i < 16; i++) {
            chip8.decodeTest(0x7069 + 256 * i);
        }

        for (int i{}; i < 16; i++) {
            REQUIRE(chip8.getRegisters()[i] == 0x0069);
        }

        // Incrementing existing values
        for (int i{}; i < 16; i++) {
            chip8.decodeTest(0x7001 + 256 * i);
        }

        for (int i{}; i < 16; i++) {
            REQUIRE(chip8.getRegisters()[i] == 0x006A);
        }
    }

    SECTION("Correct 9 Instruction") {
        uint16_t original{chip8.getPC()};

        chip8.decodeTest(0x9010);
        REQUIRE(chip8.getPC() == original);

        chip8.decodeTest(0x6101);

        chip8.decodeTest(0x9010);
        REQUIRE(chip8.getPC() == original + 2);
    }

    SECTION("Correct A Instruction") {
        chip8.decodeTest(0xA123);
        REQUIRE(chip8.getIndex() == 0x0123);
    }

    SECTION("Correct B Instruction") {
        chip8.decodeTest(0xB123);
        REQUIRE(chip8.getPC() == 0x0123);
    }

    SECTION("Correct C Instruction") {
        chip8.decodeTest(0xC012);
        REQUIRE(chip8.getRegisters()[0] != 0x0012);
    }

    SECTION("Correct D Instruction") {
        // x = register 0; y = register 1



        // CASE 1: Wrapping and basic functionality

        // Point to 0 in font
        chip8.decodeTest(0xA050);

        // Set x = 64; y = 32
        // x will wrap down to 0; y will wrap down to 0
        chip8.decodeTest(0x6040);
        chip8.decodeTest(0x6120);

        // Draw 0 at x = 0; y = 0
        chip8.decodeTest(0xD015);


        // Checking 0 has been drawn correctly
        for (int i{}; i < 4; i++) {
            REQUIRE(display.getArrayDisplay()[i][0] == 1);
        }

        for (int i{1}; i < 4; i++) {
            REQUIRE(display.getArrayDisplay()[0][i] == 1);
            REQUIRE(display.getArrayDisplay()[1][i] == 0);
            REQUIRE(display.getArrayDisplay()[2][i] == 0);
            REQUIRE(display.getArrayDisplay()[3][i] == 1);
        }

        for (int i{}; i < 4; i++) {
            REQUIRE(display.getArrayDisplay()[i][4] == 1);
        }

        for (int i{0}; i < 4; i++) {
            for (int j{}; j < 5; j++) {
                REQUIRE(display.getArrayDisplay()[i + 4][j] == 0);
            }
        }



        // CASE 2: Clipping and incomplete sprite length

        // Set x = 62; y = 0;
        chip8.decodeTest(0x603E);

        // Draw 0 at x = 62; y = 0
        // Half of the 0 will be clipped and only 3 lines of 0 will be drawn
        chip8.decodeTest(0xD013);

        // Checking 0 has been drawn correctly
        REQUIRE(display.getArrayDisplay()[62][0] == 1);
        REQUIRE(display.getArrayDisplay()[63][0] == 1);

        REQUIRE(display.getArrayDisplay()[62][1] == 1);
        REQUIRE(display.getArrayDisplay()[63][1] == 0);

        REQUIRE(display.getArrayDisplay()[62][2] == 1);
        REQUIRE(display.getArrayDisplay()[63][2] == 0);

        REQUIRE(display.getArrayDisplay()[62][3] == 0);
        REQUIRE(display.getArrayDisplay()[63][3] == 0);

        REQUIRE(display.getArrayDisplay()[62][4] == 0);
        REQUIRE(display.getArrayDisplay()[63][4] == 0);



        // CASE 3: VF Flag is set and start 0 is flipped back to blank

        // Set x = 0; y = 0
        chip8.decodeTest(0x6000);
        chip8.decodeTest(0x6100);

        // Draw 0 at x = 0; y = 0
        chip8.decodeTest(0xD015);

        // Checking 0 is flipped back to blank
        for (int i{}; i < 8; i++) {
            for (int j{}; j < 5; j++) {
                REQUIRE(display.getArrayDisplay()[i][j] == 0);
            }
        }

        // Checking Flag is set
        REQUIRE(chip8.getRegisters()[15] == 1);
    }

    SECTION("Correct E Instruction") {
        chip8.decodeTest(0xE09E);
        REQUIRE(chip8.getPC() == 0x200);

        inputHandler.setKey(0);
        chip8.decodeTest(0xE09E);
        REQUIRE(chip8.getPC() == 0x202);

        chip8.decodeTest(0x6101);
        chip8.decodeTest(0xE191);
        REQUIRE(chip8.getPC() == 0x204);

        inputHandler.setKey(1);
        chip8.decodeTest(0xE191);
        REQUIRE(chip8.getPC() == 0x204);
    }

    SECTION("Correct F Instructions") {
        // FX15
        chip8.decodeTest(0x6023);
        chip8.decodeTest(0xF015);
        REQUIRE(chip8.getDelayTimer() == 0x23);

        // FX07
        chip8.decodeTest(0xF107);
        REQUIRE(chip8.getRegisters()[1] == 0x23);

        // FX18
        chip8.decodeTest(0xF018);
        REQUIRE(chip8.getSoundTimer() == 0x23);

        // FX1E
        chip8.decodeTest(0xF01E);
        REQUIRE(chip8.getIndex() == 0x23);

        // FX0A Not Tested

        // FX29
        chip8.decodeTest(0x6F0F);
        chip8.decodeTest(0xFF29);
        REQUIRE(chip8.getIndex() == 0x9B);

        // FX33
        chip8.decodeTest(0x61FF);
        chip8.decodeTest(0xF133);
        REQUIRE(chip8.getMemory()[chip8.getIndex()] == 2);
        REQUIRE(chip8.getMemory()[chip8.getIndex() + 1] == 5);
        REQUIRE(chip8.getMemory()[chip8.getIndex() + 2] == 5);

        // FX55
        // Reset Index and seed values in registers
        chip8.decodeTest(0xA000);
        chip8.decodeTest(0x6002);
        chip8.decodeTest(0x6105);
        chip8.decodeTest(0x6205);

        chip8.decodeTest(0xF255);
        REQUIRE(chip8.getMemory()[chip8.getIndex() - 1] == 5);
        REQUIRE(chip8.getMemory()[chip8.getIndex() - 2] == 5);
        REQUIRE(chip8.getMemory()[chip8.getIndex() - 3] == 2);
        REQUIRE(chip8.getIndex() == 3);

        // FX65
        // Reset Index and replace values in registers
        chip8.decodeTest(0xA000);
        chip8.decodeTest(0x6023);
        chip8.decodeTest(0x6134);
        chip8.decodeTest(0x6245);
        chip8.decodeTest(0xF265);

        REQUIRE(chip8.getRegisters()[0] == 2);
        REQUIRE(chip8.getRegisters()[1] == 5);
        REQUIRE(chip8.getRegisters()[2] == 5);
        REQUIRE(chip8.getIndex() == 3);
    }
}