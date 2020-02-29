#include "mock.h"

#include <linux/list.h>
#include <linux/vmalloc.h>
#include <linux/kallsyms.h>
#include <linux/ftrace.h>

struct mock_pair {
    /** Pointer to the function being mocked **/
    unsigned long original;

    /** Value to return **/
    unsigned long ret;

    /** List embedding **/
    struct list_head list;
};

struct mock {
    struct list_head mock_list;
};


struct mock * init_mock(char const **function_names, unsigned long *return_values, size_t n) 
{
    int i = 0;
    struct mock_pair *pair;
    struct mock * res = vzalloc(sizeof(*res));
    
    if (NULL == res) {
        pr_warn("init_mock failed allocating memory");
        return res;
    }

    INIT_LIST_HEAD(&(res->mock_list));

    for (; i < n ; i++) {
        pair = vzalloc(sizeof(*pair));
        if (NULL == pair) {
            pr_warn("init_mock failed allocating memory");
            destroy_mock(res);
        }
        pair->original = kallsyms_lookup_name(function_names[i]);
        if (0 == pair->original) {
            pr_warn("init_mock couldn't find the function '%s'", function_names[i]);
            vfree(pair);
            destroy_mock(res);
        }
        pair->ret = return_values[i];
        list_add(&(pair->list), &(res->mock_list));
    }
    return res;
}


static inline void override_return_value(struct pt_regs *pt_regs, unsigned long value) {
     #ifdef CONFIG_X86
     typedef void (*override_return_type) (struct pt_regs *regs);
     override_return_type override_return = (override_return_type) kallsyms_lookup_name("override_function_with_return");
     regs_set_return_value(pt_regs, value);
     override_return(pt_regs);
     #elif defined(CONFIG_ARM)
     // TODO
     #endif
}

void
ftrace_callback(unsigned long ip, unsigned long parent_ip,
			  struct ftrace_ops *op, struct pt_regs *pt_regs)
{   
    // On X86_64, CALLER_ADDR2 is next to ip, CALLER_ADDR3 == parent_ip 
    // CALLER_ADDR4 is therefore the grandparent_ip, which is the call Skunk makes.
    // Hence, here we make sure that we mock the function only when mocked by Skunk.
    // The caller number may be different on other architectures / kernel compile flags.
    #ifdef CONFIG_X86_64 
    unsigned long grandparent_ip = CALLER_ADDR4;
    if (!within_module(grandparent_ip, THIS_MODULE)) {
        pr_info("Called function not within Skunk module");
        return;
    }
    #endif //CONFIG_X86_64


    if (pt_regs) {
        if (ip == kallsyms_lookup_name("call_usermodehelper_exec")) {
                override_return_value(pt_regs, 0);
                pr_info("Skunk mocking call_usermodehelper_exec on ip %lu and parent ip %lu", ip, parent_ip);
        } else if (ip == kallsyms_lookup_name("kmem_cache_alloc_trace")) {
                override_return_value(pt_regs, 0);
                pr_info("Skunk mocking kmem_cache_alloc_trace on ip %lu and parent ip %lu", ip, parent_ip);
        }
        else {
                pr_info("Couldn't find the right mock");
        }
    }
}

static struct ftrace_ops trace_ops =
{
	.func = ftrace_callback,
	.flags = FTRACE_OPS_FL_IPMODIFY | FTRACE_OPS_FL_SAVE_REGS,
};

int start_mocking(struct mock const *mock) 
{   
    struct mock_pair *pos;
    int ret;
    
    list_for_each_entry(pos, &(mock->mock_list), list) {
        ret = ftrace_set_filter_ip(&trace_ops, pos->original, 0, 0);
        if (ret) {
            pr_info("ftrace_set_filter_ip() failed: %d\n", ret);
            return ret;
        }
    }
    
    ret = register_ftrace_function(&trace_ops);
	if (WARN_ON(ret < 0)) {
		pr_info("Failed to enable function tracer %d\n", ret);
		return ret;
	}
    pr_info("success set trace");
    return ret;
}

void stop_mocking(struct mock const *mock)
{
    struct mock_pair *pos;
    unregister_ftrace_function(&trace_ops);
    
    list_for_each_entry(pos, &(mock->mock_list), list) {
        ftrace_set_filter_ip(&trace_ops, pos->original, 1, 0);
    }
}

void destroy_mock(struct mock *mock)
{
    struct mock_pair *pos, *next;
    
    if (NULL == mock){
        return;
    }
    
    list_for_each_entry_safe(pos, next, &(mock->mock_list), list) {
        vfree(pos);
    }
    
    vfree(mock);
}
