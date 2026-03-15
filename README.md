# Linux 7.0 Kernel Research & Rootkit PoC
**A collection of Ring-0 system modifications and kernel-space utilities.**

## Overview
This project is a deep-dive into the Linux 7.0-rc3 kernel architecture. It demonstrates the ability to modify the kernel at the source level and inject logic via Out-of-Tree (OOT) Kernel Modules using Kprobes and Notifier Blocks.

### 🛠 Tech Stack
- **OS:** Arch Linux (Host) / Custom BusyBox Initramfs (Guest)
- **Kernel:** Linux 7.0.0-frqme (Custom Build)
- **Language:** C / ASM
- **Virtualization:** QEMU / KVM

---

## 🚀 Accomplishments

### 1. Custom Syscall Architecture (Privilege Escalation)
- **Implementation:** Added a custom system call (`sys_frqme`) to the kernel's syscall table (`syscall_64.tbl`).
- **Functionality:** Implemented a credential-cloning backdoor. By passing a "magic number" to the syscall, the kernel prepares a clone of the current task's credentials and manually overwrites the UID/GID to `0`.
- **Security Bypass:** Successfully bypassed modern kernel security tripwires that prevent the direct creation of root credentials.

### 2. The "Immortal" Shield (Kprobe Signal Hooking)
- **Logic:** Hooked `__x64_sys_kill` and `__x64_sys_tgkill` using Kprobes.
- **Result:** Intercepted CPU registers (`pt_regs`) in mid-flight to detect if a specific PID was being targeted for termination.
- **Sabotage:** Automatically modifies the signal argument from `SIGKILL (9)` to `0`, rendering the process unkillable even by a root user with `kill -9`.

### 3. Ghost Protocol (VFS Hiding)
- **Logic:** Used a **Kretprobe** on `sys_getdents64` to manipulate the Virtual File System (VFS).
- **Implementation:** Intercepted the memory buffer returned by the kernel when listing directories (like `/proc`).
- **Memory Surgery:** Implemented a memory-stitching algorithm using `memmove` to delete specific PID entries from the directory buffer, making processes completely invisible to `ps`, `top`, and `ls`.

### 4. Hardware Input Surveillance (Keyboard Notifier)
- **Logic:** Registered a `notifier_block` within the kernel's keyboard notification chain.
- **Implementation:** Intercepted hardware scan codes directly from the input driver before they reached the user-space TTY, logging keystrokes to the kernel ring buffer.

---

## 🧪 Educational Disclaimer
This project was developed for educational purposes to understand Operating System security, CPU register states, and kernel-space memory management. All testing was performed in isolated QEMU environments.
