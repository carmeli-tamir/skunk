#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/types.h>

#include "skunk.h"

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
        return 0;
}

static struct file_operations fops = {.owner = THIS_MODULE,
									  .open = open_skunk_device,
									  .release = release_skunk_device,
									  .unlocked_ioctl = ioctl_skunk_device};

struct miscdevice skunk_device = {MISC_DYNAMIC_MINOR, SKUNK_DEVICE, &fops};