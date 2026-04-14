#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/module.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith");
MODULE_DESCRIPTION("Character device driver");

dev_t my_dev; 
struct class *my_class;
static struct cdev c_dev;

ssize_t charReadOp(struct file *ip_file, char __user *user_buf, size_t len, loff_t *ppos)
{
    return 0;
}

static const struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .read = charReadOp
};



static int my_init(void)
{
    if(alloc_chrdev_region(&my_dev, 0, 1, "my_char_driver"))
    {
        return -1;
    }
    cdev_init(&c_dev, &my_fops);
    if(cdev_add(&c_dev, my_dev, 1))
    {
        return -1;
    }
    my_class = class_create(THIS_MODULE, "my_class");
    device_create(my_class, NULL, my_dev, NULL, "my_device");

    return 0;
}

static void my_exit(void)
{
    device_destroy(my_class, my_dev);
    class_destroy(my_class);
    cdev_del(&c_dev);
    unregister_chrdev_region(my_dev, 1);
}

module_init(my_init);
module_exit(my_exit);
