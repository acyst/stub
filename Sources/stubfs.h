/*
 * stubfs.h
 *
 * Common definitions for the stubfs kernel extension.
 *
 * stubfs is a "stub" filesystem: it registers a filesystem type with the
 * XNU VFS layer and implements (adapts) the complete VFS + VNOP interface,
 * but every operation returns ENOTSUP -- it has no actual on-disk or
 * in-memory filesystem functionality.
 */

#ifndef _STUBFS_H_
#define _STUBFS_H_

#include <sys/mount.h>
#include <sys/vnode.h>

#define STUBFS_NAME     "stubfs"
#define STUBFS_VERSION  "1.0.0"

/* vnode operation vector descriptor, defined in stubfs_vnops.c */
extern struct vnodeopv_desc stubfs_vnodeop_opv_desc;

/* registration entry points, used by stubfs.c (kern_start/kern_stop) */
extern int stubfs_register(void);
extern int stubfs_unregister(void);

#endif /* _STUBFS_H_ */
