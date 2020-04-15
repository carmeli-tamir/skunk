#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include "skunk.h"
#include "interface.h"
#include "mock_dispatcher.h"

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

static long copy_user_message_and_call(unsigned long arg, long (*parse_and_operate)(char *, u32 *))
{
    long ret = 0;
    u32 message_size;
    char* message = NULL;
    
    message_size = copy_message_from_user(arg, &message);
    if (0 > message_size) {
        return message_size;
    }

    if (0 == message_size) {
        return - EINVAL;
    }

    ret = parse_and_operate(message, &message_size);

    if (0 == ret) {
        ret = copy_message_to_user(arg, message, message_size);
    }

    if (message) {
        kfree(message);
    }

    return ret;
}

static long ioctl_skunk_device(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret;

    switch (cmd) {
    case CALL_FUNCTION:
        ret = copy_user_message_and_call(arg, parse_user_buffer_and_call_function);
        break;
    case SET_MOCK:
        ret = copy_user_message_and_call(arg, parse_message_and_set_mock);
        break;
    case UNSET_MOCK:
        ret = unset_mock();
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