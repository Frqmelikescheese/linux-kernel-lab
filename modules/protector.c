#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h>

static int protect_pid = 0;
module_param(protect_pid, int, 0644);

/* This function handles the sabotage logic */
static int common_handler(struct kprobe *p, struct pt_regs *regs) {
    struct pt_regs *user_regs = (struct pt_regs *)regs->di;
    int pid = (int)user_regs->di; // PID is usually the 1st arg
    
    // For tgkill, the target PID is actually the 2nd arg (RSI)
    if (strcmp(p->symbol_name, "__x64_sys_tgkill") == 0) {
        pid = (int)user_regs->si;
    }

    if (pid == protect_pid && pid != 0) {
        // We catch the signal number
        // For kill/tkill: 2nd arg (RSI). For tgkill: 3rd arg (RDX).
        if (strcmp(p->symbol_name, "__x64_sys_tgkill") == 0) {
            user_regs->dx = 0; 
        } else {
            user_regs->si = 0;
        }
        printk(KERN_INFO "PROTECTOR: Blocked assassination of PID %d\n", pid);
    }
    return 0;
}

static struct kprobe kp_kill = { .symbol_name = "__x64_sys_kill" };
static struct kprobe kp_tkill = { .symbol_name = "__x64_sys_tkill" };
static struct kprobe kp_tgkill = { .symbol_name = "__x64_sys_tgkill" };

static int __init protector_init(void) {
    kp_kill.pre_handler = common_handler;
    kp_tkill.pre_handler = common_handler;
    kp_tgkill.pre_handler = common_handler;

    register_kprobe(&kp_kill);
    register_kprobe(&kp_tkill);
    register_kprobe(&kp_tgkill);

    printk(KERN_INFO "PROTECTOR: All doors locked for PID %d\n", protect_pid);
    return 0;
}

static void __exit protector_exit(void) {
    unregister_kprobe(&kp_kill);
    unregister_kprobe(&kp_tkill);
    unregister_kprobe(&kp_tgkill);
    printk(KERN_INFO "PROTECTOR: Shield deactivated.\n");
}

module_init(protector_init);
module_exit(protector_exit);
MODULE_LICENSE("GPL");
