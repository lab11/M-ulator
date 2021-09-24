//Seokhyeon's Settings for implementing slow loop

pmu_reg_write(71, //0x47,
			  ( (1 << 19) // Enable RO-Assisted Transition
				| (0 << 18) // Enable SAR Slow-Loop in Active mode
				| (0 << 17) // Enable UPC Slow-Loop in Active mode
				| (0 << 16) // Enable DNC Slow-Loop in Active mode
				| (1 << 15) // Enable SAR Slow-Loop in Sleep mode
				| (1 << 14) // Enable UPC Slow-Loop in Sleep mode
				| (1 << 13) // Enable DNC Slow-Loop in Sleep mode
				| (1 << 11) // PRC ClokcGen Ring Tuning (2'h0: 13 stages / 2'h1: 11 stages / 2'h2: 9 stages / 2'h3: 7 stages)
				| (3 << 8) // Clock Divider Tuning for SAR Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
				| (4 << 5) // Clock Divider Tuning for UPC Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
				| (3 << 2) // Clock Divider Tuning for DNC Charge Pump Pull-Up (3'h0: divide by 1 / 3'h1: divide by 2 / 3'h2: divide by 4 / 3'h3: divide by 8 / 3'h4: divide by 16)
				| (3) // Clock Pre-Divider Tuning for UPC/DNC Charge Pump Pull-Up (2'h0: divide by 1 / 2'h1: divide by 2 / 2'h2: divide by 4 / 2'h3: divide by 8)
				));
