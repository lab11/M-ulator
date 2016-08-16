.syntax unified

.thumb_func
.global main
main:
	MOVS R0, 0
	MOVS R1, 1
	MOVW R2, 0xffff
	MOVT R2, 0xffff

test_0P0:
	// Clear Carry
	CMP R0, R2

	ADCS R3, R0, 0

	// Expect N=0,Z=1,C=0,V=0
	BMI adcs_imm_t1_fail_0P0_flags	// N==0
	BNE adcs_imm_t1_fail_0P0_flags	// Z==1
	BCS adcs_imm_t1_fail_0P0_flags	// C==0
	BVS adcs_imm_t1_fail_0P0_flags	// V==0

	// Expect R3==0
	CMP R3, 0
	BNE adcs_imm_t1_fail_0P0_val


test_1P0:
	// Clear Carry
	CMP R0, R2

	ADCS R3, R1, 0

	// Expect N=0,Z=0,C=0,V=0
	BMI adcs_imm_t1_fail_1P0_flags	// N==0
	BEQ adcs_imm_t1_fail_1P0_flags	// Z==0
	BCS adcs_imm_t1_fail_1P0_flags	// C==0
	BVS adcs_imm_t1_fail_1P0_flags	// V==0

	// Expect R3==1
	CMP R3, 1
	BNE adcs_imm_t1_fail_1P0_val


test_maxP0:
	// Clear Carry
	CMP R0, R2

	ADCS R3, R2, 0

	// Expect N=1,Z=0,C=0,V=0
	BPL adcs_imm_t1_fail_maxP0_flags	// N==1
	BEQ adcs_imm_t1_fail_maxP0_flags	// Z==0
	BCS adcs_imm_t1_fail_maxP0_flags	// C==0
	BVS adcs_imm_t1_fail_maxP0_flags	// V==0

	// Expect R3==max (R2)
	CMP R3, R2
	BNE adcs_imm_t1_fail_maxP0_val


test_maxP1:
	// Clear Carry
	CMP R0, R2

	ADCS R3, R2, 1

	// Expect N=0,Z=1,C=1,V=1
	BMI adcs_imm_t1_fail_maxP1_flags	// N==0
	BNE adcs_imm_t1_fail_maxP1_flags	// Z==1
	BCC adcs_imm_t1_fail_maxP1_flags	// C==1
	BVC adcs_imm_t1_fail_maxP1_flags	// V==1

	// Expect R3==0
	CMP R3, 0
	BNE adcs_imm_t1_fail_maxP1_val

success:
	// All passed
	MOVS R0, 0
	BX LR

adcs_imm_t1_fail_0P0_flags:
	MOVS R0, 1
	BX LR

adcs_imm_t1_fail_0P0_val:
	MOVS R0, 2
	BX LR

adcs_imm_t1_fail_1P0_flags:
	MOVS R0, 3
	BX LR

adcs_imm_t1_fail_1P0_val:
	MOVS R0, 4
	BX LR

adcs_imm_t1_fail_maxP0_flags:
	MOVS R0, 5
	BX LR

adcs_imm_t1_fail_maxP0_val:
	MOVS R0, 6
	BX LR

adcs_imm_t1_fail_maxP1_flags:
	MOVS R0, 7
	BX LR

adcs_imm_t1_fail_maxP1_val:
	MOVS R0, 8
	BX LR
