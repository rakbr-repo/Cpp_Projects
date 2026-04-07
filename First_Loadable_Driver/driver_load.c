#include<linux/init.h>
#include<linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith Loadable driver");
MODULE_DESCRIPTION("This is a loadable driver");

static int _init my_init(void)
{
    printk("Creating loadable driver\n");
    return 0;
}

static void _exit my_exit(void)
{
    printk("Destroy loadable driver\n");
}

module_init(my_init);
module_exit(my_exit);
