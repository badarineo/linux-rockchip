/*
 * Driver for IMX219 CMOS Image Sensor from Sony
 *
 * Copyright (C) 2014, Andrew Chew <achew@nvidia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-fwnode.h>
#include <media/v4l2-image-sizes.h>
#include <media/v4l2-mediabus.h>

#define IMX219_DEFAULT_CLK_FREQ		24000000


/* Chip ID */
#define IMX219_MODEL_ID			0x0000
/* #define IMX219_MODEL_ID_VAL		0x0219 */

/* IMX219 supported geometry */
#define IMX219_TABLE_END		0xffff
#define IMX219_ANALOGUE_GAIN_MULTIPLIER	256
#define IMX219_ANALOGUE_GAIN_MIN	(1 * IMX219_ANALOGUE_GAIN_MULTIPLIER)
#define IMX219_ANALOGUE_GAIN_MAX	(11 * IMX219_ANALOGUE_GAIN_MULTIPLIER)
#define IMX219_ANALOGUE_GAIN_DEFAULT	(2 * IMX219_ANALOGUE_GAIN_MULTIPLIER)

/* In dB*256 */
#define IMX219_DIGITAL_GAIN_MIN		256
#define IMX219_DIGITAL_GAIN_MAX		43663
#define IMX219_DIGITAL_GAIN_DEFAULT	256

#define IMX219_DIGITAL_EXPOSURE_MIN	0
#define IMX219_DIGITAL_EXPOSURE_MAX	4095
#define IMX219_DIGITAL_EXPOSURE_DEFAULT	1575

#define IMX219_EXP_LINES_MARGIN		4

static const char * const imx219_supply_name[] = {
	"vddh",
	"vddl",
	"vdig",
};

#define IMX219_NUM_SUPPLIES ARRAY_SIZE(imx219_supply_name)

#define IMX219_DEFAULT_LINK_FREQ	456000000

static const s64 link_freq_menu_items[] = {
	456000000,
};

struct imx219_reg {
	u16 addr;
	u8 val;
};

struct imx219_mode {
	u32 width;
	u32 height;
	u32 max_fps;
	u32 hts_def;
	u32 vts_def;
	const struct imx219_reg *reg_list;
};

/* MCLK:24MHz  3280x2464  21.2fps   MIPI LANE2 */
static const struct imx219_reg imx219_init_tab_3280_2464_21fps[] = {
	{0x015A, 0x01},		/* INTEG TIME[15:8] */
	{0x015B, 0xF4},		/* INTEG TIME[7:0] */
	{0x0160, 0x09},		/* FRM_LENGTH_A[15:8] */
	{0x0161, 0xC4},		/* FRM_LENGTH_A[7:0] */
	{0x0162, 0x0D},		/* LINE_LENGTH_A[15:8] */
	{0x0163, 0x78},		/* LINE_LENGTH_A[7:0] */
	{0x0260, 0x09},		/* FRM_LENGTH_B[15:8] */
	{0x0261, 0xC4},		/* FRM_LENGTH_B[7:0] */
	{0x0262, 0x0D},		/* LINE_LENGTH_B[15:8] */
	{0x0263, 0x78},		/* LINE_LENGTH_B[7:0] */
	{0x0170, 0x01},		/* X_ODD_INC_A[2:0] */
	{0x0171, 0x01},		/* Y_ODD_INC_A[2:0] */
	{0x0270, 0x01},		/* X_ODD_INC_B[2:0] */
	{0x0271, 0x01},		/* Y_ODD_INC_B[2:0] */
	{0x0174, 0x00},		/* BINNING_MODE_H_A */
	{0x0175, 0x00},		/* BINNING_MODE_V_A */
	{0x0274, 0x00},		/* BINNING_MODE_H_B */
	{0x0275, 0x00},		/* BINNING_MODE_V_B */
	{0x018C, 0x0A},		/* CSI_DATA_FORMAT_A[15:8] */
	{0x018D, 0x0A},		/* CSI_DATA_FORMAT_A[7:0] */
	{0x028C, 0x0A},		/* CSI_DATA_FORMAT_B[15:8] */
	{0x028D, 0x0A},		/* CSI_DATA_FORMAT_B[7:0] */
	{0x0301, 0x05},		/* VTPXCK_DIV */
	{0x0303, 0x01},		/* VTSYCK_DIV */
	{0x0306, 0x00},		/* PLL_VT_MPY[10:8] */
	{0x0307, 0x39},		/* PLL_VT_MPY[7:0] */
	{0x0309, 0x0A},		/* OPPXCK_DIV[4:0] */
	{0x030B, 0x01},		/* OPSYCK_DIV */
	{0x030C, 0x00},		/* PLL_OP_MPY[10:8] */
	{0x030D, 0x72},		/* PLL_OP_MPY[7:0] */
	{0x455E, 0x00},		/* CIS Tuning */
	{0x471E, 0x4B},		/* CIS Tuning */
	{0x4767, 0x0F},		/* CIS Tuning */
	{0x4750, 0x14},		/* CIS Tuning */
	{0x47B4, 0x14},		/* CIS Tuning */
	{IMX219_TABLE_END, 0x00}
};

/* MCLK:24MHz  1920x1080  30fps   MIPI LANE2 */
static const struct imx219_reg imx219_init_tab_1920_1080_30fps[] = {
	{0x0160, 0x06},
	{0x0161, 0xE6},
	{0x0162, 0x0D},
	{0x0163, 0x78},
	{0x0164, 0x02},
	{0x0165, 0xA8},
	{0x0166, 0x0A},
	{0x0167, 0x27},
	{0x0168, 0x02},
	{0x0169, 0xB4},
	{0x016A, 0x06},
	{0x016B, 0xEB},
	{0x016C, 0x07},
	{0x016D, 0x80},
	{0x016E, 0x04},
	{0x016F, 0x38},
	{0x0170, 0x01},
	{0x0171, 0x01},
	{0x0174, 0x00},
	{0x0175, 0x00},
	{0x018C, 0x0A},
	{0x018D, 0x0A},
	{0x0301, 0x05},
	{0x0303, 0x01},
	{0x0306, 0x00},
	{0x0307, 0x39},
	{0x0309, 0x0A},
	{0x030B, 0x01},
	{0x030C, 0x00},
	{0x030D, 0x72},
	{0x455E, 0x00},
	{0x471E, 0x4B},
	{0x4767, 0x0F},
	{0x4750, 0x14},
	{0x4540, 0x00},
	{0x47B4, 0x14},
	{IMX219_TABLE_END, 0x00}
};

static const struct imx219_reg mode_table_common[] = {
	{0x30EB, 0x05},		/* Access Code for address over 0x3000 */
	{0x30EB, 0x0C},		/* Access Code for address over 0x3000 */
	{0x300A, 0xFF},		/* Access Code for address over 0x3000 */
	{0x300B, 0xFF},		/* Access Code for address over 0x3000 */
	{0x30EB, 0x05},		/* Access Code for address over 0x3000 */
	{0x30EB, 0x09},		/* Access Code for address over 0x3000 */

	{IMX219_TABLE_END, 0x00}
};

enum {
	TEST_PATTERN_DISABLED,
	TEST_PATTERN_SOLID_BLACK,
	TEST_PATTERN_SOLID_WHITE,
	TEST_PATTERN_SOLID_RED,
	TEST_PATTERN_SOLID_GREEN,
	TEST_PATTERN_SOLID_BLUE,
	TEST_PATTERN_COLOR_BAR,
	TEST_PATTERN_FADE_TO_GREY_COLOR_BAR,
	TEST_PATTERN_PN9,
	TEST_PATTERN_16_SPLIT_COLOR_BAR,
	TEST_PATTERN_16_SPLIT_INVERTED_COLOR_BAR,
	TEST_PATTERN_COLUMN_COUNTER,
	TEST_PATTERN_INVERTED_COLUMN_COUNTER,
	TEST_PATTERN_PN31,
	TEST_PATTERN_MAX
};

static const char *const tp_qmenu[] = {
	"Disabled",
	"Solid Black",
	"Solid White",
	"Solid Red",
	"Solid Green",
	"Solid Blue",
	"Color Bar",
	"Fade to Grey Color Bar",
	"PN9",
	"16 Split Color Bar",
	"16 Split Inverted Color Bar",
	"Column Counter",
	"Inverted Column Counter",
	"PN31",
};

#define SIZEOF_I2C_TRANSBUF 32

struct imx219 {
	struct device *dev;
	struct regmap *regmap;
	struct clk *clk;

	struct regulator_bulk_data supplies[IMX219_NUM_SUPPLIES];
	struct gpio_desc *enable_gpio;

	u16 num_lanes;

	struct v4l2_subdev sd;
	struct media_pad pad;
	struct v4l2_ctrl_handler ctrls;
	struct v4l2_rect crop_rect;
	int hflip;
	int vflip;
	u8 analogue_gain;
	u16 digital_gain;	/* bits 11:0 */
	u16 exposure_time;
	u16 test_pattern;
	u16 test_pattern_solid_color_r;
	u16 test_pattern_solid_color_gr;
	u16 test_pattern_solid_color_b;
	u16 test_pattern_solid_color_gb;
	struct v4l2_ctrl *hblank;
	struct v4l2_ctrl *vblank;
	struct v4l2_ctrl *link_freq;
	struct v4l2_ctrl *pixel_rate;
	const struct imx219_mode *cur_mode;
	u16 cur_vts;

	/*
	 * Serialize control access, get/set format, get selection
	 * and start streaming.
	 */
	struct mutex mutex;

	bool streaming;
};

static inline struct imx219 *to_imx219(struct v4l2_subdev *sd)
{
	return container_of(sd, struct imx219, sd);
}

/*
 * pixel_rate = link_freq * data-rate * nr_of_lanes / bits_per_sample
 * data rate => double data rate; number of lanes => 4; bits per pixel => 10
 */
static u64 link_freq_to_pixel_rate(u64 f)
{
	f *= 2 * 4;
	do_div(f, 10);

	return f;
}

static const struct imx219_mode supported_modes[] = {
	{
		.width = 1920,
		.height = 1080,
		.max_fps = 30,
		.hts_def = 0x0d78 - IMX219_EXP_LINES_MARGIN,
		.vts_def = 0x06E6,
		.reg_list = imx219_init_tab_1920_1080_30fps,
	},
	{
		.width = 3280,
		.height = 2464,
		.max_fps = 21,
		.hts_def = 0x0d78 - IMX219_EXP_LINES_MARGIN,
		.vts_def = 0x09c4,
		.reg_list = imx219_init_tab_3280_2464_21fps,
	},
};

/*static struct imx219 *to_imx219(const struct i2c_client *client)
{
	return container_of(i2c_get_clientdata(client), struct imx219, subdev);
}*/

static int reg_write(struct i2c_client *client, const u16 addr, const u8 data)
{
	struct i2c_adapter *adap = client->adapter;
	struct i2c_msg msg;
	u8 tx[3];
	int ret;

	msg.addr = client->addr;
	msg.buf = tx;
	msg.len = 3;
	msg.flags = 0;
	tx[0] = addr >> 8;
	tx[1] = addr & 0xff;
	tx[2] = data;
	ret = i2c_transfer(adap, &msg, 1);
	mdelay(2);

	return ret == 1 ? 0 : -EIO;
}

#define MAX_CMD 4
static int imx219_write_table(struct imx219 *imx219,
			      const struct imx219_reg table[])
{
	u8 vals[MAX_CMD];
	int i;
	int ret;

	for (; table->addr != IMX219_TABLE_END ; table++) {
/*		if (table->addr == IMX219_TABLE_WAIT_MS) {
			usleep_range(table->val * 1000,
				     table->val * 1000 + 500);
			continue;
		} */

		for (i = 0; i < MAX_CMD; i++) {
			if (table[i].addr != (table[0].addr + i))
				break;
			vals[i] = table[i].val;
		}

		ret = regmap_bulk_write(imx219->regmap, table->addr, vals, i);

		if (ret) {
			dev_err(imx219->dev, "write_table error: %d\n", ret);
			return ret;
		}

		table += i - 1;
	}

	return 0;
}

static int imx219_power_on(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct imx219 *imx219 = to_imx219(sd);
	int ret;

	dev_info(&client->dev, "imx219 power on\n");

	ret = regulator_bulk_enable(IMX219_NUM_SUPPLIES, imx219->supplies);
	if (ret < 0) {
		dev_err(imx219->dev, "failed to enable regulators: %d\n", ret);
		return ret;
	}

	ret = clk_prepare_enable(imx219->clk);
	if (ret < 0) {
		regulator_bulk_disable(IMX219_NUM_SUPPLIES, imx219->supplies);
		dev_err(imx219->dev, "clk prepare enable failed\n");
		return ret;
	}

	usleep_range(200, 300);

	gpiod_set_value_cansleep(imx219->enable_gpio, 1);

	/* t5:6ms + t6:30k clocks = 6ms at 6MHz */
	usleep_range(12000, 15000);

	return 0;
}

static int imx219_power_off(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct imx219 *imx219 = to_imx219(sd);

	gpiod_set_value_cansleep(imx219->enable_gpio, 0);
	clk_disable_unprepare(imx219->clk);
	regulator_bulk_disable(IMX219_NUM_SUPPLIES, imx219->supplies);

	dev_info(&client->dev, "imx219 power off\n");

	return 0;
}



#define IMX219_CSI_LANE_MODE	0x0114
#define IMX219_CSI_LANE_MODE_2	0x1
#define IMX219_CSI_LANE_MODE_4	0x3

#define IMX219_DPHY_CTRL	0x0128
#define IMX219_DPHY_CTRL_AUTO	0x0
#define IMX219_DPHY_CTRL_MANUAL	0x1

#define IMX219_EXCK_FREQ_15_8	0x012a
#define IMX219_EXCK_FREQ_7_0	0x012b

#define IMX219_PREPLLCK_VT_DIV	0x0304
#define IMX219_PREPLLCK_OP_DIV	0x0305

static int imx219_setup_clock(struct imx219 *imx219)
{
	unsigned long rate;
	u16 rate_reg;
	u8 div;
	int ret;

	rate = clk_get_rate(imx219->clk);
	rate /= (1000 * 1000);

	if (rate < 6 || rate > 27) {
		dev_err(imx219->dev, "clk rate needs to be 6 - 27 MHz\n");
		return -EINVAL;
	}

	if (rate < 12)
		div = 1;
	else if (rate > 24)
		div = 3;
	else
		div = 2;

	rate_reg = cpu_to_le16(rate);

printk("%s: div is %d\n", __func__, div);
printk("%s: writing 0x%x\n", __func__, rate_reg & 0xff);
	ret = regmap_write(imx219->regmap, IMX219_EXCK_FREQ_15_8,
					   rate_reg & 0xff);
	if (ret < 0)
		return ret;

printk("%s: writing 0x%x\n", __func__, (rate_reg >> 8) & 0xff);
	ret = regmap_write(imx219->regmap, IMX219_EXCK_FREQ_7_0,
					   (rate_reg >> 8) & 0xff);
	if (ret < 0)
		return ret;

	ret = regmap_write(imx219->regmap, IMX219_PREPLLCK_VT_DIV, div);
	if (ret < 0)
		return ret;

	ret = regmap_write(imx219->regmap, IMX219_PREPLLCK_OP_DIV, div);
	if (ret < 0)
		return ret;

	return 0;
}

static int imx219_start_streaming(struct imx219 *imx219)
{
	struct i2c_client *client = v4l2_get_subdevdata(&imx219->sd);
	int ret;
	u8 reg;

	mutex_lock(&imx219->mutex);

	ret = imx219_setup_clock(imx219);
	if (ret < 0) {
		dev_err(imx219->dev, "failed to setup clocks %d\n", ret);
		goto error;
	}

	ret = imx219_write_table(imx219, mode_table_common);
	if (ret) {
		dev_err(imx219->dev, "mode reg table failed %d\n", ret);
		goto error;
	}
dev_err(imx219->dev, "number of data lanes %d\n", imx219->num_lanes);
	regmap_write(imx219->regmap, IMX219_CSI_LANE_MODE,
		     imx219->num_lanes == 4 ? IMX219_CSI_LANE_MODE_4 :
					      IMX219_CSI_LANE_MODE_2);

	regmap_write(imx219->regmap, IMX219_DPHY_CTRL, IMX219_DPHY_CTRL_AUTO);

/*	{0x0114, 0x01},*/		/* CSI_LANE_MODE[1:0] */
/*	{0x0128, 0x00},*/		/* DPHY_CNTRL */


	ret = imx219_write_table(imx219, imx219->cur_mode->reg_list);
	if (ret) {
		dev_err(imx219->dev, "mode reg table failed %d\n", ret);
		goto error;
	}

	/* Handle crop */
	ret = reg_write(client, 0x0164, imx219->crop_rect.left >> 8);
	ret |= reg_write(client, 0x0165, imx219->crop_rect.left & 0xff);
	ret |= reg_write(client, 0x0166, (imx219->crop_rect.left + imx219->crop_rect.width - 1) >> 8);
	ret |= reg_write(client, 0x0167, (imx219->crop_rect.left + imx219->crop_rect.width - 1) & 0xff);
	ret |= reg_write(client, 0x0168, imx219->crop_rect.top >> 8);
	ret |= reg_write(client, 0x0169, imx219->crop_rect.top & 0xff);
	ret |= reg_write(client, 0x016A, (imx219->crop_rect.top + imx219->crop_rect.height - 1) >> 8);
	ret |= reg_write(client, 0x016B, (imx219->crop_rect.top + imx219->crop_rect.height - 1) & 0xff);
	ret |= reg_write(client, 0x016C, imx219->crop_rect.width >> 8);
	ret |= reg_write(client, 0x016D, imx219->crop_rect.width & 0xff);
	ret |= reg_write(client, 0x016E, imx219->crop_rect.height >> 8);
	ret |= reg_write(client, 0x016F, imx219->crop_rect.height & 0xff);

	if (ret) {
		dev_err(&client->dev, "setting crop failed %d\n", ret);
		goto error;
	}

	reg = 0x0;
	/* Handle flip/mirror */
	if (imx219->hflip)
		reg |= 0x1;
	if (imx219->vflip)
		reg |= 0x2;

	ret = reg_write(client, 0x0172, reg);
	if (ret) {
		dev_err(&client->dev, "setting flip failed %d\n", ret);
		goto error;
	}

	/* Handle test pattern */
	if (imx219->test_pattern) {
		ret = reg_write(client, 0x0600, imx219->test_pattern >> 8);
		ret |= reg_write(client, 0x0601, imx219->test_pattern & 0xff);
		ret |= reg_write(client, 0x0602,
				 imx219->test_pattern_solid_color_r >> 8);
		ret |= reg_write(client, 0x0603,
				 imx219->test_pattern_solid_color_r & 0xff);
		ret |= reg_write(client, 0x0604,
				 imx219->test_pattern_solid_color_gr >> 8);
		ret |= reg_write(client, 0x0605,
				 imx219->test_pattern_solid_color_gr & 0xff);
		ret |= reg_write(client, 0x0606,
				 imx219->test_pattern_solid_color_b >> 8);
		ret |= reg_write(client, 0x0607,
				 imx219->test_pattern_solid_color_b & 0xff);
		ret |= reg_write(client, 0x0608,
				 imx219->test_pattern_solid_color_gb >> 8);
		ret |= reg_write(client, 0x0609,
				 imx219->test_pattern_solid_color_gb & 0xff);
		ret |= reg_write(client, 0x0620, imx219->crop_rect.left >> 8);
		ret |= reg_write(client, 0x0621, imx219->crop_rect.left & 0xff);
		ret |= reg_write(client, 0x0622, imx219->crop_rect.top >> 8);
		ret |= reg_write(client, 0x0623, imx219->crop_rect.top & 0xff);
		ret |= reg_write(client, 0x0624, imx219->crop_rect.width >> 8);
		ret |= reg_write(client, 0x0625, imx219->crop_rect.width & 0xff);
		ret |= reg_write(client, 0x0626, imx219->crop_rect.height >> 8);
		ret |= reg_write(client, 0x0627, imx219->crop_rect.height & 0xff);
	} else {
		ret = reg_write(client, 0x0600, 0x00);
		ret |= reg_write(client, 0x0601, 0x00);
	}
	if (ret)
		goto error;

	imx219->cur_vts = imx219->cur_mode->vts_def - IMX219_EXP_LINES_MARGIN;

	ret = __v4l2_ctrl_handler_setup(&imx219->ctrls);
	if (ret < 0) {
		dev_err(imx219->dev, "could not sync v4l2 controls\n");
		goto error;
	}

	ret = regmap_write(imx219->regmap, 0x100, 1);
	if (ret < 0) {
		dev_err(imx219->dev, "could not sent start command %d\n", ret);
		goto error;
	}

	mutex_unlock(&imx219->mutex);
	return 0;

error:
	mutex_unlock(&imx219->mutex);
	return ret;
}

static int imx219_stop_streaming(struct imx219 *imx219)
{
	int ret;

	ret = regmap_write(imx219->regmap, 0x100, 0);
	if (ret < 0)
		dev_err(imx219->dev, "could not sent stop command %d\n", ret);

	return ret;
}

/* V4L2 subdev video operations */
static int imx219_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct imx219 *imx219 = to_imx219(sd);
	int ret;

	if (imx219->streaming == enable)
		return 0;

	if (enable) {
		ret = pm_runtime_get_sync(imx219->dev);
		if (ret < 0) {
			dev_err(imx219->dev, "runtime_get failed %d\n", ret);
			pm_runtime_put_noidle(imx219->dev);
			return ret;
		}

		ret = imx219_start_streaming(imx219);
		if (ret < 0) {
			dev_err(imx219->dev, "start_streaming failed %d\n", ret);
			goto err_rpm_put;
		}
	} else {
		ret = imx219_stop_streaming(imx219);
		if (ret < 0)
			goto err_rpm_put;
		pm_runtime_put(imx219->dev);
	}

	imx219->streaming = enable;
	return 0;

err_rpm_put:
	pm_runtime_put(imx219->dev);
	return ret;

}

/* V4L2 subdev core operations */
/*static int imx219_s_power(struct v4l2_subdev *sd, int on)
{
	struct imx219 *imx219 = to_imx219(sd);

	if (on) {
		return imx219_power_on(imx219->dev);
	} else if (!on) {
		return imx219_power_off(imx219->dev);
	}

	return 0;
}*/

/* V4L2 ctrl operations */
static int imx219_s_ctrl_test_pattern(struct v4l2_ctrl *ctrl)
{
	struct imx219 *priv =
	    container_of(ctrl->handler, struct imx219, ctrls);

	switch (ctrl->val) {
	case TEST_PATTERN_DISABLED:
		priv->test_pattern = 0x0000;
		break;
	case TEST_PATTERN_SOLID_BLACK:
		priv->test_pattern = 0x0001;
		priv->test_pattern_solid_color_r = 0x0000;
		priv->test_pattern_solid_color_gr = 0x0000;
		priv->test_pattern_solid_color_b = 0x0000;
		priv->test_pattern_solid_color_gb = 0x0000;
		break;
	case TEST_PATTERN_SOLID_WHITE:
		priv->test_pattern = 0x0001;
		priv->test_pattern_solid_color_r = 0x0fff;
		priv->test_pattern_solid_color_gr = 0x0fff;
		priv->test_pattern_solid_color_b = 0x0fff;
		priv->test_pattern_solid_color_gb = 0x0fff;
		break;
	case TEST_PATTERN_SOLID_RED:
		priv->test_pattern = 0x0001;
		priv->test_pattern_solid_color_r = 0x0fff;
		priv->test_pattern_solid_color_gr = 0x0000;
		priv->test_pattern_solid_color_b = 0x0000;
		priv->test_pattern_solid_color_gb = 0x0000;
		break;
	case TEST_PATTERN_SOLID_GREEN:
		priv->test_pattern = 0x0001;
		priv->test_pattern_solid_color_r = 0x0000;
		priv->test_pattern_solid_color_gr = 0x0fff;
		priv->test_pattern_solid_color_b = 0x0000;
		priv->test_pattern_solid_color_gb = 0x0fff;
		break;
	case TEST_PATTERN_SOLID_BLUE:
		priv->test_pattern = 0x0001;
		priv->test_pattern_solid_color_r = 0x0000;
		priv->test_pattern_solid_color_gr = 0x0000;
		priv->test_pattern_solid_color_b = 0x0fff;
		priv->test_pattern_solid_color_gb = 0x0000;
		break;
	case TEST_PATTERN_COLOR_BAR:
		priv->test_pattern = 0x0002;
		break;
	case TEST_PATTERN_FADE_TO_GREY_COLOR_BAR:
		priv->test_pattern = 0x0003;
		break;
	case TEST_PATTERN_PN9:
		priv->test_pattern = 0x0004;
		break;
	case TEST_PATTERN_16_SPLIT_COLOR_BAR:
		priv->test_pattern = 0x0005;
		break;
	case TEST_PATTERN_16_SPLIT_INVERTED_COLOR_BAR:
		priv->test_pattern = 0x0006;
		break;
	case TEST_PATTERN_COLUMN_COUNTER:
		priv->test_pattern = 0x0007;
		break;
	case TEST_PATTERN_INVERTED_COLUMN_COUNTER:
		priv->test_pattern = 0x0008;
		break;
	case TEST_PATTERN_PN31:
		priv->test_pattern = 0x0009;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int imx219_g_frame_interval(struct v4l2_subdev *sd,
				   struct v4l2_subdev_frame_interval *fi)
{
	struct imx219 *priv = to_imx219(sd);
	const struct imx219_mode *mode = priv->cur_mode;

	fi->interval.numerator = 10000;
	fi->interval.denominator = mode->max_fps * 10000;

	return 0;
}

static int imx219_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct imx219 *priv =
	    container_of(ctrl->handler, struct imx219, ctrls);
	struct i2c_client *client = v4l2_get_subdevdata(&priv->sd);
	u8 reg;
	int ret = 0;
	u16 gain = 256;
	u16 a_gain = 256;
	u16 d_gain = 1;

	/*
	 * Applying V4L2 control value only happens
	 * when power is up for streaming
	 */
	ret = pm_runtime_get_if_in_use(priv->dev);
	if (ret <= 0)
		return 0;

	switch (ctrl->id) {
	case V4L2_CID_HFLIP:
		priv->hflip = ctrl->val;
		break;
	case V4L2_CID_VFLIP:
		priv->vflip = ctrl->val;
		break;
	case V4L2_CID_ANALOGUE_GAIN:
	case V4L2_CID_GAIN:
		/*
		 * hal transfer (gain * 256)  to kernel
		 * than divide into analog gain & digital gain in kernel
		 */
		gain = ctrl->val;
		if (gain < 256)
			gain = 256;
		if (gain > 43663)
			gain = 43663;
		if (gain >= 256 && gain <= 2728) {
			a_gain = gain;
			d_gain = 1 * 256;
		} else {
			a_gain = 2728;
			d_gain = (gain * 256) / a_gain;
		}

		/*
		 * Analog gain, reg range[0, 232], gain value[1, 10.66]
		 * reg = 256 - 256 / again
		 * a_gain here is 256 multify
		 * so the reg = 256 - 256 * 256 / a_gain
		 */
		priv->analogue_gain = (256 - (256 * 256) / a_gain);
		if (a_gain < 256)
			priv->analogue_gain = 0;
		if (priv->analogue_gain > 232)
			priv->analogue_gain = 232;

		/*
		 * Digital gain, reg range[256, 4095], gain rage[1, 16]
		 * reg = dgain * 256
		 */
		priv->digital_gain = d_gain;
		if (priv->digital_gain < 256)
			priv->digital_gain = 256;
		if (priv->digital_gain > 4095)
			priv->digital_gain = 4095;

		/*
		 * for bank A and bank B switch
		 * exposure time , gain, vts must change at the same time
		 * so the exposure & gain can reflect at the same frame
		 */
		ret = reg_write(client, 0x0157, priv->analogue_gain);
		ret |= reg_write(client, 0x0158, priv->digital_gain >> 8);
		ret |= reg_write(client, 0x0159, priv->digital_gain & 0xff);

		goto out;
	case V4L2_CID_EXPOSURE:
		priv->exposure_time = ctrl->val;

		ret = reg_write(client, 0x015a, priv->exposure_time >> 8);
		ret |= reg_write(client, 0x015b, priv->exposure_time & 0xff);
		goto out;
	case V4L2_CID_TEST_PATTERN:
		ret = imx219_s_ctrl_test_pattern(ctrl);
		goto out;
	case V4L2_CID_VBLANK:
		if (ctrl->val < priv->cur_mode->vts_def)
			ctrl->val = priv->cur_mode->vts_def;
		if ((ctrl->val - IMX219_EXP_LINES_MARGIN) != priv->cur_vts)
			priv->cur_vts = ctrl->val - IMX219_EXP_LINES_MARGIN;
		ret = reg_write(client, 0x0160, ((priv->cur_vts >> 8) & 0xff));
		ret |= reg_write(client, 0x0161, (priv->cur_vts & 0xff));
		goto out;
	default:
		ret = -EINVAL;
		goto out;
	}

	/* If enabled, apply settings immediately */
/*	reg = reg_read(client, 0x0100);
	if ((reg & 0x1f) == 0x01)
		imx219_s_stream(&priv->sd, 1); */

out:
	pm_runtime_put(priv->dev);

	return 0;
}

static int imx219_enum_mbus_code(struct v4l2_subdev *sd,
				 struct v4l2_subdev_pad_config *cfg,
				 struct v4l2_subdev_mbus_code_enum *code)
{
	if (code->index != 0)
		return -EINVAL;
	code->code = MEDIA_BUS_FMT_SBGGR10_1X10;

	return 0;
}

static int imx219_enum_frame_sizes(struct v4l2_subdev *sd,
				   struct v4l2_subdev_pad_config *cfg,
				   struct v4l2_subdev_frame_size_enum *fse)
{
	if (fse->index >= ARRAY_SIZE(supported_modes))
		return -EINVAL;

	if (fse->code != MEDIA_BUS_FMT_SBGGR10_1X10)
		return -EINVAL;

	fse->min_width  = supported_modes[fse->index].width;
	fse->max_width  = supported_modes[fse->index].width;
	fse->max_height = supported_modes[fse->index].height;
	fse->min_height = supported_modes[fse->index].height;

	return 0;
}

static int imx219_get_reso_dist(const struct imx219_mode *mode,
				struct v4l2_mbus_framefmt *framefmt)
{
	return abs(mode->width - framefmt->width) +
	       abs(mode->height - framefmt->height);
}

static const struct imx219_mode *imx219_find_best_fit(
					struct v4l2_subdev_format *fmt)
{
	struct v4l2_mbus_framefmt *framefmt = &fmt->format;
	int dist;
	int cur_best_fit = 0;
	int cur_best_fit_dist = -1;
	int i;

	for (i = 0; i < ARRAY_SIZE(supported_modes); i++) {
		dist = imx219_get_reso_dist(&supported_modes[i], framefmt);
		if (cur_best_fit_dist == -1 || dist < cur_best_fit_dist) {
			cur_best_fit_dist = dist;
			cur_best_fit = i;
		}
	}

	return &supported_modes[cur_best_fit];
}

static int imx219_set_fmt(struct v4l2_subdev *sd,
			  struct v4l2_subdev_pad_config *cfg,
			  struct v4l2_subdev_format *fmt)
{
	struct imx219 *priv = to_imx219(sd);
	const struct imx219_mode *mode;
	s64 h_blank, v_blank, pixel_rate;

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY)
		return 0;

	mutex_lock(&priv->mutex);

	mode = imx219_find_best_fit(fmt);
	fmt->format.code = MEDIA_BUS_FMT_SRGGB10_1X10;
	fmt->format.width = mode->width;
	fmt->format.height = mode->height;
	fmt->format.field = V4L2_FIELD_NONE;
	priv->cur_mode = mode;
	h_blank = mode->hts_def - mode->width;
	__v4l2_ctrl_modify_range(priv->hblank, h_blank,
					h_blank, 1, h_blank);
	v_blank = mode->vts_def - mode->height;
	__v4l2_ctrl_modify_range(priv->vblank, v_blank,
					v_blank,
					1, v_blank);
	pixel_rate = mode->vts_def * mode->hts_def * mode->max_fps;
	__v4l2_ctrl_modify_range(priv->pixel_rate, pixel_rate,
					pixel_rate, 1, pixel_rate);

	/* reset crop window */
	priv->crop_rect.left = 1640 - (mode->width / 2);
	if (priv->crop_rect.left < 0)
		priv->crop_rect.left = 0;
	priv->crop_rect.top = 1232 - (mode->height / 2);
	if (priv->crop_rect.top < 0)
		priv->crop_rect.top = 0;
	priv->crop_rect.width = mode->width;
	priv->crop_rect.height = mode->height;

	mutex_unlock(&priv->mutex);

	return 0;
}

static int imx219_get_fmt(struct v4l2_subdev *sd,
			  struct v4l2_subdev_pad_config *cfg,
			  struct v4l2_subdev_format *fmt)
{
	struct imx219 *priv = to_imx219(sd);
	const struct imx219_mode *mode = priv->cur_mode;

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY)
		return 0;

	mutex_lock(&priv->mutex);

	fmt->format.width = mode->width;
	fmt->format.height = mode->height;
	fmt->format.code = MEDIA_BUS_FMT_SRGGB10_1X10;
	fmt->format.field = V4L2_FIELD_NONE;

	mutex_unlock(&priv->mutex);

	return 0;
}

/* Various V4L2 operations tables */
static struct v4l2_subdev_video_ops imx219_subdev_video_ops = {
	.s_stream = imx219_s_stream,
	.g_frame_interval = imx219_g_frame_interval,
};

/*static struct v4l2_subdev_core_ops imx219_subdev_core_ops = {
	.s_power = imx219_s_power,
};*/

static const struct v4l2_subdev_pad_ops imx219_subdev_pad_ops = {
	.enum_mbus_code = imx219_enum_mbus_code,
	.enum_frame_size = imx219_enum_frame_sizes,
	.set_fmt = imx219_set_fmt,
	.get_fmt = imx219_get_fmt,
};

static struct v4l2_subdev_ops imx219_subdev_ops = {
//	.core = &imx219_subdev_core_ops,
	.video = &imx219_subdev_video_ops,
	.pad = &imx219_subdev_pad_ops,
};

static const struct v4l2_ctrl_ops imx219_ctrl_ops = {
	.s_ctrl = imx219_s_ctrl,
};

static int imx219_identify_module(struct imx219 *imx219)
{
	u16 model_id;
	u8 reg[2];
	int ret;

	ret = regmap_bulk_read(imx219->regmap, IMX219_MODEL_ID, reg, 2);
	if (ret)
		return ret;
	model_id = reg[0] << 8 | reg[1];

	if (model_id != 0x0219) {
		dev_err(imx219->dev, "Model ID: %x not supported!\n", model_id);
		return -ENODEV;
	}

	dev_info(imx219->dev, "Model ID: %x\n", model_id);

	return 0;
}

static int imx219_get_regulators(struct imx219 *imx219)
{
	unsigned int i;

	for (i = 0; i < IMX219_NUM_SUPPLIES; i++)
		imx219->supplies[i].supply = imx219_supply_name[i];

	return devm_regulator_bulk_get(imx219->dev, IMX219_NUM_SUPPLIES,
				       imx219->supplies);
}

static int imx219_parse_fwnode(struct imx219 *imx219)
{
	struct fwnode_handle *ep;
	struct v4l2_fwnode_endpoint bus_cfg = {
		.bus_type = V4L2_MBUS_CSI2_DPHY,
	};
	unsigned int i;
	int ret;

	ep = fwnode_graph_get_next_endpoint(dev_fwnode(imx219->dev), NULL);
	if (!ep) {
		dev_err(imx219->dev, "endpoint node not found\n");
		return -EINVAL;
	}

	ret = v4l2_fwnode_endpoint_alloc_parse(ep, &bus_cfg);
	if (ret) {
		dev_err(imx219->dev, "parsing endpoint node failed\n");
		goto done;
	}

	if (bus_cfg.bus_type != V4L2_MBUS_CSI2_DPHY ||
	    bus_cfg.bus.mipi_csi2.num_data_lanes == 0 ||
	    bus_cfg.nr_of_link_frequencies == 0) {
		dev_err(imx219->dev, "missing CSI-2 properties in endpoint\n");
		ret = -EINVAL;
		goto done;
	}

	imx219->num_lanes = bus_cfg.bus.mipi_csi2.num_data_lanes;

	if (imx219->num_lanes != 2 && imx219->num_lanes != 4) {
		dev_err(imx219->dev, "invalid number of lanes\n");
		ret = -EINVAL;
		goto done;
	}

	for (i = 0; i < bus_cfg.nr_of_link_frequencies; i++)
		if (bus_cfg.link_frequencies[i] == IMX219_DEFAULT_LINK_FREQ)
			break;

	if (i == bus_cfg.nr_of_link_frequencies) {
		dev_err(imx219->dev, "link-frequencies %d not supported, Please review your DT\n",
			IMX219_DEFAULT_LINK_FREQ);
		ret = -EINVAL;
		goto done;
	}

done:
	v4l2_fwnode_endpoint_free(&bus_cfg);
	fwnode_handle_put(ep);
	return ret;
}

static int imx219_ctrls_init(struct v4l2_subdev *sd)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct imx219 *priv = to_imx219(sd);
	const struct imx219_mode *mode = priv->cur_mode;
	s64 pixel_rate, h_blank, v_blank;
	int ret;

	v4l2_ctrl_handler_init(&priv->ctrls, 10);
	v4l2_ctrl_new_std(&priv->ctrls, &imx219_ctrl_ops,
			  V4L2_CID_HFLIP, 0, 1, 1, 0);
	v4l2_ctrl_new_std(&priv->ctrls, &imx219_ctrl_ops,
			  V4L2_CID_VFLIP, 0, 1, 1, 0);

printk("%s: exposure\n", __func__);
	/* exposure */
	v4l2_ctrl_new_std(&priv->ctrls, &imx219_ctrl_ops,
			  V4L2_CID_ANALOGUE_GAIN,
			  IMX219_ANALOGUE_GAIN_MIN,
			  IMX219_ANALOGUE_GAIN_MAX,
			  1, IMX219_ANALOGUE_GAIN_DEFAULT);
	v4l2_ctrl_new_std(&priv->ctrls, &imx219_ctrl_ops,
			  V4L2_CID_GAIN,
			  IMX219_DIGITAL_GAIN_MIN,
			  IMX219_DIGITAL_GAIN_MAX, 1,
			  IMX219_DIGITAL_GAIN_DEFAULT);
	v4l2_ctrl_new_std(&priv->ctrls, &imx219_ctrl_ops,
			  V4L2_CID_EXPOSURE,
			  IMX219_DIGITAL_EXPOSURE_MIN,
			  IMX219_DIGITAL_EXPOSURE_MAX, 1,
			  IMX219_DIGITAL_EXPOSURE_DEFAULT);

printk("%s: blank\n", __func__);
	/* blank */
	h_blank = mode->hts_def - mode->width;
	priv->hblank = v4l2_ctrl_new_std(&priv->ctrls, NULL, V4L2_CID_HBLANK,
			  h_blank, h_blank, 1, h_blank);
	v_blank = mode->vts_def - mode->height;
	priv->vblank = v4l2_ctrl_new_std(&priv->ctrls, NULL, V4L2_CID_VBLANK,
			  v_blank, v_blank, 1, v_blank);

printk("%s: freq\n", __func__);
	/* freq */
	priv->link_freq = v4l2_ctrl_new_int_menu(&priv->ctrls, NULL,
						V4L2_CID_LINK_FREQ,
			       0, 0, link_freq_menu_items);
	if (priv->link_freq)
		priv->link_freq->flags |= V4L2_CTRL_FLAG_READ_ONLY;


printk("%s: pixelrate\n", __func__);
//	pixel_rate = mode->vts_def * mode->hts_def * mode->max_fps;
	pixel_rate = link_freq_to_pixel_rate(link_freq_menu_items[0]);
printk("%s: pixel_rate %ld <-> %ld\n",__func__, pixel_rate, mode->vts_def * mode->hts_def * mode->max_fps);
	priv->pixel_rate = v4l2_ctrl_new_std(&priv->ctrls, NULL, V4L2_CID_PIXEL_RATE,
			  0, pixel_rate, 1, pixel_rate);


	v4l2_ctrl_new_std_menu_items(&priv->ctrls, &imx219_ctrl_ops,
				     V4L2_CID_TEST_PATTERN,
				     ARRAY_SIZE(tp_qmenu) - 1, 0, 0, tp_qmenu);

	priv->sd.ctrl_handler = &priv->ctrls;
	if (priv->ctrls.error) {
		dev_err(&client->dev, "Error %d adding controls\n",
			priv->ctrls.error);
		ret = priv->ctrls.error;
		goto error;
	}

/*	ret = v4l2_ctrl_handler_setup(&priv->ctrls);
	if (ret < 0) {
		dev_err(&client->dev, "Error %d setting default controls\n",
			ret);
		goto error;
	} */

	return 0;
error:
	v4l2_ctrl_handler_free(&priv->ctrls);
	return ret;
}

static int __maybe_unused imx219_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct imx219 *imx219 = to_imx219(sd);

	if (imx219->streaming)
		imx219_stop_streaming(imx219);

	return 0;
}

static int __maybe_unused imx219_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct imx219 *imx219 = to_imx219(sd);
	int ret;

	if (imx219->streaming) {
		ret = imx219_start_streaming(imx219);
		if (ret)
			goto error;
	}

	return 0;

error:
	imx219_stop_streaming(imx219);
	imx219->streaming = 0;
	return ret;
}

static const struct dev_pm_ops imx219_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(imx219_suspend, imx219_resume)
	SET_RUNTIME_PM_OPS(imx219_power_off, imx219_power_on, NULL)
};

static const struct regmap_config sensor_regmap_config = {
	.reg_bits = 16,
	.val_bits = 8,
	.cache_type = REGCACHE_RBTREE,
};

static int imx219_probe(struct i2c_client *client,
			const struct i2c_device_id *did)
{
	struct imx219 *imx219;
	int ret;

	imx219 = devm_kzalloc(&client->dev, sizeof(struct imx219), GFP_KERNEL);
	if (!imx219)
		return -ENOMEM;

	imx219->dev = &client->dev;

	ret = imx219_parse_fwnode(imx219);
	if (ret)
		return ret;

	imx219->clk = devm_clk_get(&client->dev, NULL);
	if (IS_ERR(imx219->clk)) {
		dev_info(&client->dev, "Error %ld getting clock\n",
			 PTR_ERR(imx219->clk));
		return -EPROBE_DEFER;
	}

	ret = imx219_get_regulators(imx219);
	if (ret < 0) {
		dev_err(&client->dev, "cannot get regulators\n");
		return ret;
	}

	imx219->enable_gpio = devm_gpiod_get(&client->dev, "enable", GPIOD_OUT_LOW);
	if (IS_ERR(imx219->enable_gpio)) {
		dev_err(&client->dev, "cannot get enable gpio\n");
		return PTR_ERR(imx219->enable_gpio);
	}

	imx219->regmap = devm_regmap_init_i2c(client, &sensor_regmap_config);
	if (IS_ERR(imx219->regmap)) {
		dev_err(&client->dev, "regmap init failed\n");
		return PTR_ERR(imx219->regmap);
	}

	/* 1920 * 1080 by default */
	imx219->cur_mode = &supported_modes[0];

	imx219->crop_rect.left = 680;
	imx219->crop_rect.top = 692;
	imx219->crop_rect.width = imx219->cur_mode->width;
	imx219->crop_rect.height = imx219->cur_mode->height;

	v4l2_i2c_subdev_init(&imx219->sd, client, &imx219_subdev_ops);

	imx219_power_on(imx219->dev);

	/* Check module identity */
	ret = imx219_identify_module(imx219);
	if (ret < 0)
		goto err_powerdown;

printk("%s: ctrls_init\n", __func__);
	ret = imx219_ctrls_init(&imx219->sd);
	if (ret < 0)
		goto err_powerdown;

	imx219->sd.ctrl_handler = &imx219->ctrls;
	mutex_init(&imx219->mutex);
	imx219->ctrls.lock = &imx219->mutex;

	imx219->sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	imx219->pad.flags = MEDIA_PAD_FL_SOURCE;
	imx219->sd.dev = &client->dev;
	imx219->sd.entity.function = MEDIA_ENT_F_CAM_SENSOR;

printk("%s: media_init\n", __func__);
	ret = media_entity_pads_init(&imx219->sd.entity, 1, &imx219->pad);
	if (ret < 0)
		goto err_mutexdestroy;

	pm_runtime_set_active(imx219->dev);
	pm_runtime_enable(imx219->dev);
	pm_runtime_idle(imx219->dev);

	ret = v4l2_async_register_subdev_sensor_common(&imx219->sd);
	if (ret < 0)
		goto err_v4lsubdev;

	return ret;

err_v4lsubdev:
	media_entity_cleanup(&imx219->sd.entity);
	v4l2_ctrl_handler_free(&imx219->ctrls);
	pm_runtime_disable(&client->dev);
	pm_runtime_set_suspended(&client->dev);
err_mutexdestroy:
	mutex_destroy(&imx219->mutex);
err_powerdown:
	imx219_power_off(imx219->dev);

	return ret;
}

static int imx219_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct imx219 *imx219 = to_imx219(sd);

	v4l2_async_unregister_subdev(&imx219->sd);
	media_entity_cleanup(&imx219->sd.entity);
	v4l2_ctrl_handler_free(&imx219->ctrls);

	pm_runtime_disable(&client->dev);
	pm_runtime_set_suspended(&client->dev);
	mutex_destroy(&imx219->mutex);

	return 0;
}

static const struct of_device_id imx219_of_match[] = {
	{ .compatible = "sony,imx219" },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, imx219_of_match);

static struct i2c_driver imx219_i2c_driver = {
	.driver = {
		.name = "imx219",
		.pm = &imx219_pm_ops,
		.of_match_table = of_match_ptr(imx219_of_match),
	},
	.probe = imx219_probe,
	.remove = imx219_remove,
};

module_i2c_driver(imx219_i2c_driver);
MODULE_DESCRIPTION("Sony IMX219 Camera driver");
MODULE_AUTHOR("Guennadi Liakhovetski <g.liakhovetski@gmx.de>");
MODULE_LICENSE("GPL v2");
