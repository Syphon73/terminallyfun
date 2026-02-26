#include <iostream>
#include "chip8.h"
#include <unistd.h>
#include <cstdint>
#include <fstream>


void drawGraphics(Chip8& myChip8) {
    // Clear the terminal screen
    std::cout << "\033[H";  
    for (int y = 0; y < 32; ++y) {
      std::cout<<"Video buffer check - pixels at row 5:";
        for (int x = 0; x < 64; ++x) {
          std::cout<<myChip8.video[5 * 64 + x];
            int idx = x + (y * 64);
            
            if (myChip8.video[idx] == 1) {
                std::cout << "█";
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
  }


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
    
    if(myChip8.drawflag){
      drawGraphics(myChip8);
      myChip8.drawflag = false;
    }

    usleep(2000);

  }

  return 0;
}

