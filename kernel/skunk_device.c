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

static long copy_message_from_user(unsigned long arg, char** message)
{
    u32 message_size;

    if (copy_from_user((void *)&message_size, (void*)arg, sizeof(message_size))) {
        return -ENOMEM;
    }

    *message = kmalloc(message_size, GFP_KERNEL);
    if (NULL == *message) {
        return -ENOMEM;
    }

    if (copy_from_user(*message, ((char*)arg ) + sizeof(message_size), message_size)) {
        message_size = -ENOMEM;
        kfree(*message);
        *message = NULL;
    }

    return message_size;
}

static long copy_message_to_user(unsigned long arg, char* message, u32 message_size)
{
    if (copy_to_user((void*)arg, (void *)&message_size, sizeof(message_size))) {
        return -ENOMEM;
    }

    if (copy_to_user(((void*)arg ) + sizeof(message_size), message, message_size)) {
        return -ENOMEM;
    }
    return 0;
}


static long cmd_call_function(unsigned long arg)
{
    long ret = 0;
    u32 message_size;
    char* message = NULL;
    
    message_size = copy_message_from_user(arg, &message);
    if (0 > message_size) {
        return message_size;
    }
    ret = parse_user_buffer_and_call_function(message, &message_size);

    if (0 == ret) {
        ret = copy_message_to_user(arg, message, message_size);
    }

    if (message) {
        kfree(message);
    }

    return ret;
}

static long cmd_set_mock(unsigned long arg)
{
    Skunk__MockSetup mock_setup;
    pr_info("OMG set mock");
    
    skunk__mock_setup__init(&mock_setup);
    return 0;
}

static long ioctl_skunk_device(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret;

    switch (cmd) {
    case CALL_FUNCTION:
        ret = cmd_call_function(arg);
        break;
    case SET_MOCK:
        ret = cmd_set_mock(arg);
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