g++ -std=c++20 -Wall -Wextra \
  main.cpp chip8.cpp assembler.cpp \
  libs/imgui/imgui.cpp \
  libs/imgui/imgui_draw.cpp \
  libs/imgui/imgui_tables.cpp \
  libs/imgui/imgui_widgets.cpp \
  libs/rlImGui/rlImGui.cpp \
  -Ilibs/imgui \
  -Ilibs/rlImGui \
  -o chip8 \
  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
