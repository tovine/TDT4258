//#include <stdint.h>

// GPIO

#define GPIO_PA_BASE 0x40006000
#define GPIO_PB_BASE 0x40006024
#define GPIO_PC_BASE 0x40006048
#define GPIO_INT_BASE 0x40006100


#define CTRL_OFFSET 0x00
#define MODEL_OFFSET 0x04
#define MODEH_OFFSET 0x08
#define DOUT_OFFSET 0x0c
#define DOUTSET_OFFSET 0x10
#define DOUTCLR_OFFSET 0x14
#define DOUTTGL_OFFSET 0x18
#define DIN_OFFSET 0x1c
#define PINLOCKN_OFFSET 0x20

#define GPIO_PA_CTRL     ((volatile uint32_t*)(GPIO_PA_BASE + CTRL_OFFSET))
#define GPIO_PA_MODEL    ((volatile uint32_t*)(GPIO_PA_BASE + MODEL_OFFSET))
#define GPIO_PA_MODEH    ((volatile uint32_t*)(GPIO_PA_BASE + MODEH_OFFSET))
#define GPIO_PA_DOUT     ((volatile uint32_t*)(GPIO_PA_BASE + DOUT_OFFSET))
#define GPIO_PA_DOUTSET  ((volatile uint32_t*)(GPIO_PA_BASE + DOUTSET_OFFSET))
#define GPIO_PA_DOUTCLR  ((volatile uint32_t*)(GPIO_PA_BASE + DOUTCLR_OFFSET))
#define GPIO_PA_DOUTTGL  ((volatile uint32_t*)(GPIO_PA_BASE + DOUTGL_OFFSET))
#define GPIO_PA_DIN      ((volatile uint32_t*)(GPIO_PA_BASE + DIN_OFFSET))
#define GPIO_PA_PINLOCKN ((volatile uint32_t*)(GPIO_PA_BASE + PINLOCKN_OFFSET))

#define GPIO_PB_CTRL     ((volatile uint32_t*)(GPIO_PB_BASE + CTRL_OFFSET))
#define GPIO_PB_MODEL    ((volatile uint32_t*)(GPIO_PB_BASE + MODEL_OFFSET))
#define GPIO_PB_MODEH    ((volatile uint32_t*)(GPIO_PB_BASE + MODEH_OFFSET))
#define GPIO_PB_DOUT     ((volatile uint32_t*)(GPIO_PB_BASE + DOUT_OFFSET))
#define GPIO_PB_DOUTSET  ((volatile uint32_t*)(GPIO_PB_BASE + DOUTSET_OFFSET))
#define GPIO_PB_DOUTCLR  ((volatile uint32_t*)(GPIO_PB_BASE + DOUTCLR_OFFSET))
#define GPIO_PB_DOUTTGL  ((volatile uint32_t*)(GPIO_PB_BASE + DOUTGL_OFFSET))
#define GPIO_PB_DIN      ((volatile uint32_t*)(GPIO_PB_BASE + DIN_OFFSET))
#define GPIO_PB_PINLOCKN ((volatile uint32_t*)(GPIO_PB_BASE + PINLOCKN_OFFSET))

#define GPIO_PC_CTRL     ((volatile uint32_t*)(GPIO_PC_BASE + CTRL_OFFSET))
#define GPIO_PC_MODEL    ((volatile uint32_t*)(GPIO_PC_BASE + MODEL_OFFSET))
#define GPIO_PC_MODEH    ((volatile uint32_t*)(GPIO_PC_BASE + MODEH_OFFSET))
#define GPIO_PC_DOUT     ((volatile uint32_t*)(GPIO_PC_BASE + DOUT_OFFSET))
#define GPIO_PC_DOUTSET  ((volatile uint32_t*)(GPIO_PC_BASE + DOUTSET_OFFSET))
#define GPIO_PC_DOUTCLR  ((volatile uint32_t*)(GPIO_PC_BASE + DOUTCLR_OFFSET))
#define GPIO_PC_DOUTTGL  ((volatile uint32_t*)(GPIO_PC_BASE + DOUTGL_OFFSET))
#define GPIO_PC_DIN      ((volatile uint32_t*)(GPIO_PC_BASE + DIN_OFFSET))
#define GPIO_PC_PINLOCKN ((volatile uint32_t*)(GPIO_PC_BASE + PINLOCKN_OFFSET))

#define EXTIPSELL_OFFSET 0x00
#define EXTIPSELH_OFFSET 0x04
#define EXTIRISE_OFFSET 0x08
#define EXTIFALL_OFFSET 0x0c
#define IEN_OFFSET 0x10
#define IFC_OFFSET 0x1c

#define GPIO_EXTIPSELL ((volatile uint32_t*)(GPIO_INT_BASE + EXTIPSELL_OFFSET))
#define GPIO_EXTIPSELH ((volatile uint32_t*)(GPIO_INT_BASE + EXTIPSELH_OFFSET))
#define GPIO_EXTIRISE  ((volatile uint32_t*)(GPIO_INT_BASE + EXTIRISE_OFFSET))
#define GPIO_EXTIFALL  ((volatile uint32_t*)(GPIO_INT_BASE + EXTIFALL_OFFSET))
#define GPIO_IEN       ((volatile uint32_t*)(GPIO_INT_BASE + IEN_OFFSET))
#define GPIO_IFC       ((volatile uint32_t*)(GPIO_INT_BASE + IFC_OFFSET))

// CMU

#define CMU_BASE2 0x400c8000

#define CMU_HFPERCLKDIV  ((volatile uint32_t*)(CMU_BASE2 + 0x008))
#define CMU_HFCORECLKEN0 ((volatile uint32_t*)(CMU_BASE2 + 0x040))
#define CMU_HFPERCLKEN0  ((volatile uint32_t*)(CMU_BASE2 + 0x044))
#define CMU_CMD          ((volatile uint32_t*)(CMU_BASE2 + 0x024))

#define CMU2_HFPERCLKEN0_DAC0   (1 << 17)
#define CMU2_HFPERCLKEN0_PRS    (1 << 15)
#define CMU2_HFPERCLKEN0_GPIO   (1 << 13)
#define CMU2_HFPERCLKEN0_TIMER1 (1 << 6)

#define CMU_HFCORECLKEN0_DMA (1 << 0)

// TIMER1

#define TIMER1_BASE 0x40010400

#define TIMER1_CMD ((volatile uint32_t*)(TIMER1_BASE + 0x04))
#define TIMER1_IEN ((volatile uint32_t*)(TIMER1_BASE + 0x0c))
#define TIMER1_IFC ((volatile uint32_t*)(TIMER1_BASE + 0x18))
#define TIMER1_TOP ((volatile uint32_t*)(TIMER1_BASE + 0x1c))
#define TIMER1_CNT ((volatile uint32_t*)(TIMER1_BASE + 0x24))

// NVIC

#define ISER0 ((volatile uint32_t*)0xe000e100)
#define ISER1 ((volatile uint32_t*)0xe000e104)
#define ICER0 ((volatile uint32_t*)0xe000e180)
#define ICER1 ((volatile uint32_t*)0xe000e184)
#define ISPR0 ((volatile uint32_t*)0xe000e200)
#define ISPR1 ((volatile uint32_t*)0xe000e204)
#define ICPR0 ((volatile uint32_t*)0xe000e280)
#define ICPR1 ((volatile uint32_t*)0xe000e284)
#define IABR0 ((volatile uint32_t*)0xe000e300)
#define IABR1 ((volatile uint32_t*)0xe000e304)

// IPR

#define IPR_BASE  0xe000e400

#define IPR0 ((volatile uint32_t*)(IPR_BASE + 0x00))
#define IPR1 ((volatile uint32_t*)(IPR_BASE + 0x04))
#define IPR2 ((volatile uint32_t*)(IPR_BASE + 0x08))
#define IPR3 ((volatile uint32_t*)(IPR_BASE + 0x0c))

// EMU

#define EMU_BASE2 0x400c6000

#define EMU_CTRL ((volatile uint32_t*)(EMU_BASE2 + CTRL_OFFSET0))

// DAC0

#define DAC0_BASE2 0x40004000

#define DAC0_CTRL     ((volatile uint32_t*)(DAC0_BASE2 + CTRL_OFFSET0))
#define DAC0_CH0CTRL  ((volatile uint32_t*)(DAC0_BASE2 + CTRL_OFFSET8))
#define DAC0_CH1CTRL  ((volatile uint32_t*)(DAC0_BASE2 + CTRL_OFFSETc))
#define DAC0_IEN      ((volatile uint32_t*)(DAC0_BASE2 + 0x010))
#define DAC0_IF       ((volatile uint32_t*)(DAC0_BASE2 + 0x014))
#define DAC0_IFS      ((volatile uint32_t*)(DAC0_BASE2 + 0x018))
#define DAC0_IFC      ((volatile uint32_t*)(DAC0_BASE2 + 0x01c))
#define DAC0_CH0DATA  ((volatile uint32_t*)(DAC0_BASE2 + 0x020))
#define DAC0_CH1DATA  ((volatile uint32_t*)(DAC0_BASE2 + 0x024))
#define DAC0_COMBDATA ((volatile uint32_t*)(DAC0_BASE2 + 0x028))

// DMA

#define DMA_BASE 0x400c2000

#define DMA_STATUS      ((volatile uint32_t*)(DMA_BASE + CTRL_OFFSET00))
#define DMA_CONFIG      ((volatile uint32_t*)(DMA_BASE + CTRL_OFFSET04))
#define DMA_CTRLBASE    ((volatile uint32_t*)(DMA_BASE + CTRL_OFFSET08))
#define DMA_ALTCTRLBASE ((volatile uint32_t*)(DMA_BASE + CTRL_OFFSET0c))
#define DMA_CHUSEBURSTS ((volatile uint32_t*)(DMA_BASE + CTRL_OFFSET18))
#define DMA_CHUSEBURSTC ((volatile uint32_t*)(DMA_BASE + CTRL_OFFSET1c))
#define DMA_REQMASKC    ((volatile uint32_t*)(DMA_BASE + CTRL_OFFSET24))
#define DMA_CHENS       ((volatile uint32_t*)(DMA_BASE + CTRL_OFFSET28))
#define DMA_CHALTC      ((volatile uint32_t*)(DMA_BASE + CTRL_OFFSET34))
#define DMA_IFC         ((volatile uint32_t*)(DMA_BASE + 0x1008))
#define DMA_IEN         ((volatile uint32_t*)(DMA_BASE + 0x100c))
#define DMA_CH0_CTRL    ((volatile uint32_t*)(DMA_BASE + 0x1100))

// PRS

#define PRS_BASE 0x400cc000

#define PRS_CH0_CTRL ((volatile uint32_t*)(PRS_BASE + 0x010))

// System Control Block

#define SCR          ((volatile uint32_t*)0xe000ed10)
#define SYSTICK_CTRL ((volatile uint32_t*)0xe000e010)
#define SYSTICK_LOAD ((volatile uint32_t*)0xe000e014)


