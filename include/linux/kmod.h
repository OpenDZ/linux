#ifndef __LINUX_KMOD_H__
#define __LINUX_KMOD_H__

/*
 *	include/linux/kmod.h
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/umh.h>
#include <linux/gfp.h>
#include <linux/stddef.h>
#include <linux/errno.h>
#include <linux/compiler.h>
#include <linux/workqueue.h>
#include <linux/sysctl.h>

#define KMOD_PATH_LEN 256

#ifdef CONFIG_MODULES
extern char modprobe_path[]; /* for sysctl */
/* modprobe exit status on success, -ve on error.  Return value
 * usually useless though. */
extern __printf(4, 5)
int __request_module(bool wait, int required_cap,
		     const char *prefix, const char *name, ...);
#define try_then_request_module(x, mod...) \
	((x) ?: (__request_module(true, -1, NULL, mod), (x)))
#else
static inline __printf(4, 5)
int __request_module(bool wait, int required_cap,
		     const char *prefix, const char *name, ...)
{ return -ENOSYS; }
#define try_then_request_module(x, mod...) (x)
#endif

/**
 * request_module  Try to load a kernel module
 *
 * Automatically loads the request module.
 *
 * @mod...: The module name
 */
#define request_module(mod...) __request_module(true, -1, NULL, mod)

#define request_module_nowait(mod...) __request_module(false, -1, NULL, mod)

/**
 * request_module_cap  Load kernel module only if the required capability is set
 *
 * Automatically load a module if the required capability is set and it
 * corresponds to the appropriate subsystem that is indicated by prefix.
 *
 * This allows to load aliased modules like 'netdev-%s' with CAP_NET_ADMIN.
 *
 * ex:
 *	request_module_cap(CAP_NET_ADMIN, "netdev", "%s", mod);
 *
 * @required_cap: Required capability to load the module
 * @prefix: The module prefix if any, otherwise NULL
 * @fmt: printf style format string for the name of the module with its
 *       arguments if any
 *
 * If '@required_cap' is positive, the security subsystem will check if
 * '@prefix' is set and if caller has the required capability then the
 * operation is allowed.
 * The security subsystem can not make assumption about the boundaries
 * of other subsystems, it is their responsability to make a call with
 * the right capability and module alias.
 *
 * If '@required_cap' is positive and '@prefix' is NULL then we assume
 * that the '@required_cap' is CAP_SYS_MODULE.
 *
 * If '@required_cap' is negative then there are no permission checks, this
 * is the equivalent to request_module() function.
 *
 * This function trust callers to pass the right capability with the
 * appropriate prefix.
 *
 * Note: the permission checks may still fail, even if the required
 * capability is negative, this is due to module loading restrictions
 * that are controlled by the enduser.
 */
#define request_module_cap(required_cap, prefix, fmt...) \
	__request_module(true, required_cap, prefix, fmt)

#endif /* __LINUX_KMOD_H__ */
