#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/slab.h>

static char* ex_msg_string = NULL;
static struct timer_list ex_timer;

static int ex_mmap (struct file* filep, struct vm_area_struct* vma)
{
  // Vérifier que la projection demandée ne soit pas trop grande.
  if((unsigned long) (vma->vm_end - vma->vm_start) > PAGE_SIZE)
    return -EINVAL;
  // Réaliser la nouvelle projection
  return remap_pfn_range(vma,
			 (unsigned long) (vma->vm_start),
			 virt_to_phys(ex_msg_string) >> PAGE_SHIFT,
			 vma->vm_end - vma->vm_start,
			 vma->vm_page_prot);
}

static void ex_timer_function (unsigned long arg)
{
  struct timeval tv;

  // Lire l'heure et l'inscrire dans la page paratagée.
  do_gettimeofday(&tv);
  snprintf(ex_msg_string, PAGE_SIZE, "\rTime: %ld.%06ld ", tv.tv_sec, tv.tv_usec);
  // Reprogrammer le timer dans une seconde.
  mod_timer(&ex_timer, jiffies + HZ);
}

static struct file_operations ex_fops = {
  .owner = THIS_MODULE,
  .mmap  = ex_mmap,
};

  static struct miscdevice ex_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = THIS_MODULE->name,
    .fops = &ex_fops,
  };

static int __init ex_init (void)
{
  int err;

  ex_msg_string = kzalloc(PAGE_SIZE, GFP_KERNEL);
  if (!ex_msg_string)
    return -ENOMEM;

  // Le buffer est réservé par le noyau meme s'il sera partagé avec le processus appelant.
  SetPageReserved(virt_to_page(ex_msg_string));

  // Initialiser un timer à la seconde pour modifier le buffer.
  init_timer(&ex_timer);
  ex_timer.function = ex_timer_function;
  ex_timer.expires = jiffies + HZ;

  // Initialiser un driver caractère de classe Misc
  err = misc_register(&ex_misc);
  if (err != 0) {
    ClearPageReserved(virt_to_page(ex_msg_string));
    kfree(ex_msg_string);
  }

  return err;
}

static void __exit ex_exit (void)
{
  del_timer(&ex_timer);
  misc_deregister(&ex_misc);
  ClearPageReserved(virt_to_page(ex_msg_string));
  kfree(ex_msg_string);
}


module_init(ex_init);
module_exit(ex_exit);
