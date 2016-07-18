.syntax unified

.thumb_func
.global main
main:
	MOVS R0, 0
	MOVS R1, 1
	MVNS R2, R0	// R2 = 0xffff ffff
	MOVS R4, 0
	MOVS R5, 1
	MOVS R6, R2

test_0P0:
	// Clear Carry
	CMP R0, R2

	ADCS.N R0, R4

	// Expect N=0,Z=1,C=0,V=0
	BMI adcs_reg_t2_fail_0P0_flags	// N==0
	BNE adcs_reg_t2_fail_0P0_flags	// Z==1
	BCS adcs_reg_t2_fail_0P0_flags	// C==0
	BVS adcs_reg_t2_fail_0P0_flags	// V==0

	// Expect R0==0
	CMP R0, 0
	BNE adcs_reg_t2_fail_0P0_val


test_1P0:
	// Clear Carry
	CMP R0, R2

	ADCS.N R1, R4

	// Expect N=0,Z=0,C=0,V=0
	BMI adcs_reg_t2_fail_1P0_flags	// N==0
	BEQ adcs_reg_t2_fail_1P0_flags	// Z==0
	BCS adcs_reg_t2_fail_1P0_flags	// C==0
	BVS adcs_reg_t2_fail_1P0_flags	// V==0

	// Expect R1==1
	CMP R1, 1
	BNE adcs_reg_t2_fail_1P0_val


test_maxP0:
	// Clear Carry
	CMP R0, R2

	ADCS.N R2, R4

	// Expect N=1,Z=0,C=0,V=0
	BPL adcs_reg_t2_fail_maxP0_flags	// N==1
	BEQ adcs_reg_t2_fail_maxP0_flags	// Z==0
	BCS adcs_reg_t2_fail_maxP0_flags	// C==0
	BVS adcs_reg_t2_fail_maxP0_flags	// V==0

	// Expect R2==0xffff ffff
	CMP R2, R6
	BNE adcs_reg_t2_fail_maxP0_val


test_maxP1:
	// Clear Carry
	CMP R0, R2

	ADCS.N R2, R5

	// Expect N=0,Z=1,C=1,V=1
	BMI adcs_reg_t2_fail_maxP1_flags	// N==0
	BNE adcs_reg_t2_fail_maxP1_flags	// Z==1
	BCC adcs_reg_t2_fail_maxP1_flags	// C==1
	BVC adcs_reg_t2_fail_maxP1_flags	// V==1

	// Expect R2==0
	CMP R2, 0
	BNE adcs_reg_t2_fail_maxP1_val

	// Restore R2
	MOVS R2, R6

success:
	// All passed
	MOVS R0, 0
	BX LR

adcs_reg_t2_fail_0P0_flags:
	MOVS R0, 1
	BX LR

adcs_reg_t2_fail_0P0_val:
	MOVS R0, 2
	BX LR

adcs_reg_t2_fail_1P0_flags:
	MOVS R0, 3
	BX LR

adcs_reg_t2_fail_1P0_val:
	MOVS R0, 4
	BX LR

adcs_reg_t2_fail_maxP0_flags:
	MOVS R0, 5
	BX LR

adcs_reg_t2_fail_maxP0_val:
	MOVS R0, 6
	BX LR

adcs_reg_t2_fail_maxP1_flags:
	MOVS R0, 7
	BX LR

adcs_reg_t2_fail_maxP1_val:
	MOVS R0, 8
	BX LR

