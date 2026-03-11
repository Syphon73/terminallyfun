#include "debugger.h"

#include "imgui.h"
#include "rlImGui.h"




void DrawDebuggerUI(Chip8 &chip8)
{
    DrawRegisters(chip8);
    // DrawMemory();
    // DrawDissassembler();
    // DrawKeypad();
    // DrawState();
}
