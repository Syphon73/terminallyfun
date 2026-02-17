#include <iostream>
#include "chip8.h"
#include <unistd.h>
#include <cstdint>
#include <fstream>

//const unsigned int START_ADDRESS = 0x200;

// define the constructor
Chip8::Chip8() {
  // Initialize PC = Start of ROM
  pc = START_ADDRESS;


  opcode = 0;
  index = 0;
  sp = 0;

  memset(memory, 0, sizeof(memory));
  memset(registers, 0, sizeof(registers));
  memset(stack, 0, sizeof(stack));
       
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

void graphic(){
  // TODO: read sprite from memory

  //drawing()
  //drawflag = true


}

void Chip8::gameLoop()
{

  //FETCH DECODE EXECUTE
  //std::cout<<"game loop running"<<std::endl;
  
  uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
  pc += 2;

  switch(opcode & 0xF000){

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
            // // Just print a debug message for now to prove it works
            // uint8_t x = registers[(opcode & 0x0F00) >> 8];
            // uint8_t y = registers[(opcode & 0x00F0) >> 4];
            // uint8_t height = opcode & 0x000F;
            //
            // printf("Opcode DXYN: Drawing at (%d, %d), Height: %d\n", x, y, height);
            // printf("Sprite data is at Memory[%03X]\n", index);

          bool Chip8::refreshScreen(){

            graphic();
            // if drawflag == true then print video[] as ASCII
            // drawflag = false

          };


        }
        break;

    default:
        printf("Unknown Opcode: %04X\n", opcode);
  }


}
