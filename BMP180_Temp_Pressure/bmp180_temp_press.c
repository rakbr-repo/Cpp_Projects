#include <asm-generic/errno-base.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/i2c.h>
#include<linux/delay.h>
#include<linux/sysfs.h>
#include<linux/device.h>
#include<linux/slab.h>
#include <stdio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith");
MODULE_DESCRIPTION("Bmp180 pressure and sensor detection driver");

#define BMP180_ADDR 0x77
#define BMP_CALIB_REG 0xAA

struct bmp_calib {
    int16_t  AC1;
    int16_t  AC2;
    int16_t  AC3;
    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;
    int16_t  B1;
    int16_t  B2;
    int16_t  MB;
    int16_t  MC;
    int16_t  MD;
};

struct bmp_client
{
    struct i2c_client *client;
    struct bmp_calib calib;
    uint8_t oversample;
};

static DEVICE_ATTR_RO(temp_input);
static DEVICE_ATTR_RO(pressure_input);

static struct attribute *bmp_attrs[] = {
    &dev_attr_temp_input.attr,
    &dev_attr_pressure_input.attr,
    NULL,
};

static const struct attribute_group bmp_group = {
    .attrs = bmp_attrs,
}

static int bmp_read_block_data(struct i2c_client *client, struct bmp_calib reg, uint8_t *data, int data_size)
{
    int ret = i2c_smbus_read_i2c_block_data(client, reg, data,data_size);
    if(ret<0)
    {
        return ret;
    }
    else if(ret != len)
    {
        return -EIO;
    }
    return 0;
}

static int bmp_read_calib(struct i2_client *client , struct bmp_calib *c)
{
    uint8_t raw[22];
    int ret = bmp_read_block_data(client ,BMP_CALIB_REG, raw,sizeof(raw));

    if(ret)
    {
        dev_info(&client->dev, "Failed to read block data\n");
        return ret;
    }

    c->AC1 = (int16_t)((raw[0] << 8) | raw[1]); //Pair 2 bytes into 1 field , 0x01 , 0x02 -> 0x0102
    c->AC2 = (int16_t)((raw[2] << 8) | raw[3]);
    c->AC3 = (int16_t)((raw[4] << 8) | raw[5]);
    c->AC4 = (uint16_t)((raw[6] << 8) | raw[7]);
    c->AC5 = (uint16_t)((raw[8] << 8) | raw[9]);
    c->AC6 = (uint16_t)((raw[10] << 8) | raw[11]);
    c->B1  = (int16_t)((raw[12] << 8) | raw[13]);
    c->B2  = (int16_t)((raw[14] << 8) | raw[15]);
    c->MB  = (int16_t)((raw[16] << 8) | raw[17]);
    c->MC  = (int16_t)((raw[18] << 8) | raw[19]);
    c->MD  = (int16_t)((raw[20] << 8) | raw[21]);

    dev_dbg(&client->dev, "AC1=%d AC2=%d AC3=%d AC4=%u AC5=%u AC6=%u\n",
            c->AC1, c->AC2, c->AC3, c->AC4, c->AC5, c->AC6);
    dev_dbg(&client->dev, "B1=%d B2=%d MB=%d MC=%d MD=%d\n",
            c->B1, c->B2, c->MB, c->MC, c->MD);

    return 0;
}

static int bmp180_probe(struct i2c_client *client)
{
    struct bmp_client *bc;
    int ret;

    const struct i2c_device_id *id = i2c_client_get_device_id(client);
    const char *name = id ? id->name : "bmp180";

    dev_info(&client->dev, "probe: matched id = %s\n",name);

    bc = devm_kzalloc(&client->dev, sizeof(*bc), GFP_KERNEL);
    if(!bc)
    {
        return -ENOMEM;
    }

    bc->client = client;
    bc->oversample = 3;

    ret = bmp_read_calib(client, &bc->calib);
    if(ret)
    {
        dev_err(&client->dev, "could not read calibration data %d\n", ret);
        return ret;
    }

    // create sysfs attribute on device
    ret = sysfs_create_group(&client->dev.kobj, &bmp_group);
    if(ret)
    {
        dev_err(&client->dev, "failed to create sysfs group: %d\n", ret);
        return ret;
    }

    dev_info(&client->dev, "probing completed\n");
    return 0;

}

void bmp180_remove(struct i2c_client *client)
{
    sysfs_remove_group(&client->dev.kobj, &bmp_group);
    dev_info(&client->dev, "removed device\n");
}

struct metadata
{
    char name[32];
    int majorNumber;
    int minorNumber;
};

static struct metadata a = {
    .name = "bmp180",
    .majorNumber = 1,
    .minorNumber = 0
};

static struct i2c_device_id bmp180_ids[] = {
    {"bmp180", (kernel_ulong_t)&a},
    {}
};

MODULE_DEVICE_TABLE(i2c, bmp180);

static struct i2c_adapter bmp180_adapter;
static struct i2c_client bmp180_client;

static struct i2c_board_info bmp180_device_info = 
{
    I2C_BOARD_INFO(i2c,BMP180_ADDR),
};

static struct i2c_driver bmp180_driver = {
    .driver = {
        .name = "bmp180-temp-press",
    },
    .probe = &bmp180_probe,
    .remove = &bmp180_remove,
    .id_table = bmp180_ids
};

static int bmp180_init(void)
{
    int ret = i2c_add_driver(&bmp180_driver);
    if(ret)
    {
        pr_err("Failed to create bmp180 driver\n");
        return ret;
    }

    pr_info("Successfully created the bmp180 driver\n");

    //get i2c adapter
    bmp180_adapter = i2c_get_adapter(1);
    if(IS_ERR(bmp180_adapter))
    {
        pr_err("couldn't get the adapter\n");
        i2c_del_driver(&bmp180_driver);
        return PTR_ERR(bmp180_adapter);
    }

    //add a new client device
    bmp180_client = i2c_new_client_device(bmp180_adapter, &bmp180_device_info);
    i2c_put_adapter(bmp180_adapter);
    if(IS_ERR(bmp180_client))
    {
        pr_err("couldn't create new client device\n");
        i2c_del_driver(&bmp180_driver);
        return PTR_ERR(bmp180_client);
    }
    pr_info("bmp180 new I2C device added");

}

static void bmp180_exit(void)
{
    if(bmp180_client)
    {
        i2c_unregister_device(bmp180_client);
    }
    i2c_del_driver(&bmp180_driver);
	pr_info("bmp180_i2c_driver exit\n");
}

module_init(bmp180_init);
module_exit(bmp180_exit)