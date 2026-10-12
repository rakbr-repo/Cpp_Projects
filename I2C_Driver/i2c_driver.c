#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith");
MODULE_DESCRIPTION("I2C Client driver - BMP 180 Device");

struct my_data
{
    const char* name;
    int id;
};

static struct my_data a = {
    .name = "a obj",
    .id = 1
};

static struct my_data b = {
    .name = "b obj",
    .id = 2
};

static struct i2c_device_id my_ids[] = {
    {"bmp180-a", (kernel_ulong_t)&a},
    {"bmp180-b", (kernel_ulong_t)&b},{}
};
MODULE_DEVICE_TABLE(i2c, my_ids);

static int my_probe(struct i2c_client *client)
{
    const struct i2c_device_id *id = i2c_client_get_device_id(client);
    pr_info("Device id of I2C client driver %s\n",id->name);

    struct my_data *data = (struct my_data*)id->driver_data;
    pr_info("Private data fields value : 1) %s , 2) %d\n", data->name, data->id);

    pr_info("bmp180_i2c_driver ID:0x%x\n", i2c_smbus_read_byte_data(client, 0xD0));
    return 0;
}
	
static void driver_remove(struct i2c_client *client)
{
    pr_info("Removing the driver\n");
}

static struct i2c_driver my_driver =
{
    .probe = my_probe,
    .remove = driver_remove,
    .id_table = my_ids,
    .driver = {
        .name = "bmp-180 driver"
    }
};

#define BMP180_ADDR 0x77

static struct i2c_adapter *my_adapter;
static struct i2c_client *my_client;

static struct i2c_board_info my_board = {I2C_BOARD_INFO("bmp180-a", BMP180_ADDR),};

static int my_init(void)
{
    int ret = i2c_add_driver(&my_driver);
    if(ret)
    {
        pr_err("Error in adding bmp180 i2c driver\n");
        return ret;
    }
    pr_info("bmp180 driver created\n");

    my_adapter = i2c_get_adapter(1);
    if(IS_ERR(my_adapter))
    {
        pr_err("Failed to get the adapter on bus 1\n");
        i2c_del_driver(&my_driver);
        return PTR_ERR(my_adapter);
    }

    my_client = i2c_new_client_device(my_adapter, &my_board);
    if(IS_ERR(my_client))
    {
        pr_err("Failed to get the adapter on bus 1\n");
        i2c_del_driver(&my_driver);
        return PTR_ERR(my_client);
    }

    i2c_put_adapter(my_adapter);
    pr_info("Successfully created I2C Device");
    return 0;
}

static void my_exit(void)
{
    if(my_client)
    {
        i2c_unregister_device(my_client);
    }
    i2c_del_driver(&my_driver);
    pr_info("Deleted the bmp180 driver which was added");
}

module_init(my_init);
module_exit(my_exit);
