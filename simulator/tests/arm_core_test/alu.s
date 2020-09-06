/*------------------------------------------------------------------------------
 * alu_instruction_test.c
 *
 * Branden Ghena (brghena@umich.edu)
 * 07/10/2013
 *
 * Tests all ALU instructions on the Cortex M0 processor
 *----------------------------------------------------------------------------*/

.syntax unified

CLK_FREQ = 1000

/* adcs_test
 *
 * Tests the Add with carry and condition set instruction
 */

# Test additions without carry
.thumb_func
adcs_test:
    # Zero out Carry bit (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0 + 1 (+0) = 1
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x1
    MOVW R2, 0x1
    ADCS R0, R0, R1
    CMP  R0, R2
    BNE  adcs_test_led_lbl

    # Zero out Carry bit (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x7FFFFFFF + 1 (+0) = 0x80000000
    #LDR  R0, 0x7FFFFFFF
    MOVW R0, 0xFFFF
    MOVT R0, 0x7FFF
    MOVW R1, 0x1
    MOVW R2, 0x0
    MOVT R2, 0x8000
    ADCS R0, R0, R1
    CMP  R0, R2
    BNE  adcs_test_led_lbl

# Test additions with carry
    # Set carry bit to 1 (NZCV = 0010)
    MOVW R0, 0x1
    MOVW R1, 0x0
    MOVT R1, 0x0
    CMP R0, R1

    # 0 + 1 (+1) = 2
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x1
    MOVW R2, 0x2
    ADCS R0, R0, R1
    CMP  R0, R2
    BNE  adcs_test_led_lbl

    # Set carry bit to 1 (NZCV = 0010)
    MOVW R0, 0x1
    MOVW R1, 0x0
    MOVT R1, 0x0
    CMP R0, R1

    # 0x7FFFFFFF + 1 (+1) = 0x80000001
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R1, 0x1
    MOVW R2, 0x1
    MOVT R2, 0x8000
    ADCS R0, R0, R1
    CMP  R0, R2
    BNE  adcs_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x7FFFFFFF + 1 (+0) => NZCV = 1001
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R1, 0x1
    ADCS R0, R0, R1
    BPL  adcs_test_led_lbl
    BEQ  adcs_test_led_lbl
    BCS  adcs_test_led_lbl
    BVC  adcs_test_led_lbl

    # 1 + -1 (+0) => NZCV = 0110
    MOVW R0, 0x1
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    ADCS R0, R0, R1
    BMI  adcs_test_led_lbl
    BNE  adcs_test_led_lbl
    BCC  adcs_test_led_lbl
    BVS  adcs_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

adcs_test_led_lbl:
    MOVW R0, 1
    BX LR


/* add_imm_test
 *
 * Tests the Add immediate instruction. Only works with the SP or PC as the
 *  additive register
 */

.thumb_func
add_imm_test:
# Test addition & condition flags
    # Set NZCV bits (NZCV = 0110)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x0
    MOVT R1, 0x0
    CMP R0, R1

    # SP + 12 = 0x00002000
    # (SP = 0x00001FF4)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R2, 0x2000
    MOVT R2, 0x0
    ADD R0, SP, #12

    # SP + 4 => should not affect NZCV
    BMI add_imm_test_led_lbl
    BNE add_imm_test_led_lbl
    BCC add_imm_test_led_lbl
    BVS add_imm_test_led_lbl

    # Test that value is correct
    CMP R0, R2
    BNE add_imm_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

add_imm_test_led_lbl:
    MOVW R0, 2
    BX LR


/* adds_test
 *
 * Tests the Add with condition set instruction
 */

.thumb_func
adds_test:
# Test additions
    # 0 + 1 = 1
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x1
    MOVW R2, 0x1
    ADDS R0, R0, R1
    CMP  R0, R2
    BNE  adds_test_led_lbl

    # 0x7FFFFFFF + 1 = 0x80000000
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R1, 0x1
    MOVW R2, 0x0
    MOVT R2, 0x8000
    ADDS R0, R0, R1
    CMP  R0, R2
    BNE  adds_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x7FFFFFFF + 1 => NZCV = 1001
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R1, 0x1
    ADDS R0, R0, R1
    BPL  adds_test_led_lbl
    BEQ  adds_test_led_lbl
    BCS  adds_test_led_lbl
    BVC  adds_test_led_lbl

    # 1 + -1 => NZCV = 0110
    MOVW R0, 0x1
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    ADDS R0, R0, R1
    BMI  adds_test_led_lbl
    BNE  adds_test_led_lbl
    BCC  adds_test_led_lbl
    BVS  adds_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

adds_test_led_lbl:
    MOVW R0, 3
    BX LR


/* adds_imm_test
 *
 * Tests the Add with immediate and condition set instruction
 */

.thumb_func
adds_imm_test:
# Test additions
    # 0 + 1 = 1
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R2, 0x1
    ADDS R0, R0, #1
    CMP  R0, R2
    BNE  adds_imm_test_led_lbl

    # 0x7FFFFFFF + 1 = 0x80000000
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R2, 0x0
    MOVT R2, 0x8000
    ADDS R0, R0, #1
    CMP  R0, R2
    BNE  adds_imm_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x7FFFFFFF + 1 => NZCV = 1001
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    ADDS R0, R0, #1
    BPL  adds_imm_test_led_lbl
    BEQ  adds_imm_test_led_lbl
    BCS  adds_imm_test_led_lbl
    BVC  adds_imm_test_led_lbl

    # 1 + -1 => NZCV = 0110
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    ADDS R0, R0, #1
    BMI  adds_imm_test_led_lbl
    BNE  adds_imm_test_led_lbl
    BCC  adds_imm_test_led_lbl
    BVS  adds_imm_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

adds_imm_test_led_lbl:
    MOVW R0, 4
    BX LR


/* ands_test
 *
 * Tests the And with condition set instruction
 */

.thumb_func
ands_test:
# Test and
    # 0x00000000 & 0xFFFFFFFF = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0x0
    MOVT R2, 0x0
    ANDS R0, R0, R1
    CMP  R0, R2
    BNE  ands_test_led_lbl

    # 0xFFFFFFFF & 0xA5A5A5A5 = 0xA5A5A5A5
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0xa5a5
    MOVT R1, 0xa5a5
    MOVW R2, 0xa5a5
    MOVT R2, 0xa5a5
    ANDS R0, R0, R1
    CMP  R0, R2
    BNE  ands_test_led_lbl

    # 0xFFFFFFFF & 0x5A5A5A5A = 0x5A5A5A5A
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x5a5a
    MOVT R1, 0x5a5a
    MOVW R2, 0x5a5a
    MOVT R2, 0x5a5a
    ANDS R0, R0, R1
    CMP  R0, R2
    BNE  ands_test_led_lbl

    # 0xFFFFFFFF & 0xFFFFFFFF = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    ANDS R0, R0, R1
    CMP  R0, R2
    BNE  ands_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x00000000 & 0xFFFFFFFF => NZCV = 0100
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    ANDS R0, R0, R1
    BMI  ands_test_led_lbl
    BNE  ands_test_led_lbl
    BCS  ands_test_led_lbl
    BVS  ands_test_led_lbl

    # 0x80000000 & 0xFFFFFFFF => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    ANDS R0, R0, R1
    BPL  ands_test_led_lbl
    BEQ  ands_test_led_lbl
    BCS  ands_test_led_lbl
    BVS  ands_test_led_lbl

    # 0x7FFFFFFF & 0x7FFFFFFF => NZCV = 0000
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R1, 0xffff
    MOVT R1, 0x7fff
    ANDS R0, R0, R1
    BMI  ands_test_led_lbl
    BEQ  ands_test_led_lbl
    BCS  ands_test_led_lbl
    BVS  ands_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

ands_test_led_lbl:
    MOVW R0, 5
    BX LR


/* asrs_test
 *
 * Tests the Arithmetic Shift Right with condition set instruction
 */

.thumb_func
asrs_test:
# Test right shift
    # 0x00000080 >> 4 = 0x00000008
    MOVW R0, 0x80
    MOVW R1, 0x4
    MOVW R2, 0x8
    ASRS R0, R0, R1
    CMP  R0, R2
    BNE  asrs_test_led_lbl

    # 0x80000000 >> 3 = 0xF0000000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x3
    MOVW R2, 0x0
    MOVT R2, 0xf000
    ASRS R0, R0, R1
    CMP  R0, R2
    BNE  asrs_test_led_lbl

    # 0x7FFFFFFF >> 32 = 0
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R1, 0x32
    MOVW R2, 0x0
    MOVT R2, 0x0
    ASRS R0, R0, R1
    CMP  R0, R2
    BNE  asrs_test_led_lbl

    # 0xFFFFFFFF >> 32 = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x32
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    ASRS R0, R0, R1
    CMP  R0, R2
    BNE  asrs_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x00000001 >> 1 => NZCV = 0110
    MOVW R0, 0x1
    MOVW R1, 0x1
    ASRS R0, R0, R1
    BMI  asrs_test_led_lbl
    BNE  asrs_test_led_lbl
    BCC  asrs_test_led_lbl
    BVS  asrs_test_led_lbl

    # 0x80000000 >> 1 => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x1
    ASRS R0, R0, R1
    BPL  asrs_test_led_lbl
    BEQ  asrs_test_led_lbl
    BCS  asrs_test_led_lbl
    BVS  asrs_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

asrs_test_led_lbl:
    MOVW R0, 6
    BX LR


/* asrs_imm_test
 *
 * Tests the Arithmetic Shift Right with immediate and condition set
 *  instruction
 */

.thumb_func
asrs_imm_test:
# Test right shift
    # 0x00000080 >> 4 = 0x00000008
    MOVW R0, 0x80
    MOVW R2, 0x8
    ASRS R0, R0, #4
    CMP  R0, R2
    BNE  asrs_imm_test_led_lbl

    # 0x80000000 >> 3 = 0xF0000000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R2, 0x0
    MOVT R2, 0xf000
    ASRS R0, R0, #3
    CMP  R0, R2
    BNE  asrs_imm_test_led_lbl

    # 0x7FFFFFFF >> 32 = 0
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R2, 0x0
    MOVT R2, 0x0
    ASRS R0, R0, #32
    CMP  R0, R2
    BNE  asrs_imm_test_led_lbl

    # 0xFFFFFFFF >> 32 = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    ASRS R0, R0, #32
    CMP  R0, R2
    BNE  asrs_imm_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x00000001 >> 1 => NZCV = 0110
    MOVW R0, 0x1
    ASRS R0, R0, #1
    BMI  asrs_imm_test_led_lbl
    BNE  asrs_imm_test_led_lbl
    BCC  asrs_imm_test_led_lbl
    BVS  asrs_imm_test_led_lbl

    # 0x80000000 >> 1 => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    ASRS R0, R0, #1
    BPL  asrs_imm_test_led_lbl
    BEQ  asrs_imm_test_led_lbl
    BCS  asrs_imm_test_led_lbl
    BVS  asrs_imm_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

asrs_imm_test_led_lbl:
    MOVW R0, 7
    BX LR


/* bics_test
 *
 * Tests the Bit Clear with condition set instruction
 */

.thumb_func
bics_test:
# Test Clearing Bits
    # 0xFFFFFFFF clear(0xFFFFFFFF) = 0x00000000
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0x0
    MOVT R2, 0x0
    BICS R0, R0, R1
    CMP  R0, R2
    BNE  bics_test_led_lbl

    # 0xFFFFFFFF clear(0x00000000) = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    BICS R0, R0, R1
    CMP  R0, R2
    BNE  bics_test_led_lbl

    # 0x00000000 clear(0xFFFFFFFF) = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0x0
    MOVT R2, 0x0
    BICS R0, R0, R1
    CMP  R0, R2
    BNE  bics_test_led_lbl

    # 0x00000000 clear(0x00000000) = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    BICS R0, R0, R1
    CMP  R0, R2
    BNE  bics_test_led_lbl

    # 0xA5A5A5A5 clear(0xF0F0F0F0) = 0x05050505
    MOVW R0, 0xa5a5
    MOVT R0, 0xa5a5
    MOVW R1, 0xf0f0
    MOVT R1, 0xf0f0
    MOVW R2, 0x505
    MOVT R2, 0x505
    BICS R0, R0, R1
    CMP  R0, R2
    BNE  bics_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0xFFFFFFFF clear(0x7FFFFFFF) => NZCV = 1000
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0xffff
    MOVT R1, 0x7fff
    BICS R0, R0, R1
    BPL  bics_test_led_lbl
    BEQ  bics_test_led_lbl
    BCS  bics_test_led_lbl
    BVS  bics_test_led_lbl

    # 0x05050505 clear(0x0F0F0F0F) => NZCV = 0100
    MOVW R0, 0x505
    MOVT R0, 0x505
    MOVW R1, 0xf0f
    MOVT R1, 0xf0f
    BICS R0, R0, R1
    BMI  bics_test_led_lbl
    BNE  bics_test_led_lbl
    BCS  bics_test_led_lbl
    BVS  bics_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

bics_test_led_lbl:
    MOVW R0, 8
    BX LR


/* cmn_test
 *
 * Tests the Compare Negative instruction
 */

.thumb_func
cmn_test:
# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x7FFFFFFF cmn 1 => NZCV = 1001
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R1, 0x1
    CMN R0, R1
    BPL cmn_test_led_lbl
    BEQ cmn_test_led_lbl
    BCS cmn_test_led_lbl
    BVC cmn_test_led_lbl

    # 1 cmn -1 => NZCV = 0110
    MOVW R0, 0x1
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMN R0, R1
    BMI cmn_test_led_lbl
    BNE cmn_test_led_lbl
    BCC cmn_test_led_lbl
    BVS cmn_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

cmn_test_led_lbl:
    MOVW R0, 9
    BX LR


/* eors_test
 *
 * Tests the Exclusive Or with condition set instruction
 */

.thumb_func
eors_test:
# Test xor
    # 0x00000000 ^ 0xFFFFFFFF = 0xFFFFFFFF
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    EORS R0, R0, R1
    CMP  R0, R2
    BNE  eors_test_led_lbl

    # 0x00000000 ^ 0x00000000 = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    EORS R0, R0, R1
    CMP  R0, R2
    BNE  eors_test_led_lbl

    # 0xFFFFFFFF ^ 0x00000000 = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    EORS R0, R0, R1
    CMP  R0, R2
    BNE  eors_test_led_lbl

    # 0xFFFFFFFF ^ 0xFFFFFFFF = 0x00000000
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0x0
    MOVT R2, 0x0
    EORS R0, R0, R1
    CMP  R0, R2
    BNE  eors_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0xFFFFFFFF ^ 0xFFFFFFFF => NZCV = 0100
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    EORS R0, R0, R1
    BMI  eors_test_led_lbl
    BNE  eors_test_led_lbl
    BCS  eors_test_led_lbl
    BVS  eors_test_led_lbl

    # 0x00000000 ^ 0xFFFFFFFF => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    EORS R0, R0, R1
    BPL  eors_test_led_lbl
    BEQ  eors_test_led_lbl
    BCS  eors_test_led_lbl
    BVS  eors_test_led_lbl

    # 0x00000001 ^ 0x00000000 => NZCV = 0000
    MOVW R0, 0x1
    MOVW R1, 0x0
    MOVT R1, 0x0
    EORS R0, R0, R1
    BMI  eors_test_led_lbl
    BEQ  eors_test_led_lbl
    BCS  eors_test_led_lbl
    BVS  eors_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

eors_test_led_lbl:
    MOVW R0, 10
    BX LR


/* lsls_test
 *
 * Tests the Logical Shift Left with condition set instruction
 */

.thumb_func
lsls_test:
# Test left shift
    # 0x00000001 << 4 = 0x00000010
    MOVW R0, 0x1
    MOVW R1, 0x4
    MOVW R2, 0x10
    LSLS R0, R0, R1
    CMP  R0, R2
    BNE  lsls_test_led_lbl

    # 0xFFFFFFFF << 1 = 0xFFFFFFFE
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x1
    MOVW R2, 0xfffe
    MOVT R2, 0xffff
    LSLS R0, R0, R1
    CMP  R0, R2
    BNE  lsls_test_led_lbl

    # 0xFFFFFFFF << 32 = 0
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x32
    MOVW R2, 0x0
    MOVT R2, 0x0
    LSLS R0, R0, R1
    CMP  R0, R2
    BNE  lsls_test_led_lbl

    # 0x00000001 << 0 = 0x00000001
    MOVW R0, 0x1
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0x1
    LSLS R0, R0, R1
    CMP  R0, R2
    BNE  lsls_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x80000000 << 1 => NZCV = 0110
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x1
    LSLS R0, R0, R1
    BMI  lsls_test_led_lbl
    BNE  lsls_test_led_lbl
    BCC  lsls_test_led_lbl
    BVS  lsls_test_led_lbl

    # 0x40000000 << 1 => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x4000
    MOVW R1, 0x1
    LSLS R0, R0, R1
    BPL  lsls_test_led_lbl
    BEQ  lsls_test_led_lbl
    BCS  lsls_test_led_lbl
    BVS  lsls_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

lsls_test_led_lbl:
    MOVW R0, 11
    BX LR


/* lsls_imm_test
 *
 * Tests the Logical Shift Left with immediate and condition set instruction
 */

.thumb_func
lsls_imm_test:
# Test left shift
    # 0x00000001 << 4 = 0x00000010
    MOVW R0, 0x1
    MOVW R2, 0x10
    LSLS R0, R0, #4
    CMP  R0, R2
    BNE  lsls_imm_test_led_lbl

    # 0xFFFFFFFF << 1 = 0xFFFFFFFE
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R2, 0xfffe
    MOVT R2, 0xffff
    LSLS R0, R0, #1
    CMP  R0, R2
    BNE  lsls_imm_test_led_lbl

    # 0xFFFFFFFF << 32 = 0x80000000
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R2, 0x0
    MOVT R2, 0x8000
    LSLS R0, R0, #31
    CMP  R0, R2
    BNE  lsls_imm_test_led_lbl

    # 0x00000001 << 0 = 0x00000001
    MOVW R0, 0x1
    MOVW R2, 0x1
    LSLS R0, R0, #0
    CMP  R0, R2
    BNE  lsls_imm_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x80000000 << 1 => NZCV = 0110
    MOVW R0, 0x0
    MOVT R0, 0x8000
    LSLS R0, R0, #1
    BMI  lsls_imm_test_led_lbl
    BNE  lsls_imm_test_led_lbl
    BCC  lsls_imm_test_led_lbl
    BVS  lsls_imm_test_led_lbl

    # 0x40000000 << 1 => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x4000
    LSLS R0, R0, #1
    BPL  lsls_imm_test_led_lbl
    BEQ  lsls_imm_test_led_lbl
    BCS  lsls_imm_test_led_lbl
    BVS  lsls_imm_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

lsls_imm_test_led_lbl:
    MOVW R0, 12
    BX LR


/* lsrs_test
 *
 * Tests the Logical Shift Right with condition set instruction
 */

.thumb_func
lsrs_test:
# Test right shift
    # 0x00000080 >> 4 = 0x00000008
    MOVW R0, 0x80
    MOVW R1, 0x4
    MOVW R2, 0x8
    LSRS R0, R0, R1
    CMP  R0, R2
    BNE  lsrs_test_led_lbl

    # 0x80000000 >> 3 = 0x10000000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x3
    MOVW R2, 0x0
    MOVT R2, 0x1000
    LSRS R0, R0, R1
    CMP  R0, R2
    BNE  lsrs_test_led_lbl

    # 0xFFFFFFFF >> 32 = 0
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x32
    MOVW R2, 0x0
    MOVT R2, 0x0
    LSRS R0, R0, R1
    CMP  R0, R2
    BNE  lsrs_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x00000001 >> 1 => NZCV = 0110
    MOVW R0, 0x1
    MOVW R1, 0x1
    LSRS R0, R0, R1
    BMI  lsrs_test_led_lbl
    BNE  lsrs_test_led_lbl
    BCC  lsrs_test_led_lbl
    BVS  lsrs_test_led_lbl

    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0xFFFFFFFF >> 0 => NZCV = 1000
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x0
    MOVT R1, 0x0
    LSRS R0, R0, R1
    BPL  lsrs_test_led_lbl
    BEQ  lsrs_test_led_lbl
    BCS  lsrs_test_led_lbl
    BVS  lsrs_test_led_lbl

    # 0x80000000 >> 1 => NZCV = 0000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x1
    LSRS R0, R0, R1
    BMI  lsrs_test_led_lbl
    BEQ  lsrs_test_led_lbl
    BCS  lsrs_test_led_lbl
    BVS  lsrs_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

lsrs_test_led_lbl:
    MOVW R0, 13
    BX LR


/* lsrs_imm_test
 *
 * Tests the Logical Shift Right with immediate and condition set instruction
 */

.thumb_func
lsrs_imm_test:
# Test right shift
    # 0x00000080 >> 4 = 0x00000008
    MOVW R0, 0x80
    MOVW R2, 0x8
    LSRS R0, R0, #4
    CMP  R0, R2
    BNE  lsrs_imm_test_led_lbl

    # 0x80000000 >> 3 = 0x10000000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R2, 0x0
    MOVT R2, 0x1000
    LSRS R0, R0, #3
    CMP  R0, R2
    BNE  lsrs_imm_test_led_lbl

    # 0xFFFFFFFF >> 32 = 0
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R2, 0x0
    MOVT R2, 0x0
    LSRS R0, R0, #32
    CMP  R0, R2
    BNE  lsrs_imm_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x00000001 >> 1 => NZCV = 0110
    MOVW R0, 0x1
    MOVW R1, 0x1
    LSRS R0, R0, #1
    BMI  lsrs_imm_test_led_lbl
    BNE  lsrs_imm_test_led_lbl
    BCC  lsrs_imm_test_led_lbl
    BVS  lsrs_imm_test_led_lbl

    # 0x80000000 >> 1 => NZCV = 0000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    LSRS R0, R0, #1
    BMI  lsrs_imm_test_led_lbl
    BEQ  lsrs_imm_test_led_lbl
    BCS  lsrs_imm_test_led_lbl
    BVS  lsrs_imm_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

lsrs_imm_test_led_lbl:
    MOVW R0, 14
    BX LR


/* mov_test
 *
 * Tests the Move instruction
 */

.thumb_func
mov_test:
# Test move
    # R1 = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOV R1, R0
    CMP R0, R1
    BNE mov_test_led_lbl

    # R1 = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOV R1, R0
    CMP R0, R1
    BNE mov_test_led_lbl

    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # Condition bits should be unaffected
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOV R1, R0
    BMI lsrs_imm_test_led_lbl
    BEQ lsrs_imm_test_led_lbl
    BCS lsrs_imm_test_led_lbl
    BVS lsrs_imm_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

mov_test_led_lbl:
    MOVW R0, 15
    BX LR


/* movs_test
 *
 * Tests the Move with condition set instruction
 */

.thumb_func
movs_test:
# Test move
    # R1 = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVS R1, R0
    CMP  R0, R1
    BNE  movs_test_led_lbl

    # R1 = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVS R1, R0
    CMP  R0, R1
    BNE  movs_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # R1 = 0x00000000 => NZCV = 0100
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVS R1, R0
    BMI  movs_test_led_lbl
    BNE  movs_test_led_lbl
    BCS  movs_test_led_lbl
    BVS  movs_test_led_lbl

    # R1 = 0x80000000 => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVS R1, R0
    BPL  movs_test_led_lbl
    BEQ  movs_test_led_lbl
    BCS  movs_test_led_lbl
    BVS  movs_test_led_lbl

    # R1 = 0x7FFFFFFF => NZCV = 0000
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVS R1, R0
    BMI  movs_test_led_lbl
    BEQ  movs_test_led_lbl
    BCS  movs_test_led_lbl
    BVS  movs_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

movs_test_led_lbl:
    MOVW R0, 16
    BX LR


/* movs_imm_test
 *
 * Tests the Move with immediate and condition set instruction
 */

.thumb_func
movs_imm_test:
# Test move
    # R1 = 0x00000000
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVS R0, #0
    CMP  R0, R1
    BNE  movs_imm_test_led_lbl

    # R1 = 0x000000FF
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xff
    MOVS R0, #0xFF
    CMP  R0, R1
    BNE  movs_imm_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # R1 = 0x00000000 => NZCV = 0100
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVS R1, #0
    BMI  movs_imm_test_led_lbl
    BNE  movs_imm_test_led_lbl
    BCS  movs_imm_test_led_lbl
    BVS  movs_imm_test_led_lbl

    # R1 = 0x000000FF => NZCV = 0000
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVS R1, #0xFF
    BMI  movs_imm_test_led_lbl
    BEQ  movs_imm_test_led_lbl
    BCS  movs_imm_test_led_lbl
    BVS  movs_imm_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

movs_imm_test_led_lbl:
    MOVW R0, 17
    BX LR


/* muls_test
 *
 * Tests the Multiply with condition set instruction
 */

.thumb_func
muls_test:
# Test multiply
    # 0x00000002 * 0x00000005 = 0x0000000A
    MOVW R0, 0x2
    MOVW R1, 0x5
    MOVW R2, 0xa
    MULS R0, R1, R0
    CMP  R0, R2
    BNE  muls_test_led_lbl

    # 0x00000000 * 0x00000000 = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    MULS R0, R1, R0
    CMP  R0, R2
    BNE  muls_test_led_lbl

    # 0xFFFFFFFF * 0xFFFFFFFF = 0x00000001 (0xFFFFFFFE00000001)
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0x1
    MULS R0, R1, R0
    CMP  R0, R2
    BNE  muls_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0xFFFFFFFF * 0x00000001 => NZCV = 1000
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x1
    MULS R0, R1, R0
    BPL  muls_test_led_lbl
    BEQ  muls_test_led_lbl
    BCS  muls_test_led_lbl
    BVS  muls_test_led_lbl

    # 0xFFFFFFFF * 0x00000000 => NZCV = 0100
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x0
    MOVT R1, 0x0
    MULS R0, R1, R0
    BMI  muls_test_led_lbl
    BNE  muls_test_led_lbl
    BCS  muls_test_led_lbl
    BVS  muls_test_led_lbl

    # 0x00000001 * 0x00000001 => NZCV = 0000
    MOVW R0, 0x1
    MOVW R1, 0x1
    MULS R0, R1, R0
    BMI  muls_test_led_lbl
    BEQ  muls_test_led_lbl
    BCS  muls_test_led_lbl
    BVS  muls_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

muls_test_led_lbl:
    MOVW R0, 18
    BX LR


/* mvns_test
 *
 * Tests the Move Not with condition set instruction
 */

.thumb_func
mvns_test:
# Test move not
    # R0 = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0x0
    MOVT R2, 0x0
    MVNS R0, R1
    CMP  R0, R2
    BNE  mvns_test_led_lbl

    # R0 = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    MVNS R0, R1
    CMP  R0, R2
    BNE  mvns_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # R0 = 0x00000000 => NZCV = 0100
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MVNS R0, R1
    BMI  mvns_test_led_lbl
    BNE  mvns_test_led_lbl
    BCS  mvns_test_led_lbl
    BVS  mvns_test_led_lbl

    # R0 = 0x80000000 => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0x7fff
    MVNS R0, R1
    BPL  mvns_test_led_lbl
    BEQ  mvns_test_led_lbl
    BCS  mvns_test_led_lbl
    BVS  mvns_test_led_lbl

    # R0 = 0x7FFFFFFF => NZCV = 0000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x0
    MOVT R1, 0x8000
    MVNS R0, R1
    BMI  mvns_test_led_lbl
    BEQ  mvns_test_led_lbl
    BCS  mvns_test_led_lbl
    BVS  mvns_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

mvns_test_led_lbl:
    MOVW R0, 19
    BX LR


/* orrs_test
 *
 * Tests the Or with condition set instruction
 */

.thumb_func
orrs_test:
# Test or
    # 0x00000000 | 0xFFFFFFFF = 0xFFFFFFFF
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    ORRS R0, R0, R1
    CMP  R0, R2
    BNE  orrs_test_led_lbl

    # 0x00000000 | 0x00000000 = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    ORRS R0, R0, R1
    CMP  R0, R2
    BNE  orrs_test_led_lbl

    # 0xFFFFFFFF | 0x00000000 = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    ORRS R0, R0, R1
    CMP  R0, R2
    BNE  orrs_test_led_lbl

    # 0xFFFFFFFF | 0xFFFFFFFF = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    ORRS R0, R0, R1
    CMP  R0, R2
    BNE  orrs_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x00000000 | 0x00000000 => NZCV = 0100
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x0
    MOVT R1, 0x0
    ORRS R0, R0, R1
    BMI  orrs_test_led_lbl
    BNE  orrs_test_led_lbl
    BCS  orrs_test_led_lbl
    BVS  orrs_test_led_lbl

    # 0x00000000 | 0xFFFFFFFF => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    ORRS R0, R0, R1
    BPL  orrs_test_led_lbl
    BEQ  orrs_test_led_lbl
    BCS  orrs_test_led_lbl
    BVS  orrs_test_led_lbl

    # 0x00000001 | 0x00000000 => NZCV = 0000
    MOVW R0, 0x1
    MOVW R1, 0x0
    MOVT R1, 0x0
    ORRS R0, R0, R1
    BMI  orrs_test_led_lbl
    BEQ  orrs_test_led_lbl
    BCS  orrs_test_led_lbl
    BVS  orrs_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

orrs_test_led_lbl:
    MOVW R0, 20
    BX LR


/* rev_test
 *
 * Tests the Reverse instruction
 */

.thumb_func
rev_test:
# Test reverse
    # rev(0xDEADBEEF) = 0xEFBEADDE
    MOVW R0, 0xbeef
    MOVT R0, 0xdead
    MOVW R2, 0xadde
    MOVT R2, 0xefbe
    REV R0, R0
    CMP R0, R2
    BNE rev_test_led_lbl

    # rev(0x00000000) = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    REV R0, R0
    CMP R0, R2
    BNE rev_test_led_lbl

    # rev(0xFFFFFFFF) = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    REV R0, R0
    CMP R0, R2
    BNE rev_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # Flags should be unaffected
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    REV R0, R0
    BMI rev_test_led_lbl
    BEQ rev_test_led_lbl
    BCS rev_test_led_lbl
    BVS rev_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

rev_test_led_lbl:
    MOVW R0, 21
    BX LR


/* rev16_test
 *
 * Tests the Reverse Halfword instruction
 */

.thumb_func
rev16_test:
# Test reverse
    # rev(0xDEADBEEF) = 0xADDEEFBE
    MOVW R0, 0xbeef
    MOVT R0, 0xdead
    MOVW R2, 0xefbe
    MOVT R2, 0xadde
    REV16 R0, R0
    CMP   R0, R2
    BNE   rev16_test_led_lbl

    # rev(0x00000000) = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    REV16 R0, R0
    CMP   R0, R2
    BNE   rev16_test_led_lbl

    # rev(0xFFFFFFFF) = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    REV16 R0, R0
    CMP   R0, R2
    BNE   rev16_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # Flags should be unaffected
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    REV16 R0, R0
    BMI   rev16_test_led_lbl
    BEQ   rev16_test_led_lbl
    BCS   rev16_test_led_lbl
    BVS   rev16_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

rev16_test_led_lbl:
    MOVW R0, 22
    BX LR


/* revsh_test
 *
 * Tests the Reverse Signed Halfword instruction
 */

.thumb_func
revsh_test:
# Test reverse
    # rev(0xDEADBEEF) = 0xFFFFEFBE
    MOVW R0, 0xbeef
    MOVT R0, 0xdead
    MOVW R2, 0xefbe
    MOVT R2, 0xffff
    REVSH R0, R0
    CMP   R0, R2
    BNE   revsh_test_led_lbl

    # rev(0xFFFFFF7F) = 0x00007FFF
    MOVW R0, 0xff7f
    MOVT R0, 0xffff
    MOVW R2, 0x7fff
    MOVT R2, 0x0
    REVSH R0, R0
    CMP   R0, R2
    BNE   revsh_test_led_lbl

    # rev(0x00000000) = 0x00000000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    REV16 R0, R0
    CMP   R0, R2
    BNE   revsh_test_led_lbl

    # rev(0xFFFFFFFF) = 0xFFFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    REV16 R0, R0
    CMP   R0, R2
    BNE   revsh_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # Flags should be unaffected
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    REV16 R0, R0
    BMI   revsh_test_led_lbl
    BEQ   revsh_test_led_lbl
    BCS   revsh_test_led_lbl
    BVS   revsh_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

revsh_test_led_lbl:
    MOVW R0, 23
    BX LR


/* rors_test
 *
 * Tests the Rotate Right with condition set instruction
 */

.thumb_func
rors_test:
# Test right rotate
    # 0xDEADBEEF rot 4 = 0xFDEADBEE
    MOVW R0, 0xbeef
    MOVT R0, 0xdead
    MOVW R1, 0x4
    MOVW R2, 0xdbee
    MOVT R2, 0xfdea
    RORS R0, R0, R1
    CMP  R0, R2
    BNE  rors_test_led_lbl

    # 0x80000000 rot 3 = 0x10000000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x3
    MOVW R2, 0x0
    MOVT R2, 0x1000
    RORS R0, R0, R1
    CMP  R0, R2
    BNE  rors_test_led_lbl

    # 0xDEADBEEF rot 32 = 0xDEADBEEF
    MOVW R0, 0xbeef
    MOVT R0, 0xdead
    MOVW R1, 0x32
    MOVW R2, 0xbeef
    MOVT R2, 0xdead
    RORS R0, R0, R1
    CMP  R0, R2
    BNE  rors_test_led_lbl

    # 0x80000000 rot 35 = 0x10000000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x35
    MOVW R2, 0x0
    MOVT R2, 0x1000
    RORS R0, R0, R1
    CMP  R0, R2
    BNE  rors_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0xFFFFFFFF rot 1 => NZCV = 1010
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R1, 0x1
    RORS R0, R0, R1
    BPL  rors_test_led_lbl
    BEQ  rors_test_led_lbl
    BCC  rors_test_led_lbl
    BVS  rors_test_led_lbl

    # 0x00000000 rot 31 => NZCV = 0100
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x31
    RORS R0, R0, R1
    BMI  rors_test_led_lbl
    BNE  rors_test_led_lbl
    BCS  rors_test_led_lbl
    BVS  rors_test_led_lbl

    # 0x80000000 rot 1 => NZCV = 0000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x1
    RORS R0, R0, R1
    BMI  rors_test_led_lbl
    BEQ  rors_test_led_lbl
    BCS  rors_test_led_lbl
    BVS  rors_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

rors_test_led_lbl:
    MOVW R0, 24
    BX LR


/* rsbs_test
 *
 * Tests the Reverse Subtract with condition set instruction
 */

.thumb_func
rsbs_test:
# Test reverse subtract
    # 0 - 1 = 0xFFFFFFFF
    MOVW R0, 0x1
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    RSBS R0, R0, #0
    CMP  R0, R2
    BNE  rsbs_test_led_lbl

    # 0 - 0xFFFFFFFF = 1
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    MOVW R2, 0x1
    RSBS R0, R0, #0
    CMP  R0, R2
    BNE  rsbs_test_led_lbl

    # 0 - 0x80000000 = 0x80000000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R2, 0x0
    MOVT R2, 0x8000
    RSBS R0, R0, #0
    CMP  R0, R2
    BNE  rsbs_test_led_lbl

    # 0 - 0x7FFFFFFF = 0x80000001
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R2, 0x1
    MOVT R2, 0x8000
    RSBS R0, R0, #0
    CMP  R0, R2
    BNE  rsbs_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0 - 1 => NZCV = 1000
    MOVW R0, 0x1
    RSBS R0, R0, #0
    BPL  rsbs_test_led_lbl
    BEQ  rsbs_test_led_lbl
    BCS  rsbs_test_led_lbl
    BVS  rsbs_test_led_lbl

    # 0 - 0 => NZCV = 0110
    MOVW R0, 0x0
    MOVT R0, 0x0
    RSBS R0, R0, #0
    BMI  rsbs_test_led_lbl
    BNE  rsbs_test_led_lbl
    BCC  rsbs_test_led_lbl
    BVS  rsbs_test_led_lbl

    # 0 - 0x80000000 => NZCV = 1001
    MOVW R0, 0x0
    MOVT R0, 0x8000
    RSBS R0, R0, #0
    BPL  rsbs_test_led_lbl
    BEQ  rsbs_test_led_lbl
    BCS  rsbs_test_led_lbl
    BVC  rsbs_test_led_lbl

    # 0 - 0xFFFFFFFF => NZCV = 0000
    MOVW R0, 0xffff
    MOVT R0, 0xffff
    RSBS R0, R0, #0
    BMI  rsbs_test_led_lbl
    BEQ  rsbs_test_led_lbl
    BCS  rsbs_test_led_lbl
    BVS  rsbs_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

rsbs_test_led_lbl:
    MOVW R0, 25
    BX LR


/* sbcs_test
 *
 * Tests the Subtract with carry and condition set instruction
 *
 * Note: The GenericUserGuide is incorrect as to the functionality of this
 *  instruction. It is actually implemented as Rn-Rm-1+Carry. So a further one
 *  is deducted UNLESS the carry bit is SET.
 *
 * SBCS R0, R1, R2 -> R0 := R1 - R2 + C - 1
 */

.thumb_func
sbcs_test:
# Test subractions without carry
    # Zero out Carry bit (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0 - 1 (-1+0) = -2
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x1
    MOVW R2, 0xfffe
    MOVT R2, 0xffff
    SBCS R0, R0, R1
    CMP  R0, R2
    BNE  sbcs_test_led_lbl

    # Zero out Carry bit (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x80000000 - 1 (-1+0) = 0x7FFFFFFE
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x1
    MOVW R2, 0xfffe
    MOVT R2, 0x7fff
    SBCS R0, R0, R1
    CMP  R0, R2
    BNE  sbcs_test_led_lbl

    # Zero out Carry bit (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x00000008 - 1 (-1+0) = 0x00000006
    MOVW R0, 0x8
    MOVW R1, 0x1
    MOVW R2, 0x6
    SBCS R0, R0, R1
    CMP  R0, R2
    BNE  sbcs_test_led_lbl

# Test subractions with carry
    # Set carry bit to 1 (NZCV = 0010)
    MOVW R0, 0x1
    MOVW R1, 0x0
    MOVT R1, 0x0
    CMP R0, R1

    # 0 - 1 (-1+1) = -1
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x1
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    SBCS R0, R0, R1
    CMP  R0, R2
    BNE  sbcs_test_led_lbl

    # Set carry bit to 1 (NZCV = 0010)
    MOVW R0, 0x1
    MOVW R1, 0x0
    MOVT R1, 0x0
    CMP R0, R1

    # 0x80000000 - 1 (-1+1) = 0x7FFFFFFF
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x1
    MOVW R2, 0xffff
    MOVT R2, 0x7fff
    SBCS R0, R0, R1
    CMP  R0, R2
    BNE  sbcs_test_led_lbl

    # Set carry bit to 1 (NZCV = 0010)
    MOVW R0, 0x1
    MOVW R1, 0x0
    MOVT R1, 0x0
    CMP R0, R1

    # 0x00000008 - 1 (-1+1) = 0x00000007
    MOVW R0, 0x8
    MOVW R1, 0x1
    MOVW R2, 0x7
    SBCS R0, R0, R1
    CMP  R0, R2
    BNE  sbcs_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x80000000 - 1 (-1+0) => NZCV = 0011
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x1
    SBCS R0, R0, R1
    BMI  sbcs_test_led_lbl
    BEQ  sbcs_test_led_lbl
    BCC  sbcs_test_led_lbl
    BVC  sbcs_test_led_lbl

    # Set carry bit to 1 (NZCV = 0010)
    MOVW R0, 0x1
    MOVW R1, 0x0
    MOVT R1, 0x0
    CMP R0, R1

    # 1 - 1 (-1+1) => NZCV = 0110
    MOVW R0, 0x1
    MOVW R1, 0x1
    SBCS R0, R0, R1
    BMI  sbcs_test_led_lbl
    BNE  sbcs_test_led_lbl
    BCC  sbcs_test_led_lbl
    BVS  sbcs_test_led_lbl

    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # -2 - -2 (-1+0) => NZCV = 1000
    MOVW R0, 0xfffe
    MOVT R0, 0xffff
    MOVW R1, 0xfffe
    MOVT R1, 0xffff
    SBCS R0, R0, R1
    BPL  sbcs_test_led_lbl
    BEQ  sbcs_test_led_lbl
    BCS  sbcs_test_led_lbl
    BVS  sbcs_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

sbcs_test_led_lbl:
    MOVW R0, 26
    BX LR


/* sub_imm_test
 *
 * Tests the Subtract with immediate instruction
 *
 * Note: this test utilizes the ADD command which has previously been tested
 */

.thumb_func
sub_imm_test:
# Test subtraction & condition flags
    # Set NZCV bits (NZCV = 0110)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x0
    MOVT R1, 0x0
    CMP R0, R1

    # SP - 4 = 0x00001FF0
    # (SP = 0x00001FF4)
    MOVW R2, 0x1ff0
    MOVT R2, 0x0
    SUB SP, SP, #4

    # SP - 4 => should not affect NZCV
    BMI sub_imm_test_led_lbl
    BNE sub_imm_test_led_lbl
    BCC sub_imm_test_led_lbl
    BVS sub_imm_test_led_lbl

    # Test that value is correct
    CMP SP, R2
    BNE sub_imm_test_led_lbl

    # Fix the SP
    ADD SP, SP, #4

# Test Complete
    MOVW R0, 0
    BX LR

sub_imm_test_led_lbl:
    MOVW R0, 27
    BX LR


/* subs_test
 *
 * Tests the Subtract with condition set instruction
 */

.thumb_func
subs_test:
# Test subtraction
    # 0 - 1 = -1
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x1
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    SUBS R0, R0, R1
    CMP  R0, R2
    BNE  subs_test_led_lbl

    # 0x80000000 - 1 = 0x7FFFFFFF
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x1
    MOVW R2, 0xffff
    MOVT R2, 0x7fff
    SUBS R0, R0, R1
    CMP  R0, R2
    BNE  subs_test_led_lbl

    # 0x7FFFFFFF - -1 = 0x80000000
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0x0
    MOVT R2, 0x8000
    SUBS R0, R0, R1
    CMP  R0, R2
    BNE  subs_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x80000000 - 1 => NZCV = 0011
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0x1
    SUBS R0, R0, R1
    BMI  subs_test_led_lbl
    BEQ  subs_test_led_lbl
    BCC  subs_test_led_lbl
    BVC  subs_test_led_lbl

    # 1 - 1 => NZCV = 0110
    MOVW R0, 0x1
    MOVW R1, 0x1
    SUBS R0, R0, R1
    BMI  subs_test_led_lbl
    BNE  subs_test_led_lbl
    BCC  subs_test_led_lbl
    BVS  subs_test_led_lbl

    # 0 - 1 => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0x1
    SUBS R0, R0, R1
    BPL  subs_test_led_lbl
    BEQ  subs_test_led_lbl
    BCS  subs_test_led_lbl
    BVS  subs_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

subs_test_led_lbl:
    MOVW R0, 28
    BX LR


/* subs_imm_test
 *
 * Tests the Subtract with immediate and condition set instruction
 */

.thumb_func
subs_imm_test:
# Test subraction
    # 0 - 1 = -1
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    SUBS R0, R0, #1
    CMP  R0, R2
    BNE  subs_imm_test_led_lbl

    # 0x80000000 - 1 = 0x7FFFFFFF
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R2, 0xffff
    MOVT R2, 0x7fff
    SUBS R0, R0, #1
    CMP  R0, R2
    BNE  subs_imm_test_led_lbl

    # 0x7FFFFFFF - 0 = 0x7FFFFFFF
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R2, 0xffff
    MOVT R2, 0x7fff
    SUBS R0, R0, #0
    CMP  R0, R2
    BNE  subs_imm_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x80000000 - 1 => NZCV = 0011
    MOVW R0, 0x0
    MOVT R0, 0x8000
    SUBS R0, R0, #1
    BMI  subs_imm_test_led_lbl
    BEQ  subs_imm_test_led_lbl
    BCC  subs_imm_test_led_lbl
    BVC  subs_imm_test_led_lbl

    # 1 - 1 => NZCV = 0110
    MOVW R0, 0x1
    SUBS R0, R0, #1
    BMI  subs_imm_test_led_lbl
    BNE  subs_imm_test_led_lbl
    BCC  subs_imm_test_led_lbl
    BVS  subs_imm_test_led_lbl

    # 0 - 1 => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x0
    SUBS R0, R0, #1
    BPL  subs_imm_test_led_lbl
    BEQ  subs_imm_test_led_lbl
    BCS  subs_imm_test_led_lbl
    BVS  subs_imm_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

subs_imm_test_led_lbl:
    MOVW R0, 29
    BX LR


/* sxtb_test
 *
 * Tests the Sign Extend Byte instruction
 */

.thumb_func
sxtb_test:
# Test sign extend
    # sxtb(0x000000FF) -> 0xFFFFFFFF
    MOVW R1, 0xff
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    SXTB R0, R1
    CMP  R0, R2
    BNE  sxtb_test_led_lbl

    # sxtb(0x00000000) -> 0x00000000
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    SXTB R0, R1
    CMP  R0, R2
    BNE  sxtb_test_led_lbl

    # sxtb(0xFFFFFF7F) -> 0x0000007F
    MOVW R1, 0xff7f
    MOVT R1, 0xffff
    MOVW R2, 0x7f
    SXTB R0, R1
    CMP  R0, R2
    BNE  sxtb_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # Condition bits should be unaffected
    MOVW R1, 0x0
    MOVT R1, 0x0
    SXTB R0, R1
    BMI  sxtb_test_led_lbl
    BEQ  sxtb_test_led_lbl
    BCS  sxtb_test_led_lbl
    BVS  sxtb_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

sxtb_test_led_lbl:
    MOVW R0, 30
    BX LR


/* sxth_test
 *
 * Tests the Sign Extend Halfword instruction
 */

.thumb_func
sxth_test:
# Test sign extend
    # sxth(0x0000FFFF) -> 0xFFFFFFFF
    MOVW R1, 0xffff
    MOVT R1, 0x0
    MOVW R2, 0xffff
    MOVT R2, 0xffff
    SXTH R0, R1
    CMP  R0, R2
    BNE  sxth_test_led_lbl

    # sxth(0x00000000) -> 0x00000000
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    SXTH R0, R1
    CMP  R0, R2
    BNE  sxth_test_led_lbl

    # sxth(0xFFFF7FFF) -> 0x00007FFF
    MOVW R1, 0x7fff
    MOVT R1, 0xffff
    MOVW R2, 0x7fff
    MOVT R2, 0x0
    SXTH R0, R1
    CMP  R0, R2
    BNE  sxth_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # Condition bits should be unaffected
    MOVW R1, 0x0
    MOVT R1, 0x0
    SXTH R0, R1
    BMI  sxth_test_led_lbl
    BEQ  sxth_test_led_lbl
    BCS  sxth_test_led_lbl
    BVS  sxth_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

sxth_test_led_lbl:
    MOVW R0, 31
    BX LR


/* uxtb_test
 *
 * Tests the Zero Extend Byte instruction
 */

.thumb_func
uxtb_test:
# Test zero extend
    # uxtb(0x000000FF) -> 0x000000FF
    MOVW R1, 0xff
    MOVW R2, 0xff
    UXTB R0, R1
    CMP  R0, R2
    BNE  uxtb_test_led_lbl

    # uxtb(0x00000000) -> 0x00000000
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    UXTB R0, R1
    CMP  R0, R2
    BNE  uxtb_test_led_lbl

    # uxtb(0xFFFFFFFF) -> 0x000000FF
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0xff
    UXTB R0, R1
    CMP  R0, R2
    BNE  uxtb_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # Condition bits should be unaffected
    MOVW R1, 0x0
    MOVT R1, 0x0
    UXTB R0, R1
    BMI  uxtb_test_led_lbl
    BEQ  uxtb_test_led_lbl
    BCS  uxtb_test_led_lbl
    BVS  uxtb_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

uxtb_test_led_lbl:
    MOVW R0, 32
    BX LR


/* uxth_test
 *
 * Tests the Zero Extend Halfword instruction
 */

.thumb_func
uxth_test:
# Test zero extend
    # uxth(0x0000FFFF) -> 0x0000FFFF
    MOVW R1, 0xffff
    MOVT R1, 0x0
    MOVW R2, 0xffff
    MOVT R2, 0x0
    UXTH R0, R1
    CMP  R0, R2
    BNE  uxth_test_led_lbl

    # uxth(0x00000000) -> 0x00000000
    MOVW R1, 0x0
    MOVT R1, 0x0
    MOVW R2, 0x0
    MOVT R2, 0x0
    UXTH R0, R1
    CMP  R0, R2
    BNE  uxth_test_led_lbl

    # uxth(0xFFFFFFFF) -> 0x0000FFFF
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    MOVW R2, 0xffff
    MOVT R2, 0x0
    UXTH R0, R1
    CMP  R0, R2
    BNE  uxth_test_led_lbl

# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # Condition bits should be unaffected
    MOVW R1, 0x0
    MOVT R1, 0x0
    UXTH R0, R1
    BMI  uxth_test_led_lbl
    BEQ  uxth_test_led_lbl
    BCS  uxth_test_led_lbl
    BVS  uxth_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

uxth_test_led_lbl:
    MOVW R0, 33
    BX LR


/* tst_test
 *
 * Tests the Test Bits instruction
 */

.thumb_func
tst_test:
# Test condition flags
    # Zero out NZCV bits (NZCV = 0000)
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    CMP R0, R1

    # 0x00000000 & 0xFFFFFFFF => NZCV = 0100
    MOVW R0, 0x0
    MOVT R0, 0x0
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    TST R0, R1
    BMI tst_test_led_lbl
    BNE tst_test_led_lbl
    BCS tst_test_led_lbl
    BVS tst_test_led_lbl

    # 0x80000000 & 0xFFFFFFFF => NZCV = 1000
    MOVW R0, 0x0
    MOVT R0, 0x8000
    MOVW R1, 0xffff
    MOVT R1, 0xffff
    TST R0, R1
    BPL tst_test_led_lbl
    BEQ tst_test_led_lbl
    BCS tst_test_led_lbl
    BVS tst_test_led_lbl

    # 0x7FFFFFFF & 0x7FFFFFFF => NZCV = 0000
    MOVW R0, 0xffff
    MOVT R0, 0x7fff
    MOVW R1, 0xffff
    MOVT R1, 0x7fff
    TST R0, R1
    BMI tst_test_led_lbl
    BEQ tst_test_led_lbl
    BCS tst_test_led_lbl
    BVS tst_test_led_lbl

# Test Complete
    MOVW R0, 0
    BX LR

tst_test_led_lbl:
    MOVW R0, 34
    BX LR


/* alu_instruction_test
 *
 * Calls all ALU test functions
 */
.thumb_func
.global alu_instruction_test
alu_instruction_test:
    BL adcs_test
    CMP R0, 0
    BNE alu_error
    BL add_imm_test
    CMP R0, 0
    BNE alu_error
    BL adds_test
    CMP R0, 0
    BNE alu_error
    BL adds_imm_test
    CMP R0, 0
    BNE alu_error
    BL ands_test
    CMP R0, 0
    BNE alu_error
    BL asrs_test
    CMP R0, 0
    BNE alu_error
    BL asrs_imm_test
    CMP R0, 0
    BNE alu_error
    BL bics_test
    CMP R0, 0
    BNE alu_error
    BL cmn_test
    CMP R0, 0
    BNE alu_error
    BL eors_test
    CMP R0, 0
    BNE alu_error
    BL lsls_test
    CMP R0, 0
    BNE alu_error
    BL lsls_imm_test
    CMP R0, 0
    BNE alu_error
    BL lsrs_test
    CMP R0, 0
    BNE alu_error
    BL lsrs_imm_test
    CMP R0, 0
    BNE alu_error
    BL mov_test
    CMP R0, 0
    BNE alu_error
    BL movs_test
    CMP R0, 0
    BNE alu_error
    BL movs_imm_test
    CMP R0, 0
    BNE alu_error
    BL muls_test
    CMP R0, 0
    BNE alu_error
    BL mvns_test
    CMP R0, 0
    BNE alu_error
    BL orrs_test
    CMP R0, 0
    BNE alu_error
    BL rev_test
    CMP R0, 0
    BNE alu_error
    BL rev16_test
    CMP R0, 0
    BNE alu_error
    BL revsh_test
    CMP R0, 0
    BNE alu_error
    BL rors_test
    CMP R0, 0
    BNE alu_error
    BL rsbs_test
    CMP R0, 0
    BNE alu_error
    BL sbcs_test
    CMP R0, 0
    BNE alu_error
    BL sub_imm_test
    CMP R0, 0
    BNE alu_error
    BL subs_test
    CMP R0, 0
    BNE alu_error
    BL subs_imm_test
    CMP R0, 0
    BNE alu_error
    BL sxtb_test
    CMP R0, 0
    BNE alu_error
    BL sxth_test
    CMP R0, 0
    BNE alu_error
    BL uxtb_test
    CMP R0, 0
    BNE alu_error
    BL uxth_test
    CMP R0, 0
    BNE alu_error
    BL tst_test
    CMP R0, 0
    BNE alu_error
    BX LR


.thumb_func
alu_error:
    B alu_error
