#include <iostream>
#include "chip8.h"
#include "raylib.h"
#include <unistd.h>
#include <cstdint>
#include <fstream>

const int SCALE = 10;

void drawGraphics(Chip8& myChip8) {
    // Clear the terminal screen
    BeginDrawing();
    ClearBackground(BLACK);

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (myChip8.video[x + (y * 64)]) {
                DrawRectangle(
                    x * SCALE,
                    y * SCALE,
                    SCALE,
                    SCALE,
                    WHITE
                );
            }
        }
    }

    EndDrawing();
    // std::cout << "\033[H";  
    // for (int y = 0; y < 32; ++y) {
    //   std::cout<<"Video buffer check - pixels at row 5:";
    //     for (int x = 0; x < 64; ++x) {
    //       std::cout<<myChip8.video[5 * 64 + x];
    //         int idx = x + (y * 64);
    //
    //         if (myChip8.video[idx] == 1) {
    //             std::cout << "█";
    //         } else {
    //             std::cout << " ";
    //         }
    //     }
    //     std::cout << std::endl;
    // }
  }


int main (int argc, char *argv[]) {
  
  //use the constructor
  Chip8 myChip8;

  std::cout << "Loading ROM..." << std::endl;
  myChip8.LoadROM("testROM/2-ibm-logo.ch8"); 

  std::cout << "ROM loaded! Starting emulation loop..." << std::endl;

  InitWindow(64 * SCALE, 32 * SCALE, "CHIP-8 Emulator");
  SetTargetFPS(60);
  
  while(!WindowShouldClose()){
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

