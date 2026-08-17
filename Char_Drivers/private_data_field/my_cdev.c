#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/types.h>
#include<linux/kdev_t.h> 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith");
MODULE_DESCRIPTION("First character device driver");

#define DEFINE_BUFFER_SIZE 64
static struct mutex dev_mtx;

static const char* my_device = "my_char_dev";
static dev_t dev_nr;//major number : minor number packed into this

static struct cdev my_cdev;
static struct class *dev_class;
static struct device *dev;


static int my_open(struct inode *pNode, struct file *pFile)
{
    char *buffer = (char *)kzalloc(DEFINE_BUFFER_SIZE, GFP_KERNEL);
    if(!buffer)
    {
        return ENOMEM;
    }
    pFile->private_data = buffer;
    pr_info("Allocating %d of private data\n", DEFINE_BUFFER_SIZE);
    return 0;
}

static int my_release(struct inode *pNode, struct file *pFile)
{
    char *buffer = (char *)pFile->private_data;
    if(buffer)
    {
        kfree(buffer);
    }
    pr_info("Freed %d bytes which was allocated\n", DEFINE_BUFFER_SIZE);
    return 0;
}

static ssize_t my_read(struct file *pFile, char __user *pUser_buff, size_t count, loff_t *pOffset)
{
    size_t bytes_to_copy, not_copied, copied;
    char *dev_buffer = (char *)pFile->private_data;
    if(!dev_buffer)
    {
        return EINVAL;
    }

    pr_err("Read called : requested bytes -> %zu\n",count);
    if(mutex_lock_interruptible(&dev_mtx))
    {
        pr_err("Mutex lock interrupted by an external signal\n");
        return ERESTARTSYS;
    }
    bytes_to_copy = count > strlen(dev_buffer) ? strlen(dev_buffer) : count;

    not_copied = copy_to_user(pUser_buff, dev_buffer, bytes_to_copy); //insert data from a file position , and only certain no of bytes we want
    copied = bytes_to_copy - not_copied;

    if(not_copied)
    {
        pr_info("Copy to user only copied %zu/%zu\n", copied,bytes_to_copy);
    }
    pr_info("Read done return = %zu\n",copied);

    mutex_unlock(&dev_mtx);
    return (ssize_t)copied;

}

static ssize_t my_write(struct file *pFile, const char __user *pUserSpace, size_t count, loff_t *pOffset)
{
    size_t bytes_to_copy , not_copied, copied;
    char *dev_buffer = (char *)pFile->private_data;
    if(!dev_buffer)
    {
        return EINVAL;
    }
    
    pr_info("Requested write bytes : %zu\n", count);
    if(mutex_lock_interruptible(&dev_mtx))
    {
        pr_err("Mutex lock interrupted by an external signal\n");
        return ERESTARTSYS;
    }

    //If count is 66, for a 64 byte size buffer , No space retured
    if(count>=DEFINE_BUFFER_SIZE)
    {
        pr_err("Requested more bytes than buffer");
        return ENOSPC;
    }
    else
    {
        bytes_to_copy = count;
        pr_info("Writing the requested number of bytes : %zu\n",bytes_to_copy);
    }

    not_copied = copy_from_user(dev_buffer, pUserSpace, bytes_to_copy);
    copied = bytes_to_copy-not_copied;

    if(not_copied)
    {
        pr_warn("Could only copy %zu/%zu bytes\n",copied,bytes_to_copy);
    }

    pr_info("Write operation done for %zu\n",copied);
    mutex_unlock(&dev_mtx);
    return (ssize_t)copied;
}

static struct file_operations fp =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write
};

static int my_init(void)
{
    /*Mutex initialization*/
    mutex_init(&dev_mtx);

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
        printk("Failed to create a class\n");
        cdev_del(&my_cdev);
        return PTR_ERR(dev_class);
    }

    dev = device_create(dev_class,NULL,dev_nr,NULL,"mydevice%d",0);

    if(IS_ERR(dev))
    {
        pr_err("Failed to create dev node\n");
        class_destroy(dev_class);
        return PTR_ERR(dev);
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
    device_destroy(dev_class, dev_nr);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_nr, 1);
    pr_err("Succesfully unregistered device %d\n",MAJOR(dev_nr));
}

module_init(my_init);
module_exit(my_exit);
