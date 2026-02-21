#include <iostream>
#include "chip8.h"
#include <unistd.h>
#include <cstdint>
#include <fstream>

//const unsigned int START_ADDRESS = 0x200;
const unsigned int FONT_START_ADDRESS = 0x50;

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

      std::cout<<"printing the buffer: "<<std::endl;
      for(long i = 0; i < size; ++i){

        std::cout<<buffer[i]<<std::endl;

      }
      // Free the buffer
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
  
  for (int yline = 0; yline < height; yline++)
  {
      pixel = memory[index + yline];

      for (int xline = 0; xline < 8; xline++)
      {
          if ((pixel & (0x80 >> xline)) != 0)
          {
  
              int targetX = (x + xline) % 64;
              int targetY = (y + yline) % 32;
              int idx = targetX + (targetY * 64);

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
  //drawflag = true

}


void Chip8::gameLoop()
{

  //FETCH DECODE EXECUTE
  //sd::cout<<"game loop running"<<std::endl;
  
  uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
  pc += 2;

  switch(opcode & 0xF000)
  {
    case 0x0000: 
    {
      switch(opcode & 0x00FF)
      {
        case 0xE0: // Clear Screen
          // Set all pixels to 0
          memset(video, 0, sizeof(video));
          drawflag = true; // Tell main loop to redraw (as empty)
          break;

      }
    }
    break;

    case 0x1000: // JUMP
      pc = opcode & 0x0FFF; // Jump to NNN
      printf("Opcode 1NNN: Jumped to %03X\n", pc);
      break;

    case 0x6000: // SET REGISTER
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t nn = opcode & 0x00FF;
            registers[x] = nn;
            printf("Opcode 6XNN: Set V%X to %d\n", x, nn);
        }
        break;

    case 0xA000: // SET INDEX
        index = opcode & 0x0FFF;
        printf("Opcode ANNN: Set I to %03X\n", index); 
        break;

    case 0xD000: // DRAW
        {
  
            uint8_t x = registers[(opcode & 0x0F00) >> 8];
            uint8_t y = registers[(opcode & 0x00F0) >> 4];
            uint8_t height = opcode & 0x000F;

            //reset collision flag (VF) to handle physics
            registers[0xF] = 0;

            graphic(x,y,height);
            
            drawflag = true;

        }
        break;

    default:
        printf("Unknown Opcode: %04X\n", opcode);
  }


}
