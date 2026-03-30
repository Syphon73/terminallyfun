#include <iostream>
#include "chip8.h"
#include "raylib.h"
#include <unistd.h>
#include <cstdint>
#include <fstream>

#include "rlImGui.h"
#include "imgui.h"
#include "assembler.h"

const int SCALE = 30;
//const unsigned int CHIP8_NUM_KEYS = 16;

void drawGraphics(Chip8& myChip8) {
    // Clear the terminal screen
    //BeginDrawing();
    //ClearBackground(BLACK);
    std::cout << "video ptr: " << (void*)myChip8.video << std::endl;
    std::cout << "video size: " << sizeof(myChip8.video) << std::endl;
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (myChip8.video[x + (y * 64)]) {
                DrawRectangle(
                    x ,
                    y ,
                    1,
                    1,
                    WHITE
                );         
            }
        }
    }

    //EndDrawing();
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

int main () {
  
  //use the constructor
  Chip8 myChip8;

  std::cout << "Loading ROM..." << std::endl; 
  myChip8.LoadROM("testROM/snake.ch8"); 
  std::cout << "ROM loaded! Starting emulation loop..." << std::endl;
  
  InitWindow(2000, 1000, "CHIP-8 Emulator");

  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();

  int texWidth = 64 * SCALE;
  int texHeight = 32 * SCALE;

  int posX = (screenWidth - texWidth) / 2;
  int posY = (screenHeight - texHeight) / 2;

  SetTargetFPS(60);
  //bool flag = false;
  rlImGuiSetup(true);
  //bool paused = false;
  myChip8.initRaylibTexture();

  while(!WindowShouldClose()){ 
    // loop (fetch -> decode -> execute)
    myChip8.keyboard_poll(myChip8.keypad);
    myChip8.gameLoop(); 
    
    BeginDrawing(); 
    // draw into texture
    BeginTextureMode(myChip8.target);
      //std::cout<< "inside texture mode"<< std::endl;
      if(myChip8.drawflag){
          ClearBackground(BLACK);
          //std::cout<<"inside drawflag"<<std::endl;
          drawGraphics(myChip8);
          //DrawRectangle(0, 0, 10, 10, WHITE);
          myChip8.drawflag = false;
      }
    EndTextureMode();
    //std::cout<<"out of texture mode"<<std::endl;
    // draw texture to screen
    DrawTexturePro(myChip8.target.texture, 
                (Rectangle){ 0, 0, 64, -32 }, 
                (Rectangle){(float)posX, (float)posY, (float)texWidth, (float)texHeight}, 
                (Vector2){ 0, 0 }, 0.0f, WHITE);
    //-------ImGUI block-----------
    rlImGuiBegin();
      //when state paused then render the window
      DrawDebuggerUI(myChip8); 
    rlImGuiEnd();
    //---------------------------
    //usleep(2000);
    EndDrawing();

  }
  myChip8.clearRaylibTexture();
  rlImGuiShutdown();

  return 0;
}

