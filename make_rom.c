#include <stdio.h>

int main() {
    // This array represents the binary bytes of our Test ROM.
    unsigned char rom_data[] = {
        // --- CODE SECTION ---
        0x60, 0x00,  // 0x200: Set V0 = 0 (X coordinate)
        0x61, 0x00,  // 0x202: Set V1 = 0 (Y coordinate)
        0xA2, 0x0A,  // 0x204: Set I = 0x20A (Pointer to sprite data below)
        0xD0, 0x15,  // 0x206: Draw at (V0, V1), 5 pixels high
        0x12, 0x00,  // 0x208: Jump back to 0x200 (Infinite Loop)

        // --- DATA SECTION (The Sprite) ---
        // Address 0x20A starts here:
        0xFF,        // 11111111 (Full line)
        0x81,        // 10000001 (Ends only)
        0x81,        // 10000001 (Ends only)
        0x81,        // 10000001 (Ends only)
        0xFF         // 11111111 (Full line)
    };

    FILE *f = fopen("test.ch8", "wb");
    if (!f) {
        printf("Error creating file!\n");
        return 1;
    }
    
    // Write the array to the file
    fwrite(rom_data, sizeof(rom_data), 1, f);
    fclose(f);
    
    printf("Successfully created 'test.ch8'.\n");
    printf("Size: %lu bytes.\n", sizeof(rom_data));
    return 0;
}
