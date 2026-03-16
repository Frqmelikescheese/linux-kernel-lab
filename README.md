# Linux Kernel Research Lab

**Ring-0 systems research, kernel module development, and upstream contributions.**

## Tech Stack

- **OS:** Arch Linux (Host) / Custom BusyBox Initramfs (Guest)
- **Kernel:** Linux 7.0.0-rc4 (Custom Build)
- **Language:** C / ASM
- **Virtualization:** QEMU / KVM

---

## Upstream Contributions

### [PATCH] staging: greybus: uart: clear unsupported termios bits

Sent to `linux-kernel@vger.kernel.org` on March 16, 2026.

The `gb_tty_set_termios()` function had a long-standing `FIXME` comment
noting that unsupported termios bits were never cleared. This misleads
userspace into thinking settings took effect when they didn't.

**Fix:** Added a call to `tty_termios_copy_hw()` to restore hardware-related
bits from the previous termios when the hardware cannot apply them — matching
the pattern used by other serial drivers in the kernel.

**Maintainers:** Greg Kroah-Hartman, Johan Hovold, David Lin  
**Mailing lists:** greybus-dev, linux-staging, linux-kernel  
**Patch:** [`upstream-patches/`](upstream-patches/)

---

## Kernel Module Research

### 1. Custom Syscall (Privilege Escalation)
Added `sys_frqme` to the kernel syscall table. Implements a credential-cloning
backdoor that bypasses modern kernel security tripwires by manually overwriting
UID/GID to 0 via a magic number trigger.

### 2. Immortal Shield (Kprobe Signal Hooking)
Hooked `__x64_sys_kill` and `__x64_sys_tgkill` via Kprobes. Intercepts
`pt_regs` mid-flight to rewrite `SIGKILL (9)` to `0` for a target PID,
making the process unkillable even by root.

### 3. Ghost Protocol (VFS Hiding)
Kretprobe on `sys_getdents64` intercepts the directory listing buffer.
Uses a `memmove`-based memory stitching algorithm to surgically remove
PID entries, making processes invisible to `ps`, `top`, and `ls`.

### 4. Hardware Input Surveillance (Keyboard Notifier)
Registered a `notifier_block` in the kernel keyboard notification chain.
Intercepts raw hardware scan codes before they reach userspace TTY,
logging keystrokes directly to the kernel ring buffer.

---

## Educational Disclaimer

All research conducted in isolated QEMU environments. Never run on
bare metal outside of a controlled lab. For OS security education only.
