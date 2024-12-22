/*
* specific.c -- a specific module
*
*/
 
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
 
#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>         /* kmalloc() */
#include <linux/fs.h>           /* everything... */
#include <linux/errno.h>        /* error codes */
#include <linux/types.h>        /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>        /* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>

//#include <asm/system.h>         /* cli(), *_flags */
#include <asm/uaccess.h>        /* copy_*_user */
 
 
/*
* Our parameters which can be set at load time.
*/

 
int specific_major =   0;
int specific_minor =   0;
int memsize	= 255;

module_param(specific_major, int, S_IRUGO);
module_param(specific_minor, int, S_IRUGO);
module_param(memsize, int, S_IRUGO);

MODULE_AUTHOR("Sarvenaz Ashoori");
MODULE_LICENSE("Dual BSD/GPL");


struct specific_dev {
         char *data;  /* Pointer to data area */
	 int memsize;
         struct semaphore sem;     /* mutual exclusion semaphore    */
         struct cdev cdev;         /* structure for char devices */
};
 
struct specific_dev specific_device; 



int specific_open(struct inode *inode, struct file *filp)
{
         struct specific_dev *dev; 	/* a pointer to a simple_dev structire */
 
         dev = container_of(inode->i_cdev, struct specific_dev, cdev);
         filp->private_data = dev; /* stored here to be re-used in other system call*/
 
         return 0;           
}


int specific_close(struct inode *inode, struct file *filp)
{
         return 0;
}

ssize_t specific_write(struct file *filp, const char __user *buf, size_t count,
                 loff_t *f_pos)
{
         struct specific_dev *dev = filp->private_data;
         ssize_t retval = 0; /* return value */
 
         if (down_interruptible(&dev->sem))
                 return -ERESTARTSYS;

	 if (count >= dev->memsize) 
                 count = dev->memsize;

         if (copy_from_user(dev->data, buf, count)) {
                 retval = -EFAULT;
                 goto out;
         }
          printk("%s", dev->data);  /* Logs "[1" to the kernel log */
         retval = count;

	out:
         	up(&dev->sem);
         	return retval;
}

struct file_operations specific_fops = {
         .owner =    THIS_MODULE,
         .write =    specific_write,
         .open =     specific_open,
         .release =  specific_close,
};

void specific_cleanup_module(void)
{
         dev_t devno = MKDEV(specific_major, specific_minor);
 
         /* Free the cdev entries  */
         cdev_del(&specific_device.cdev);

	 /* Free the memory */
         kfree(specific_device.data);

	 unregister_chrdev_region(devno, 1);
}

int specific_init_module(void)
{
         int result, err;
         dev_t dev = 0;
 

	 if (specific_major) {   //the major number is given as a parameter
	            dev = MKDEV(specific_major, specific_minor);
	            result = register_chrdev_region(dev, 1, "specific");
	 } 
	 else {		// otherwise
                 result = alloc_chrdev_region(&dev, specific_minor, 1, "specific");
                 specific_major = MAJOR(dev);
	 }
	if (result < 0) {
                 printk(KERN_WARNING "specific: can't get major %d\n", specific_major);
                 return result;
        }

	/* Prepare the memory area */
	specific_device.memsize = memsize;
	specific_device.data = kmalloc(memsize * sizeof(char), GFP_KERNEL);
        memset(specific_device.data, 0, memsize * sizeof(char));

        /* Initialize the semaphore */
        sema_init(&specific_device.sem,1);

	/* Initialize cdev */
        cdev_init(&specific_device.cdev, &specific_fops);
        specific_device.cdev.owner = THIS_MODULE;
       specific_device.cdev.ops = &specific_fops;
        err = cdev_add (&specific_device.cdev, dev, 1);

        if (err)  printk(KERN_NOTICE "Error %d adding specific", err);
	else
        	printk(KERN_NOTICE "specific Added major: %d minor: %d", specific_major, specific_minor);
 
        return 0; 

}
module_init(specific_init_module);
module_exit(specific_cleanup_module);
