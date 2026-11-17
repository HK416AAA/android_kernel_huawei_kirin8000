/*
 * Copyright (c) 2025 Southchip Semiconductor Technology(Shanghai) Co., Ltd.
 *
 * SC66XX Type-C Port Control Driver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __LINUX_SC66XX_H__
#define __LINUX_SC66XX_H__

#define ENABLE_SC66XX_DBG           0

#define SC66XX_REG_ANA_CTRL1        0x90
#define SC66XX_REG_VCONN_OCP_CTRL   0x93
#define SC66XX_REG_ANA_STATUS       0x97
#define SC66XX_REG_ANA_INT          0x98
#define SC66XX_REG_ANA_MASK         0x99
#define SC66XX_REG_ANA_CTRL2        0x9B

#define SC66XX_REG_RST_CTRL         0xA0
#define SC66XX_REG_DRP_CTRL         0xA2
#define SC66XX_REG_DRP_DUTY_CTRL    0xA3

#ifdef CONFIG_TCPC_RT1715
#define SC66XX_REG_CC_STATUS            0x1d
#define SC66XX_CC_STATUS_FOR_DOUBLE_56K 5
#define CC_STATUS_MASK                  0xf
#endif

/**
 * SC66XX_REG_ANA_CTRL1             (0x90)
 */
#define SC66XX_REG_VCONN_DISCHARGE_EN       (1 << 5)
#define SC66XX_REG_LPM_EN                   (1 << 3)

/**
 * SC66XX_REG_ANA_STATUS        (0x97)
 */
#define SC66XX_REG_VBUS_80      (1 << 1)

/**
 * SC66XX_REG_ANA_INT        (0x98)
 */
#define SC66XX_REG_INT_HDRST          (1 << 7)
#define SC66XX_REG_INT_RA_DATECH    (1 << 5)
#define SC66XX_REG_INT_CC_OVP       (1 << 2)
#define SC66XX_REG_INT_VBUS_80      (1 << 1)

/**
 * SC66XX_REG_ANA_MASK          (0x99)
 */
#define SC66XX_REG_MASK_HDRST          (1 << 7)
#define SC66XX_REG_MASK_RA_DATECH    (1 << 5)
#define SC66XX_REG_MASK_CC_OVP       (1 << 2)
#define SC66XX_REG_MASK_VBUS_80      (1 << 1)

/**
 * SC66XX_REG_ANA_CTRL2          (0x9B)
 */
#define SC66XX_REG_SHUTDOWN_OFF         (1 << 5)

#if ENABLE_SC66XX_DBG
#define SC66XX_INFO(format, args...) \
	pd_dbg_info("%s() line-%d: " format,\
	__func__, __LINE__, ##args)
#else
#define SC66XX_INFO(foramt, args...)
#endif

#endif /* __LINUX_SC66XX_H__ */
