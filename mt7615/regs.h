/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (C) 2019 MediaTek Inc. */

#ifndef __MT7615_REGS_H
#define __MT7615_REGS_H

#define MT_HW_REV			0x1000
#define MT_HW_CHIPID			0x1008
#define MT_TOP_MISC2			0x1134
#define MT_TOP_MISC2_FW_STATE		GENMASK(2, 0)

#define MT_MCU_BASE			0x2000
#define MT_MCU(ofs)			(MT_MCU_BASE + (ofs))

#define MT_MCU_PCIE_REMAP_1		MT_MCU(0x500)
#define MT_MCU_PCIE_REMAP_1_OFFSET	GENMASK(17, 0)
#define MT_MCU_PCIE_REMAP_1_BASE	GENMASK(31, 18)
#define MT_PCIE_REMAP_BASE_1		0x40000

#define MT_MCU_PCIE_REMAP_2		MT_MCU(0x504)
#define MT_MCU_PCIE_REMAP_2_OFFSET	GENMASK(18, 0)
#define MT_MCU_PCIE_REMAP_2_BASE	GENMASK(31, 19)
#define MT_PCIE_REMAP_BASE_2		0x80000

#define MT_HIF_BASE			0x4000
#define MT_HIF(ofs)			(MT_HIF_BASE + (ofs))

#define MT_CFG_LPCR_HOST		MT_HIF(0x1f0)
#define MT_CFG_LPCR_HOST_FW_OWN		BIT(0)
#define MT_CFG_LPCR_HOST_DRV_OWN	BIT(1)

#define MT_INT_SOURCE_CSR		MT_HIF(0x200)
#define MT_INT_MASK_CSR			MT_HIF(0x204)
#define MT_DELAY_INT_CFG		MT_HIF(0x210)

#define MT_INT_RX_DONE(_n)		BIT(_n)
#define MT_INT_RX_DONE_ALL		GENMASK(1, 0)
#define MT_INT_TX_DONE_ALL		GENMASK(7, 4)
#define MT_INT_TX_DONE(_n)		BIT((_n) + 4)

#define MT_INT_RX_COHERENT		BIT(20)
#define MT_INT_TX_COHERENT		BIT(21)

#define MT_WPDMA_GLO_CFG		MT_HIF(0x208)
#define MT_WPDMA_GLO_CFG_TX_DMA_EN	BIT(0)
#define MT_WPDMA_GLO_CFG_TX_DMA_BUSY	BIT(1)
#define MT_WPDMA_GLO_CFG_RX_DMA_EN	BIT(2)
#define MT_WPDMA_GLO_CFG_RX_DMA_BUSY	BIT(3)
#define MT_WPDMA_GLO_CFG_DMA_BURST_SIZE	GENMASK(5, 4)
#define MT_WPDMA_GLO_CFG_TX_WRITEBACK_DONE	BIT(6)
#define MT_WPDMA_GLO_CFG_BIG_ENDIAN	BIT(7)
#define MT_WPDMA_GLO_CFG_TX_BT_SIZE_BIT0	BIT(9)
#define MT_WPDMA_GLO_CFG_MULTI_DMA_EN	GENMASK(11, 10)
#define MT_WPDMA_GLO_CFG_FIFO_LITTLE_ENDIAN	BIT(12)
#define MT_WPDMA_GLO_CFG_TX_BT_SIZE_BIT21	GENMASK(23, 22)
#define MT_WPDMA_GLO_CFG_SW_RESET	BIT(24)
#define MT_WPDMA_GLO_CFG_FORCE_TX_EOF	BIT(25)
#define MT_WPDMA_GLO_CFG_FIRST_TOKEN_ONLY	BIT(26)
#define MT_WPDMA_GLO_CFG_OMIT_RX_INFO	BIT(27)
#define MT_WPDMA_GLO_CFG_OMIT_TX_INFO	BIT(28)
#define MT_WPDMA_GLO_CFG_RX_2B_OFFSET	BIT(31)

#define MT_WPDMA_RST_IDX		MT_HIF(0x20c)

#define MT_TX_RING_BASE			MT_HIF(0x300)
#define MT_RX_RING_BASE			MT_HIF(0x400)

#define MT_WPDMA_GLO_CFG1		MT_HIF(0x500)
#define MT_WPDMA_TX_PRE_CFG		MT_HIF(0x510)
#define MT_WPDMA_RX_PRE_CFG		MT_HIF(0x520)
#define MT_WPDMA_ABT_CFG		MT_HIF(0x530)
#define MT_WPDMA_ABT_CFG1		MT_HIF(0x534)

#define MT_WF_PHY_BASE			0x10000
#define MT_WF_PHY(ofs)			(MT_WF_PHY_BASE + (ofs))

#define MT_WF_PHY_WF2_RFCTRL0		MT_WF_PHY(0x1900)
#define MT_WF_PHY_WF2_RFCTRL0_LPBCN_EN	BIT(9)

#define MT_WF_CFG_BASE			0x20200
#define MT_WF_CFG(ofs)			(MT_WF_CFG_BASE + (ofs))

#define MT_CFG_CCR			MT_WF_CFG(0x000)
#define MT_CFG_CCR_MAC_D1_1X_GC_EN	BIT(24)
#define MT_CFG_CCR_MAC_D0_1X_GC_EN	BIT(25)
#define MT_CFG_CCR_MAC_D1_2X_GC_EN	BIT(30)
#define MT_CFG_CCR_MAC_D0_2X_GC_EN	BIT(31)

#define MT_WF_AGG_BASE			0x20a00
#define MT_WF_AGG(ofs)			(MT_WF_AGG_BASE + (ofs))

#define MT_AGG_PCR1_RTS			MT_WF_AGG(0x05c)
#define MT_AGG_PCR1_RTS_LEN_THR		GENMASK(19, 0)
#define MT_AGG_PCR1_RTS_PKT_THR		GENMASK(31, 25)

#define MT_AGG_SCR			MT_WF_AGG(0x0fc)
#define MT_AGG_SCR_NLNAV_MID_PTEC_DIS	BIT(3)

#define MT_WF_DMA_BASE			0x21800
#define MT_WF_DMA(ofs)			(MT_WF_DMA_BASE + (ofs))

#define MT_DMA_DCR0			MT_WF_DMA(0x000)
#define MT_DMA_DCR0_RX_HDR_TRANS_EN	BIT(19)

#define MT_WF_TMAC_BASE			0x21000
#define MT_WF_TMAC(ofs)			(MT_WF_TMAC_BASE + (ofs))

#define MT_TMAC_CTCR0			MT_WF_TMAC(0x0f4)
#define MT_TMAC_CTCR0_INS_DDLMT_REFTIME	GENMASK(5, 0)
#define MT_TMAC_CTCR0_INS_DDLMT_DENSITY	GENMASK(15, 12)
#define MT_TMAC_CTCR0_INS_DDLMT_EN	BIT(17)
#define MT_TMAC_CTCR0_INS_DDLMT_VHT_SMPDU_EN	BIT(18)

#define MT_MIB_BASE			0x24800
#define MT_MIB(_n)			(MT_MIB_BASE + (_n))

#define MT_MIB_M0SDR0			MT_MIB(0x10)
#define MT_MIB_M0SDR0_BCN_TXCNT		GENMASK(15, 0)

#define MT_MIB_M0DR0			MT_MIB(0xa0)
#define MT_MIB_M0DR0_20MHZ_TXCNT	GENMASK(15, 0)
#define MT_MIB_M0DR0_40MHZ_TXCNT	GENMASK(31, 16)

#define MT_EFUSE_BASE			0x81070000
#define MT_EFUSE_BASE_CTRL		0x000
#define MT_EFUSE_BASE_CTRL_EMPTY	BIT(30)

#define MT_EFUSE_CTRL			0x008
#define MT_EFUSE_CTRL_AOUT		GENMASK(5, 0)
#define MT_EFUSE_CTRL_MODE		GENMASK(7, 6)
#define MT_EFUSE_CTRL_LDO_OFF_TIME	GENMASK(13, 8)
#define MT_EFUSE_CTRL_LDO_ON_TIME	GENMASK(15, 14)
#define MT_EFUSE_CTRL_AIN		GENMASK(25, 16)
#define MT_EFUSE_CTRL_VALID		BIT(29)
#define MT_EFUSE_CTRL_KICK		BIT(30)
#define MT_EFUSE_CTRL_SEL		BIT(31)

#define MT_EFUSE_WDATA(_i)		(0x010 + ((_i) * 4))
#define MT_EFUSE_RDATA(_i)		(0x030 + ((_i) * 4))

#endif