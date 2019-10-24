#include <linux/module.h>     /* Needed by all modules */
#include <linux/init.h>       /* Needed for the macros */
#include <linux/miscdevice.h>


MODULE_LICENSE("GPL");
 
MODULE_AUTHOR("carmeli.tamir@gmail.com");
 
MODULE_DESCRIPTION("Module that facilitates kernel function invocation from user mode");
 
MODULE_VERSION("0.1");

extern struct miscdevice skunk_device;

static int __init skunk_init(void)
{
    return misc_register(&skunk_device);
}
 
static void __exit skunk_end(void)
{
    misc_deregister(&skunk_device);
}
 
module_init(skunk_init);
module_exit(skunk_end);