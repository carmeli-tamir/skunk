#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#include "skunk.h"
#include "interface.h"

static int open_skunk_device(struct inode *inodep, struct file *filep)
{
	pr_info("skunk dev: %ld", CALL_FUNCTION);
    return 0;
}

static int release_skunk_device(struct inode *inodep, struct file *filep)
{
	return 0;
}

static long ioctl_skunk_device(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret = 0;
    u32 message_size = 0;

    switch (cmd) {
    case CALL_FUNCTION:
        if (copy_from_user((void *)&message_size, (void *)arg, sizeof(message_size))) {
            return -ENOMEM;
        }
        return parse_user_buffer_and_call_function( ((void*)arg) + sizeof(message_size), message_size);
        break;
    default:
        ret = -EINVAL;
    }
    return ret;
}

static struct file_operations fops = {.owner = THIS_MODULE,
									  .open = open_skunk_device,
									  .release = release_skunk_device,
									  .unlocked_ioctl = ioctl_skunk_device};

struct miscdevice skunk_device = {MISC_DYNAMIC_MINOR, SKUNK_DEVICE, &fops};