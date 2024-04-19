#include <cstdio>
#include <iostream>
#include <thread>
#include <fstream>
#include <chrono>
 #include "schip.h"
#include "../constants.h"

#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

// Main
void SChip::run(std::string& filename, bool& stopSignal) {
    if (!fetch(filename)) {
        return;
    }

    while (!stopSignal) {
        auto start {std::chrono::high_resolution_clock::now()};

        if (delay_timer > 0) {
            delay_timer--;
        }

        if (sound_timer > 0) {
            sound_timer--;
        }

        // Limited by 60 sprite per second
        int count{};
        while (true) {
            uint16_t ins = (memory[program_counter] << 8) + memory[program_counter + 1];
            // std::cout << "Instruction: " << std::hex << ins << "   ";
            program_counter += 2;

            if (!decode(ins)) {
                printf("Instruction: %d. Program Counter: %d.\n", ins, program_counter - 2);
                return;
            }

            // Comment these out before running tests
            if (sound_timer > 0) {
                SDL_PauseAudio(0);
            } else {
                SDL_PauseAudio(1);
            }

            // DXYN waits for the next vertical interrupt
            if (count > 16.66 / 1000.0 * INSTRUCTION_PER_SECOND) {
                break;
            }

            count++;
        }

        display.updateWindowSurface();

        using namespace std::chrono_literals;

        // Sleep 1.2ms less to account for thread waking up later
        std::this_thread::sleep_until(start + 15.6ms);
    }
}


bool SChip::fetch(std::string& filename) const {
    std::ifstream input;
    input.open(filename, std::fstream::binary);

    if (!input.is_open()) {
        printf("Error: Specified input file not found\n");
        return false;
    }

    int i{0x200};
    while (!input.eof()) {
        uint8_t byte;
        input >> std::noskipws >> byte;
        memory[i] = byte;
        i++;
    }

    return true;
}


// Memory
// SCHIP flags size is limited to 8 since only registers 0 to 7 will be addressed
SChip::SChip(AdvancedDisplay& display, InputHandler& inputHandler)
        : program_counter{0x200}, index_register{}, stack{}, registers(16, 0), flags(8, 0),
          display{display}, inputHandler{inputHandler}, sound_timer{}, delay_timer{}
{
    memory = new uint8_t[RAM_SIZE]();

    // Loading up random number generator
    std::random_device r;
    std::seed_seq seed{ r(), r(), r(), r(), r(), r(), r() };
    engine.seed(seed);

    loadFont();
}

SChip::~SChip() {
    delete[] memory;
}

void SChip::loadFont() const {
    for (int i{0x50}; i <= 0x9F; i++) {
        memory[i] = FONT[i - 0x50];
    }

    for (int i{0xA0}; i < 0x140; i++) {
        memory[i] = SCHIP_FONT[i - 0xA0];
    }
}


// Processor Logic
bool SChip::decode(uint16_t ins) {
    switch (ins >> 12) {
        case 0x0:
            // 0NNN instruction is ignored
            if ((ins >> 4) == 0xC) {
                display.scrollDown(ins % 0x10);
                break;
            }

            switch (ins) {
                case 0x00E0:
                    DEBUG_MSG("Clear Screen");
                    display.clearScreen();
                    break;
                case 0x00EE:
                    program_counter = stack.top();
                    stack.pop();
                    DEBUG_MSG("Return from function. Program Counter: " << std::hex << program_counter);
                    break;
                case 0x00FB:
                    display.scrollRight(4);
                    break;
                case 0x00FC:
                    display.scrollLeft(4);
                    break;
                case 0x00FD:
                    return false;
                case 0x00FE:
                    DEBUG_MSG("Lores Mode");
                    display.switchOperationalMode(false);
                    break;
                case 0x00FF:
                    DEBUG_MSG("Hires Mode");
                    display.switchOperationalMode(true);
                    break;
                default:
                    goto DEFAULT;
            }
            break;
        case 0x1:
            program_counter = ins % 0x1000;
            DEBUG_MSG("Jump to " << std::hex << program_counter);
            break;
        case 0x2:
            stack.push(program_counter);
            program_counter = ins % 0x2000;
            DEBUG_MSG("Begin Function. Program Counter: " << std::hex << program_counter);
            break;
        case 0x3:
            DEBUG_MSG("CHECK if Register[" << ((ins % 0x3000) >> 8) << "]: " << +registers[(ins % 0x3000) >> 8] << " == " << ins % 0x100);
            if (registers[(ins % 0x3000) >> 8] == ins % 0x100) {
                program_counter += 2;
            }
            break;
        case 0x4:
            DEBUG_MSG("CHECK if Register[" << ((ins % 0x4000) >> 8) << "]: " << +registers[(ins % 0x4000) >> 8] << " != " << ins % 0x100);
            if (registers[(ins % 0x4000) >> 8] != ins % 0x100) {
                program_counter += 2;
            }
            break;
        case 0x5:
            DEBUG_MSG("Check if Register[" << ((ins % 0x5000) >> 8) << "]: " << +registers[(ins % 0x5000) >> 8] << "== Register[" << ((ins % 0x0100) >> 4) << "]: " << +registers[(ins % 0x0100) >> 4]);
            if (registers[(ins % 0x5000) >> 8] == registers[(ins % 0x0100) >> 4]) {
                program_counter += 2;
            }
            break;
        case 0x6:
            DEBUG_MSG("Set Register[" << ((ins % 0x6000) >> 8) << "] to " << std::hex << ins % 0x100);
            registers[(ins % 0x6000) >> 8] = ins % 0x100;
            break;
        case 0x7:
            DEBUG_MSG("Add to Register[" << ((ins % 0x7000) >> 8) << "] value " << std::hex << ins % 0x100);
            registers[(ins % 0x7000) >> 8] += (ins % 0x100);
            break;
        case 0x8: {
            int x{ (ins % 0x8000) >> 8 };
            int y{ (ins % 0x100) >> 4 };

            switch (ins % 0x10) {
                case 0:
                    DEBUG_MSG("Set Register[" << x << "]: " << +registers[x] << " to Register[" << y << "]: " << +registers[y]);
                    registers[x] = registers[y];
                    break;
                case 1:
                    DEBUG_MSG("OR Register[" << x << "]: " << +registers[x] << " with Register[" << y << "]: " << +registers[y]);
                    registers[x] |= registers[y];
                    break;
                case 2:
                    DEBUG_MSG("AND Register[" << x << "]: " << +registers[x] << " with Register[" << y << "]: " << +registers[y]);
                    registers[x] &= registers[y];
                    break;
                case 3:
                    DEBUG_MSG("XOR Register[" << x << "]: " << +registers[x] << " with Register[" << y << "]: " << +registers[y]);
                    registers[x] ^= registers[y];
                    break;
                case 4: {
                    DEBUG_MSG("ADD Register[" << x << "]: " << +registers[x] << " with Register[" << y << "]: " << +registers[y]);
                    uint8_t flag = (registers[x] + registers[y]) > 255 ? 1 : 0;
                    registers[x] += registers[y];
                    registers[15] = flag;
                    break;
                }
                case 5: {
                    DEBUG_MSG("SUBTRACT Register[" << x << "]: " << +registers[x] << " with Register[" << y << "]: " << +registers[y]);
                    uint8_t flag = registers[x] >= registers[y] ? 1 : 0;
                    registers[x] -= registers[y];
                    registers[15] = flag;
                    break;
                }
                case 7: {
                    DEBUG_MSG("SUBTRACT REVERSE Register[" << x << "]: " << +registers[x] << " with Register[" << y << "]: " << +registers[y]);
                    uint8_t flag = registers[y] >= registers[x] ? 1 : 0;
                    registers[x] = registers[y] - registers[x];
                    registers[15] = flag;
                    break;
                }
                case 6: {
                    DEBUG_MSG("SHIFT RIGHT Register[" << x << "]: " << +registers[x] << " with Register[" << y << "]: " << +registers[y]);
                    uint8_t flag = registers[x] % 2 ? 1 : 0;
                    registers[x] >>= 1;
                    registers[15] = flag;

                    break;
                }
                case 0xE: {
                    DEBUG_MSG("SHIFT LEFT Register[" << x << "]: " << +registers[x] << " with Register[" << y << "]: " << +registers[y]);
                    uint8_t flag = registers[x] >= 0x80 ? 1 : 0; // Is leftmost bit 1
                    registers[x] <<= 1;
                    registers[15] = flag;

                    break;
                }
                default:
                    goto DEFAULT;
            }
            break;
        }
        case 0x9:
            DEBUG_MSG("Check if Register[" << ((ins % 0x9000) >> 8) << "]: " << +registers[(ins % 0x9000) >> 8] << "!= Register[" << ((ins % 0x0100) >> 4) << "]: " << +registers[(ins % 0x0100) >> 4]);
            if (registers[(ins % 0x9000) >> 8] != registers[(ins % 0x0100) >> 4]) {
                program_counter += 2;
            }
            break;
        case 0xA:
            DEBUG_MSG("Set Index Register to " << std::hex << ins % 0xA000);
            index_register = ins % 0xA000;
            break;
        case 0xB:
            // TODO: Make a toggle for the other behaviour
            DEBUG_MSG("Jump with offset " << std::hex << (ins % 0xB000) + registers[(ins % 0xB000) >> 8]);
//            DEBUG_MSG("Register [" << ((ins % 0xB000) >> 8) << "]: " << +registers[(ins % 0xB000) >> 8]);
//            if (ins == 0xBEA9) {
//                program_counter = (ins % 0xB000) + 0;
//                break;
//            }
//            program_counter = (ins % 0xB000) + registers[(ins % 0xB000) >> 8];
            program_counter = (ins % 0xB000) + registers[0];
            break;
        case 0xC:
            DEBUG_MSG("Random Register[" << ((ins % 0xC000) >> 8) << "]: " << +registers[(ins % 0xC000) >> 8]);
            registers[(ins % 0xC000) >> 8] = dist(engine) & (ins % 0x100);
            break;
        case 0xD: {
            uint8_t origin_y = registers[(ins % 0x100) >> 4];
            uint8_t origin_x = registers[(ins % 0xD000) >> 8];

            // If no pixel are flipped, this value will remain to be 0
            registers[15] = 0;

            // DXY0 Instruction
            if (ins % 0x10 == 0) {
                for (uint8_t line{}; line < 16; line++) {
                    // Modulo to wrap position
                    uint8_t y = (origin_y + line) % display.getHeight();
                    uint8_t x = origin_x % display.getWidth();

                    std::vector<bool> v(16);
                    decodeBigSprite(index_register + line * 2, v);

                    DEBUG_MSG("Display At X: " << +x << " , Y: " << +y << " " << spriteToString(v));

                    int i{};
                    while (i < 16) {
                        if (v[i])
                            if (display.flipPixel(x, y))
                                registers[15] = 1;

                        x++;
                        i++;
                        x = x % display.getWidth();
                    }

                    if (y >= display.getHeight() - 1)
                        break;
                }
            } else { // DXYN Instruction
                for (uint8_t line{}; line < (uint8_t) ins % 0x10; line++) {
                    // Modulo to wrap position
                    uint8_t y = (origin_y + line) % display.getHeight();
                    uint8_t x = origin_x % display.getWidth();

                    std::vector<bool> v(8);
                    decodeSmallSprite(index_register + line, v);

                    DEBUG_MSG("Display At X: " << +x << " , Y: " << +y << " " << spriteToString(v));

                    int i{};
                    while (i < 8 && x < display.getWidth()) {
                        if (v[i])
                            if (display.flipPixel(x, y))
                                registers[15] = 1;

                        x++;
                        i++;
                    }

                    if (y >= display.getHeight() - 1)
                        break;
                }
            }

            break;
        }
        case 0xE:
            if (ins % 0x10 == 0xE) {
                if (inputHandler.isKeyPressed(registers[(ins % 0xE000) >> 8])) {
                    program_counter += 2;
                }
            } else if (ins % 0x10 == 0x1) {
                if (!inputHandler.isKeyPressed(registers[(ins % 0xE000) >> 8]))
                    program_counter += 2;
            } else {
                goto DEFAULT;
            }
            break;
        case 0xF: {
            uint8_t x = (ins % 0xF000) >> 8;
            switch ((ins % 0x100)) {
                case 0x07:
                    DEBUG_MSG("Set Register[" << x << "] " << +registers[x] << " to Delay Timer: " << +delay_timer);
                    registers[x] = delay_timer;
                    break;
                case 0x15:
                    DEBUG_MSG("Set Delay Timer: " << +delay_timer << " to Register[" << x << "]: " << +registers[x]);
                    delay_timer = registers[x];
                    break;
                case 0x18:
                    DEBUG_MSG("Set Sound Timer: " << +sound_timer << " to Register[" << x << "]: " << +registers[x]);
                    sound_timer = registers[x];
                    break;
                case 0x1E:
                    // Behaviour with carry bit when overflowed
                    DEBUG_MSG("Increment Index Register by " << std::hex << +registers[x]);
                    index_register += registers[x];
                    if (index_register >= 4096) {
                        registers[15] = 1;
                        index_register -= 4096;
                    }
                    break;
                case 0x0A: {
                    // Key is registered on KEYDOWN instead of after KEYUP on original COSMAC VIP
                    int key{ inputHandler.getKeyBeingPressed() };
                    if (key == -1) {
                        program_counter -= 2;
                    } else {
                        registers[x] = key;
                        while (true)
                            if (inputHandler.getKeyBeingPressed() != key)
                                break;
                    }
                    break;
                }
                case 0x29:
                    DEBUG_MSG("Point Index Register to " << std::hex << registers[x] % 0x10);
                    index_register = (registers[x] % 0x10) * 5 + 0x50;
                    break;
                case 0x30:
                    DEBUG_MSG("Point Index Register to LARGE" << std::hex << registers[x] % 0x10);
                    index_register = (registers[x] % 0x10) * 10 + 0xA0;
                    break;
                case 0x33:
                    DEBUG_MSG("Decode To Decimal: " << +registers[x]);
                    memory[index_register] = registers[x] / 100;
                    memory[index_register + 1] = (registers[x] % 100) / 10;
                    memory[index_register + 2] = registers[x] % 10;
                    break;
                case 0x55:
                    DEBUG_MSG("Load Registers from 0 to " << +x);
                    for (uint8_t i{}; i <= x; i++) {
                        memory[index_register + i] = registers[i];
                    }
                    break;
                case 0x65:
                    DEBUG_MSG("Store Registers from 0 to " << +x);
                    for (uint8_t i{}; i <= x; i++) {
                        registers[i] = memory[index_register + i];
                    }
                    break;
                case 0x75:
                    if (x > 7)
                        goto DEFAULT;

                    DEBUG_MSG("Store Flags from 0 to " << +x);
                    for (uint8_t i{}; i <= x; i++) {
                        flags[i] = registers[i];
                    }
                    break;
                case 0x85:
                    if (x > 7)
                        goto DEFAULT;

                    DEBUG_MSG("Load Flags from 0 to " << +x);
                    for (uint8_t i{}; i <= x; i++)
                        registers[i] = flags[i];

                    break;
                default:
                    goto DEFAULT;
            }
            break;
        }
        DEFAULT:
        default:
            DEBUG_MSG("Instruction set decode error");
            return false;
    }
    fflush(stdout);
    return true;
}


// Helper
void SChip::decodeSmallSprite(uint16_t position, std::vector<bool>& v) const {
    uint8_t sprite{memory[position]};
    for (int i{}; i < 8; i++) {
        v[7 - i] = sprite % 2 == 1;
        sprite /= 2;
    }
}

void SChip::decodeBigSprite(uint16_t position, std::vector<bool>& v) const {
    uint16_t sprite{ static_cast<uint16_t >(memory[position + 1] + memory[position] * 256) };
    for (int i{}; i < 16; i++) {
        v[15 - i] = sprite % 2 == 1;
        sprite /= 2;
    }
}

std::string SChip::spriteToString(std::vector<bool>& v) {
    std::string s{ "[" };
    for (bool b : v) {
        b ? s.append(" 1") : s.append(" 0");
    }
    s.append("]");
    return s;
}