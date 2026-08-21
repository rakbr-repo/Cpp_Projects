#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/types.h>
#include<linux/kdev_t.h> 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith");
MODULE_DESCRIPTION("First character device driver");

static char *kmalloc_ptr;
static char *kzalloc_ptr;
static char *vmalloc_ptr;

#define ALLOC_SIZE_SMALL 1024
#define ALLOC_SIZE_LARGE (1024 * 1024)

struct my_object
{
    int id;
    char name[32];
}

static struct kmem_cache *my_cache;
static struct my_object *obj1;

static int my_init(void)
{
    //kmalloc
    kmalloc_ptr = kmalloc(ALLOC_SIZE_SMALL, GFP_KERNEL);
    if(!kmalloc_ptr)
    {
        pr_err("kmalloc : Error while allocating memory\n");
        goto label_kmalloc_free;
    }

    pr_info("kmalloc allocated at %px (physical %lld)", kmalloc_ptr, virt_to_phys(kmalloc_ptr));

    //kzalloc
    kzalloc_ptr = kzalloc(ALLOC_SIZE_SMALL, GFP_KERNEL);
    if(!kzalloc_ptr)
    {
        pr_err("kzalloc : Error while allocating memory\n");
        goto label_kzalloc_free;
    }

    pr_info("kzalloc allocated at %px (physical %lld)",kzalloc_ptr,virt_to_physc(kzalloc_ptr));

    //vmalloc
    vmalloc = vmalloc(ALLOC_SIZE_LARGE);
    if(!vmalloc_ptr)
    {
        pr_err("vmalloc : Error while allocating memory\n");
        goto label_vmalloc_free;
    }
    pr_info("vmalloc allocated at %px", vmalloc_ptr);

    my_cache = kmem_cache_create("my_cache",sizeof(my_object),0,SLAB_HWCACHE_ALIGN,NULL);
    if(!my_cache)
    {
        goto label_clear_memalloc;
    }

    obj1 = kmem_cache_alloc(my_cache, GFP_KERNEL);
    if(!obj1)
    {
        goto label_kmem_cache_free;
    }

    obj1->id = 20;
    strcpy(obj1->name, "Raks");

    pr_info("Slab allocated at %px",obj1);
    return 0;


label_kmem_cache_free:
    kmem_cache_destory(my_cache);
label_kzalloc_free:
    kfree(kzalloc_ptr);
label_vmalloc_free:
    kfree(vmalloc_ptr);
label_kmalloc_free:
    kfree(kmalloc_ptr);
label_clear_memalloc:
    kfree(kzalloc_ptr);
    kfree(vmalloc_ptr);
    kfree(kmalloc_ptr);

}

static void my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);
