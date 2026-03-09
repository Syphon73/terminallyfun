#include <iostream>
#include "chip8.h"
#include <unistd.h>
#include "raylib.h"
#include <cstdint>
#include <fstream>
#include <cstdlib>
#include <array>

//const unsigned int START_ADDRESS = 0x200;
const unsigned int FONT_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;
const unsigned int CHIP8_NUM_KEYS = 16;

//static const unsigned int FONTSET_SIZE = 80;
uint8_t fontset[80] =
{
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

// define the constructor
Chip8::Chip8() {
  // Initialize PC = Start of ROM
  pc = START_ADDRESS;
  opcode = 0;
  index = 0;
  sp = 0; 
  
  drawflag = false;

  memset(memory, 0, sizeof(memory));
  memset(registers, 0, sizeof(registers));
  memset(stack, 0, sizeof(stack));
  memset(video, 0, sizeof(video));
  memset(keypad, 0, sizeof(keypad));
  memset(stack, 0, sizeof(stack));

  for (int i = 0; i < FONTSET_SIZE; ++i) {
      memory[i] = fontset[i];
  }
       
};

// Layout:
//   1 2 3 C    →  1 2 3 4
//   4 5 6 D    →  Q W E R
//   7 8 9 E    →  A S D F
//   A 0 B F    →  Z X C V
uint8_t keymap[CHIP8_NUM_KEYS] = {
    KEY_X,     // 0x0 → X
    KEY_ONE,   // 0x1 → 1
    KEY_TWO,   // 0x2 → 2
    KEY_THREE, // 0x3 → 3
    KEY_Q,     // 0x4 → Q
    KEY_W,     // 0x5 → W
    KEY_E,     // 0x6 → E
    KEY_A,     // 0x7 → A
    KEY_S,     // 0x8 → S
    KEY_D,     // 0x9 → D
    KEY_Z,     // 0xA → Z
    KEY_C,     // 0xB → C
    KEY_FOUR,  // 0xC → 4
    KEY_R,     // 0xD → R
    KEY_F,     // 0xE → F
    KEY_V,     // 0xF → V
};

//for detecting FX0A release key 
std::array<uint8_t, CHIP8_NUM_KEYS> prev_keypad = {};
int get_released_key(uint8_t keypad[16]) {
    for (int i = 0; i < CHIP8_NUM_KEYS; i++) {
        if (prev_keypad[i] == 1 && keypad[i] == 0) {
            return i;
        }
    }
    return -1;
}

void Chip8::LoadROM(char const* filename)
{
	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
    if(size>3584){
      std::cout<<"ROM too large"<<std::endl;
      return;
    }
    else{
      char* buffer = new char[size];

      // Go back to the beginning of the file and fill the buffer
      file.seekg(0, std::ios::beg);
    	file.read(buffer, size);
      file.close();

      // Load the ROM contents into the Chip8's memory, starting at 0x200
      for (long i = 0; i < size; ++i)
      {
        memory[START_ADDRESS + i] = buffer[i];
      }

      //Load fonts into the memory
      for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
      {
        memory[FONT_START_ADDRESS + i] = fontset[i];
      }

      //print the buffer

      // std::cout<<"printing the buffer: "<<std::endl;
      // for(long i = 0; i < size; ++i){
      //
      //   std::cout<<buffer[i]<<std::endl;
      //
      // }
      // // Free the buffer
      delete[] buffer;

    }
	}
};



void Chip8::graphic(uint8_t x, uint8_t y, uint8_t height){
  // TODO: read sprite from memory
  uint8_t pixel;
  //drawing()
  int targetX = x % 64;
  int targetY = y % 32;

  
  for (int yline = 0; yline < height; yline++)
  {
      int sy = y + yline;
      pixel = memory[index + yline];

      if (sy >= 32){
        break; //vertical clipping
      }
      for (int xline = 0; xline < 8; xline++)
      {
          int sx = x + xline;
          if (sx >= 64) {
            break; //horiozontal clip
          }

          if ((pixel & (0x80 >> xline)) != 0)
          {
             int idx = sx + (sy * 64);
            // COLLISION DETECTION
            if (video[idx] == 1)
            {
                registers[0xF] = 1;
            }
            // 1 ^ 1 = 0 (Erase)
            // 0 ^ 1 = 1 (Draw)
            video[idx] ^= 1;

            
          }
       }
  }
};

void Chip8::keyboard_poll(uint8_t keypad[16]) {
    for (int i = 0; i < CHIP8_NUM_KEYS; i++) {
        prev_keypad[i] = keypad[i];
        keypad[i] = IsKeyDown(keymap[i]) ? 1 : 0;
        if (keypad[i]) {
            printf("KEY PRESSED: chip8 key %X\n", i);
        }
    }
};


void Chip8::gameLoop()
{

  //FETCH DECODE EXECUTE
  //sd::cout<<"game loop running"<<std::endl;
 
  //get the next opcode from ram
  uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
  //pre-increment pc here instead of doing manually for each opcode
  //printf("Address: (pc=0x%03X) opcode: (0x%04X)\n",pc,opcode);

  pc += 2;


  uint16_t NNN = opcode & 0x0FFF;
  uint8_t  NN  = opcode & 0x00FF;
  uint8_t  N   = opcode & 0x000F;
  uint8_t  X   = (opcode >> 8) & 0x0F;
  uint8_t  Y   = (opcode >> 4) & 0x0F;

  switch ((opcode >> 12) & 0x0F)
  {
    case 0x00:
    {
        if (NN == 0xE0) {
            // 0x00E0 Clear the screen
            printf("00E0: Clear screen (pc=0x%03X)\n", pc);
            memset(video, 0, sizeof(video));
            drawflag = true;
        } else if (NN == 0xEE) {
            // 0x00EE Return from subroutine
            printf("00EE: Return from subroutine (sp=%d, pc=0x%03X)\n", sp, stack[sp - 1]);
            sp--;
            pc = stack[sp];
        }
    }
    break;

    case 0x01:
        // 0x1NNN: Jump to address NNN
        //printf("1NNN: Jump to 0x%03X (pc=0x%03X)\n", NNN, pc);
        pc = NNN;
        break;

    case 0x02:
        // 0x2NNN: Call subroutine at NNN
        printf("2NNN: Call subroutine at 0x%03X (pc=0x%03X, sp=%d)\n", NNN, pc, sp);
        stack[sp] = pc;
        sp++;
        pc = NNN;
        break;
    
    case 0x03:
        printf("3XNN: Skip if V%X(%d) == %d (pc=0x%03X)\n", X, registers[X], NN, pc);
        if (registers[X] == NN){
          pc += 2;
        }
        break;

    case 0x04:
        printf("4XNN: Skip if V%X(%d) != %d (pc=0x%03X)\n", X, registers[X], NN, pc);
        if(registers[X] != NN){
          pc += 2;
        }
        break;
    
    case 0x05:
        printf("5XY0: Skip if V%X(%d) == V%X(%d) (pc=0x%03X)\n", X, registers[X], Y, registers[Y], pc);
        if(registers[X] == registers[Y]){
          pc += 2;
        }
        break;

    case 0x06: // SET REGISTER
        {
            printf("6XNN: V%X = %d (pc=0x%03X)\n", X, NN, pc);
            registers[X] = NN;
        }
        break;

    case 0x07:
        printf("7XNN: V%X += %d (V%X was %d) (pc=0x%03X)\n", X, NN, X, registers[X], pc);
        registers[X] += NN;
        break;

    case 0x08:
        if(N == 0x0){
          printf("8XY0: V%X = V%X(%d) (pc=0x%03X)\n", X, Y, registers[Y], pc);
          registers[X] = registers[Y];
        }
        else if(N == 0x1){
          printf("8XY1: V%X |= V%X (V%X=%d, V%X=%d) (pc=0x%03X)\n", X, Y, X, registers[X], Y, registers[Y], pc);
          registers[X] |= registers[Y];
        }
        else if(N == 0x2){
          printf("8XY2: V%X &= V%X (V%X=%d, V%X=%d) (pc=0x%03X)\n", X, Y, X, registers[X], Y, registers[Y], pc);
          registers[X] &= registers[Y];
        }
        else if(N == 0x3){
          printf("8XY3: V%X ^= V%X (V%X=%d, V%X=%d) (pc=0x%03X)\n", X, Y, X, registers[X], Y, registers[Y], pc);
          registers[X] ^= registers[Y];
        }
        else if(N == 0x4){
          printf("8XY4: V%X += V%X (V%X=%d, V%X=%d) (pc=0x%03X)\n", X, Y, X, registers[X], Y, registers[Y], pc);
          uint16_t sum = registers[X] + registers[Y];
          if(sum > 255){
            //carry bit Vf
            registers[0xF] = 1;
            registers[X] = sum & 0xFF;
          }
          else{
            registers[0xF] = 0;
            registers[X] = sum & 0xFF;
          }
        }
        else if(N == 0x5){
          printf("8XY5: V%X -= V%X (V%X=%d, V%X=%d) (pc=0x%03X)\n", X, Y, X, registers[X], Y, registers[Y], pc);
          uint8_t vf = 0;
          if(registers[X] >= registers[Y]){
            vf = 1;
            registers[0xF] = vf;
            registers[X] = registers[X] - registers[Y];
          }
          else{
            registers[0xF] = vf;
            registers[X] = registers[X] - registers[Y];

          }
        }
        else if (N == 0x6) {
            printf("8XY6: V%X >>= 1 (V%X=%d) (pc=0x%03X)\n", X, X, registers[X], pc);
            uint8_t vf = registers[X] & 0x1;
            registers[X] >>= 1;
            registers[0xF] = vf;
        }
        else if (N == 0x7) {
            printf("8XY7: V%X = V%X - V%X (V%X=%d, V%X=%d) (pc=0x%03X)\n", X, Y, X, Y, registers[Y], X, registers[X], pc);
            uint8_t vf = (registers[Y] >= registers[X]) ? 1 : 0;
            registers[X] = registers[Y] - registers[X];
            registers[0xF] = vf;
        }
        else if (N == 0xE) {
            printf("8XYE: V%X <<= 1 (V%X=%d) (pc=0x%03X)\n", X, X, registers[X], pc);
            uint8_t vf = (registers[X] >> 7) & 0x1;
            registers[X] <<= 1;
            registers[0xF] = vf;
        }
        else {
            printf("Unknown 0x8 sub-opcode: %04X\n", opcode);
        }

      break;
        
    
    case 0x09:
      printf("9XY0: Skip if V%X(%d) != V%X(%d) (pc=0x%03X)\n", X, registers[X], Y, registers[Y], pc);
      if(registers[X] != registers[Y]){
        pc += 2;
      }
      break;

    case 0x0A: // SET INDEX
        printf("ANNN: I = 0x%03X (pc=0x%03X)\n", NNN, pc);
        index = NNN;
        break;

    case 0x0B:
        printf("BNNN: Jump to 0x%03X + V0(%d) = 0x%03X (pc=0x%03X)\n", NNN, registers[0x0], NNN + registers[0x0], pc);
        pc = NNN + registers[0x0];
        break;

    case 0x0C:
        printf("CXNN: V%X = rand() & %d (pc=0x%03X)\n", X, NN, pc);
        //add a random variable to Vx
        registers[X] = (rand() % 256) & NN;
        break;

    case 0x0D: // DRAW
        {
  
            uint8_t x = registers[X];
            uint8_t y = registers[Y];
            uint8_t height = N;

            printf("DXYN: Draw at V%X(%d), V%X(%d), h=%d, I=0x%03X (pc=0x%03X)\n", X, x, Y, y, height, index, pc);

            //reset collision flag (VF) to handle physics
            registers[0xF] = 0;

            graphic(x,y,height);
            
            drawflag = true;

        }
        break;

    case 0x0E:
        if (NN == 0x9E) {
            // 0xEX9E: Skip if key VX is pressed
            printf("EX9E: Skip if key V%X(%d) pressed (keypad[%d]=%d) (pc=0x%03X)\n", X, registers[X], registers[X], keypad[registers[X]], pc);
            if (keypad[registers[X]] == 1) {
                pc += 2;
            }
        }
        else if (NN == 0xA1) {
            // 0xEXA1: Skip if key VX is NOT pressed
            printf("EXA1: Skip if key V%X(%d) NOT pressed (keypad[%d]=%d) (pc=0x%03X)\n", X, registers[X], registers[X], keypad[registers[X]], pc);
            if (keypad[registers[X]] != 1) {
                pc += 2;
            }
        }
        else {
            printf("Unknown 0xE sub-opcode: %04X\n", opcode);
        }
      break;


    case 0x0F:
      {
          if (NN == 0x07) {
              // 0xFX07: VX = delay timer
              printf("FX07: V%X = delayTimer(%d) (pc=0x%03X)\n", X, delayTimer, pc);
              registers[X] = delayTimer;
          }
          else if (NN == 0x0A) {
              // 0xFX0A: Wait for key press, store in VX
              printf("FX0A: Waiting for key release (pc=0x%03X)\n", pc);
              // If no key is pressed, undo the pc increment to stay on this instruction
              int key_pressed = get_released_key(keypad);
              for (uint8_t i = 0; i < 16; i++) {
                  if (key_pressed == 1) {
                      printf("FX0A: Key %d released, stored in V%X (pc=0x%03X)\n", key_pressed, X, pc);
                      registers[X] = key_pressed;
                      break;
                  }
                  else{
                    pc -= 2;
                  }
              }
              
          }
          else if (NN == 0x15) {
              // 0xFX15: Set delay timer = VX
              printf("FX15: delayTimer = V%X(%d) (pc=0x%03X)\n", X, registers[X], pc);
              delayTimer = registers[X];
          }
          else if (NN == 0x18) {
              // 0xFX18: Set sound timer = VX
              printf("FX18: soundTimer = V%X(%d) (pc=0x%03X)\n", X, registers[X], pc);
              soundTimer = registers[X];
          }
          else if (NN == 0x1E) {
              // 0xFX1E: I += VX
              printf("FX1E: I += V%X(%d) (I was 0x%03X, now 0x%03X) (pc=0x%03X)\n", X, registers[X], index, index + registers[X], pc);
              index += registers[X];
          }
          else if (NN == 0x29) {
              // 0xFX29: I = address of font sprite for digit VX
              printf("FX29: I = font[V%X(%d)] = 0x%03X (pc=0x%03X)\n", X, registers[X], registers[X] * 5, pc);
              // Each font sprite is 5 bytes, fonts start at 0x000 in memory
              index = registers[X] * 5;
          }
          else if (NN == 0x33) {
              // 0xFX33: Store BCD of VX at I, I+1, I+2
              printf("FX33: BCD of V%X(%d) -> mem[0x%03X] (pc=0x%03X)\n", X, registers[X], index, pc);
              uint8_t value = registers[X];
              memory[index + 2] = value % 10;         // ones
              value /= 10;
              memory[index + 1] = value % 10;         // tens
              value /= 10;
              memory[index]     = value % 10;         // hundreds
          }
          else if (NN == 0x55) {
              // 0xFX55: Store V0 through VX into memory starting at I
              printf("FX55: Store V0-V%X to mem[0x%03X] (pc=0x%03X)\n", X, index, pc);
              for (uint8_t i = 0; i <= X; i++) {
                  memory[index + i] = registers[i];
              }
          }
          else if (NN == 0x65) {
              // 0xFX65: Load V0 through VX from memory starting at I
              printf("FX65: Load V0-V%X from mem[0x%03X] (pc=0x%03X)\n", X, index, pc);
              for (uint8_t i = 0; i <= X; i++) {
                  registers[i] = memory[index + i];
              }
          }
          else {
              printf("Unknown 0xF sub-opcode: %04X\n", opcode);
          }
      }
      break;

    default:
        printf("Unknown Opcode: %04X (pc=0x%03X)\n", opcode, pc);
  }

}
