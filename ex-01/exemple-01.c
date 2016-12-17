#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/sched.h>

// Documentation
// - https://perso.telecom-paristech.fr/duc/cours/tpt35/interface_user_space.html#file-operations

// Fonction read
// -La fonction read a pour prototype : ssize_t foo_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos);
// -Elle est appelée lorsque l'application qui a ouvert le fichier effectue une lecture depuis ce fichier (appel système read).
// -Si cette fonction n'est pas définie, la lecture se soldera par l'erreur -EINVAL.
// -Ses arguments sont :
//   struct file *file : La structure représentant le fichier
//   char __user *buf : Le tampon, dans l'espace utilisateur, à remplir avec les données en provenance du périphérique
//   size_t count : Le nombre maximum d'octets à lire
//   loff_t *f_pos : Pointeur vers la la position courante dans le fichier, à mettre à jour
 
static ssize_t ex_read(struct file *filp, char *u_buffer, size_t max_lg, loff_t *offset)
{
  int lg;
  char k_msg[128];

  // Remplir une chaine de salutation avec le PID de l'appelant.
  snprintf(k_msg, 128, "Hello '%s/%u'!\n", current->comm, current->pid);

  // Calculer la longueur restant à envoyer.
  lg = strlen(k_msg) - (*offset);
  if(lg <= 0)
    return 0;

  // Tronquer si nécessaire (si le buffer fourni est trop petit).
  if(lg > max_lg)
    lg = max_lg;

  // Copier le message dans le buffer de l'espace utilisateur.
  if(copy_to_user(u_buffer, &k_msg[*offset], lg) != 0)
    return -EFAULT;

  *offset += lg;
  return lg;
}

// File operations
//
//   - Cette structure est utilisée pour d'écrire l'interface entre un système de fichiers (ext3, reiserfs, fat, ntfs etc...) et l'utilisateur.
//   - Les applications interagissent avec un périphérique caractère via des opérations classiques de lecture (read) et d'écriture (write) sur le fichier spécial représentant le périphérique (/dev/module).
//   - Un pilote de périphérique caractère doit donc implémenter ces opérations afin de pouvoir réagir aux actions émises par les applications.
//   - Au niveau du noyau, ces opérations sont décrites dans la structure "struct file_operations" définie dans include/linux/fs.h
static struct file_operations ex_fops = {
  .owner = THIS_MODULE,
  .read  = ex_read,
};


// Frameworks
//
//De plus en plus de périphériques ne sont plus implémentés directement comme des périphériques caractères mais utilisent un framework qui permet de factoriser les parties identiques des pilotes contrôlant le même type de périphériques et qui permet d'offrir une interface unique et cohérente aux applications (mêmes paramètres ioctl quel que soit le pilote par exemple). Du point de vue des applications, le périphérique reste vu comme un périphérique caractère normal que son pilote utilise un framework ou pas.

// Misc framework
//
//Lorsqu'aucun framework ne semble convenir pour un périphérique, il est possible de l'implémenter comme un périphérique caractère directement ou d'utiliser le framework misc qui simplifie cette tâche.

//Un périphérique utilisant ce framework est décrit par la structure struct miscdevice (définie dans include/linux/miscdevice.h) :

//struct miscdevice {
//    int minor;
//    const char *name;
//    const struct file_operations *fops;
//    struct list_head list;
//    struct device *parent;
//    struct device *this_device;
//    const struct attribute_group **groups;
//    const char *nodename;
//    umode_t mode;
//};

//Les principaux champs de cette structures sont :
//
//    minor : le numéro minor désiré pour le périphérique (le numéro major sera automatiquement celui des périphériques misc, c'est-à-dire 130) ou MISC_DYNAMIC_MINOR pour en obtenir un dynamiquement
//    name : le nom du périphérique, utilisé pour créer plus ou moins automatiquement le fichier spécial correspondant dans /dev
//    fops : pointeur vers la structure struct file_operations qui décrit les fonctions utilisées pour répondre aux opérations de lecture, écriture, etc.

//Un périphérique s'enregistre en tant auprès du framework misc grâce à la fonction : int misc_register(struct miscdevice *misc);
//L'appel à cette fonction est traditionnellement effectué dans la fonction probe du pilote.
//Lorsque le périphérique n'est plus présent, il faut penser à appeler la fonction : int misc_deregister(struct miscdevice *misc);
//L'appel à cette fonction est traditionnellement effectué dans la fonction remove.

  static struct miscdevice ex_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = THIS_MODULE->name,
    .fops  = &ex_fops,
  };

static int __init ex_init (void)
{
  // Initialiser un driver caractère de classe Misc.
  return misc_register(& ex_misc);
}

static void __exit ex_exit (void)
{
  misc_deregister(& ex_misc);
}

module_init(ex_init);
module_exit(ex_exit);
