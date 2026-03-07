#include <iostream>
#include "chip8.h"
#include <unistd.h>
#include <cstdint>
#include <fstream>
#include <cstdlib>

//const unsigned int START_ADDRESS = 0x200;
const unsigned int FONT_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;


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
}

// bool Chip8::refreshScreen(){
//
// }

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
}


void Chip8::gameLoop()
{

  //FETCH DECODE EXECUTE
  //sd::cout<<"game loop running"<<std::endl;
 
  //get the next opcode from ram
  uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
  //pre-increment pc here instead of doing manually for each opcode
  printf("Address: (pc=0x%03X) opcode: (0x%04X)\n",pc,opcode);

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
            memset(video, 0, sizeof(video));
            drawflag = true;
        } else if (NN == 0xEE) {
            // 0x00EE Return from subroutine
            sp--;
            pc = stack[sp];
        }
    }
    break;

    case 0x01:
        // 0x1NNN: Jump to address NNN
        pc = NNN;
        break;

    case 0x02:
        // 0x2NNN: Call subroutine at NNN
        stack[sp] = pc;
        sp++;
        pc = NNN;
        break;
    
    case 0x03:
        if (registers[X] == NN){
          pc += 2;
        }
        break;

    case 0x04:
        if(registers[X] != NN){
          pc += 2;
        }
        break;
    
    case 0x05:
        if(registers[X] == registers[Y]){
          pc += 2;
        }
        break;

    case 0x06: // SET REGISTER
        {
            registers[X] = NN;
            //printf("Opcode 6XNN: Set V%X to %d\n", x, nn);
            printf("6XNN: V%X = %d (pc=0x%03X)\n", X, NN, pc);
        }
        break;

    case 0x07:
        registers[X] += NN;
        break;

    case 0x08:
        if(N == 0x0){
          registers[X] = registers[Y];
        }
        else if(N == 0x1){
          registers[X] |= registers[Y];
        }
        else if(N == 0x2){
          registers[X] &= registers[Y];
        }
        else if(N == 0x3){
          registers[X] ^= registers[Y];
        }
        else if(N == 0x4){
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
            uint8_t vf = registers[X] & 0x1;
            registers[X] >>= 1;
            registers[0xF] = vf;
        }
        else if (N == 0x7) {
            uint8_t vf = (registers[Y] >= registers[X]) ? 1 : 0;
            registers[X] = registers[Y] - registers[X];
            registers[0xF] = vf;
        }
        else if (N == 0xE) {
            uint8_t vf = (registers[X] >> 7) & 0x1;
            registers[X] <<= 1;
            registers[0xF] = vf;
        }
        else {
            printf("Unknown 0x8 sub-opcode: %04X\n", opcode);
        }

      break;
        
    
    case 0x09:
      if(registers[X] != registers[Y]){
        pc += 2;
      }
      break;

    case 0x0A: // SET INDEX
        index = NNN;
        //printf("Opcode ANNN: Set I to %03X\n", index);
        printf("ANNN: I = 0x%03X (pc=0x%03X)\n", index, pc);
        break;

    case 0x0B:
        pc = NNN + registers[0x0];
        break;
    case 0x0C:
        //add a random variable to Vx
        registers[X] = (rand() % 256) & NN;
        break;

    case 0x0D: // DRAW
        {
  
            uint8_t x = registers[X];
            uint8_t y = registers[Y];
            uint8_t height = N;

            printf("DRAW: V[%d]=%d, V[%d]=%d, h=%d, I=0x%03X\n",X, x,Y,y,height, index);

            //reset collision flag (VF) to handle physics
            registers[0xF] = 0;

            graphic(x,y,height);
            
            drawflag = true;

        }
        break;

    case 0x0E:
        if (NN == 0x9E) {
            // 0xEX9E: Skip if key VX is pressed
            if (keypad[registers[X]] == 1) {
                pc += 2;
            }
        }
        else if (NN == 0xA1) {
            // 0xEXA1: Skip if key VX is NOT pressed
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
              registers[X] = delayTimer;
          }
          else if (NN == 0x0A) {
              // 0xFX0A: Wait for key press, store in VX
              // If no key is pressed, undo the pc increment to stay on this instruction
              bool key_pressed = false;
              for (uint8_t i = 0; i < 16; i++) {
                  if (keypad[i] == 1) {
                      registers[X] = i;
                      key_pressed = true;
                      break;
                  }
              }
              if (!key_pressed) {
                  pc -= 2;  // re-run this instruction next cycle
              }
          }
          else if (NN == 0x15) {
              // 0xFX15: Set delay timer = VX
              delayTimer = registers[X];
          }
          else if (NN == 0x18) {
              // 0xFX18: Set sound timer = VX
              soundTimer = registers[X];
          }
          else if (NN == 0x1E) {
              // 0xFX1E: I += VX
              index += registers[X];
          }
          else if (NN == 0x29) {
              // 0xFX29: I = address of font sprite for digit VX
              // Each font sprite is 5 bytes, fonts start at 0x000 in memory
              index = registers[X] * 5;
          }
          else if (NN == 0x33) {
              // 0xFX33: Store BCD of VX at I, I+1, I+2
              uint8_t value = registers[X];
              memory[index + 2] = value % 10;         // ones
              value /= 10;
              memory[index + 1] = value % 10;         // tens
              value /= 10;
              memory[index]     = value % 10;         // hundreds
          }
          else if (NN == 0x55) {
              // 0xFX55: Store V0 through VX into memory starting at I
              for (uint8_t i = 0; i <= X; i++) {
                  memory[index + i] = registers[i];
              }
          }
          else if (NN == 0x65) {
              // 0xFX65: Load V0 through VX from memory starting at I
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
        printf("Unknown Opcode: %04X\n", opcode);
  }


}
