/*
 * lyw6178.c
 *
 * lyw6178 driver for backlight
 *
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 */

#include "lyw6178.h"
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/leds.h>
#include <linux/backlight.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/regmap.h>
#include <linux/semaphore.h>
#include <securec.h>

static bool g_lyw6178_used = false;
static bool g_init_status = false;
static struct class *g_lyw_class = NULL;
static struct lyw6178_chip_data *g_lyw_dev = NULL;
static struct lyw6178_info g_bl_info;

static struct gpio_desc g_gpio_on_cmds[] = {
	{ DTYPE_GPIO_REQUEST, WAIT_TYPE_US,
		DELAY_0_US, GPIO_LYW6178_EN_NAME,
		&g_bl_info.lyw6178_hw_en_gpio, 0 },
	{ DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS,
		DELAY_5_MS, GPIO_LYW6178_EN_NAME,
		&g_bl_info.lyw6178_hw_en_gpio, 1 },
};
static struct gpio_desc g_gpio_disable_cmds[] = {
	{ DTYPE_GPIO_OUTPUT, WAIT_TYPE_US,
		DELAY_0_US, GPIO_LYW6178_EN_NAME,
		&g_bl_info.lyw6178_hw_en_gpio, 0 },
	{ DTYPE_GPIO_INPUT, WAIT_TYPE_US,
		DELAY_10_US, GPIO_LYW6178_EN_NAME,
		&g_bl_info.lyw6178_hw_en_gpio, 0 },
};
static struct gpio_desc g_gpio_free_cmds[] = {
	{ DTYPE_GPIO_FREE, WAIT_TYPE_US,
		DELAY_50_US, GPIO_LYW6178_EN_NAME,
		&g_bl_info.lyw6178_hw_en_gpio, 0 },
};
static char *g_dts_str[LYW6178_RW_REG_MAX] = {
	"lyw6178_reg_0",
	"lyw6178_reg_1",
	"lyw6178_reg_2",
	"lyw6178_reg_3",
	"lyw6178_reg_4",
	"lyw6178_reg_5",
	"lyw6178_reg_6",
	"lyw6178_reg_7",
	"lyw6178_reg_8",
	"lyw6178_reg_9",
	"lyw6178_reg_a",
	"lyw6178_reg_b",
	"lyw6178_reg_c",
};
static unsigned int g_reg_addr[LYW6178_RW_REG_MAX] = {
	LYW6178_REG_0_ADDR,
	LYW6178_REG_1_ADDR,
	LYW6178_REG_2_ADDR,
	LYW6178_REG_3_ADDR,
	LYW6178_REG_4_ADDR,
	LYW6178_REG_5_ADDR,
	LYW6178_REG_6_ADDR,
	LYW6178_REG_7_ADDR,
	LYW6178_REG_8_ADDR,
	LYW6178_REG_9_ADDR,
	LYW6178_REG_A_ADDR,
	LYW6178_REG_B_ADDR,
	LYW6178_REG_C_ADDR,
};

unsigned int g_lyw6178_msg_level = DEFAULT_MSG_LEVEL;
module_param_named(debug_lyw6178_msg_level, g_lyw6178_msg_level, int, 0640);
MODULE_PARM_DESC(debug_lyw6178_msg_level, "backlight lyw6178 msg level");

static int lyw6178_parse_dts(struct device_node *np)
{
	int i;
	int ret;

	if ((np == NULL) || (g_lyw_dev == NULL)) {
		printk(KERN_ERR "np or lyw6178_g_chip is null\n");
		return -1;
	}
	for (i = 0; i < LYW6178_RW_REG_MAX; i++) {
		ret = of_property_read_u32(np, g_dts_str[i],
			&g_bl_info.reg[i]);
		if (ret < 0) {
			g_bl_info.reg[i] = PARSE_FAILED;
			lyw6178_info(g_lyw_dev->dev, "Not find :%s\n",
				g_dts_str[i]);
		}
	}
	ret = of_property_read_u32(np, "bl_set_long_slope",
		&g_bl_info.bl_set_long_slope);
	if (ret < 0)
		lyw6178_info(g_lyw_dev->dev,
			"bl_set_long_slope dts don't config\n");
	ret = of_property_read_u32(np, "dual_ic", &g_bl_info.dual_ic);
	if (ret < 0) {
		lyw6178_info(g_lyw_dev->dev, "can not get dual_ic dts node\n");
	} else {
		ret = of_property_read_u32(np, "lyw6178_i2c_bus_id", &g_bl_info.lyw6178_i2c_bus_id);
		if (ret < 0)
			lyw6178_info(g_lyw_dev->dev, "can not get lyw6178_i2c_bus_id dts node\n");
	}
	ret = of_property_read_u32(np, "lyw6178_hw_en_gpio",
		&g_bl_info.lyw6178_hw_en_gpio);
	if (ret < 0) {
		lyw6178_err(g_lyw_dev->dev,
			"get lyw6178_hw_en_gpio dts config failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "bl_on_kernel_mdelay",
		&g_bl_info.bl_on_kernel_mdelay);
	if (ret < 0) {
		lyw6178_err(g_lyw_dev->dev,
			"get bl_on_kernel_mdelay dts config failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "bl_led_num",
		&g_bl_info.bl_led_num);
	if (ret < 0) {
		lyw6178_err(g_lyw_dev->dev, "get bl_led_num dts config failed\n");
		return ret;
	}
	return ret;
}

static int lyw6178_2_config_write(struct lyw6178_chip_data *pchip,
	unsigned int reg[], unsigned int val[], unsigned int size)
{
	struct i2c_adapter *adap = NULL;
	struct i2c_msg msg = {0};
	char buf[2];
	int ret = 0;
	unsigned int i;

	if (!g_lyw_dev) {
		printk(KERN_ERR "lyw6178_g_chip is null\n");
		return -1;
	}
	if ((pchip == NULL) || (reg == NULL) || (val == NULL) || (pchip->client == NULL)) {
		lyw6178_err(g_lyw_dev->dev, "pchip or reg or val or clinet is null pointer\n");
		return -1;
	}

	lyw6178_info(g_lyw_dev->dev, "lyw6178_2_config_write\n");
	/* get i2c adapter */
	adap = i2c_get_adapter(g_bl_info.lyw6178_i2c_bus_id);
	if (!adap) {
		lyw6178_err(g_lyw_dev->dev, "i2c device %d not found\n", g_bl_info.lyw6178_i2c_bus_id);
		ret = -ENODEV;
		goto out;
	}
	msg.addr = pchip->client->addr;
	msg.flags = pchip->client->flags;
	msg.len = 2;
	msg.buf = buf;
	for (i = 0; i < size; i++) {
		buf[0] = reg[i];
		buf[1] = val[i];
		if (val[i] != PARSE_FAILED) {
			ret = i2c_transfer(adap, &msg, 1);
			lyw6178_info(g_lyw_dev->dev, "lyw6178_2_config_write reg=0x%x,val=0x%x\n", buf[0], buf[1]);
		}
	}
out:
	i2c_put_adapter(adap);
	return ret;
}

static int lyw6178_config_write(struct lyw6178_chip_data *pchip,
	unsigned int reg[], unsigned int val[], unsigned int size)
{
	unsigned int i;
	int ret = 0;

	if (!g_lyw_dev) {
		printk(KERN_ERR "lyw6178_g_chip is null\n");
		return -1;
	}
	if ((pchip == NULL) || (reg == NULL) || (val == NULL)) {
		lyw6178_err(g_lyw_dev->dev, "pchip or reg or val is null pointer\n");
		return -1;
	}
	for (i = 0; i < size; i++) {
		if (val[i] != PARSE_FAILED) {
			ret = regmap_write(pchip->regmap, reg[i], val[i]);
			if (ret < 0) {
				lyw6178_err(g_lyw_dev->dev,
					"write reg 0x%x failed\n", reg[i]);
				goto exit;
			}
		}
	}
exit:
	return ret;
}

static int lyw6178_config_read(struct lyw6178_chip_data *pchip,
	unsigned int reg[], unsigned int val[], unsigned int size)
{
	int ret = 0;
	unsigned int i;

	if (!g_lyw_dev) {
		printk(KERN_ERR "lyw6178_g_chip is null\n");
		return -1;
	}
	if ((pchip == NULL) || (reg == NULL) || (val == NULL)) {
		lyw6178_err(g_lyw_dev->dev, "pchip or reg or val is null pointer\n");
		return -1;
	}
	for (i = 0; i < size; i++) {
		ret = regmap_read(pchip->regmap, reg[i], &val[i]);
		if (ret < 0) {
			lyw6178_err(g_lyw_dev->dev,
				"read lyw6178 register 0x%x failed\n", reg[i]);
			goto exit;
		} else {
			lyw6178_info(g_lyw_dev->dev, "read 0x%x value = 0x%x\n",
				reg[i], val[i]);
		}
	}
exit:
	return ret;
}

static int lyw6178_chip_init(struct lyw6178_chip_data *pchip)
{
	int ret;

	if ((pchip == NULL) || (g_lyw_dev == NULL)) {
		printk(KERN_ERR "pchip or lyw6178_g_chip is null\n");
		return -1;
	}
	lyw6178_info(g_lyw_dev->dev, "in\n");
	if (g_bl_info.dual_ic) {
		ret = lyw6178_2_config_write(pchip, g_reg_addr, g_bl_info.reg, LYW6178_RW_REG_MAX);
		if (ret < 0) {
			lyw6178_err(g_lyw_dev->dev, "lyw6178 slave register failed\n");
			goto out;
		}
	}
	ret = lyw6178_config_write(pchip, g_reg_addr,
		g_bl_info.reg, LYW6178_RW_REG_MAX);
	if (ret < 0) {
		lyw6178_err(g_lyw_dev->dev, "lyw6178 register failed\n");
		goto out;
	}
	lyw6178_info(g_lyw_dev->dev, "OK\n");
	return ret;
out:
	dev_err(pchip->dev, "i2c failed to access register\n");
	return ret;
}

ssize_t lyw6178_set_backlight_init(uint32_t bl_level)
{
	int ret = 0;

	if (!g_lyw_dev) {
		printk(KERN_ERR "lyw6178_g_chip is null\n");
		return -1;
	}
	if (g_fake_lcd_flag) {
		lyw6178_info(g_lyw_dev->dev, "is fake lcd\n");
		return ret;
	}
	if (down_trylock(&(g_lyw_dev->test_sem))) {
		lyw6178_info(g_lyw_dev->dev, "Now in test mode\n");
		return 0;
	}
	if ((g_init_status == false) && (bl_level > 0)) {
		mdelay(g_bl_info.bl_on_kernel_mdelay);
		gpio_cmds_tx(g_gpio_on_cmds,
			ARRAY_SIZE(g_gpio_on_cmds));
		/* chip initialize */
		ret = lyw6178_chip_init(g_lyw_dev);
		if (ret < 0) {
			lyw6178_err(g_lyw_dev->dev, "lyw6178 init fail\n");
			goto out;
		}
		g_init_status = true;
	} else {
		lyw6178_debug(g_lyw_dev->dev,
			"lyw6178 %u, 0: off; else inited\n", bl_level);
	}
out:
	up(&(g_lyw_dev->test_sem));
	return ret;
}

bool is_lyw6178_used(void)
{
	return g_lyw6178_used;
}

static ssize_t lyw6178_reg_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t ret = -1;
	struct i2c_client *client = NULL;
	struct lyw6178_chip_data *pchip = NULL;

	if (!g_lyw_dev) {
		printk(KERN_ERR "lyw6178_g_chip is null\n");
		return -1;
	}
	if (!buf) {
		lyw6178_err(g_lyw_dev->dev, "buf is null\n");
		return ret;
	}
	if (!dev) {
		ret = snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "dev is null\n");
		return ret;
	}
	pchip = dev_get_drvdata(dev);
	if (!pchip) {
		ret = snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "data is null\n");
		return ret;
	}
	client = pchip->client;
	if (!client) {
		ret = snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "client is null\n");
		return ret;
	}
	ret = lyw6178_config_read(pchip,
		g_reg_addr, g_bl_info.reg, LYW6178_RW_REG_MAX);
	if (ret < 0) {
		lyw6178_err(g_lyw_dev->dev, "lyw6178 config read failed\n");
		goto i2c_error;
	}
	/* transfer lyw6178 regiesters config information to Application layer */
	ret = snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1,
		"lyw6178_reg_0 = 0x%x\nlyw6178_reg_1 = 0x%x\n \
		\rlyw6178_reg_2 = 0x%x\nlyw6178_reg_3 = 0x%x\n \
		\rlyw6178_reg_4 = 0x%x\nlyw6178_reg_5 = 0x%x\n \
		\rlyw6178_reg_6 = 0x%x\nlyw6178_reg_7 = 0x%x\n \
		\rlyw6178_reg_8 = 0x%x\nlyw6178_reg_9 = 0x%x\n \
		\rlyw6178_reg_a = 0x%x\nlyw6178_reg_b = 0x%x\n \
		\rlyw6178_reg_c = 0x%x\n",
		g_bl_info.reg[0], g_bl_info.reg[1],
		g_bl_info.reg[2], g_bl_info.reg[3],
		g_bl_info.reg[4], g_bl_info.reg[5],
		g_bl_info.reg[6], g_bl_info.reg[7],
		g_bl_info.reg[8], g_bl_info.reg[9],
		g_bl_info.reg[10], g_bl_info.reg[11],
		g_bl_info.reg[12]
		);
	return ret;
i2c_error:
	ret = snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%s: i2c access failed\n", __func__);
	return ret;
}

static ssize_t lyw6178_reg_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	ssize_t ret;
	unsigned int val;
	unsigned int reg;
	unsigned int mask;
	struct lyw6178_chip_data *pchip = NULL;

	if (!g_lyw_dev) {
		printk(KERN_ERR "lyw6178_g_chip is null\n");
		return -1;
	}
	if (!buf) {
		lyw6178_err(g_lyw_dev->dev, "buf is null\n");
		return -1;
	}
	if (!dev) {
		lyw6178_err(g_lyw_dev->dev, "dev is null\n");
		return -1;
	}
	pchip = dev_get_drvdata(dev);
	ret = sscanf_s(buf, "reg=0x%x, mask=0x%x, val=0x%x", &reg, &mask, &val);
	if (ret < 0) {
		lyw6178_info(g_lyw_dev->dev, "check your input\n");
		goto out_input;
	}
	if (reg > 0xFF || mask > 0xFF || val > 0xFF) {
		lyw6178_err(g_lyw_dev->dev, "input value error\n");
		goto out_input;
	}
	lyw6178_info(g_lyw_dev->dev,
		"reg=0x%x,mask=0x%x,val=0x%x\n", reg, mask, val);
	ret = regmap_update_bits(pchip->regmap, reg, mask, val);
	if (ret < 0)
		goto i2c_error;
	return size;
i2c_error:
	dev_err(pchip->dev, "i2c access fail to register\n");
	return -1;
out_input:
	dev_err(pchip->dev, "input conversion fail\n");
	return -1;
}

static DEVICE_ATTR(reg, 0644, lyw6178_reg_show, lyw6178_reg_store);

static struct attribute *g_lyw6178_attributes[] = {
	&dev_attr_reg.attr,
	NULL,
};

static const struct attribute_group g_lyw6178_group = {
	.attrs = g_lyw6178_attributes,
};

static const struct regmap_config lyw6178_regmap = {
	.reg_bits = 8,
	.val_bits = 8,
	.reg_stride = 1,
};

#ifdef CONFIG_LCD_KIT_DRIVER
#include "lcd_kit_common.h"

static void lyw6178_enable(void)
{
	int ret;

	if (!g_lyw_dev) {
		printk(KERN_ERR "lyw6178_g_chip is null\n");
		return;
	}
	mdelay(g_bl_info.bl_on_kernel_mdelay);
	gpio_cmds_tx(g_gpio_on_cmds, ARRAY_SIZE(g_gpio_on_cmds));
	/* chip initialize */
	ret = lyw6178_chip_init(g_lyw_dev);
	if (ret < 0) {
		lyw6178_err(g_lyw_dev->dev, "lyw6178_chip_init fail\n");
		return;
	}
	g_init_status = true;
}

static void lyw6178_disable(void)
{
	gpio_cmds_tx(g_gpio_disable_cmds,
		ARRAY_SIZE(g_gpio_disable_cmds));
	gpio_cmds_tx(g_gpio_free_cmds,
		ARRAY_SIZE(g_gpio_free_cmds));
	msleep(LYW6178_DISABLE_DELAY);
	g_init_status = false;
}

static int lyw6178_set_backlight(uint32_t bl_value)
{
	int ret;
	int32_t bl_max;
	unsigned int bl_msb;
	unsigned int bl_lsb;
	uint32_t bl_level = bl_value;

	if (!g_lyw_dev) {
		printk(KERN_ERR "lyw6178_g_chip is null\n");
		return -1;
	}
	if (down_trylock(&(g_lyw_dev->test_sem))) {
		lyw6178_info(g_lyw_dev->dev, "Now in test mode\n");
		return 0;
	}

	/* first set backlight, enable lyw6178 */
	if ((g_init_status == false) && (bl_level > 0))
		lyw6178_enable();

	bl_max = bl_config_max_value();
	if (bl_max > 0)
		bl_level = bl_level * LYW6178_BL_MAX / bl_max;
	if (bl_level > LYW6178_BL_MAX)
		bl_level = LYW6178_BL_MAX;

	/* set backlight level lyw6178 backlight level 12bits */
	bl_msb = (bl_level >> 8) & 0x0F; /* 4 msb bits */
	bl_lsb = bl_level & 0xFF; /* 8 lsb bits */

	ret = regmap_write(g_lyw_dev->regmap,
		g_bl_info.lyw6178_level_lsb, bl_lsb);
	if (ret < 0)
		lyw6178_err(g_lyw_dev->dev,
			"write bl level lsb:0x%x failed\n", bl_lsb);

	ret = regmap_write(g_lyw_dev->regmap,
		g_bl_info.lyw6178_level_msb, bl_msb);
	if (ret < 0)
		lyw6178_err(g_lyw_dev->dev,
			"write bl level msb:0x%x failed\n", bl_msb);

	/* if set backlight level 0, disable lyw6178 */
	if ((g_init_status == true) && (bl_level == 0))
		lyw6178_disable();
	up(&(g_lyw_dev->test_sem));
	return ret;
}

static int lyw6178_en_backlight(uint32_t bl_value)
{
	int ret = 0;

	if (!g_lyw_dev) {
		printk(KERN_ERR "lyw6178_g_chip is null\n");
		return -1;
	}
	if (down_trylock(&(g_lyw_dev->test_sem))) {
		lyw6178_info(g_lyw_dev->dev, "Now in test mode\n");
		return 0;
	}
	lyw6178_info(g_lyw_dev->dev, "lyw6178_en_backlight bl_value=%d\n", bl_value);

	/* first set backlight, enable lyw6178 */
	if ((g_init_status == false) && (bl_value > 0))
		lyw6178_enable();

	/* if set backlight level 0, disable lyw6178 */
	if ((g_init_status == true) && (bl_value == 0))
		lyw6178_disable();

	up(&(g_lyw_dev->test_sem));
	return ret;
}

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = lyw6178_set_backlight,
	.en_backlight = lyw6178_en_backlight,
	.name = "6178",
};
#endif

static int lcd_kit_lyw6178_parse_dts(struct device_node *np,
	struct lyw6178_chip_data *pchip)
{
	unsigned int vendor = 0;
	int ret = 0;

	if ((np == NULL) || (pchip == NULL) || (g_lyw_dev == NULL)) {
		printk(KERN_ERR "np or pchip or lyw6178_g_chip is null\n");
		return -1;
	}
#ifdef CONFIG_LCD_KIT_DRIVER
	np = of_find_compatible_node(NULL, NULL, DTS_COMP_LYW6178);
	if (!np) {
		lyw6178_err(g_lyw_dev->dev, "NOT FOUND device node %s\n", DTS_COMP_LYW6178);
		goto err;
	}
	/* Only testing lyw6178 used */
	ret = regmap_read(pchip->regmap, LYW6178_REG_E_ADDR,
		&vendor);
	if ((ret < 0) || (vendor != VENDOR_ID)) {
		lyw6178_err(g_lyw_dev->dev, "lyw6178 not used, vendor=0x%x\n", vendor);
		goto err;
	}
	/* Testing lyw6178-2 used */
	if (g_bl_info.dual_ic) {
		ret = lyw6178_2_config_write(pchip, g_reg_addr, g_bl_info.reg, 1);
		if (ret < 0) {
			lyw6178_err(g_lyw_dev->dev, "lyw6178 slave not used\n");
			goto err;
		}
	}
	ret = of_property_read_u32(np, "lyw6178_level_lsb",
		&g_bl_info.lyw6178_level_lsb);
	if (ret < 0) {
		lyw6178_err(g_lyw_dev->dev, "get lyw6178_level_lsb failed\n");
		goto err;
	}
	ret = of_property_read_u32(np, "lyw6178_level_msb",
		&g_bl_info.lyw6178_level_msb);
	if (ret < 0) {
		lyw6178_err(g_lyw_dev->dev, "get lyw6178_level_msb failed\n");
		goto err;
	}
	ret = lcd_kit_bl_register(&bl_ops);
	if (ret < 0) {
		lyw6178_err(g_lyw_dev->dev, "bl register fail\n");
		goto err;
	}
	g_lyw6178_used = true;
#endif

	return ret;
err:
	return -1;
}

static int lyw6178_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int ret;
	struct device_node *np = NULL;
	struct lyw6178_chip_data *pchip = NULL;
	struct i2c_adapter *adapter = client->adapter;

	lyw6178_info(&client->dev, "in\n");
	if (!i2c_check_functionality(adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "i2c functionality check fail\n");
		return -EOPNOTSUPP;
	}
	pchip = devm_kzalloc(&client->dev,
		sizeof(struct lyw6178_chip_data), GFP_KERNEL);
	if (!pchip)
		return -ENOMEM;
#ifdef CONFIG_REGMAP_I2C
	pchip->regmap = devm_regmap_init_i2c(client, &lyw6178_regmap);
	if (IS_ERR(pchip->regmap)) {
		ret = PTR_ERR(pchip->regmap);
		dev_err(&client->dev, "allocate regmap failed: %d\n", ret);
		goto err_out;
	}
#endif
	g_lyw_dev = pchip;
	pchip->client = client;
	i2c_set_clientdata(client, pchip);
	sema_init(&(pchip->test_sem), 1);
	g_lyw_class = class_create(THIS_MODULE, "lyw6178");
	if (IS_ERR(g_lyw_class)) {
		lyw6178_err(g_lyw_dev->dev,
			"Create lyw6178 class failed; errno = %ld\n",
			PTR_ERR(g_lyw_class));
		g_lyw_class = NULL;
	}
	pchip->dev = device_create(g_lyw_class, NULL, 0, "%s", client->name);
	if (IS_ERR(pchip->dev)) {
		/* Not fatal */
		lyw6178_err(g_lyw_dev->dev,
			"Create device failed; errno = %ld\n",
			PTR_ERR(pchip->dev));
		pchip->dev = NULL;
	} else {
		dev_set_drvdata(pchip->dev, pchip);
		ret = sysfs_create_group(&pchip->dev->kobj, &g_lyw6178_group);
		if (ret)
			goto err_sysfs;
	}
	ret = memset_s(&g_bl_info, sizeof(struct lyw6178_info),
			0, sizeof(struct lyw6178_info));
	if (ret != 0) {
		lyw6178_err(g_lyw_dev->dev, "memset bl_info failed!\n");
		goto err_sysfs;
	}
	np = of_find_compatible_node(NULL, NULL, DTS_COMP_LYW6178);
	if (!np) {
		lyw6178_err(g_lyw_dev->dev, "NOT FOUND device node %s\n", DTS_COMP_LYW6178);
		goto err_sysfs;
	}
	ret = lyw6178_parse_dts(np);
	if (ret < 0) {
		lyw6178_err(g_lyw_dev->dev, "parse lyw6178 dts failed\n");
		goto err_sysfs;
	}
	ret = lcd_kit_lyw6178_parse_dts(np, pchip);
	if (ret < 0) {
		lyw6178_err(g_lyw_dev->dev, "parse lcd kit lyw6178 dts failed\n");
		goto err_sysfs;
	}

	return ret;
err_sysfs:
	lyw6178_err(g_lyw_dev->dev, "sysfs error\n");
	device_destroy(g_lyw_class, 0);
err_out:
	devm_kfree(&client->dev, pchip);
	return ret;
}

static int lyw6178_remove(struct i2c_client *client)
{
	sysfs_remove_group(&client->dev.kobj, &g_lyw6178_group);
	return 0;
}

static const struct i2c_device_id lyw6178_id[] = {
	{ LYW6178_NAME, 0 },
	{},
};

static const struct of_device_id lyw6178_of_id_table[] = {
	{ .compatible = "lyw,lyw6178" },
	{},
};

MODULE_DEVICE_TABLE(i2c, lyw6178_id);
static struct i2c_driver lyw6178_i2c_driver = {
	.driver = {
		.name = "lyw6178",
		.owner = THIS_MODULE,
		.of_match_table = lyw6178_of_id_table,
	},
	.probe = lyw6178_probe,
	.remove = lyw6178_remove,
	.id_table = lyw6178_id,
};

static int __init lyw6178_module_init(void)
{
	return i2c_add_driver(&lyw6178_i2c_driver);
}

late_initcall(lyw6178_module_init);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("HW lyw6178 LED driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
