.syntax unified

.thumb_func
.global main
main:
	MOVW R0, 0xffff
	MOVT R0, 0xffff
	MOVS R1, 0
	MOVS R2, 1

	// Clear NZCV
	CMP R1, R1

	ADCS R3, R1, 0

	// Expect N=0,Z=1,C=0,V=0
	BMI adcs_imm_t1_fail_0P0_flags	// N==0
	BNE adcs_imm_t1_fail_0P0_flags	// Z==1
	BCS adcs_imm_t1_fail_0P0_flags	// C==0
	BVS adcs_imm_t1_fail_0P0_flags	// V==0

	// Expect R3==0
	CMP R3, 0
	BNE adcs_imm_t1_fail_0P0_val


	// Clear NZCV
	CMP R0, R0

	ADCS R3, R2, 0

	// Expect N=0,Z=0,C=0,V=0
	BMI adcs_imm_t1_fail_1P0_flags	// N==0
	BEQ adcs_imm_t1_fail_1P0_flags	// Z==0
	BCS adcs_imm_t1_fail_1P0_flags	// C==0
	BVS adcs_imm_t1_fail_1P0_flags	// V==0

	// Expect R3==1
	CMP R3, 1
	BNE adcs_imm_t1_fail_1P0_val

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
