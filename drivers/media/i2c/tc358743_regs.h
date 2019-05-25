/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * tc358743 - Toshiba HDMI to CSI-2 bridge - register names and bit masks
 *
 * Copyright 2015 Cisco Systems, Inc. and/or its affiliates. All rights
 * reserved.
 */

/*
 * References (c = chapter, p = page):
 * REF_01 - Toshiba, TC358743XBG (H2C), Functional Specification, Rev 0.60
 */

/* Bit masks has prefix 'MASK_' and options after '_'. */

#ifndef __TC358743_REGS_H
#define __TC358743_REGS_H

#define CHIPID					0x0000
#define MASK_CHIPID				0xff00
#define MASK_REVID				0x00ff

#define SYSCTL					0x0002
#define MASK_ABRST				BIT(14) /* tc358749 */
#define MASK_SLMBRST				BIT(13) /* tc358749 */
#define MASK_VIPRST				BIT(12) /* tc358749 */
#define MASK_IRRST				BIT(11)
#define MASK_CECRST				BIT(10)
#define MASK_CTXRST				BIT(9)
#define MASK_HDMIRST				BIT(8)
#define MASK_I2SDIS				BIT(7) /* tc358749 */
#define MASK_SLEEP				BIT(0)

#define CONFCTL					0x0004
#define MASK_PWRISO				BIT(15)
#define MASK_SLMBEN				BIT(14)
#define MASK_ACLKOPT				BIT(12)
#define MASK_AUDCHNUM				(0x3 << 10)
#define MASK_AUDCHNUM_8				(0x0 << 10)
#define MASK_AUDCHNUM_6				(0x1 << 10)
#define MASK_AUDCHNUM_4				(0x2 << 10)
#define MASK_AUDCHNUM_2				(0x3 << 10)
#define MASK_AUDCHSEL				BIT(9)
#define MASK_I2SDLYOPT				BIT(8)
#define MASK_YCBCRFMT				(0x3 << 6)
#define MASK_YCBCRFMT_444			(0x0 << 6)
#define MASK_YCBCRFMT_422_12_BIT		(0x1 << 6)
#define MASK_YCBCRFMT_COLORBAR			(0x2 << 6)
#define MASK_YCBCRFMT_422_8_BIT			(0x3 << 6)
#define MASK_INFRMEN				BIT(5) /* not tc358749 */
#define MASK_AUDOUTSEL				(0x3 << 3)
#define MASK_AUDOUTSEL_CSI			(0x0 << 3)
#define MASK_AUDOUTSEL_I2S			(0x2 << 3)
#define MASK_AUDOUTSEL_TDM			(0x3 << 3)
#define MASK_AUTOINDEX				BIT(2)
#define MASK_ABUFEN				BIT(1)
#define MASK_VBUFEN				BIT(0)

#define FIFOCTL					0x0006

#define INTSTATUS				0x0014
#define MASK_AMUTE_INT				BIT(10)
#define MASK_HDMI_INT				BIT(9)
#define MASK_CSI_INT				BIT(8)
#define MASK_SLMB_INT				BIT(7) /* tc358749 */
#define MASK_SYS_INT				BIT(5)
#define MASK_CEC_EINT				BIT(4)
#define MASK_CEC_TINT				BIT(3)
#define MASK_CEC_RINT				BIT(2)
#define MASK_IR_EINT				BIT(1)
#define MASK_IR_DINT				BIT(0)

#define INTMASK					0x0016
#define MASK_AMUTE_MSK				BIT(10)
#define MASK_HDMI_MSK				BIT(9)
#define MASK_CSI_MSK				BIT(8)
#define MASK_SLMB_MSK				BIT(7) /* tc358749 */
#define MASK_SYS_MSK				BIT(5)
#define MASK_CEC_EMSK				BIT(4)
#define MASK_CEC_TMSK				BIT(3)
#define MASK_CEC_RMSK				BIT(2)
#define MASK_IR_EMSK				BIT(1)
#define MASK_IR_DMSK				BIT(0)

#define INTFLAG					0x0018
#define INTSYSSTATUS				0x001A

#define PLLCTL0					0x0020
#define MASK_PLL_PRD				(0xf << 12)
#define SET_PLL_PRD(prd)			((((prd) - 1) << 12) & \
							MASK_PLL_PRD)
#define MASK_PLL_FBD				(0x1ff << 0)
#define SET_PLL_FBD(fbd)			(((fbd) - 1) & MASK_PLL_FBD)

#define PLLCTL1					0x0022
#define MASK_PLL_FRS				(0x3 << 10)
#define SET_PLL_FRS(frs)			(((frs) << 10) & MASK_PLL_FRS)
#define MASK_PLL_LBWS				(0x3 << 8)
#define MASK_LFBREN				BIT(6)
#define MASK_BYPCKEN				BIT(5)
#define MASK_CKEN				BIT(4)
#define MASK_RESETB				BIT(1)
#define MASK_PLL_EN				BIT(0)

#define CLW_CNTRL				0x0140
#define MASK_CLW_LANEDISABLE			BIT(0)

#define D0W_CNTRL				0x0144
#define MASK_D0W_LANEDISABLE			BIT(0)

#define D1W_CNTRL				0x0148
#define MASK_D1W_LANEDISABLE			BIT(0)

#define D2W_CNTRL				0x014C
#define MASK_D2W_LANEDISABLE			BIT(0)

#define D3W_CNTRL				0x0150
#define MASK_D3W_LANEDISABLE			BIT(0)

#define STARTCNTRL				0x0204
#define MASK_START				BIT(0)

#define LINEINITCNT				0x0210
#define LPTXTIMECNT				0x0214
#define TCLK_HEADERCNT				0x0218
#define TCLK_TRAILCNT				0x021C
#define THS_HEADERCNT				0x0220
#define TWAKEUP					0x0224
#define TCLK_POSTCNT				0x0228
#define THS_TRAILCNT				0x022C
#define HSTXVREGCNT				0x0230

#define HSTXVREGEN				0x0234
#define MASK_D3M_HSTXVREGEN			BIT(4)
#define MASK_D2M_HSTXVREGEN			BIT(3)
#define MASK_D1M_HSTXVREGEN			BIT(2)
#define MASK_D0M_HSTXVREGEN			BIT(1)
#define MASK_CLM_HSTXVREGEN			BIT(0)


#define TXOPTIONCNTRL				0x0238
#define MASK_CONTCLKMODE			BIT(0)

#define CSI_CONTROL				0x040C
#define MASK_CSI_MODE				BIT(15)
#define MASK_PRTOEN				BIT(13) /* tc358749 */
#define MASK_TATOEN				BIT(12) /* tc358749 */
#define MASK_LRXTOEN				BIT(11) /* tc358749 */
#define MASK_HTXTOEN				BIT(10)
#define MASK_CNTDIS				BIT(9) /* tc358749 */
#define MASK_ECCDIS				BIT(8) /* tc358749 */
#define MASK_TXHSMD				BIT(7)
#define MASK_CRCDIS				BIT(6) /* tc358749 */
#define MASK_HSCKMD				BIT(5)
#define MASK_NOL				(0x3 << 1)
#define MASK_NOL_1				(0x0 << 1)
#define MASK_NOL_2				(0x1 << 1)
#define MASK_NOL_3				(0x2 << 1)
#define MASK_NOL_4				(0x3 << 1)
#define MASK_EOTDIS				BIT(0)

#define CSI_INT					0x0414
#define MASK_INTAK				BIT(18) /* tc358749 */
#define MASK_INTHLT				BIT(3)
#define MASK_INTER				BIT(2)
#define MASK_INTRXER				BIT(1) /* tc358749 */
#define MASK_INTAKER				BIT(0) /* tc358749 */

#define CSI_INT_ENA				0x0418
#define MASK_IENAK				BIT(18) /* tc358749 */
#define MASK_IENHLT				BIT(3)
#define MASK_IENER				BIT(2)
#define MASK_IENRXER				BIT(1) /* tc358749 */
#define MASK_IENAKER				BIT(0) /* tc358749 */

#define CSI_ERR					0x044C
#define MASK_INER				BIT(9)
#define MASK_WCER				BIT(8)
#define MASK_SYNTO				BIT(7) /* tc358749 */
#define MASK_RXFRDER				BIT(6) /* tc358749 */
#define MASK_TEER				BIT(5) /* tc358749 */
#define MASK_QUNK				BIT(4)
#define MASK_QWRER				BIT(3) /* tc358749 */
#define MASK_HTXTO				BIT(2) /* tc358749 */
#define MASK_TXBRK				BIT(1)
#define MASK_CNTN				BIT(0) /* tc358749 */

#define CSI_ERR_INTENA				0x0450
#define CSI_ERR_HALT				0x0454

#define CSI_CONFW				0x0500
#define MASK_MODE				(0x7 << 29)
#define MASK_MODE_SET				(0x5 << 29)
#define MASK_MODE_CLEAR				(0x6 << 29)
#define MASK_ADDRESS				(0x1f << 24)
#define MASK_ADDRESS_CSI_CONTROL		(0x03 << 24)
#define MASK_ADDRESS_CSI_INT_ENA		(0x06 << 24)
#define MASK_ADDRESS_CSI_ERR_INTENA		(0x14 << 24)
#define MASK_ADDRESS_CSI_ERR_HALT		(0x15 << 24)
#define MASK_DATA				(0xffff << 0)

#define CSI_INT_CLR				0x050C
#define MASK_ICRAK				BIT(18) /* tc358749 */
#define MASK_ICRHLT				BIT(3) /* tc358749 */
#define MASK_ICRER				BIT(2)
#define MASK_ICRXER				BIT(1) /* tc358749 */
#define MASK_ICAKER				BIT(0) /* tc358749 */

#define CSI_START				0x0518
#define MASK_STRT				BIT(0)

/* *** CEC (32 bit) *** */
#define CECHCLK					0x0028	/* 16 bits */
#define MASK_CECHCLK				(0x7ff << 0)

#define CECLCLK					0x002a	/* 16 bits */
#define MASK_CECLCLK				(0x7ff << 0)

#define CECEN					0x0600
#define MASK_CECEN				BIT(0)

#define CECADD					0x0604
#define CECRST					0x0608
#define MASK_CECRESET				BIT(0)

#define CECREN					0x060c
#define MASK_CECREN				BIT(0)

#define CECRCTL1				0x0614
#define MASK_CECACKDIS				BIT(24)
#define MASK_CECHNC				(0x3 << 20)
#define MASK_CECLNC				(0x7 << 16)
#define MASK_CECMIN				(0x7 << 12)
#define MASK_CECMAX				(0x7 << 8)
#define MASK_CECDAT				(0x7 << 4)
#define MASK_CECTOUT				(0x3 << 2)
#define MASK_CECRIHLD				BIT(1)
#define MASK_CECOTH				BIT(0)

#define CECRCTL2				0x0618
#define MASK_CECSWAV3				(0x7 << 12)
#define MASK_CECSWAV2				(0x7 << 8)
#define MASK_CECSWAV1				(0x7 << 4)
#define MASK_CECSWAV0				(0x7 << 0)

#define CECRCTL3				0x061c
#define MASK_CECWAV3				(0x7 << 20)
#define MASK_CECWAV2				(0x7 << 16)
#define MASK_CECWAV1				(0x7 << 12)
#define MASK_CECWAV0				(0x7 << 8)
#define MASK_CECACKEI				BIT(4)
#define MASK_CECMINEI				BIT(3)
#define MASK_CECMAXEI				BIT(2)
#define MASK_CECRSTEI				BIT(1)
#define MASK_CECWAVEI				BIT(0)

#define CECTEN					0x0620
#define MASK_CECTBUSY				BIT(1)
#define MASK_CECTEN				BIT(0)

#define CECTCTL					0x0628
#define MASK_CECSTRS				(0x7 << 20)
#define MASK_CECSPRD				(0x7 << 16)
#define MASK_CECDTRS				(0x7 << 12)
#define MASK_CECDPRD				(0xf << 8)
#define MASK_CECBRD				BIT(4)
#define MASK_CECFREE				(0xf << 0)

#define CECRSTAT				0x062c
#define MASK_CECRIWA				BIT(6)
#define MASK_CECRIOR				BIT(5)
#define MASK_CECRIACK				BIT(4)
#define MASK_CECRIMIN				BIT(3)
#define MASK_CECRIMAX				BIT(2)
#define MASK_CECRISTA				BIT(1)
#define MASK_CECRIEND				BIT(0)

#define CECTSTAT				0x0630
#define MASK_CECTIUR				BIT(4)
#define MASK_CECTIACK				BIT(3)
#define MASK_CECTIAL				BIT(2)
#define MASK_CECTIEND				BIT(1)

#define CECRBUF1				0x0634
#define MASK_CECRACK				BIT(9)
#define MASK_CECEOM				BIT(8)
#define MASK_CECRBYTE				(0xff << 0)

#define CECTBUF1				0x0674
#define MASK_CECTEOM				BIT(8)
#define MASK_CECTBYTE				(0xff << 0)

#define CECRCTR					0x06b4
#define MASK_CECRCTR				(0x1f << 0)

#define CECIMSK					0x06c0
#define MASK_CECTIM				BIT(1)
#define MASK_CECRIM				BIT(0)

#define CECICLR					0x06cc
#define MASK_CECTICLR				BIT(1)
#define MASK_CECRICLR				BIT(0)


#define HDMI_INT0				0x8500
#define MASK_I_KEY				BIT(7)
#define MASK_I_MISC				BIT(1)
#define MASK_I_PHYERR				BIT(0)

#define HDMI_INT1				0x8501
#define MASK_I_GBD				BIT(7)
#define MASK_I_HDCP				BIT(6)
#define MASK_I_ERR				BIT(5)
#define MASK_I_AUD				BIT(4)
#define MASK_I_CBIT				BIT(3)
#define MASK_I_PACKET				BIT(2)
#define MASK_I_CLK				BIT(1)
#define MASK_I_SYS				BIT(0)

#define SYS_INT					0x8502
#define MASK_I_ACR_CTS				BIT(7)
#define MASK_I_ACRN				BIT(6)
#define MASK_I_DVI				BIT(5)
#define MASK_I_HDMI				BIT(4)
#define MASK_I_NOPMBDET				BIT(3)
#define MASK_I_DPMBDET				BIT(2)
#define MASK_I_TMDS				BIT(1)
#define MASK_I_DDC				BIT(0)

#define CLK_INT					0x8503
#define MASK_I_OUT_H_CHG			BIT(6)
#define MASK_I_IN_DE_CHG			BIT(5)
#define MASK_I_IN_HV_CHG			BIT(4)
#define MASK_I_DC_CHG				BIT(3)
#define MASK_I_PXCLK_CHG			BIT(2)
#define MASK_I_PHYCLK_CHG			BIT(1)
#define MASK_I_TMDSCLK_CHG			BIT(0)

#define CBIT_INT				0x8505
#define MASK_I_AF_LOCK				BIT(7)
#define MASK_I_AF_UNLOCK			BIT(6)
#define MASK_I_AU_DST				BIT(5) /* tc358749 */
#define MASK_I_AU_DSD				BIT(4) /* tc358749 */
#define MASK_I_AU_HBR				BIT(3) /* tc358749 */
#define MASK_I_CBIT_NLPCM			BIT(2) /* tc358749 */
#define MASK_I_CBIT_FS				BIT(1)
#define MASK_I_CBIT				BIT(0) /* tc358749 */

#define AUDIO_INT				0x8506

#define ERR_INT					0x8507
#define MASK_I_EESS_ERR				BIT(7)
#define MASK_I_AU_FRAME				BIT(6) /* tc358749 */
#define MASK_I_NO_ACP				BIT(5) /* tc358749 */
#define MASK_I_NO_AVI				BIT(4) /* tc358749 */
#define MASK_I_DC_NOCD				BIT(3) /* tc358749 */
#define MASK_I_DC_DEERR				BIT(2) /* tc358749 */
#define MASK_I_DC_BUFERR			BIT(1) /* tc358749 */
#define MASK_I_DC_PPERR				BIT(0) /* tc358749 */

#define HDCP_INT				0x8508
#define MASK_I_AVM_SET				BIT(7)
#define MASK_I_AVM_CLR				BIT(6)
#define MASK_I_LINKERR				BIT(5)
#define MASK_I_SHA_END				BIT(4)
#define MASK_I_R0_END				BIT(3)
#define MASK_I_KM_END				BIT(2)
#define MASK_I_AKSV_END				BIT(1)
#define MASK_I_AN_END				BIT(0)

#define MISC_INT				0x850B
#define MASK_I_AS_LAYOUT			BIT(4)
#define MASK_I_NO_SPD				BIT(3)
#define MASK_I_NO_VS				BIT(2) //0x3?, manual says BIT(2)
#define MASK_I_SYNC_CHG				BIT(1)
#define MASK_I_AUDIO_MUTE			BIT(0)

#define KEY_INT					0x850F

#define SYS_INTM				0x8512
#define MASK_M_ACR_CTS				BIT(7)
#define MASK_M_ACR_N				BIT(6)
#define MASK_M_DVI_DET				BIT(5)
#define MASK_M_HDMI_DET				BIT(4)
#define MASK_M_NOPMBDET				BIT(3)
#define MASK_M_BPMBDET				BIT(2)
#define MASK_M_TMDS				BIT(1)
#define MASK_M_DDC				BIT(0)

#define CLK_INTM				0x8513
#define MASK_M_OUT_H_CHG			BIT(6)
#define MASK_M_IN_DE_CHG			BIT(5)
#define MASK_M_IN_HV_CHG			BIT(4)
#define MASK_M_DC_CHG				BIT(3)
#define MASK_M_PXCLK_CHG			BIT(2)
#define MASK_M_PHYCLK_CHG			BIT(1)
#define MASK_M_TMDS_CHG				BIT(0)

#define PACKET_INTM				0x8514

#define CBIT_INTM				0x8515
#define MASK_M_AF_LOCK				BIT(7)
#define MASK_M_AF_UNLOCK			BIT(6)
#define MASK_M_AU_DST				BIT(5) /* tc358749 */
#define MASK_M_AU_DSD				BIT(4) /* tc358749 */
#define MASK_M_AU_HBR				BIT(3) /* tc358749 */
#define MASK_M_CBIT_NLPCM			BIT(2) /* tc358749 */
#define MASK_M_CBIT_FS				BIT(1)
#define MASK_M_CBIT				BIT(0) /* tc358749 */

#define AUDIO_INTM				0x8516
#define MASK_M_BUF_OVER				BIT(7) /* tc358749 */
#define MASK_M_BUF_NO2				BIT(6) /* tc358749 */
#define MASK_M_BUF_NO1				BIT(5) /* tc358749 */
#define MASK_M_BUF_CENTER			BIT(4) /* tc358749 */
#define MASK_M_BUF_NU1				BIT(3) /* tc358749 */
#define MASK_M_BUF_NU2				BIT(2) /* tc358749 */
#define MASK_M_BUF_UNDER			BIT(1) /* tc358749 */
#define MASK_M_BUFINIT_END			BIT(0)

#define ERR_INTM				0x8517
#define MASK_M_EESS_ERR				BIT(7)
#define MASK_M_AU_FRAME				BIT(6) /* tc358749 */
#define MASK_M_NO_ACP				BIT(5) /* tc358749 */
#define MASK_M_NO_AVI				BIT(4) /* tc358749 */
#define MASK_M_DC_NOCD				BIT(3) /* tc358749 */
#define MASK_M_DC_DEERR				BIT(2) /* tc358749 */
#define MASK_M_DC_BUFERR			BIT(1) /* tc358749 */
#define MASK_M_DC_PPERR				BIT(0) /* tc358749 */

#define HDCP_INTM				0x8518
#define MASK_M_AVM_SET				BIT(7)
#define MASK_M_AVM_CLR				BIT(6)
#define MASK_M_LINKERR				BIT(5)
#define MASK_M_SHA_END				BIT(4)
#define MASK_M_R0_END				BIT(3)
#define MASK_M_KM_END				BIT(2)
#define MASK_M_AKSV_END				BIT(1)
#define MASK_M_AN_END				BIT(0)

#define MISC_INTM				0x851B
#define MASK_M_AS_LAYOUT			BIT(4)
#define MASK_M_NO_SPD				BIT(3)
#define MASK_M_NO_VS				BIT(2) //0x3?, manual says BIT(2)
#define MASK_M_SYNC_CHG				BIT(1)
#define MASK_M_AUDIO_MUTE			BIT(0)

#define KEY_INTM				0x851F

#define SYS_STATUS				0x8520
#define MASK_S_SYNC				BIT(7)
#define MASK_S_AVMUTE				BIT(6)
#define MASK_S_HDCP				BIT(5)
#define MASK_S_HDMI				BIT(4)
#define MASK_S_PHY_SCDT				BIT(3)
#define MASK_S_PHY_PLL				BIT(2)
#define MASK_S_TMDS				BIT(1)
#define MASK_S_DDC5V				BIT(0)

#define CSI_STATUS				0x0410
#define MASK_S_WSYNC				BIT(10)
#define MASK_S_TXACT				BIT(9)
#define MASK_S_RXACT				BIT(8)
#define MASK_S_RXAF				BIT(7) /* tc358749 */
#define MASK_S_RXAE				BIT(6) /* tc358749 */
#define MASK_S_RXEM				BIT(5) /* tc358749 */
#define MASK_S_HLT				BIT(0)

#define VI_STATUS1				0x8522
#define MASK_S_V_GBD				BIT(7)
#define MASK_S_DEEPCOLOR			(0x3 << 2)
#define MASK_S_V_422				BIT(1)
#define MASK_S_V_INTERLACE			BIT(0)

#define AU_STATUS0				0x8523
#define MASK_S_A_MUTE				BIT(7) /* tc358749 */
#define MASK_S_A_DST				BIT(4) /* tc358749 */
#define MASK_S_A_DSD				BIT(3) /* tc358749 */
#define MASK_S_A_HBR				BIT(2) /* tc358749 */
#define MASK_S_A_NLPCM				BIT(1) /* tc358749 */
#define MASK_S_A_SAMPLE				BIT(0)

#define VI_STATUS3				0x8528
#define MASK_S_V_COLOR				(0x1e << 0)
#define MASK_LIMITED				BIT(0)

#define PHY_CTL0				0x8531
#define MASK_PHY_SYSCLK_IND			BIT(1)
#define MASK_PHY_CTL				BIT(0)


#define PHY_CTL1				0x8532 /* Not in REF_01 */
#define MASK_PHY_AUTO_RST1			0xf0
#define MASK_PHY_AUTO_RST1_OFF			0x00
#define SET_PHY_AUTO_RST1_US(us)		((((us) / 200) << 4) & \
							MASK_PHY_AUTO_RST1)
#define MASK_FREQ_RANGE_MODE			0x0f
#define SET_FREQ_RANGE_MODE_CYCLES(cycles)	(((cycles) - 1) & \
							MASK_FREQ_RANGE_MODE)

#define PHY_CTL2				0x8533 /* Not in REF_01 */
#define MASK_PHY_AUTO_RST4			0x04
#define MASK_PHY_AUTO_RST3			0x02
#define MASK_PHY_AUTO_RST2			0x01
#define MASK_PHY_AUTO_RSTn			(MASK_PHY_AUTO_RST4 | \
						 MASK_PHY_AUTO_RST3 | \
						 MASK_PHY_AUTO_RST2)

#define PHY_EN					0x8534
#define MASK_ENABLE_PHY				BIT(0)

#define PHY_RST					0x8535
#define MASK_RESET_CTRL				BIT(0)   /* Reset active low */

#define PHY_BIAS				0x8536 /* Not in REF_01 */

#define PHY_CSQ					0x853F /* Not in REF_01 */
#define MASK_CSQ_CNT				0x0f
#define SET_CSQ_CNT_LEVEL(n)			(n & MASK_CSQ_CNT)

#define SYS_FREQ0				0x8540
#define SYS_FREQ1				0x8541

#define SYS_CLK					0x8542 /* Not in REF_01 */
#define MASK_CLK_DIFF				0x0C
#define MASK_CLK_DIV				0x03

#define DDC_CTL					0x8543
#define MASK_DDC_ACK_POL			BIT(3)
#define MASK_DDC_ACTION				BIT(2)
#define MASK_DDC5V_MODE				(0x03 << 0)
#define MASK_DDC5V_MODE_0MS			(0x00 << 0)
#define MASK_DDC5V_MODE_50MS			(0x01 << 0)
#define MASK_DDC5V_MODE_100MS			(0x02 << 0)
#define MASK_DDC5V_MODE_200MS			(0x03 << 0)

#define HPD_CTL					0x8544
#define MASK_HPD_CTL0				BIT(4)
#define MASK_HPD_OUT0				BIT(0)

#define ANA_CTL					0x8545
#define MASK_APPL_PCSX				(0x3 << 4)
#define MASK_APPL_PCSX_HIZ			(0x0 << 4)
#define MASK_APPL_PCSX_L_FIX			(0x1 << 4)
#define MASK_APPL_PCSX_H_FIX			(0x2 << 4)
#define MASK_APPL_PCSX_NORMAL			(0x3 << 4)
#define MASK_ANALOG_ON				BIT(0)

#define AVM_CTL					0x8546

#define INIT_END				0x854A
#define MASK_INIT_END				BIT(0)

#define HDMI_DET				0x8552 /* Not in REF_01 */
#define MASK_HDMI_DET_MOD1			0x80
#define MASK_HDMI_DET_MOD0			0x40
#define MASK_HDMI_DET_V				0x30
#define MASK_HDMI_DET_V_SYNC			0x00
#define MASK_HDMI_DET_V_ASYNC_25MS		0x10
#define MASK_HDMI_DET_V_ASYNC_50MS		0x20
#define MASK_HDMI_DET_V_ASYNC_100MS		0x30
#define MASK_HDMI_DET_NUM			0x0f

#define HDCP_MODE				0x8560
#define MASK_MODE_RST_TN			BIT(5)
#define MASK_LINE_REKEY				BIT(4)
#define MASK_AUTO_CLR				BIT(2)
#define MASK_MANUAL_AUTHENTICATION		0x02 /* Not in REF_01 */

#define HDCP_REG1				0x8563 /* Not in REF_01 */
#define MASK_AUTH_UNAUTH_SEL			0x70
#define MASK_AUTH_UNAUTH_SEL_12_FRAMES		0x70
#define MASK_AUTH_UNAUTH_SEL_8_FRAMES		0x60
#define MASK_AUTH_UNAUTH_SEL_4_FRAMES		0x50
#define MASK_AUTH_UNAUTH_SEL_2_FRAMES		0x40
#define MASK_AUTH_UNAUTH_SEL_64_FRAMES		0x30
#define MASK_AUTH_UNAUTH_SEL_32_FRAMES		0x20
#define MASK_AUTH_UNAUTH_SEL_16_FRAMES		0x10
#define MASK_AUTH_UNAUTH_SEL_ONCE		0x00
#define MASK_AUTH_UNAUTH			0x01
#define MASK_AUTH_UNAUTH_AUTO			0x01

#define HDCP_REG2				0x8564 /* Not in REF_01 */
#define MASK_AUTO_P3_RESET			0x0F
#define SET_AUTO_P3_RESET_FRAMES(n)		(n & MASK_AUTO_P3_RESET)
#define MASK_AUTO_P3_RESET_OFF			0x00

#define VI_MODE					0x8570
#define MASK_SG_ON				BIT(4) /* tc358749 */
#define MASK_RGB_DVI				BIT(3) /* Not in REF_01 */

#define VOUT_SET2				0x8573
#define MASK_SEL422				BIT(7)
/* 422FIL supports additional variants */
#define MASK_VOUT_422FIL_100			(0x4 << 4)
#define MASK_VOUTCOLORMODE			(0x03 << 0)
#define MASK_VOUTCOLORMODE_THROUGH		(0x00 << 0)
#define MASK_VOUTCOLORMODE_AUTO			(0x01 << 0)
#define MASK_VOUTCOLORMODE_MANUAL		(0x03 << 0)

#define VOUT_SET3				0x8574
#define MASK_VOUT_LIM				BIT(7) /* tc358749 */
#define MASK_VOUT_BIT				(0x3 << 4) /* tc358749 */
#define MASK_VOUT_BIT_16			(0x0 << 4) /* tc358749 */
#define MASK_VOUT_BIT_12			(0x1 << 4) /* tc358749 */
#define MASK_VOUT_BIT_10			(0x2 << 4) /* tc358749 */
#define MASK_VOUT_BIT_8				(0x3 << 4) /* tc358749 */
#define MASK_VOUT_EXTCNT			BIT(3)

#define VI_REP					0x8576
#define MASK_VOUT_COLOR_SEL			(0x7 << 5)
#define MASK_VOUT_COLOR_RGB_FULL		(0x0 << 5)
#define MASK_VOUT_COLOR_RGB_LIMITED		(0x1 << 5)
#define MASK_VOUT_COLOR_601_YCBCR_FULL		(0x2 << 5)
#define MASK_VOUT_COLOR_601_YCBCR_LIMITED	(0x3 << 5)
#define MASK_VOUT_COLOR_709_YCBCR_FULL		(0x4 << 5)
#define MASK_VOUT_COLOR_709_YCBCR_LIMITED	(0x5 << 5)
#define MASK_VOUT_COLOR_FULL_TO_LIMITED		(0x6 << 5)
#define MASK_VOUT_COLOR_LIMITED_TO_FULL		(0x7 << 5)
#define MASK_IN_REP_HEN				BIT(4)
#define MASK_IN_REP				(0xf << 0)

#define VI_MUTE					0x857F
#define MASK_AUTO_MUTE				0xc0
#define MASK_VI_MUTE				0x10

#define DE_WIDTH_H_LO				0x8582 /* Not in REF_01 */
#define DE_WIDTH_H_HI				0x8583 /* Not in REF_01 */
#define DE_WIDTH_V_LO				0x8588 /* Not in REF_01 */
#define DE_WIDTH_V_HI				0x8589 /* Not in REF_01 */
#define H_SIZE_LO				0x858A /* Not in REF_01 */
#define H_SIZE_HI				0x858B /* Not in REF_01 */
#define V_SIZE_LO				0x858C /* Not in REF_01 */
#define V_SIZE_HI				0x858D /* Not in REF_01 */
#define FV_CNT_LO				0x85A1 /* Not in REF_01 */
#define FV_CNT_HI				0x85A2 /* Not in REF_01 */

#define FH_MIN0					0x85AA /* Not in REF_01 */
#define FH_MIN1					0x85AB /* Not in REF_01 */
#define FH_MAX0					0x85AC /* Not in REF_01 */
#define FH_MAX1					0x85AD /* Not in REF_01 */

#define HV_RST					0x85AF /* Not in REF_01 */
#define MASK_H_PI_RST				0x20
#define MASK_V_PI_RST				0x10

#define EDID_MODE				0x85C7
#define MASK_EDID_SPEED				BIT(4)
#define MASK_EDID_MODE				(0x3 << 0)
#define MASK_EDID_MODE_DISABLE			(0x0 << 0)
#define MASK_EDID_MODE_DDC2B			(0x1 << 0)
#define MASK_EDID_MODE_E_DDC			(0x2 << 0)

#define EDID_LEN1				0x85CA
#define EDID_LEN2				0x85CB

#define HDCP_REG3				0x85D1 /* Not in REF_01 */
#define KEY_RD_CMD				0x01

#define FORCE_MUTE				0x8600
#define MASK_FORCE_AMUTE			BIT(4)
#define MASK_FORCE_DMUTE			BIT(0)

#define CMD_AUD					0x8601
#define MASK_CMD_BUFINIT			BIT(2)
#define MASK_CMD_LOCKDET			BIT(1)
#define MASK_CMD_MUTE				BIT(0)

#define AUTO_CMD0				0x8602
#define MASK_AUTO_MUTE7				BIT(7)
#define MASK_AUTO_MUTE6				BIT(6)
#define MASK_AUTO_MUTE5				BIT(5)
#define MASK_AUTO_MUTE4				BIT(4)
#define MASK_AUTO_MUTE3				BIT(3)
#define MASK_AUTO_MUTE2				BIT(2)
#define MASK_AUTO_MUTE1				BIT(1)
#define MASK_AUTO_MUTE0				BIT(0)

#define AUTO_CMD1				0x8603
#define MASK_AUTO_MUTE10			BIT(2)
#define MASK_AUTO_MUTE9				BIT(1)
#define MASK_AUTO_MUTE8				BIT(0)

#define AUTO_CMD2				0x8604
#define MASK_AUTO_PLAY3				BIT(3)
#define MASK_AUTO_PLAY2				BIT(2)

#define BUFINIT_START				0x8606
#define SET_BUFINIT_START_MS(milliseconds)	((milliseconds) / 100)

#define FS_MUTE					0x8607
#define MASK_FS_ELSE_MUTE			BIT(7)
#define MASK_FS22_MUTE				BIT(6)
#define MASK_FS24_MUTE				BIT(5)
#define MASK_FS88_MUTE				BIT(4)
#define MASK_FS96_MUTE				BIT(3)
#define MASK_FS176_MUTE				BIT(2)
#define MASK_FS192_MUTE				BIT(1)
#define MASK_FS_NO_MUTE				BIT(0)

#define FS_IMODE				0x8620
#define MASK_NLPCM_HMODE			BIT(6)
#define MASK_NLPCM_SMODE			BIT(5)
#define MASK_NLPCM_IMODE			BIT(4)
#define MASK_FS_HMODE				BIT(3)
#define MASK_FS_AMODE				BIT(2)
#define MASK_FS_SMODE				BIT(1)
#define MASK_FS_IMODE				BIT(0)

#define FS_SET					0x8621
#define MASK_DTS_DOUBLE				BIT(7) /* tc358749 */
#define MASK_NLPCM				BIT(4) /* tc358749 */
#define MASK_FS					(0xf << 0)

#define LOCKDET_REF0				0x8630
#define LOCKDET_REF1				0x8631
#define LOCKDET_REF2				0x8632

#define ACR_MODE				0x8640
#define MASK_ACR_LOAD				BIT(4)
#define MASK_N_MODE				BIT(2)
#define MASK_CTS_MODE				BIT(0)

#define ACR_MDF0				0x8641
#define MASK_ACR_L2MDF				(0x7 << 4)
#define MASK_ACR_L2MDF_0_PPM			(0x0 << 4)
#define MASK_ACR_L2MDF_61_PPM			(0x1 << 4)
#define MASK_ACR_L2MDF_122_PPM			(0x2 << 4)
#define MASK_ACR_L2MDF_244_PPM			(0x3 << 4)
#define MASK_ACR_L2MDF_488_PPM			(0x4 << 4)
#define MASK_ACR_L2MDF_976_PPM			(0x5 << 4)
#define MASK_ACR_L2MDF_1976_PPM			(0x6 << 4)
#define MASK_ACR_L2MDF_3906_PPM			(0x7 << 4)
#define MASK_ACR_L1MDF				(0x7 << 0)
#define MASK_ACR_L1MDF_0_PPM			(0x0 << 0)
#define MASK_ACR_L1MDF_61_PPM			(0x1 << 0)
#define MASK_ACR_L1MDF_122_PPM			(0x2 << 0)
#define MASK_ACR_L1MDF_244_PPM			(0x3 << 0)
#define MASK_ACR_L1MDF_488_PPM			(0x4 << 0)
#define MASK_ACR_L1MDF_976_PPM			(0x5 << 0)
#define MASK_ACR_L1MDF_1976_PPM			(0x6 << 0)
#define MASK_ACR_L1MDF_3906_PPM			(0x7 << 0)

#define ACR_MDF1				0x8642
#define MASK_ACR_L3MDF				(0x7 << 0)
#define MASK_ACR_L3MDF_0_PPM			(0x0 << 0)
#define MASK_ACR_L3MDF_61_PPM			(0x1 << 0)
#define MASK_ACR_L3MDF_122_PPM			(0x2 << 0)
#define MASK_ACR_L3MDF_244_PPM			(0x3 << 0)
#define MASK_ACR_L3MDF_488_PPM			(0x4 << 0)
#define MASK_ACR_L3MDF_976_PPM			(0x5 << 0)
#define MASK_ACR_L3MDF_1976_PPM			(0x6 << 0)
#define MASK_ACR_L3MDF_3906_PPM			(0x7 << 0)

#define SDO_MODE1				0x8652
#define MASK_SDO_BIT_LENG			(0x7 << 4)
#define MASK_I2s_HBR4L_MODE			BIT(3) /* tc358749 */
#define MASK_I2S_MODE				BIT(2) /* tc358749 */
#define MASK_SDO_FMT				(0x3 << 0)
#define MASK_SDO_FMT_RIGHT			(0x0 << 0)
#define MASK_SDO_FMT_LEFT			(0x1 << 0)
#define MASK_SDO_FMT_I2S			(0x2 << 0)

#define DIV_MODE				0x8665 /* Not REF_01, but x749 */
#define MASK_DIV_DLY				(0xf << 4)
#define SET_DIV_DLY_MS(milliseconds)		((((milliseconds) / 100) << 4) & \
							MASK_DIV_DLY)
#define MASK_DIV_MODE				BIT(0)

#define NCO_F0_MOD				0x8670
#define MASK_NCO_F0_MOD				(0x3 << 0)
#define MASK_NCO_F0_MOD_42MHZ			(0x0 << 0)
#define MASK_NCO_F0_MOD_27MHZ			(0x1 << 0)

#define PK_INT_MODE				0x8709
#define MASK_ISRC2_INT_MODE			BIT(7)
#define MASK_ISRC_INT_MODE			BIT(6)
#define MASK_ACP_INT_MODE			BIT(5)
#define MASK_VS_INT_MODE			BIT(4)
#define MASK_SPD_INT_MODE			BIT(3)
#define MASK_MS_INT_MODE			BIT(2)
#define MASK_AUD_INT_MODE			BIT(1)
#define MASK_AVI_INT_MODE			BIT(0)

#define NO_PKT_LIMIT				0x870B
#define MASK_NO_ACP_LIMIT			(0xf << 4)
#define SET_NO_ACP_LIMIT_MS(milliseconds)	((((milliseconds) / 80) << 4) & \
							MASK_NO_ACP_LIMIT)
#define MASK_NO_AVI_LIMIT			(0xf << 0)
#define SET_NO_AVI_LIMIT_MS(milliseconds)	(((milliseconds) / 80) & \
							MASK_NO_AVI_LIMIT)

#define NO_PKT_CLR				0x870C
#define MASK_NO_VS_CLR				BIT(6)
#define MASK_NO_SPD_CLR				BIT(5)
#define MASK_NO_ACP_CLR				BIT(4)
#define MASK_NO_AVI_CLR1			BIT(1)
#define MASK_NO_AVI_CLR0			BIT(0)

#define ERR_PK_LIMIT				0x870D
#define NO_PKT_LIMIT2				0x870E
#define PK_AVI_0HEAD				0x8710
#define PK_AVI_1HEAD				0x8711
#define PK_AVI_2HEAD				0x8712
#define PK_AVI_0BYTE				0x8713
#define PK_AVI_1BYTE				0x8714
#define PK_AVI_2BYTE				0x8715
#define PK_AVI_3BYTE				0x8716
#define PK_AVI_4BYTE				0x8717
#define PK_AVI_5BYTE				0x8718
#define PK_AVI_6BYTE				0x8719
#define PK_AVI_7BYTE				0x871A
#define PK_AVI_8BYTE				0x871B
#define PK_AVI_9BYTE				0x871C
#define PK_AVI_10BYTE				0x871D
#define PK_AVI_11BYTE				0x871E
#define PK_AVI_12BYTE				0x871F
#define PK_AVI_13BYTE				0x8720
#define PK_AVI_14BYTE				0x8721
#define PK_AVI_15BYTE				0x8722
#define PK_AVI_16BYTE				0x8723

#define BKSV					0x8800

#define BCAPS					0x8840
#define MASK_HDMI_RSVD				BIT(7)
#define MASK_REPEATER				BIT(6)
#define MASK_READY				BIT(5)
#define MASK_FASTI2C				BIT(4)
#define MASK_1_1_FEA				BIT(1)
#define MASK_FAST_REAU				BIT(0)

#define BSTATUS1				0x8842
#define MASK_HDMI_RSVD				BIT(5) /* tc358749 */
#define MASK_HDMI_MODE				BIT(4) /* tc358749 */
#define MASK_MAX_EXCED				BIT(3)
#define MASK_DEPTH				(0x7 << 0) /* tc358749 */

#define EDID_RAM				0x8C00
#define NO_GDB_LIMIT				0x9007

#endif
