/*
 * lyw6178.h
 *
 * lyw6178 driver for backlight
 *
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 */

#ifndef _BL_LYW6178_H_
#define _BL_LYW6178_H_
#include "dpu_fb.h"
#include "../dpu_fb_panel.h"
#if defined(CONFIG_LCDKIT_DRIVER)
#include "lcdkit_panel.h"
#endif

#define LYW6178_NAME  "lyw6178"
#define DTS_COMP_LYW6178  "lyw,lyw6178"
#define GPIO_LYW6178_EN_NAME  "lyw6178_hw_en"

#define LYW6178_BL_MIN  (0)
#define LYW6178_BL_MAX  (4095)
#define LYW6178_RW_REG_MAX  (13)

/* lyw6178 reg address */
#define LYW6178_REG_0_ADDR  (0x00)
#define LYW6178_REG_1_ADDR  (0x01)
#define LYW6178_REG_2_ADDR  (0x02)
#define LYW6178_REG_3_ADDR  (0x03)
#define LYW6178_REG_4_ADDR  (0x04)
#define LYW6178_REG_5_ADDR  (0x05)
#define LYW6178_REG_6_ADDR  (0x06)
#define LYW6178_REG_7_ADDR  (0x07)
#define LYW6178_REG_8_ADDR  (0x08)
#define LYW6178_REG_9_ADDR  (0x09)
#define LYW6178_REG_A_ADDR  (0x0A)
#define LYW6178_REG_B_ADDR  (0x0B)
#define LYW6178_REG_C_ADDR  (0x0C)
#define LYW6178_REG_E_ADDR  (0x0E)
#define VENDOR_ID  (0x68)

#define PARSE_FAILED  (0xffff)
#define DELAY_0_US  (0)
#define DELAY_5_MS  (5)
#define DELAY_10_US  (10)
#define DELAY_50_US  (50)
#define DEFAULT_MSG_LEVEL  (7)

#define I2C3_BUSNUM  (3)
#define I2C4_BUSNUM  (4)
#define LYW6178_DISABLE_DELAY  (60)

#ifndef bit
#define bit(x)  (1 << (x))
#endif

#define lyw6178_emerg(dev, msg, ...) \
	do { if (g_lyw6178_msg_level > 0) \
		dev_emerg(dev, "%s: "msg, __func__, ## __VA_ARGS__); } while (0)
#define lyw6178_alert(dev, msg, ...) \
	do { if (g_lyw6178_msg_level > 1) \
		dev_alert(dev, "%s: "msg, __func__, ## __VA_ARGS__); } while (0)
#define lyw6178_crit(dev, msg, ...) \
	do { if (g_lyw6178_msg_level > 2) \
		dev_crit(dev, "%s: "msg, __func__, ## __VA_ARGS__); } while (0)
#define lyw6178_err(dev, msg, ...) \
	do { if (g_lyw6178_msg_level > 3) \
		dev_err(dev, "%s: "msg, __func__, ## __VA_ARGS__); } while (0)
#define lyw6178_warning(dev, msg, ...) \
	do { if (g_lyw6178_msg_level > 4) \
		dev_warn(dev, "%s: "msg, __func__, ## __VA_ARGS__); } while (0)
#define lyw6178_notice(dev, msg, ...) \
	do { if (g_lyw6178_msg_level > 5) \
		dev_notice(dev, "%s: "msg, __func__, ## __VA_ARGS__); } while (0)
#define lyw6178_info(dev, msg, ...) \
	do { if (g_lyw6178_msg_level > 6) \
		_dev_info(dev, "%s: "msg, __func__, ## __VA_ARGS__); } while (0)
#define lyw6178_debug(dev, msg, ...) \
	do { if (g_lyw6178_msg_level > 7) \
		_dev_info(dev, "%s: "msg, __func__, ## __VA_ARGS__); } while (0)

struct lyw6178_chip_data {
	struct device *dev;
	struct i2c_client *client;
	struct regmap *regmap;
	struct semaphore test_sem;
};
struct lyw6178_info {
	/* whether support lyw6178 or not */
	int lyw6178_support;
	/* which i2c bus controller lyw6178 mount */
	int lyw6178_i2c_bus_id;
	/* lyw6178 hw_en gpio */
	int lyw6178_hw_en_gpio;
	uint32_t reg[LYW6178_RW_REG_MAX];
	int bl_on_kernel_mdelay;
	int lyw6178_level_lsb;
	int lyw6178_level_msb;
	int bl_led_num;
	int bl_set_long_slope;
	int dual_ic;
};
ssize_t lyw6178_set_backlight_init(uint32_t bl_level);
bool is_lyw6178_used(void);
#endif /* _BL_LYW6178_H */
