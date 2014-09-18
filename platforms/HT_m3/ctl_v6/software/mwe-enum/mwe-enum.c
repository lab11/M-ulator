#include "m3_proc.h"
#define SNS_ADDR 0x4

int main() {
  uint32_t _sns_r0 = (0x0<<22)|(0x1<<21)|(0x6<<18)|(0x6<<15)|(0x0<<14)|(0x0<<12)|(0x4<<9)|(0x1<<8)|(0x1<<7)|(0x0<<6)|(0x1<<5)|(0x0<<4)|(0x7<<1)|(0x0<<0);
  _sns_r0 &= ~(0x1<<21);
  enumerate(SNS_ADDR);
  while (1){
    write_mbus_register(SNS_ADDR,0,_sns_r0);
  }
}
