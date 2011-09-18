#ifndef __LED_H__
#define __LED_H__

void LED_set(uint8_t bank, uint8_t led_no, uint8_t val);
uint8_t LED_get(uint8_t bank, uint8_t led_no);

/* TTTA: Why *4 ? */

#define RED_BANK (0*4)
#define GRN_BANK (1*4)
#define BLU_BANK (2*4)

/* TTTA: What's up with !!(_v)?
  _Hint_: What does !!(0), !!(1), and !!(5) evaluate to?
 */

#define red_LED0_set(_v) LED_set(RED_BANK, 0, !!(_v))
#define red_LED1_set(_v) LED_set(RED_BANK, 1, !!(_v))
#define red_LED2_set(_v) LED_set(RED_BANK, 2, !!(_v))
#define red_LED3_set(_v) LED_set(RED_BANK, 3, !!(_v))
#define red_LED4_set(_v) LED_set(RED_BANK, 4, !!(_v))
#define red_LED5_set(_v) LED_set(RED_BANK, 5, !!(_v))
#define red_LED6_set(_v) LED_set(RED_BANK, 6, !!(_v))
#define red_LED7_set(_v) LED_set(RED_BANK, 7, !!(_v))

#define grn_LED0_set(_v) LED_set(GRN_BANK, 0, !!(_v))
#define grn_LED1_set(_v) LED_set(GRN_BANK, 1, !!(_v))
#define grn_LED2_set(_v) LED_set(GRN_BANK, 2, !!(_v))
#define grn_LED3_set(_v) LED_set(GRN_BANK, 3, !!(_v))
#define grn_LED4_set(_v) LED_set(GRN_BANK, 4, !!(_v))
#define grn_LED5_set(_v) LED_set(GRN_BANK, 5, !!(_v))
#define grn_LED6_set(_v) LED_set(GRN_BANK, 6, !!(_v))
#define grn_LED7_set(_v) LED_set(GRN_BANK, 7, !!(_v))

#define blu_LED0_set(_v) LED_set(BLU_BANK, 0, !!(_v))
#define blu_LED1_set(_v) LED_set(BLU_BANK, 1, !!(_v))
#define blu_LED2_set(_v) LED_set(BLU_BANK, 2, !!(_v))
#define blu_LED3_set(_v) LED_set(BLU_BANK, 3, !!(_v))
#define blu_LED4_set(_v) LED_set(BLU_BANK, 4, !!(_v))
#define blu_LED5_set(_v) LED_set(BLU_BANK, 5, !!(_v))
#define blu_LED6_set(_v) LED_set(BLU_BANK, 6, !!(_v))
#define blu_LED7_set(_v) LED_set(BLU_BANK, 7, !!(_v))

#endif // __LED_H__
