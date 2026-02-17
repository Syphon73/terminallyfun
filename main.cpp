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

  while(true){
    // loop (fetch -> decode -> execute)
    myChip8.gameLoop();
    //refreshScreen();

    usleep(1200);

  }

  return 0;
}

