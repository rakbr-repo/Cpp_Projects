#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/types.h>
#include<linux/kdev_t.h> 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith");
MODULE_DESCRIPTION("First character device driver");

static const char* my_device = "my_char_dev";
static dev_t dev_nr;//major number : minor number packed into this
static struct file_operations fp =
{

};
static struct cdev my_cdev;
static struct class *dev_clas;

static int my_init(void)
{
    int status = alloc_chrdev_region(&dev_nr,0,1,my_device);
    if(status)
    {
        pr_err("Allocation of major,minor numbers failed\n");
        return status;
    }

    cdev_init(&my_cdev, &fp);
    my_cdev.owner = THIS_MODULE;

    status = cdev_add(&my_cdev, dev_nr,1);
    if(status)
    {
        pr_err("Error adding cdev : %d\n",status);
        unregister_chrdev_region(dev_nr,1);//release major+minor nos if registration fails
        return status;
    }

    dev_class = class_create("my_class");

    if (IS_ERR(dev_class))
    {
        printk("Failed\n");
        return PTR_ERR(dev_class);
    }

    pr_err("Succesfully registered device, MAJOR Number : %d, MINOR Number : %d\n",MAJOR(dev_nr),MINOR(dev_nr));
    return 0;
    // major = register_chrdev(0,my_device,&fp);//LEGACY!
    // if(major<0)
    // {
    //     pr_err("Invalid major number %d",major);
    //     return major;
    // }

}

static void my_exit(void)
{
    // unregister_chrdev(major,my_device);//LEGACY
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_nr, 1);
    pr_err("Succesfully unregistered device %d\n",MAJOR(dev_nr));
}

module_init(my_init);
module_exit(my_exit);
