sram_init:
  la      t0, 0xB8001300
  li      t1, 0x1F000001
  sw      t1, 0(t0)
  li      t2, 0x0000000A
  sw      t2, 4(t0)

  la      t0, 0xB8004000
  sw      t1, 0(t0) //SRAMSAR0 for base address SRAM maps to

#if 0
/* reset the whole SRAM to zero */
  li      t0, SRAM_SIZE
  li      t1, UCSRAM_BASE
  addu    t0, t0, t1
1:
  sw      zero, 0x00(t1)
  sw      zero, 0x04(t1)
  sw      zero, 0x08(t1)
  sw      zero, 0x0c(t1)
  sw      zero, 0x10(t1)
  sw      zero, 0x14(t1)
  sw      zero, 0x18(t1)
  sw      zero, 0x1c(t1)
  sw      zero, 0x20(t1)
  sw      zero, 0x24(t1)
  sw      zero, 0x28(t1)
  sw      zero, 0x2c(t1)
  sw      zero, 0x30(t1)
  sw      zero, 0x34(t1)
  sw      zero, 0x38(t1)
  addi    t1, 0x40
  bne     t0, t1, 1b
  sw      zero, 0x3c(t1)
#endif

