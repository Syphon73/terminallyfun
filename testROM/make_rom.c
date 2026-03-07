#include <stdio.h>
#include <stdint.h>

/*
 * make_rom.c — Minimal CHIP-8 draw test ROM
 *
 * Only uses: 6XNN (set reg), ANNN (set I), DXYN (draw), 1NNN (jump)
 *
 * Compile:  gcc make_rom.c -o make_rom
 * Run:      ./make_rom
 * Output:   test.ch8
 */

static uint8_t rom[4096];
static int pc = 0;

static void emit(uint8_t hi, uint8_t lo)
{
    rom[pc++] = hi;
    rom[pc++] = lo;
}

/* Write a raw byte (for sprite data) */
static void raw(uint8_t byte)
{
    rom[pc++] = byte;
}

int main(void)
{
    /*
     * Memory layout:
     *   0x200..  = code
     *   Font sprites are pre-loaded by interpreter at 0x050..0x09F
     *
     * We'll also embed custom sprite data right after the halt
     * instruction and reference it with ANNN.
     */

    /* ================================================================
     *  Test 1: Draw built-in font digit "0" at (10, 5)
     *
     *  Font sprites live at 0x050. Each digit is 5 bytes.
     *  Digit "0" is at address 0x050.
     * ================================================================ */
    emit(0x6A, 0x0A);  /* VA = 10                                     */
    emit(0x6B, 0x05);  /* VB = 5                                      */
    emit(0xA0, 0x50);  /* I  = 0x050  (font sprite for "0")           */
    emit(0xDA, 0xB5);  /* DRAW VA, VB, 5                              */

    /* ================================================================
     *  Test 2: Draw built-in font digit "1" at (18, 5)
     *
     *  Digit "1" starts at 0x050 + 5 = 0x055
     * ================================================================ */
    emit(0x6A, 0x12);  /* VA = 18                                     */
    emit(0xA0, 0x55);  /* I  = 0x055  (font sprite for "1")           */
    emit(0xDA, 0xB5);  /* DRAW VA, VB, 5                              */

    /* ================================================================
     *  Test 3: Draw built-in font digit "2" at (26, 5)
     *
     *  Digit "2" starts at 0x05A
     * ================================================================ */
    emit(0x6A, 0x1A);  /* VA = 26                                     */
    emit(0xA0, 0x5A);  /* I  = 0x05A  (font sprite for "2")           */
    emit(0xDA, 0xB5);  /* DRAW VA, VB, 5                              */

    /* ================================================================
     *  Test 4: Draw a custom smiley sprite at (10, 15)
     *
     *  We point I to sprite data embedded after the halt instruction.
     *  (Address calculated below after we know where halt lands.)
     * ================================================================ */

    /* placeholder — we'll patch these 2 bytes once we know the addr   */
    int patch_smiley = pc;
    emit(0xA0, 0x00);  /* I = ???? (patched below)                    */
    emit(0x6A, 0x0A);  /* VA = 10                                     */
    emit(0x6B, 0x0F);  /* VB = 15                                     */
    emit(0xDA, 0xB5);  /* DRAW VA, VB, 5                              */

    /* ================================================================
     *  Test 5: Collision test — redraw digit "0" at (10, 5) again
     *
     *  The "0" was already drawn there in Test 1.
     *  XOR redraw should ERASE it.
     *  After this, VF should be 1 (collision).
     *
     *  We can't easily display VF without FX29, so just check
     *  visually: the "0" at (10,5) should DISAPPEAR.
     * ================================================================ */
    emit(0x6A, 0x0A);  /* VA = 10                                     */
    emit(0x6B, 0x05);  /* VB = 5                                      */
    emit(0xA0, 0x50);  /* I  = 0x050  (font "0" again)                */
    emit(0xDA, 0xB5);  /* DRAW VA, VB, 5  → XOR erases, VF = 1       */

    /* ================================================================
     *  Test 6: Edge clipping — draw digit "8" at (60, 29)
     *
     *  Digit "8" at font address 0x050 + (8*5) = 0x078
     *  Position (60,29) means only 4 pixels fit horizontally
     *  and 3 rows fit vertically. The rest should be CLIPPED.
     * ================================================================ */
    emit(0x6A, 0x3C);  /* VA = 60                                     */
    emit(0x6B, 0x1D);  /* VB = 29                                     */
    emit(0xA0, 0x78);  /* I  = 0x078  (font sprite for "8")           */
    emit(0xDA, 0xB5);  /* DRAW VA, VB, 5  → clipped at edges          */

    /* ================================================================
     *  Test 7: Draw a tall sprite (1px wide vertical line)
     *  at (40, 2), height = 8
     *
     *  Sprite data: 8 bytes of 0x80 = one pixel on the left each row
     *  This tests that height parameter works for non-5 values.
     * ================================================================ */
    int patch_vline = pc;
    emit(0xA0, 0x00);  /* I = ???? (patched below)                    */
    emit(0x6A, 0x28);  /* VA = 40                                     */
    emit(0x6B, 0x02);  /* VB = 2                                      */
    emit(0xDA, 0xB8);  /* DRAW VA, VB, 8  → 8-row tall sprite         */

    /* ================================================================
     *  Halt: infinite loop
     * ================================================================ */
    int halt_addr = 0x200 + pc;
    emit((0x10 | ((halt_addr >> 8) & 0x0F)),
                        (halt_addr & 0xFF));

    /* ================================================================
     *  Embedded sprite data (after code — never executed)
     * ================================================================ */

    /* Smiley face (5 bytes) */
    int smiley_addr = 0x200 + pc;
    raw(0x66);  /*  .##..##.  */
    raw(0x99);  /*  #..##..#  */
    raw(0x00);  /*  ........  */
    raw(0x81);  /*  #......#  */
    raw(0x7E);  /*  .######.  */

    /* Vertical line (8 bytes of 0x80) */
    int vline_addr = 0x200 + pc;
    raw(0x80);  /*  #.......  */
    raw(0x80);  /*  #.......  */
    raw(0x80);  /*  #.......  */
    raw(0x80);  /*  #.......  */
    raw(0x80);  /*  #.......  */
    raw(0x80);  /*  #.......  */
    raw(0x80);  /*  #.......  */
    raw(0x80);  /*  #.......  */

    /* ================================================================
     *  Patch the ANNN instructions to point to sprite data
     * ================================================================ */
    rom[patch_smiley]     = 0xA0 | ((smiley_addr >> 8) & 0x0F);
    rom[patch_smiley + 1] = smiley_addr & 0xFF;

    rom[patch_vline]      = 0xA0 | ((vline_addr >> 8) & 0x0F);
    rom[patch_vline + 1]  = vline_addr & 0xFF;

    /* ================================================================
     *  Write ROM
     * ================================================================ */
    FILE *f = fopen("test.ch8", "wb");
    if (!f) {
        fprintf(stderr, "Error: could not open test.ch8 for writing\n");
        return 1;
    }
    fwrite(rom, 1, pc, f);
    fclose(f);

    printf("Written test.ch8 (%d bytes)\n\n", pc);

    printf("Opcodes used: 6XNN, ANNN, DXYN, 1NNN  (that's it!)\n\n");

    printf("=== Expected Screen ===\n\n");
    printf("  (10,5)  → digit '0' drawn THEN ERASED (should be blank)\n");
    printf("  (18,5)  → digit '1' visible\n");
    printf("  (26,5)  → digit '2' visible\n");
    printf("  (10,15) → smiley face visible\n");
    printf("  (40,2)  → vertical line (8px tall, 1px wide)\n");
    printf("  (60,29) → PARTIAL digit '8' (clipped at screen edge)\n");
    printf("\n");
    printf("=== What to check ===\n\n");
    printf("  [PASS] '0' at (10,5) is GONE        → XOR erase works\n");
    printf("  [PASS] partial '8' bottom-right ONLY → clipping works\n");
    printf("  [FAIL] '0' still visible at (10,5)   → XOR is broken\n");
    printf("  [FAIL] '8' fragments on left/top      → wrapping bug\n");

    return 0;
}
