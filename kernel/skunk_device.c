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
    Skunk__ReturnValue skunk_ret;
    long ret = 0;
    u32 message_size;
    u32 ret_message_size;
    char* message = NULL;

    skunk__return_value__init(&skunk_ret);

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
            ret = -ENOMEM;
            goto out;
        }

        if (0 == parse_user_buffer_and_call_function(message, message_size, &skunk_ret)) {
            // Pack return value back to buffer
            ret_message_size = skunk__return_value__get_packed_size(&skunk_ret);
            if (ret_message_size > message_size) {
                pr_info("Buffer is too small");
                ret = -ENOMEM;
                goto out;
            }
            skunk__return_value__pack(&skunk_ret, message);
            if (copy_to_user((void*)arg, (void *)&ret_message_size, sizeof(ret_message_size))) {
                ret = -ENOMEM;
                goto out;
            }
            if (copy_to_user(((void*)arg ) + sizeof(message_size), message, ret_message_size)) {
                kfree(message);
                ret = -ENOMEM;
                goto out;
            }
        }

        break;
    default:
        ret = -EINVAL;
    }

out:
    if (message) {
        kfree(message);
    }
    return ret;
}

static struct file_operations fops = {.owner = THIS_MODULE,
									  .open = open_skunk_device,
									  .release = release_skunk_device,
									  .unlocked_ioctl = ioctl_skunk_device};

struct miscdevice skunk_device = {MISC_DYNAMIC_MINOR, SKUNK_DEVICE, &fops};