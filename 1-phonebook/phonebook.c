// modprobe phonebook
// mknod phonebook c {major_number} 0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include<linux/slab.h> 

static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);

MODULE_LICENSE("GPL");
#define DEVICE_NAME "phonebook"

struct Node {
     struct list_head list;
     char name[64];
     char surname[64];
     char phone[20];
     char email[64];
     int age;
};

LIST_HEAD(head_node);

static struct file_operations fops = {
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int major;

static int __init phonebook_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    printk(KERN_INFO "Phonebook loaded. Major number: %d\n", major);
    return 0;
}

static void __exit phonebook_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Phonebook unloaded\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "Phonebook opened\n");
   return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer,
                         size_t len, loff_t *offset) {
   int command = -1;
   sscanf(buffer, "%d", &command);
   if (command == -1) {
     printk("Wrong input\n");
   }
   if (command == 0) { // add
     struct Node *new_node = kmalloc(sizeof(struct Node), GFP_KERNEL);
     memset(new_node, 0, sizeof(struct Node));
     sscanf(buffer, "%d %s %s %d %s %s", &command, new_node->name, new_node->surname,
                                         &new_node->age, new_node->phone, new_node->email);

     printk(KERN_INFO "Added user: %s %s, %d. %s-%s", new_node->name, 
            new_node->surname, new_node->age, new_node->phone, new_node->email);

     INIT_LIST_HEAD(&new_node->list);

     list_add_tail(&new_node->list, &head_node);
   } else if (command == 1 || command == 2) { // delete or find
      struct Node* iter;
      char surname[64] = {0};
      sscanf(buffer, "%d %s", &command, surname);
      printk(KERN_INFO "Looking for %s...\n", surname);
      list_for_each_entry(iter, &head_node, list) {
        if (!strcmp(iter->surname, surname)) {
          printk(KERN_INFO "Found entry\n");
          if (command == 1) {
            list_del(&iter->list);
          } else {
            printk(KERN_INFO "name: %s\nsurname: %s\nage: %d\nphone: %s\nemail: %s\n", iter->name, 
              iter->surname, iter->age, iter->phone, iter->email);
          }
          return len;
        }
      }
      printk(KERN_INFO "Entry was not found\n");
   } 
   return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "Phonebook closed\n");
   return 0;
}

static ssize_t dev_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
  struct Node* iter;
  int cnt = 0;
  printk(KERN_INFO "Traversing phonebook");
  list_for_each_entry(iter, &head_node, list) {
    printk(KERN_INFO "%d: %s-%s\n", ++cnt, iter->name, iter->surname);
  }
  return 0;
}

module_init(phonebook_init);
module_exit(phonebook_exit);

