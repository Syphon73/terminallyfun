
#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <cstring>
#include "raylib.h"


//const unsigned int START_ADDRESS = 0x200;

//declaring constructor chip8.h -> defining it in chip8.cpp -> initiating it in main.cpp
class Chip8
{
public:

  Chip8();

  void LoadROM(char const* filename);
  void gameLoop();

  bool refreshScreen();
  void graphic(uint8_t x,uint8_t y,uint8_t height);
  
  void keyboard_poll(uint8_t keypad[16]);

  static const unsigned int FONTSET_SIZE = 80;
  uint8_t fontset[FONTSET_SIZE];
  bool drawflag;
  

  RenderTexture2D target;
  void initRaylibTexture();
  void clearRaylibTexture();

	uint8_t registers[16]{};
	uint8_t memory[4096]{};
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[16]{};
	uint8_t sp{};
  uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint8_t keypad[16]{};
	uint32_t video[64 * 32]{};
	uint16_t opcode;
  
};


#endif
