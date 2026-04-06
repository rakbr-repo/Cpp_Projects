#include<linux/init.h>
#include<linux/module.h>

MODULE_LICENSE("GPL")
MODULE_AUTHOR("Rakshith Loadable driver")
MODULE_DESCRIPTION("This is a loadable driver")

static int c_tor(void)
{
    printk("Creating loadable driver")
    return 0;
}

static void d_tor(void)
{
    printk("Destroy loadable driver");
}

module_init(c_tor);
module_exit(d-tor);
