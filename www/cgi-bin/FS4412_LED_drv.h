
#ifndef _FS4412_LED_DRV_H
#define _FS4412_LED_DRV_H

	/*
	 * 	SFR memory map
	 */
#define GPIO_RIGHT_BASE 0x11000000U
#define GPIO_LEFT_BASE 0x11400000U
#define GPF3_BASE (GPIO_LEFT_BASE + 0x01E0U)
#define GPX1_BASE (GPIO_RIGHT_BASE + 0x0C20U)
#define GPX2_BASE (GPIO_RIGHT_BASE + 0x0C40U)

	/*
	 * 	SFR structures
	 */
typedef struct
{
	uint32_t CON;
	uint32_t DAT;
} GPIO_TypeDef;

	/*
	 * 	Bit operation
	 */

	/* Bit operation for GPF3_CON */
#define GPF3_CON_4_POS (16U)
#define GPF3_CON_4_MSK (0x0FU << GPF3_CON_4_POS)
#define GPF3_CON_4_CLR (~GPF3_CON_4_MSK)
#define GPF3_CON_4_OUT (0x01U << GPF3_CON_4_POS)

#define GPF3_CON_5_POS (20U)
#define GPF3_CON_5_MSK (0x0FU << GPF3_CON_5_POS)
#define GPF3_CON_5_CLR (~GPF3_CON_5_MSK)
#define GPF3_CON_5_OUT (0x01U << GPF3_CON_5_POS)

	/* Bit operation for GPX1_CON */
#define GPX1_CON_0_POS (0U)
#define GPX1_CON_0_MSK (0x0FU << GPX1_CON_0_POS)
#define GPX1_CON_0_CLR (~GPX1_CON_0_MSK)
#define GPX1_CON_0_OUT (0x01U << GPX1_CON_0_POS)

	/* Bit operation for GPX2_CON */
#define GPX2_CON_7_POS (28U)
#define GPX2_CON_7_MSK (0x0FU << GPX2_CON_7_POS)
#define GPX2_CON_7_CLR (~GPX2_CON_7_MSK)
#define GPX2_CON_7_OUT (0x01U << GPX2_CON_7_POS)

	/* Bit operation for GPF3_DAT */
#define GPF3_DAT_4_POS (4U)
#define GPF3_DAT_4_MSK (0x01U << GPF3_DAT_4_POS)
#define GPF3_DAT_4_CLR (~GPF3_DAT_4_MSK)

#define GPF3_DAT_5_POS (5U)
#define GPF3_DAT_5_MSK (0x01U << GPF3_DAT_5_POS)
#define GPF3_DAT_5_CLR (~GPF3_DAT_5_MSK)

	/* Bit operation for GPX1_DAT */
#define GPX1_DAT_0_POS (0U)
#define GPX1_DAT_0_MSK (0x01U << GPX1_DAT_0_POS)
#define GPX1_DAT_0_CLR (~GPX1_DAT_0_MSK)

	/* Bit operation for GPX2_DAT */
#define GPX2_DAT_7_POS (7U)
#define GPX2_DAT_7_MSK (0x01U << GPX2_DAT_7_POS)
#define GPX2_DAT_7_CLR (~GPX2_DAT_7_MSK)

#endif
