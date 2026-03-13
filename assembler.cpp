#include "assembler.h"
#include <format>
#include "imgui.h"
#include "rlImGui.h"

void DrawRegisters(Chip8 &chip8){
  ImGui::Begin("Registers");

    for(int i=0;i<16;i++)
    {
        ImGui::Text("V%X : %02X", i, chip8.registers[i]);
    }

    ImGui::Separator();

    ImGui::Text("I  : %04X", chip8.index);
    ImGui::Text("PC : %04X", chip8.pc);
    ImGui::Text("SP : %02X", chip8.sp);

    ImGui::Text("Delay : %d", chip8.delayTimer);
    ImGui::Text("Sound : %d", chip8.soundTimer);

    ImGui::End();
}

std::string DrawDissassembler(uint16_t instr)
{
  int nnn = instr & 0xfff;
  int n = instr & 0xf;
  int nn = instr & 0xff;
  int x = (instr >> 8) & 0xf;
  int y = (instr >> 4) & 0xf;

  switch ((instr >> 12) & 0xf) {
  case 0x0:
    if (instr == 0x00e0) {
      return "CLS";
    } else if (instr == 0x00ee) {
      return "RET";
    } else if (((n >> 4) & 0xf) == 0xc) {
      // Super Chip-48
      return std::format("SCD {:x}h", n);
    } else if (nn == 0xfb) {
      // Super Chip-48
      return "SCR";
    } else if (nn == 0xfc) {
      // Super Chip-48
      return "SCL";
    } else if (nn == 0xfd) {
      // Super Chip-48
      return "EXIT";
    } else if (nn == 0xfe) {
      // Super Chip-48
      return "LOW";
    } else if (nn == 0xff) {
      // Super Chip-48
      return "HIGH";
    } else {
      return std::format("SYS {:03x}h", nnn);
    }
  case 0x1:
    return std::format("JP {:03x}h", nnn);
  case 0x2:
    return std::format("CALL {:03x}h", nnn);
  case 0x3:
    return std::format("SE V{:x}, {:02x}h", x, nn);
  case 0x4:
    return std::format("SNE V{:x}, {:02x}h", x, nn);
  case 0x5:
    if (n == 0) {
      return std::format("SE V{:x}, V{:x}", x, y);
    }
    break;
  case 0x6:
    return std::format("LD V{:x}, {:02x}h", x, nn);
  case 0x7:
    return std::format("ADD V{:x}, {:02x}h", x, nn);
  case 0x8:
    switch (n) {
    case 0:
      return std::format("LD V{:x} V{:x}", x, y);
    case 1:
      return std::format("OR V{:x}, V{:x}", x, y);
    case 2:
      return std::format("AND V{:x}, V{:x}", x, y);
    case 3:
      return std::format("XOR V{:x}, V{:x}", x, y);
    case 4:
      return std::format("ADD V{:x}, V{:x}", x, y);
    case 5:
      return std::format("SUB V{:x}, V{:x}", x, y);
    case 6:
      return std::format("SHR V{:x}, V{:x}", x, y);
    case 7:
      return std::format("SUBN V{:x}, V{:x}", x, y);
    case 0xe:
      return std::format("SHL V{:x}, V{:x}", x, y);
    default:
      break;
    }
    break;
  case 0x9:
    if (n == 0) {
      return std::format("SNE V{:x}, V{:x}", x, y);
    }
    break;
  case 0xa:
    return std::format("LD I, {:03x}h", nnn);
  case 0xb:
    return std::format("JP V0, {:03x}h", nnn);
  case 0xc:
    return std::format("RND V{:x}, {:02x}h", x, nn);
  case 0xd:
    return std::format("DRW V{:x}, V{:x}, {:x}h", x, y, n);
  case 0xe:
    switch (nn) {
    case 0x9e:
      return std::format("SKP V{:x}", x);
    case 0xa1:
      return std::format("SKNP V{:x}", x);
    default:
      break;
    }
    break;
  case 0xf:
    switch (nn) {
    case 0x07:
      return std::format("LD V{:x}, DT", x);
    case 0x0a:
      return std::format("LD V{:x}, K", x);
    case 0x15:
      return std::format("LD DT, V{:x}", x);
    case 0x18:
      return std::format("LD ST, V{:x}", x);
    case 0x1e:
      return std::format("ADD I, V{:x}", x);
    case 0x29:
      return std::format("LD F, V{:x}", x);
    case 0x33:
      return std::format("LD B, V{:x}", x);
    case 0x55:
      return std::format("LD [I], V{:x}", x);
    case 0x65:
      return std::format("LD V{:x}, [I]", x);
    // Super Chip-48 Instructions
    case 0x30:
      return std::format("LD HF, V{:x}", x);
    case 0x75:
      return std::format("LD R, V{:x}", x);
    case 0x85:
      return std::format("LD V{:x}, R", x);
    default:
      break;
    }
    break;
  }

  return "";

}

void DrawInstructions(Chip8& chip8){
  ImGui::Begin("Instructions");
  int start_addr = chip8.pc - 20;
  for(int i=0;i<20;i++)
  {
      int curr_addr = start_addr + i*2;

      uint16_t opcode =
          chip8.memory[curr_addr] << 8 |
          chip8.memory[curr_addr+1];

      std::string inst = DrawDissassembler(opcode);

      if(curr_addr == chip8.pc)
          ImGui::TextColored(ImVec4(1,1,0,1),
              "%03X  %04X  %s",
              curr_addr, opcode, inst.c_str());
      else
          ImGui::Text(
              "%03X  %04X  %s",
              curr_addr, opcode, inst.c_str());
  }
  ImGui::End();
}


void DrawDebuggerUI(Chip8& chip8)
{
    DrawRegisters(chip8);
    // DrawMemory();
    DrawDissassembler(chip8.opcode);
    DrawInstructions(chip8);
    // DrawKeypad();
    // DrawState();
}
