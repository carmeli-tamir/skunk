#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include "skunk.h"
#include "interface.h"

static int open_skunk_device(struct inode *inodep, struct file *filep)
{
    return 0;
}

static int release_skunk_device(struct inode *inodep, struct file *filep)
{
	return 0;
}

static long ioctl_skunk_device(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret = 0;
    u32 message_size;
    char* message;

    switch (cmd) {
    case CALL_FUNCTION:
        if (copy_from_user((void *)&message_size, (void*)arg, sizeof(message_size))) {
            return -ENOMEM;
        }
        message = kmalloc(message_size, GFP_KERNEL);
        if (NULL == message) {
            return -ENOMEM;
        }
        if (copy_from_user(message, ((char*)arg ) + sizeof(message_size), message_size)) {
            kfree(message);
            return -ENOMEM;
        }
        ret = parse_user_buffer_and_call_function(message, message_size);
        kfree(message);
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