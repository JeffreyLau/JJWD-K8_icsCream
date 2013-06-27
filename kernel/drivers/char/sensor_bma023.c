/* 
 * linux/drivers/char/sensor_bma023.c
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Jun, 2008 
 * Description: Telechips Linux BACK-LIGHT DRIVER
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/earlysuspend.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/mach-types.h>
#include <linux/delay.h>
#include <mach/bsp.h>

#ifdef CONFIG_I2C
#include <linux/i2c.h>
#endif
#include <asm/gpio.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <mach/sensor_ioctl.h>
#include "bma023.h"

#define SENSOR_DEV_NAME			"sensor"
#define SENSOR_DEV_MAJOR		249
#define SENSOR_DEV_MINOR		1

#define SENSOR_ENABLE			1
#define SENSOR_DISABLE			0

#define SENSOR_SUPPORT			1
#define SENSOR_NOT_SUPPORT	0

#define SENSOR_G_MASK			0x1
#define SENSOR_T_MASK			0x2

#define BMA_DEBUG    0
#define BMA_DEBUG_D  0

#if BMA_DEBUG
#define sensor_dbg(fmt, arg...)     printk(fmt, ##arg)
#else
#define sensor_dbg(arg...)
#endif

#if BMA_DEBUG_D
#define sensor_dbg_d(fmt, arg...)     printk(fmt, ##arg)
#else
#define sensor_dbg_d(arg...)
#endif

typedef struct {
    int x;
    int y;
    int z;
    int resolution;
    int delay_time;
} tcc_sensor_accel_t;

typedef struct _matrix3by3 {
	short	_11;
	short	_12;
	short	_13;
	short	_21;
	short 	_22;
	short 	_23;
	short	_31;
	short 	_32;
	short 	_33;
} matrix3by3;

#define LAYOUT_CHIP2HAL	0
#define LAYOUT_HAL2CHIP	1

matrix3by3 gsenlayout[2] = 
{
    // LAYOUT_CHIP2HAL
    // LAYOUT_HAL2CHIP
#if defined(CONFIG_GSEN_TOP)
    #if defined(CONFIG_GSEN_ROTATE_0)
    {1,	0,	0,	0,	1,	0,	0,	0,	1},
    {1,	0,	0,	0,	1,	0,	0,	0,	1},
    #elif defined(CONFIG_GSEN_ROTATE_90)
    {0,	1,	0,	-1,	0,	0,	0,	0,	1},
    {0,	-1,	0,	1,	0,	0,	0,	0,	1},
    #elif defined(CONFIG_GSEN_ROTATE_180)
    {-1,	0,	0,	0,	-1,	0,	0,	0,	1},
    {-1,	0,	0,	0,	-1,	0,	0,	0,	1},
    #else // CONFIG_GSEN_ROTATE_270
    {0,	-1,	0,	1,	0,	0,	0,	0,	1},
    {0,	1,	0,	-1,	0,	0,	0,	0,	1},
    #endif
#else // CONFIG_GSEN_BOTTOM
    #if defined(CONFIG_GSEN_ROTATE_0)
    {-1,	0,	0,	0,	1,	0,	0,	0,	-1},
    {-1,	0,	0,	0,	1,	0,	0,	0,	-1},
    #elif defined(CONFIG_GSEN_ROTATE_90)
    {0,	-1,	0,	-1,	0,	0,	0,	0,	-1},
    {0,	1,	0,	1,	0,	0,	0,	0,	-1},
    #elif defined(CONFIG_GSEN_ROTATE_180)
    {1,	0,	0,	0,	-1,	0,	0,	0,	-1},
    {1,	0,	0,	0,	-1,	0,	0,	0,	-1},
    #else // CONFIG_GSEN_ROTATE_270
    {0,	1,	0,	1,	0,	0,	0,	0,	-1},
    {0,	-1,	0,	-1,	0,	0,	0,	0,	-1},
    #endif
#endif
};

struct bmasensoracc{
	short	x;
	short	y;
	short	z;
} ;

struct bmacalib{
	atomic_t x;
	atomic_t y;
	atomic_t z;
};

enum{
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
	AXIS_MAX
};

enum{
	DEVICE_TYPE_CHAR,
	DEVICE_TYPE_INPUT,
	DEVICE_TYPE_MAX
};

enum{
	CALIBRATION_SOFT,
	CALIBRATION_HW_AUTO,
	CALIBRATION_HW_MANUAL,
	CALIBRATION_MAX
};

//#define SENSOR_TUNING

#ifdef SENSOR_TUNING
#define SENSOR_TUNING_DIVIDE_2	2  	// 0.5
#define SENSOR_TUNING_DIVIDE_4	4 	// 0.25
#define SENSOR_TUNING_DIVIDE_5	5 	// 0.2
#define SENSOR_TUNING_DIVIDE_10	10 	// 0.1

#define SENSOR_TUNING_DIVIDE 	SENSOR_TUNING_DIVIDE_5

#define SENSOR_TUNING_BIT_WIDTH_EXTEND
#endif 

struct bmasensor_data {
	struct i2c_client * i2cClient;
	#ifdef SENSOR_TUNING
	atomic_t realDelayCnt;
	atomic_t realDelayTriggerCnt;
	atomic_t realDelay;
	struct bmasensoracc avgValue;
	#endif
	atomic_t delay;
	atomic_t enable;
	atomic_t suspend;
	atomic_t inputDevice;
	atomic_t resolution;
	atomic_t calibMode;
	unsigned short mode;
	struct input_dev *input;
	struct bmasensoracc value;
	struct bmacalib calibOffset;
	struct mutex value_mutex;
	struct mutex enable_mutex;
	struct mutex suspend_mutex;
	struct mutex mode_mutex;
	struct delayed_work work;
	//struct work_struct irq_work;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif
	int IRQ;
};

struct bmasensor_data *mData;

static int sensor_used_count=0;

#define BMA_MAX_DELAY			200 
#define BMA_DEFAULT_DELAY		20 

#define BMA_CHIP_ID				2
#define BMA_ORG_DATA_BIT_WIDTH	10
#ifdef SENSOR_TUNING_BIT_WIDTH_EXTEND
#define BMA_DATA_TUNING_WIDTH	2
#else
#define BMA_DATA_TUNING_WIDTH	0
#endif
#define BMA_DATA_BIT_WIDTH		(BMA_ORG_DATA_BIT_WIDTH + BMA_DATA_TUNING_WIDTH)
#define BMA_1G_RESOLUTIOIN    	(1<<(BMA_DATA_BIT_WIDTH-2))
#define BMA_DATA_ABS			(1<<(BMA_DATA_BIT_WIDTH-1))
#define ABSMIN					(-BMA_DATA_ABS)
#define ABSMAX					(BMA_DATA_ABS)
#define BMA_TEMPERATURE_RESOLUTION	2
#define BMA_TEMPERATURE_CENTER		(-30)

#define BMA023_I2C_ADDRESS      	0x38
#define BMA_CHIP_ID_REG		BMA023_CHIP_ID_REG
#define BMA_CHIP_ID				2 // bma023 chip_id

#define SENSOR_I2C_ADDRESS		(BMA023_I2C_ADDRESS)
#define SENSOR_DEFAULT_RANGE	(BMA023_RANGE_2G)
#define SENSOR_DEFAULT_BW		(BMA023_BW_25HZ)
#define SENSOR_NAME 			"tcc-accel"
#define SENSOR_HAL_NAME		"BMA023 3-axis Accelerometer"
#define SENSOR_CALIBRATION_MODE	(CALIBRATION_SOFT)
#define SENSOR_DEF_DEVICE_TYPE	(DEVICE_TYPE_CHAR)
#define SENSOR_TEMPERATURE_SUPPORT	(SENSOR_NOT_SUPPORT) // there is no therometer operation in bma023
#define SENSOR_TEMPERATURE_RESOLUTION (BMA_TEMPERATURE_RESOLUTION)
#define SENSOR_TEMPERATURE_CENTER	(BMA_TEMPERATURE_CENTER)

static int bma023_read_accel_xyz(struct i2c_client *client,	struct bmasensoracc *acc);
static void tcc_sensor_convertCoordination(struct bmasensoracc *sensor_accel, matrix3by3 *layout);
//static void tcc_sensor_set_enable_by_client_for_calibration(struct bmasensor_data *sensor_data, int enable);

#ifdef CONFIG_I2C
static struct i2c_driver sensor_i2c_driver;
//Hibernation reference pointer
struct bmasensor_data *pdata;
static const struct i2c_device_id sensor_i2c_id[] = {
    { SENSOR_NAME, 0, },
    { }
};

static int bma023_smbus_read_byte_block(struct i2c_client *client,
		unsigned char reg_addr, unsigned char *data, unsigned char len)
{
	s32 dummy;
	dummy = i2c_smbus_read_i2c_block_data(client, reg_addr, len, data);
	if (dummy < 0)
		return -1;
	return 0;
}

static void SENSOR_SEND_CMD(struct i2c_client *client,unsigned char reg, unsigned char val)
{
    unsigned char cmd[2];
    sensor_dbg_d("%s\n", __func__);
    cmd[0] = reg;
    cmd[1] = val;	
    i2c_master_send(client, cmd, 2);
}

static unsigned char SENSOR_READ_DAT(struct i2c_client *client,unsigned char reg)
{
    unsigned char buf;
    sensor_dbg_d("%s\n", __func__);
    i2c_master_send(client, &reg, 1);
    i2c_master_recv(client, &buf, 1);

    return 	buf;
}

 #endif  /* #ifdef CONFIG_I2C */

int bma023_set_range(struct i2c_client *client,char range) 
{			
	unsigned char data;
	if (range<3) {	
		data = SENSOR_READ_DAT(client,BMA023_RANGE__REG);
		data = BMA023_SET_BITSLICE(data, BMA023_RANGE, range);		  	
		SENSOR_SEND_CMD(client,BMA023_RANGE__REG, data);
	}
	return 1;
}
int bma023_set_bandwidth(struct i2c_client *client,char bw) 
{
	unsigned char data;

	data = SENSOR_READ_DAT(client,BMA023_BANDWIDTH__REG);
	data = BMA023_SET_BITSLICE(data, BMA023_BANDWIDTH, bw);
	SENSOR_SEND_CMD(client,BMA023_BANDWIDTH__REG, data);
	return 1;
}

int bma023_calibration(struct bmasensor_data *sensor_data)
{
	return 0;
}

static int bma023_read_accel_xyz(struct i2c_client *client,
		struct bmasensoracc *acc)
{
	int comres;
	unsigned char data[6];

	comres = bma023_smbus_read_byte_block(client,
			BMA023_ACC_X_LSB__REG, data, 6);

	acc->x = BMA023_GET_BITSLICE(data[0],BMA023_ACC_X_LSB);
	acc->x |= (BMA023_GET_BITSLICE(data[1],BMA023_ACC_X_MSB)<<BMA023_ACC_X_LSB__LEN);    
	acc->x = acc->x << (sizeof(short)*8-(BMA023_ACC_X_LSB__LEN+BMA023_ACC_X_MSB__LEN));
	acc->x = acc->x >> (sizeof(short)*8-(BMA023_ACC_X_LSB__LEN+BMA023_ACC_X_MSB__LEN));

	//    acc->y = BMA023_GET_BITSLICE(data[2],BMA023_ACC_Y_LSB) | (BMA023_GET_BITSLICE(data[3],BMA023_ACC_Y_MSB)<<BMA023_ACC_Y_LSB__LEN);
	acc->y = BMA023_GET_BITSLICE(data[2],BMA023_ACC_Y_LSB);
	acc->y |= (BMA023_GET_BITSLICE(data[3],BMA023_ACC_Y_MSB)<<BMA023_ACC_Y_LSB__LEN);
	acc->y = acc->y << (sizeof(short)*8-(BMA023_ACC_Y_LSB__LEN + BMA023_ACC_Y_MSB__LEN));
	acc->y = acc->y >> (sizeof(short)*8-(BMA023_ACC_Y_LSB__LEN + BMA023_ACC_Y_MSB__LEN));

	acc->z = BMA023_GET_BITSLICE(data[4],BMA023_ACC_Z_LSB); 
	acc->z |= (BMA023_GET_BITSLICE(data[5],BMA023_ACC_Z_MSB)<<BMA023_ACC_Z_LSB__LEN);
	acc->z = acc->z << (sizeof(short)*8-(BMA023_ACC_Z_LSB__LEN+BMA023_ACC_Z_MSB__LEN));
	acc->z = acc->z >> (sizeof(short)*8-(BMA023_ACC_Z_LSB__LEN+BMA023_ACC_Z_MSB__LEN));

	sensor_dbg_d(KERN_INFO "%s %d %d %d \n", __FUNCTION__,acc->x,acc->y,acc->z);

	return comres;
}

static int bma023_read_temperature(struct i2c_client *client,
		signed char *temperature)
{
	return 0;
}

int tcc_sensor_set_range(struct i2c_client *client,char range) 
{			
	return bma023_set_range(client,range);
}

int tcc_sensor_set_bandwidth(struct i2c_client *client,char bw) 
{
	return bma023_set_bandwidth(client,bw);
}

int tcc_sensor_auto_calibration(struct bmasensor_data *sensor_data)
{
	// TODO
	bma023_calibration(sensor_data);
	return 0;
}

int tcc_sensor_calibration(struct bmasensor_data *sensor_data, short x_offset,short y_offset, short z_offset)
{
	int calibMode = atomic_read(&sensor_data->calibMode);
	struct bmasensoracc calib; // calibOffset
	
	calib.x = (BMA_1G_RESOLUTIOIN * x_offset)/100;
	calib.y = (BMA_1G_RESOLUTIOIN * y_offset)/100;
	calib.z = (BMA_1G_RESOLUTIOIN * z_offset)/100;

	sensor_dbg_d("%s: A %d %d %d\n", __func__, calib.x,calib.y,calib.z);

	atomic_set(&sensor_data->calibOffset.x,calib.x);
	atomic_set(&sensor_data->calibOffset.y,calib.y);
	atomic_set(&sensor_data->calibOffset.z,calib.z);

	if(calibMode == CALIBRATION_HW_MANUAL){
		#ifdef SENSOR_TUNING_BIT_WIDTH_EXTEND
		calib.x= calib.x >> BMA_DATA_TUNING_WIDTH;
		calib.y= calib.y >> BMA_DATA_TUNING_WIDTH;
		calib.z= calib.z >> BMA_DATA_TUNING_WIDTH;
		#endif
		tcc_sensor_convertCoordination(&calib, &gsenlayout[LAYOUT_HAL2CHIP]);

		sensor_dbg_d("%s: B %d %d %d\n", __func__, calib.x,calib.y,calib.z);
		
		//bma023_set_offset_xyz(sensor_data->i2cClient,-calib.x,-calib.y,-calib.z);
	}

	return 0;
}

static int tcc_sensor_read_accel_xyz(struct i2c_client *client,
		struct bmasensoracc *acc)
{
	// TODO : get the accelerometer data form chip
	return bma023_read_accel_xyz(client, acc);
}

static int tcc_sensor_read_temperature(struct i2c_client *client,
		signed char *temperature)
{
	return bma023_read_temperature(client, temperature);
}

static int tcc_sensor_chip_init(struct i2c_client *client)
{
	int err = 0;
	unsigned char tempvalue;

	/* read chip id */
	tempvalue = SENSOR_READ_DAT(client,BMA_CHIP_ID_REG);

	if (tempvalue == BMA_CHIP_ID) {
		printk(KERN_INFO "Bosch Sensortec Device detected!\n" 
				"BMA0230 registered I2C driver!\n");
	} else{
		printk(KERN_INFO "Bosch Sensortec Device not found"
				"i2c error %d \n", tempvalue);
		err = -ENODEV;
		return err;
	}

	tempvalue = SENSOR_READ_DAT(client,0x01);	
	tcc_sensor_set_range(client,SENSOR_DEFAULT_RANGE);
	tcc_sensor_set_bandwidth(client,SENSOR_DEFAULT_BW);		
	return 0;
}

static ssize_t tcc_sensor_attr_autocalibration_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);
	
	return sprintf(buf, "%d\n", (atomic_read(&sensor_data->calibMode) == CALIBRATION_HW_AUTO)? 1:0);
}

static ssize_t tcc_sensor_attr_autocalibration_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);

	tcc_sensor_auto_calibration(sensor_data);

	return count;
}

static ssize_t tcc_sensor_attr_calibration_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", 0);
}

static ssize_t tcc_sensor_attr_calibration_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	long data1,data2,data3;
	int error;
	struct i2c_client *client = to_i2c_client(dev);
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);

	sensor_dbg(KERN_INFO "%s  count = %d\n", __FUNCTION__,count);	
	//error = strict_strtol(buf, 10, &data);
	error = sscanf(buf, "%ld %ld %ld\n",&data1,&data2,&data3);
	sensor_dbg(KERN_INFO "%s : data = %d %d %d error = %d \n", __FUNCTION__,data1,data2,data3,error);	
	if (error != 3)
		return error;

	tcc_sensor_calibration(sensor_data,data1,data2,data3);

	sensor_dbg(KERN_INFO "%s end\n", __FUNCTION__);	

	return count;
}

static ssize_t tcc_sensor_attr_resolution_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", atomic_read(&sensor_data->resolution));

}

static ssize_t tcc_sensor_attr_resolution_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	unsigned long data;
	int error;
	struct i2c_client *client = to_i2c_client(dev);
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);

	error = strict_strtoul(buf, 10, &data);
	sensor_dbg(KERN_INFO "%s : data = %d error = %d \n", __FUNCTION__,data,error);	
	if (error)
		return error;
	if (data > BMA_MAX_DELAY)
		data = BMA_MAX_DELAY;
	atomic_set(&sensor_data->resolution, (unsigned int) data);

	return count;
}

static ssize_t tcc_sensor_attr_inputdevice_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", atomic_read(&sensor_data->inputDevice));

}

static ssize_t tcc_sensor_attr_inputdevice_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	unsigned long data;
	int error;
	struct i2c_client *client = to_i2c_client(dev);
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);

	error = strict_strtoul(buf, 10, &data);
	sensor_dbg(KERN_INFO "%s : data = %d error = %d \n", __FUNCTION__,data,error);	
	if (error)
		return error;
	if (data > BMA_MAX_DELAY)
		data = BMA_MAX_DELAY;
	atomic_set(&sensor_data->inputDevice, (unsigned int) data);

	return count;
}

static ssize_t tcc_sensor_attr_delay_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", atomic_read(&sensor_data->delay));

}

static ssize_t tcc_sensor_attr_delay_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	unsigned long data;
	int error;
	struct i2c_client *client = to_i2c_client(dev);
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);

	error = strict_strtoul(buf, 10, &data);
	sensor_dbg(KERN_INFO "%s : data = %d error = %d \n", __FUNCTION__,data,error);	
	if (error)
		return error;
	if (data > BMA_MAX_DELAY)
		data = BMA_MAX_DELAY;
	atomic_set(&sensor_data->delay, (unsigned int) data);
#ifdef SENSOR_TUNING
	atomic_set(&sensor_data->realDelay, (unsigned int) data/SENSOR_TUNING_DIVIDE);
	atomic_set(&sensor_data->realDelayCnt, 0);
	atomic_set(&sensor_data->realDelayTriggerCnt, SENSOR_TUNING_DIVIDE);
#endif
	return count;
}

static ssize_t tcc_sensor_attr_enable_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", atomic_read(&sensor_data->enable));

}

/*
static void tcc_sensor_set_enable_by_client_for_calibration(struct bmasensor_data *sensor_data, int enable)
{
	int pre_enable = atomic_read(&sensor_data->enable);

	sensor_dbg(KERN_INFO "%s : enable = %d \n", __FUNCTION__,enable);	

	mutex_lock(&sensor_data->enable_mutex);
	
	if (enable) {
		if (pre_enable !=0) {
			#ifdef SENSOR_TUNING
			schedule_delayed_work(&sensor_data->work,
					msecs_to_jiffies(atomic_read(&sensor_data->realDelay)));
			#else
			schedule_delayed_work(&sensor_data->work,
					msecs_to_jiffies(atomic_read(&sensor_data->delay)));
			#endif
		}

	} else {
		if (pre_enable !=0) {
			cancel_delayed_work_sync(&sensor_data->work);
		}
	}
	mutex_unlock(&sensor_data->enable_mutex);

}
*/

static void tcc_sensor_attr_set_enable_by_client(struct bmasensor_data *sensor_data, int enable)
{
	int pre_enable = atomic_read(&sensor_data->enable);
	int what = ((enable&SENSOR_T_MASK) != 0)? 2:1;
	int enableDisable = enable&SENSOR_ENABLE;

	sensor_dbg(KERN_INFO "%s : enable = %d \n", __FUNCTION__,enable);	

	mutex_lock(&sensor_data->enable_mutex);
	
	if (enableDisable) {
		if (pre_enable ==0) {
			#ifdef SENSOR_TUNING
			schedule_delayed_work(&sensor_data->work,
					msecs_to_jiffies(atomic_read(&sensor_data->realDelay)));
			#else			
			schedule_delayed_work(&sensor_data->work,
					msecs_to_jiffies(atomic_read(&sensor_data->delay)));
			#endif
		}
		pre_enable |= what;
		atomic_set(&sensor_data->enable, pre_enable);
	} else {
		pre_enable = pre_enable&(~what);
		atomic_set(&sensor_data->enable, pre_enable);
	}
	//printk(" ~~~~set enable = 0x%x enableDisable=0x%x pre_enable =0x%x\n",enable,enableDisable,pre_enable);
	mutex_unlock(&sensor_data->enable_mutex);

}

static void tcc_sensor_attr_set_enable(struct device *dev, int enable)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);
	tcc_sensor_attr_set_enable_by_client(sensor_data,enable);
}

static ssize_t tcc_sensor_attr_enable_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	unsigned long data;
	int error;

	error = strict_strtoul(buf, 10, &data);

	sensor_dbg(KERN_INFO "%s : data = %d error = %d  \n", __FUNCTION__,data,error);	
	
	if (error)
		return error;
	if ((data >= 0) && (data<4)) {
		tcc_sensor_attr_set_enable(dev,data);
	}

	return count;
}

static ssize_t tcc_sensor_attr_halname_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", SENSOR_HAL_NAME);
}

static ssize_t tcc_sensor_attr_halname_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	return 0;
}

static ssize_t tcc_sensor_attr_temperature_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d %d %d\n", SENSOR_TEMPERATURE_SUPPORT, 
		SENSOR_TEMPERATURE_CENTER,SENSOR_TEMPERATURE_RESOLUTION);
	//return sprintf(buf, "%d %d %d\n", 1,24,2);
}

static ssize_t tcc_sensor_attr_temperature_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	return 0;
}

static DEVICE_ATTR(delay, S_IRUGO|S_IWUSR|S_IWGRP,
		tcc_sensor_attr_delay_show, tcc_sensor_attr_delay_store);
static DEVICE_ATTR(enable, S_IRUGO|S_IWUSR|S_IWGRP,
		tcc_sensor_attr_enable_show, tcc_sensor_attr_enable_store);
static DEVICE_ATTR(resolution, S_IRUGO|S_IWUSR|S_IWGRP,
		tcc_sensor_attr_resolution_show, tcc_sensor_attr_resolution_store);
static DEVICE_ATTR(inputdevice, S_IRUGO|S_IWUSR|S_IWGRP,
		tcc_sensor_attr_inputdevice_show, tcc_sensor_attr_inputdevice_store);
static DEVICE_ATTR(calibration, S_IRUGO|S_IWUSR|S_IWGRP,
		tcc_sensor_attr_calibration_show, tcc_sensor_attr_calibration_store);
static DEVICE_ATTR(autocalibration, S_IRUGO|S_IWUSR|S_IWGRP,
		tcc_sensor_attr_autocalibration_show, tcc_sensor_attr_autocalibration_store);
static DEVICE_ATTR(halname, S_IRUGO|S_IWUSR|S_IWGRP,
		tcc_sensor_attr_halname_show, tcc_sensor_attr_halname_store);
static DEVICE_ATTR(temperature, S_IRUGO|S_IWUSR|S_IWGRP,
		tcc_sensor_attr_temperature_show, tcc_sensor_attr_temperature_store);
	
static struct attribute *tcc_sensor_attributes[] = {
	&dev_attr_delay.attr,
	&dev_attr_enable.attr,
	&dev_attr_resolution.attr,
	&dev_attr_inputdevice.attr,
	&dev_attr_calibration.attr,
	&dev_attr_autocalibration.attr,
	&dev_attr_halname.attr,
	&dev_attr_temperature.attr,
	NULL
};

static struct attribute_group tcc_sensor_attribute_group = {
	.attrs = tcc_sensor_attributes
};

static int tcc_sensor_i2c_register(void)
{
    struct i2c_board_info info;
    struct i2c_adapter *adapter;
    struct i2c_client *client;

    memset(&info, 0, sizeof(struct i2c_board_info));
    info.addr = SENSOR_I2C_ADDRESS;
    strlcpy(info.type, SENSOR_NAME, I2C_NAME_SIZE);

    sensor_dbg(KERN_INFO "%s \n",__FUNCTION__);
    	  
#if defined(CONFIG_ARCH_TCC93XX)
    adapter = i2c_get_adapter(3);
#else
    if(machine_is_m57te()){
        adapter = i2c_get_adapter(1);
    }
    else if(machine_is_m801())  // 89_froyo
    {
        adapter = i2c_get_adapter(0);
    }
    else{  // 88_froyo (88_93 demo black board)  // M8801_8803
        sensor_dbg(KERN_INFO "%s : i2c_get_adapter(0)\n", __FUNCTION__);
        adapter = i2c_get_adapter(0);
    }
#endif
    if (!adapter) 
    {
        sensor_dbg(KERN_ERR "can't get i2c adapter 0 for tcc-accel-sensor\n");
        return -ENODEV;
    }

    client = i2c_new_device(adapter, &info);
    i2c_put_adapter(adapter);
    if (!client) 
    {
        sensor_dbg(KERN_ERR "can't add i2c device at 0x%x\n", (unsigned int)info.addr);
        return -ENODEV;
    }

    return 0;
}

static void tcc_sensor_convertCoordination(struct bmasensoracc *sensor_accel, matrix3by3 *layout)
{
    short x,y,z;
    if(sensor_accel == NULL)
        return;
    x = sensor_accel->x;
    y = sensor_accel->y;
    z = sensor_accel->z;

    sensor_accel->x = x*layout->_11 + y*layout->_12 + z*layout->_13;
    sensor_accel->y = x*layout->_21 + y*layout->_22 + z*layout->_23;
    sensor_accel->z = x*layout->_31 + y*layout->_32 + z*layout->_33;	
}

static void tcc_sensor_compensation(struct bmasensor_data *sensor_data,struct bmasensoracc *accelData)
{
	if(atomic_read(&sensor_data->calibMode) == CALIBRATION_SOFT){
		#ifdef SENSOR_TUNING_BIT_WIDTH_EXTEND
		accelData->x <<= BMA_DATA_TUNING_WIDTH;
		accelData->y <<= BMA_DATA_TUNING_WIDTH;
		accelData->z <<= BMA_DATA_TUNING_WIDTH;
		#endif		
		accelData->x += (short)atomic_read(&sensor_data->calibOffset.x);
		accelData->y += (short)atomic_read(&sensor_data->calibOffset.y);
		accelData->z += (short)atomic_read(&sensor_data->calibOffset.z);
	}
}

static void tcc_sensor_set_data(struct bmasensor_data *sensor_data,struct bmasensoracc *accelData)
{
	short x,y,z;
	struct bmasensoracc tmpData;

	x = accelData->x;
	y = accelData->y;
	z = accelData->z;

	tmpData.x = x;
	tmpData.y = y;
	tmpData.z = z;
	tcc_sensor_convertCoordination(&tmpData, &gsenlayout[LAYOUT_CHIP2HAL]);

	tcc_sensor_compensation(sensor_data,&tmpData);

	accelData->x = tmpData.x;
	accelData->y = tmpData.y;
	accelData->z = tmpData.z;
	
}

#ifdef SENSOR_TUNING
static void tcc_sensor_avg_data(struct bmasensoracc *avgAcc,struct bmasensoracc *acc)
{
	int data[3];
	int readData[3];
	int avgData[3];
	int alpha = 100/SENSOR_TUNING_DIVIDE;

	readData[0] = (int)acc->x;
	readData[1] = (int)acc->y;
	readData[2] = (int)acc->z;

	avgData[0] = (int)avgAcc->x;
	avgData[1] = (int)avgAcc->y;
	avgData[2] = (int)avgAcc->z;	
	
	data[0] = ((int)(avgData[0] * (100 - alpha)) + (int)(readData[0] * (alpha)))/100;
	data[1] = ((int)(avgData[1] * (100 - alpha)) + (int)(readData[1] * (alpha)))/100;
	data[2] = ((int)(avgData[2]* (100 - alpha)) + (int)(readData[2] * (alpha)))/100;
	avgAcc->x = (short)data[0];
	avgAcc->y = (short)data[1];
	avgAcc->z = (short)data[2];
	acc->x = (short)data[0];
	acc->y = (short)data[1];
	acc->z = (short)data[2];
}
#endif

static void tcc_sensor_work_func(struct work_struct *work)
{
	int pre_enable;
	int usedAsInputDevice = 0;

	struct bmasensor_data *sensor_data = container_of((struct delayed_work *)work,
			struct bmasensor_data, work);
	static struct bmasensoracc acc;
	#ifdef SENSOR_TUNING
	struct bmasensoracc *avgAcc = &sensor_data->avgValue;
	unsigned long delay = msecs_to_jiffies(atomic_read(&sensor_data->realDelay));
	int checkCnt = atomic_read(&sensor_data->realDelayCnt);
	int triggerCnt = atomic_read(&sensor_data->realDelayTriggerCnt); 
	#else	
	unsigned long delay = msecs_to_jiffies(atomic_read(&sensor_data->delay));
	#endif			
	struct i2c_client *client = sensor_data->i2cClient;
	int sensorWhat = 0;

	mutex_lock(&sensor_data->enable_mutex);
	sensorWhat = atomic_read(&sensor_data->enable);
	mutex_unlock(&sensor_data->enable_mutex);

	if(sensorWhat & SENSOR_G_MASK){
	tcc_sensor_read_accel_xyz(client, &acc);

	tcc_sensor_set_data(sensor_data,&acc);

	#ifdef SENSOR_TUNING
	if(delay == 0)
		delay = 1;
	tcc_sensor_avg_data(avgAcc,&acc);
	checkCnt++;
	atomic_set(&sensor_data->realDelayCnt, checkCnt);
	if(checkCnt < triggerCnt){
		goto set_schedule;
	}
	atomic_set(&sensor_data->realDelayCnt, 0);
	#endif
	}

	sensor_dbg(KERN_INFO "%s %d %d %d \n", __FUNCTION__,acc.x,acc.y,acc.z);
	
	usedAsInputDevice = atomic_read(&sensor_data->inputDevice);

	//if(SENSOR_TEMPERATURE_SUPPORT == SENSOR_SUPPORT)
	if(sensorWhat & SENSOR_T_MASK)
	{
		signed char temperature = 0;		
		tcc_sensor_read_temperature(client, &temperature);		
		input_report_abs(sensor_data->input, ABS_THROTTLE, (int)temperature);
	}
	
       if(usedAsInputDevice && (sensorWhat & SENSOR_G_MASK))
       {
	    input_report_abs(sensor_data->input, ABS_X, acc.x);
	    input_report_abs(sensor_data->input, ABS_Y, acc.y);
	    input_report_abs(sensor_data->input, ABS_Z, acc.z);
       }

	if((usedAsInputDevice && (sensorWhat & SENSOR_G_MASK)) || (sensorWhat & SENSOR_T_MASK))
	{
	    input_sync(sensor_data->input);
	}

	mutex_lock(&sensor_data->value_mutex);
	sensor_data->value = acc;
	mutex_unlock(&sensor_data->value_mutex);
#ifdef SENSOR_TUNING	
set_schedule:
#endif
	mutex_lock(&sensor_data->enable_mutex);
	pre_enable = atomic_read(&sensor_data->enable);
	if(pre_enable != 0){
		schedule_delayed_work(&sensor_data->work, delay);
	}
	mutex_unlock(&sensor_data->enable_mutex);
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void tcc_sensor_suspend(struct early_suspend *handler)
{
	struct bmasensor_data *sensor_data = container_of(handler, struct bmasensor_data, early_suspend);
	int enableFlag = 0;

	mutex_lock(&sensor_data->enable_mutex);
	enableFlag = atomic_read(&sensor_data->enable);

	if (enableFlag !=0) {
		//cancel_delayed_work_sync(&sensor_data->work);
		mutex_lock(&sensor_data->suspend_mutex);
		atomic_set(&sensor_data->suspend,enableFlag);
		atomic_set(&sensor_data->enable,0);
		mutex_unlock(&sensor_data->suspend_mutex);
	}
	mutex_unlock(&sensor_data->enable_mutex);	
}

static void tcc_sensor_resume(struct early_suspend *handler)
{
	struct bmasensor_data *sensor_data = container_of(handler, struct bmasensor_data, early_suspend);
	int suspendFlag = 0;
	mutex_lock(&sensor_data->suspend_mutex);
	suspendFlag = atomic_read(&sensor_data->suspend);

	if (suspendFlag!=0) {
		mutex_lock(&sensor_data->enable_mutex);
		atomic_set(&sensor_data->enable,suspendFlag);
		mutex_unlock(&sensor_data->enable_mutex);
		#ifdef SENSOR_TUNING
		schedule_delayed_work(&sensor_data->work,
				msecs_to_jiffies(atomic_read(&sensor_data->realDelay)));	
		#else
		schedule_delayed_work(&sensor_data->work,
				msecs_to_jiffies(atomic_read(&sensor_data->delay)));
		#endif
	}
	mutex_unlock(&sensor_data->suspend_mutex);	
}
#endif

void tcc_sensor_early_suspend(void)
{
	int enableFlag = 0;
	mutex_lock(&pdata->enable_mutex);
	enableFlag = atomic_read(&pdata->enable);

	if (enableFlag !=0) {
		//cancel_delayed_work_sync(&sensor_data->work);
		mutex_lock(&pdata->suspend_mutex);
		atomic_set(&pdata->suspend,enableFlag);
		atomic_set(&pdata->enable,0);
		mutex_unlock(&pdata->suspend_mutex);
	}
	mutex_unlock(&pdata->enable_mutex);	
}
EXPORT_SYMBOL(tcc_sensor_early_suspend);

void tcc_sensor_late_resume(void)
{
	int suspendFlag = 0;
	mutex_lock(&pdata->suspend_mutex);
	suspendFlag = atomic_read(&pdata->suspend);

	if (suspendFlag!=0) {
		tcc_sensor_chip_init(pdata->i2cClient);
		mutex_lock(&pdata->enable_mutex);
		atomic_set(&pdata->enable,suspendFlag);
		mutex_unlock(&pdata->enable_mutex);
		#ifdef SENSOR_TUNING
		schedule_delayed_work(&pdata->work,
				msecs_to_jiffies(atomic_read(&pdata->realDelay)));	
		#else
		schedule_delayed_work(&pdata->work,
				msecs_to_jiffies(atomic_read(&pdata->delay)));
		#endif
	}
	mutex_unlock(&pdata->suspend_mutex);	
}
EXPORT_SYMBOL(tcc_sensor_late_resume);


static int sensor_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int err = 0;
	unsigned char tempvalue;
	struct bmasensor_data *data;
	struct input_dev *dev;

	sensor_dbg(KERN_INFO "%s  :  %s \n", __func__,client->name);

	data = kzalloc(sizeof(struct bmasensor_data), GFP_KERNEL);
	if (!data) {
		err = -ENOMEM;
		goto exit;
	}

	atomic_set(&data->resolution, BMA_1G_RESOLUTIOIN);
	atomic_set(&data->calibOffset.x, 0);
	atomic_set(&data->calibOffset.y, 0);
	atomic_set(&data->calibOffset.z, 0);
	atomic_set(&data->inputDevice, SENSOR_DEF_DEVICE_TYPE);
	atomic_set(&data->calibMode,SENSOR_CALIBRATION_MODE);

	data->i2cClient = client;
	i2c_set_clientdata(client, data);
	mutex_init(&data->value_mutex);
	mutex_init(&data->mode_mutex);
	mutex_init(&data->enable_mutex);
	mutex_init(&data->suspend_mutex);

       mData = NULL;	

	tempvalue = tcc_sensor_chip_init(data->i2cClient);
	if( tempvalue < 0){
		printk(KERN_INFO "sensor_init fail %d \n", tempvalue);
		err = -ENODEV;
		goto kfree_exit;
	}

	INIT_DELAYED_WORK(&data->work, tcc_sensor_work_func);
	atomic_set(&data->delay, BMA_MAX_DELAY);
	#ifdef SENSOR_TUNING
	atomic_set(&data->realDelay, BMA_MAX_DELAY/SENSOR_TUNING_DIVIDE);
	atomic_set(&data->realDelayCnt, 0);
	atomic_set(&data->realDelayTriggerCnt, SENSOR_TUNING_DIVIDE);
	#endif
	atomic_set(&data->enable, 0);
	atomic_set(&data->suspend, 0);

	dev = input_allocate_device();
	if (!dev)
		return -ENOMEM;
	dev->name = SENSOR_NAME;
	dev->id.bustype = BUS_I2C;

	set_bit(EV_ABS, dev->evbit);
	input_set_abs_params(dev, ABS_X, ABSMIN, ABSMAX, 0, 0);
	input_set_abs_params(dev, ABS_Y, ABSMIN, ABSMAX, 0, 0);
	input_set_abs_params(dev, ABS_Z, ABSMIN, ABSMAX, 0, 0);
	input_set_abs_params(dev, ABS_THROTTLE, -128, 128, 0, 0);

	input_set_drvdata(dev, data);

	err = input_register_device(dev);
	if (err < 0) {
		input_free_device(dev);
		goto kfree_exit;
	}

	data->input = dev;

	err = sysfs_create_group(&data->input->dev.kobj,
			&tcc_sensor_attribute_group);
	if (err < 0)
		goto error_sysfs;

       mData = (struct bmasensor_data *)input_get_drvdata(dev);

#ifdef CONFIG_HAS_EARLYSUSPEND
	data->early_suspend.suspend = tcc_sensor_suspend;
	data->early_suspend.resume = tcc_sensor_resume;
	register_early_suspend(&data->early_suspend);
   pdata = data;
#endif	
	return 0;

error_sysfs:
	input_unregister_device(data->input);

kfree_exit:
	kfree(data);
exit:
	return err;
}

static int sensor_i2c_remove(struct i2c_client *client)
{
    struct bmasensor_data *data  = i2c_get_clientdata(client);
    sysfs_remove_group(&data->input->dev.kobj, &tcc_sensor_attribute_group);
    input_unregister_device(data->input);
#ifdef CONFIG_HAS_EARLYSUSPEND	
    unregister_early_suspend(&data->early_suspend);
#endif
    kfree(data);
    mData = NULL;

    return 0;
}

#if 0
#ifdef CONFIG_PM
static int sensor_i2c_suspend(struct i2c_client *client, pm_message_t mesg)
{
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);
	int enableFlag = 0;

	mutex_lock(&sensor_data->enable_mutex);
	enableFlag = atomic_read(&sensor_data->enable);
	if (enableFlag !=0) {
		//cancel_delayed_work_sync(&sensor_data->work);
		mutex_lock(&sensor_data->suspend_mutex);
		atomic_set(&sensor_data->suspend,enableFlag);
		atomic_set(&sensor_data->enable,0);
		mutex_unlock(&sensor_data->suspend_mutex);
	}
	mutex_unlock(&sensor_data->enable_mutex);

	return 0;
}

static int sensor_i2c_resume(struct i2c_client *client)
{
	struct bmasensor_data *sensor_data = i2c_get_clientdata(client);
	int suspendFlag = 0;
	mutex_lock(&sensor_data->suspend_mutex);
	suspendFlag = atomic_read(&sensor_data->suspend);
	if (suspendFlag!=0) {
		mutex_lock(&sensor_data->enable_mutex);
		atomic_set(&sensor_data->enable,suspendFlag);
		mutex_unlock(&sensor_data->enable_mutex);
		#ifdef SENSOR_TUNING
		schedule_delayed_work(&sensor_data->work,
				msecs_to_jiffies(atomic_read(&sensor_data->realDelay)));	
		#else
		schedule_delayed_work(&sensor_data->work,
				msecs_to_jiffies(atomic_read(&sensor_data->delay)));
		#endif
	}
	mutex_unlock(&sensor_data->suspend_mutex);

	return 0;
}

#else
#define sensor_i2c_suspend		NULL
#define sensor_i2c_resume		NULL
#endif /* CONFIG_PM */
#endif

/* bmaxxx i2c control layer */
static struct i2c_driver sensor_i2c_driver = {
    .driver = {
	.owner	= THIS_MODULE,
	.name	= SENSOR_NAME,
    },
    .probe      = sensor_i2c_probe,
    .remove     = sensor_i2c_remove,
    //.suspend   = sensor_i2c_suspend,
    //.resume   = sensor_i2c_resume,
    .id_table   = sensor_i2c_id,
};

static ssize_t tcc_sensor_write(struct file *file, const char __user *user, size_t size, loff_t *o)
{
    sensor_dbg("%s\n", __func__);
    return 0;
 }

static ssize_t tcc_sensor_read(struct file *file, char __user *user, size_t size, loff_t *o)
{
    tcc_sensor_accel_t tccData;
    //sensor_dbg("%s\n", __func__);    

    mutex_lock(&mData->value_mutex);
    tccData.x = mData->value.x;
    tccData.y = mData->value.y;
    tccData.z = mData->value.z;
    mutex_unlock(&mData->value_mutex);

    tccData.delay_time = (int)atomic_read(&mData->delay);
    tccData.resolution = (int)atomic_read(&mData->resolution);	
    sensor_dbg("%s: %d, %d, %d\n", __func__, tccData.x, tccData.y, tccData.z);

    if(copy_to_user(( tcc_sensor_accel_t*) user, (const void *)&tccData, sizeof( tcc_sensor_accel_t))!=0)
    {
        sensor_dbg("tcc_gsensor_read error\n");
    }
    return 0;
}

long tcc_sensor_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    sensor_dbg("%s  (0x%x)  \n", __FUNCTION__, cmd);
	
    switch (cmd)
    {
        case IOCTL_SENSOR_SET_INPUTDEVICE:
        {
            unsigned int value = 0;
            if(copy_from_user((void *)&value, (unsigned int*) arg, sizeof(unsigned int))!=0)
            {
                sensor_dbg("copy_from error\n");
            }					
            sensor_dbg(KERN_INFO "%s:  IOCTL_SENSOR_SET_INPUTDEVICE (0x%x) %d \n", __FUNCTION__, cmd, value);
            atomic_set(&mData->inputDevice, value);		
        }
            break;
        case IOCTL_SENSOR_GET_RESOLUTION:
        {
            unsigned int value = 0;
            value = BMA_1G_RESOLUTIOIN;
            if(copy_to_user((unsigned int*) arg, (const void *)&value, sizeof(unsigned int))!=0)
            {
                sensor_dbg("copy_to error\n");
            }			
            sensor_dbg(KERN_INFO "%s:  IOCTL_SENSOR_GET_RESOLUTION (0x%x) %d \n", __FUNCTION__, cmd, value);			
        }
            break;
        case IOCTL_SENSOR_GET_TEMP_INFO:
        {
            unsigned int value[3];
            value[0] = SENSOR_TEMPERATURE_SUPPORT;
            value[1] = SENSOR_TEMPERATURE_CENTER;
            value[2] = SENSOR_TEMPERATURE_RESOLUTION;
	
            if(copy_to_user((unsigned int*) arg, (const void *)&value[0], sizeof(unsigned int)*3)!=0)
            {
                sensor_dbg("copy_to error\n");
            }			
            sensor_dbg(KERN_INFO "%s:  IOCTL_SENSOR_GET_TEMP_INFO (0x%x) %d %d %d \n", __FUNCTION__, cmd, value[0],value[1],value[2]);				
        }
            break;  
        default:
            sensor_dbg("sensor: unrecognized ioctl (0x%x)\n", cmd); 
            return -EINVAL;
            break;
    }
    return 0;
}

static int tcc_sensor_release(struct inode *inode, struct file *filp)
{
    sensor_dbg("%s (%d)\n", __FUNCTION__, sensor_used_count);
    sensor_used_count--;
    if (sensor_used_count < 0) {
        sensor_dbg("sensor: release error (over)\n"); 
        sensor_used_count = 0;
    }

    if (sensor_used_count == 0)
    {
        if(mData != NULL && mData->i2cClient != NULL)
            tcc_sensor_attr_set_enable_by_client(mData, SENSOR_DISABLE);
        //atomic_set(&mData->inputDevice,DEVICE_TYPE_INPUT);
    }
    return 0;
}

static int tcc_sensor_open(struct inode *inode, struct file *filp)
{
    sensor_dbg("%s : \n", __FUNCTION__);

    if(mData == NULL && mData->i2cClient == NULL)
    {
        printk(KERN_INFO "%s:   mData == NULL(%d) or mData.i2cClient == NULL(%d)\n", __func__,(mData ==NULL)? 1: 0,(mData->i2cClient == NULL)? 1:0);
        return -1;
    }
    if (sensor_used_count == 0)
    {
        atomic_set(&mData->delay, BMA_DEFAULT_DELAY);
	#ifdef SENSOR_TUNING
		atomic_set(&mData->realDelay, BMA_MAX_DELAY/SENSOR_TUNING_DIVIDE);
		atomic_set(&mData->realDelayCnt, 0);
		atomic_set(&mData->realDelayTriggerCnt, SENSOR_TUNING_DIVIDE);
	#endif		
        tcc_sensor_attr_set_enable_by_client(mData, SENSOR_ENABLE);
        sensor_used_count++;
    }

    //atomic_set(&mData->inputDevice,0);
    return 0;
}

struct file_operations tcc_sensor_fops =
{
    .owner    = THIS_MODULE,
    .open     = tcc_sensor_open,
    .release  = tcc_sensor_release,
    .unlocked_ioctl    = tcc_sensor_ioctl,
    .read     = tcc_sensor_read,
    .write    = tcc_sensor_write,	
};

int __init tcc_sensor_init(void)
{
    int ret,ret1;
    struct class *sensor_class;
    //int num = MINOR(inode->i_rdev);

    sensor_dbg(KERN_INFO "%s \n", __FUNCTION__);

    //if (sensor_used_count == 0)
    {
        if(machine_is_m57te())
        {  // 89_froyo m57te
            //output gpio_f17
            gpio_request(TCC_GPF(17), NULL);
            tcc_gpio_config(TCC_GPF(17), GPIO_FN(0));
            gpio_direction_output(TCC_GPF(17),0);
            gpio_set_value(TCC_GPF(17), 0);

            //Input gpio_f24
            gpio_request(TCC_GPF(24), NULL);
            tcc_gpio_config(TCC_GPF(24), GPIO_FN(0));
            gpio_direction_input(TCC_GPF(24));
        }
        else if(machine_is_m801()){  // 89_froyo M801
            gpio_request(TCC_GPF(24), NULL);
            tcc_gpio_config(TCC_GPF(24), GPIO_FN(0));
            gpio_direction_input(TCC_GPF(24));
        }
        else if(machine_is_m801_88()){  // M801_88 board
            gpio_request(TCC_GPB(28), NULL);
            tcc_gpio_config(TCC_GPB(28), GPIO_FN(0));
            gpio_direction_input(TCC_GPB(28));
        }else {
            sensor_dbg("%s : machine is demo board \n", __FUNCTION__);
        #if defined(CONFIG_ARCH_TCC88XX)
            // 88���� ���� 88_D2_6.0 version GPIOF(25)
            gpio_request(TCC_GPF(25), NULL);
            tcc_gpio_config(TCC_GPF(25), GPIO_FN(0));
            gpio_direction_input(TCC_GPF(25));
        #endif
        }
    }
#ifdef CONFIG_I2C
        // Initialize I2C driver for BMA023
        ret = i2c_add_driver(&sensor_i2c_driver);
        if(ret < 0) 
        {
            sensor_dbg("%s() [Error] failed i2c_add_driver() = %d\n", __func__, ret);
            return ret;
        }
        ret = tcc_sensor_i2c_register();   // call register after ( called probe )
        if(ret < 0) 
        {
            sensor_dbg("%s() [Error] Failed register i2c client driver for bma023, return is %d\n", __func__, ret);
            return ret;
        }
        sensor_dbg("%s: post bmaxx_i2c_register : %x\n", __func__, ret);
#endif

    ret1 = register_chrdev(SENSOR_DEV_MAJOR, SENSOR_DEV_NAME, &tcc_sensor_fops);
    sensor_dbg("%s: register_chrdev ret1 : %d\n", __func__, ret1);
    if(ret1 >= 0)
    {
        sensor_class = class_create(THIS_MODULE, SENSOR_DEV_NAME);
        device_create(sensor_class,NULL,MKDEV(SENSOR_DEV_MAJOR,SENSOR_DEV_MINOR),NULL,SENSOR_DEV_NAME);
    }

    return ret;
}

void __exit tcc_sensor_exit(void)
{
    sensor_dbg(KERN_INFO "%s\n", __FUNCTION__);
    i2c_del_driver(&sensor_i2c_driver);
    unregister_chrdev(SENSOR_DEV_MAJOR, SENSOR_DEV_NAME);
}

module_init(tcc_sensor_init);
module_exit(tcc_sensor_exit);

MODULE_AUTHOR("Telechips Inc. c2-g2-2 linux@telechips.com");
MODULE_DESCRIPTION("TCCxxx accel-gsensor driver");
MODULE_LICENSE("GPL");

