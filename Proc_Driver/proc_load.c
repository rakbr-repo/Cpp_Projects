#include<linux/init.h>
#include<linux/module.h>
#include<linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith");
MODULE_DESCRIPTION("Proc file read operation example");

static struct proc_dir_entry *custom_proc_node;

static ssize_t	driver_read_op(struct file *file_ptr, char __user *user_space_buffer, size_t count, loff_t *offset)
{
    printk("Driver read operation start");
    char ackMsg[] = "Ack!";
    size_t len = strlen(ackMsg);
    if(*offset >= len)
    {
        return 0;
    }
    int returnValCopyToUser = copy_to_user(user_space_buffer, ackMsg, len);
    *offset+=len;
    return len;
}

struct proc_ops driver_proc_ops = {
    .proc_read = driver_read_op
};

static int my_init(void)
{
    printk("My init constructor entry\n");
    custom_proc_node = proc_create("my_proc_driver", 0, NULL , &driver_proc_ops);//creating a proc file directory /proc/my_proc_driver
    printk("My init constructor exit\n");
    return 0;
}

static void my_exit(void)
{
    printk("My exit destuctor entry\n");
    proc_remove(custom_proc_node);
    printk("My exit destuctor exit\n");
}

module_init(my_init);
module_exit(my_exit);
