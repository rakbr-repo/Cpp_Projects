#include<linux/init.h>
#include<linux/module.h>
#include<linux/i2c.h>
#include<linux/delay.h>
#include<linux/sysfs.h>
#include<linux/device.h>
#include<linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith");
MODULE_DESCRIPTION("Bmp180 pressure and sensor detection driver");

#define BMP180_ADDR 0x77

int bmp180_probe(struct i2c_client *client)
{

}

void bmp180_remove(struct i2c_client *client)
{

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