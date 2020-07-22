// Define where the top of memory is.
#define TOP_OF_RAM 0x1FFCU

extern int  main(void); // Use C-library initialization function.
extern void nmi_handler(void);
extern void irq0_handler(void);

__attribute__ ((section("__Vectors")))
static void (* const vector_table[])(void) =
{
    (void (*)(void)) TOP_OF_RAM,    //0x00: Stack Pointer
    (void (*)(void)) main,          //0x04: Reset Handler
    (void (*)(void)) nmi_handler,   //0x08: NMI Handler
    (void (*)(void)) 0,             //0x0C: HardFault Handler
    (void (*)(void)) 0,             //0x10: reserved
    (void (*)(void)) 0,             //0x14: reserved
    (void (*)(void)) 0,             //0x18: reserved
    (void (*)(void)) 0,             //0x1C: reserved
    (void (*)(void)) 0,             //0x20: reserved
    (void (*)(void)) 0,             //0x24: reserved
    (void (*)(void)) 0,             //0x28: reserved
    (void (*)(void)) 0,             //0x2C: SVCall Handler
    (void (*)(void)) 0,             //0x30: reserved
    (void (*)(void)) 0,             //0x34: reserved
    (void (*)(void)) 0,             //0x38: PendSV
    (void (*)(void)) 0,             //0x3C: SysTick (Reserved)
    (void (*)(void)) irq0_handler,  //0x40: IRQ0  Handler
    (void (*)(void)) 0,             //0x44: IRQ1  Handler
    (void (*)(void)) 0,             //0x48: IRQ2  Handler
    (void (*)(void)) 0,             //0x4C: IRQ3  Handler
    (void (*)(void)) 0,             //0x50: IRQ4  Handler
    (void (*)(void)) 0,             //0x54: IRQ5  Handler
    (void (*)(void)) 0,             //0x58: IRQ6  Handler
    (void (*)(void)) 0,             //0x5C: IRQ7  Handler
    (void (*)(void)) 0,             //0x60: IRQ8  Handler
    (void (*)(void)) 0,             //0x64: IRQ9  Handler
    (void (*)(void)) 0,             //0x68: IRQ10 Handler
    (void (*)(void)) 0,             //0x6C: IRQ11 Handler
    (void (*)(void)) 0,             //0x70: IRQ12 Handler
    (void (*)(void)) 0,             //0x74: IRQ13 Handler
    (void (*)(void)) 0,             //0x78: IRQ14 Handler
    (void (*)(void)) 0,             //0x7C: IRQ15 Handler
};
