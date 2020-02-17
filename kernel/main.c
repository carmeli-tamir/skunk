#include <linux/module.h>     /* Needed by all modules */
#include <linux/init.h>       /* Needed for the macros */
#include <linux/miscdevice.h>

#include <linux/ftrace.h>
#include <linux/namei.h>

MODULE_LICENSE("GPL");
 
MODULE_AUTHOR("carmeli.tamir@gmail.com");
 
MODULE_DESCRIPTION("Module that facilitates kernel function invocation from user mode");
 
MODULE_VERSION("0.1");

extern struct miscdevice skunk_device;

int fake_kern_path(const char *name, unsigned int flags, struct path *path)
{
	return 5;
}

void
function_test_events_call(unsigned long ip, unsigned long parent_ip,
			  struct ftrace_ops *op, struct pt_regs *pt_regs)
{
    if (!within_module(parent_ip, THIS_MODULE)) {
        return;
    }
    pr_info("Yeay called on kern_path from %ld on ip %ld", parent_ip, ip);
    if (pt_regs) {
        pt_regs->ip = (unsigned long)fake_kern_path;
    }
}

static struct ftrace_ops trace_ops =
{
	.func = function_test_events_call,
	.flags = FTRACE_OPS_FL_IPMODIFY | FTRACE_OPS_FL_SAVE_REGS,
};


static void trace_kernpath(void)
{
    struct path path;
    int ret;


    ret = ftrace_set_filter_ip(&trace_ops, kallsyms_lookup_name("kern_path"), 0, 0);
    if (ret) {
            pr_info("ftrace_set_filter_ip() failed: %d\n", ret);
            return;
    }
    ret = register_ftrace_function(&trace_ops);
	if (WARN_ON(ret < 0)) {
		pr_info("Failed to enable function tracer for event tests %d\n", ret);
		return;
	}
    
    ret = kern_path("/bin/bash", LOOKUP_FOLLOW, &path);  

    unregister_ftrace_function(&trace_ops);
    ftrace_set_filter_ip(&trace_ops, kallsyms_lookup_name("kern_path"), 1, 0);
    
    pr_info("kern_path on address %ld returned %d",kallsyms_lookup_name("kern_path"),  ret);
}


static int __init skunk_init(void)
{   trace_kernpath();
    return misc_register(&skunk_device);
}
 
static void __exit skunk_end(void)
{
    misc_deregister(&skunk_device);
}
 
module_init(skunk_init);
module_exit(skunk_end);