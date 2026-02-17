#include <iostream>
#include "chip8.h"
#include <unistd.h>
#include <cstdint>
#include <fstream>

int main (int argc, char *argv[]) {
  
  //use the constructor
  Chip8 myChip8;

  std::cout << "Loading ROM..." << std::endl;
  myChip8.LoadROM("test.ch8"); 

  std::cout << "ROM loaded! Starting emulation loop..." << std::endl;
  int i = 0;
  while(i<=5){
    // loop (fetch -> decode -> execute)
    myChip8.gameLoop();
    i++;

  }

  return 0;
}

