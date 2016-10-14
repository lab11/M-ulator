//****************************************************************************************************
//Author:       Gyouho Kim
//              ZhiYoong Foo
//Description:  Changes PMU sleep frequency for speeding up GOC
//****************************************************************************************************
#include "mbus.h"
#include "PRCv9E.h"
#include "HRVv1.h"
#include "RADv5.h"
#include "FLSv1_GPIO.h"

//***************************************************
// Global variables
//***************************************************

//************************************
// PMU Related Functions
//************************************
inline static void set_pmu_sleep_clk_low(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F770039; // 0x8F770039: use GOC x0.6-2
}
inline static void set_pmu_sleep_clk_default(){
    // PRCv9 Default: 0x8F770049
    *((volatile uint32_t *) 0xA200000C) = 0x8F770049; // 0x8F770049: use GOC x10-25
}
//inline static void set_pmu_sleep_clk_fastest(){
    // PRCv9 Default: 0x8F770049
//    *((volatile uint32_t *) 0xA200000C) = 0x8F770079; // 0x8F770079: use GOC x70-230
//}
inline static void set_pmu_sleep_clk_fastest(){
    // PRCv11 Default: 0x8F770049
    // 0x4F773879: use GOC x70-x230 for "poly" and "poly2"
    // 0x4F773879: use GOC x40-
	*((volatile uint32_t *) 0xA200000C) =
		( (0x0 << 31) /* PMU_DIV6_OVRD */
		| (0x1 << 30) /* PMU_DIV5_OVRD */
		| (0x0 << 28) /* PMU_LC_TYPE and PMU_SEL_HARV_SRC */
		| (0x3 << 26) /* PMU_WUTH */ | (0x3 << 24) /* PMU_HARV_CLK_SEL */
		| (0x7 << 20) /* PMU_HARV_TUNE_FRAC */ | (0x7 << 16) /* PMU_HARV_STOP_CNT */
		| (0x0 << 14) /* PMU_OSCACT_DIV[1:0] */
		| (0x7 << 11) /* PMU_OSCACT_SEL[2:0] */
		| (0x0 << 9) /* PMU_OSCGOC_DIV[1:0] */
		| (0x0 << 7) /* PMU_OSCSLP_DIV[1:0] */
		| (0x7 << 4) /* PMU_OSCSLP_SEL[2:0] */
		| (0x2 << 2)  /* PMU_OVCHG_SEL */ | (0x1 << 0)  /* PMU_SCNDIV_SEL_TUNE */
		);
}

//***************************************************
// End of Program Sleep Operation
//***************************************************
static void operation_sleep(void){

  // Reset wakeup counter
  // This is required to go back to sleep!!
  //set_wakeup_timer (0xFFF, 0x0, 0x1);
  *((volatile uint32_t *) 0xA2000014) = 0x1;

  // Reset IRQ10VEC
  *((volatile uint32_t *) IRQ10VEC/*IMSG0*/) = 0;

  // Go to Sleep
  sleep();
  while(1);

}

static void operation_sleep_notimer(void){
    
  // Disable Timer
  set_wakeup_timer (0, 0x0, 0x0);
  
  // Go to sleep without timer
  operation_sleep();
}


//***************************************************************************************
// MAIN function starts here             
//***************************************************************************************
int main() {
  
	// PRCv9: 0x8F770079
	// Use GOC x70-230
	// FOR WGOC only
	// For Light GOC, frontend setting has to be changed as well
    set_pmu_sleep_clk_fastest();
	delay(100);

	write_mbus_message(0xAA, 0x1234ABCD);
	delay(100);

    // Go to sleep without timer
    operation_sleep_notimer();

    while(1);
}

