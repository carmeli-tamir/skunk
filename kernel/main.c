#include <linux/module.h>     /* Needed by all modules */
#include <linux/kernel.h>     /* Needed for KERN_INFO */
#include <linux/init.h>       /* Needed for the macros */
#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>

#include "skunk.h"

MODULE_LICENSE("GPL");
 
MODULE_AUTHOR("carmeli.tamir@gmail.com");
 
MODULE_DESCRIPTION("Module that facilitates kernel function invocation from user mode");
 
MODULE_VERSION("0.1");

typedef void * (*ptrRetOneArg)(void *arg1);
typedef void * (*ptrRetTwoArg)(void *arg1, void *arg2);
typedef void * (*ptrRetThreeArg)(void *arg1, void *arg2, void *arg3);

extern struct miscdevice skunk_device;

void demo(void)
{
    unsigned long kalsyms_addr = kallsyms_lookup_name("kallsyms_lookup_name");
    unsigned long round_jiffies_addr = kallsyms_lookup_name("round_jiffies");
    void *mem, *ret;
    struct path p;
    unsigned long kern_path_addr = kallsyms_lookup_name("kern_path");
    
    pr_info("Hello kallsysm %p == %p \n", (void*)kalsyms_addr, ((ptrRetOneArg)kalsyms_addr)("kallsyms_lookup_name"));
    pr_info("Hello round jiffies %ld \n", (unsigned long)((ptrRetOneArg)round_jiffies_addr)((void*)133713371337));
    
    mem = kmalloc(sizeof(struct path), GFP_KERNEL);
    ret = ((ptrRetThreeArg)kern_path_addr)("/etc/shadow", (void*)1 /*LOOKUP_FOLLOW*/, mem);
    memcpy(&p, mem, sizeof(struct path));
    pr_info("Hello kern_path %ld, %s \n", (unsigned long)ret, p.dentry->d_iname);
    kfree(mem);
    
}

static int __init skunk_init(void)
{
    demo();
    return misc_register(&skunk_device);
}
 
static void __exit skunk_end(void)
{
    misc_deregister(&skunk_device);
}
 
module_init(skunk_init);
module_exit(skunk_end);