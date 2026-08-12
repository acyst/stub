/*
 * stubfs_vnops.c
 *
 * Vnode (VNOP) operation stubs.
 *
 * Every vnode operation is adapted to the VNOP interface but implemented as
 * a stub that returns ENOTSUP.  This demonstrates the complete set of vnode
 * operations a filesystem must wire into its operation vector.
 *
 * The operation descriptors (vnop_*_desc) and argument structures
 * (struct vnop_*_args) are declared in <sys/vnode_if.h>.
 */

#include <sys/param.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <sys/vnode_if.h>
#include <sys/errno.h>

#include "stubfs.h"

/*
 * VNOP_MONITOR is not present in every kernel SDK (e.g. MacKernelSDK).
 * It defaults to enabled; the build system defines STUBFS_HAVE_VNOP_MONITOR=0
 * when the target headers do not declare vnop_monitor_desc.
 */
#ifndef STUBFS_HAVE_VNOP_MONITOR
#define STUBFS_HAVE_VNOP_MONITOR 1
#endif

/*
 * Generate a stub implementation for a vnode operation.
 * The generated function takes (struct vnop_<name>_args *ap) and returns
 * ENOTSUP.  The argument is unused, hence (void)ap.
 */
#define STUBFS_VNOP(name)                                            \
	static int stubfs_##name(struct vnop_##name##_args *ap)      \
	{                                                            \
		(void)ap;                                            \
		return ENOTSUP;                                      \
	}

/* namespace / lookup operations */
STUBFS_VNOP(lookup)      /* VNOP_LOOKUP  */
STUBFS_VNOP(create)      /* VNOP_CREATE  */
STUBFS_VNOP(whiteout)    /* VNOP_WHITEOUT (obsolete) */
STUBFS_VNOP(mknod)       /* VNOP_MKNOD   */

/* file lifecycle operations */
STUBFS_VNOP(open)        /* VNOP_OPEN    */
STUBFS_VNOP(close)       /* VNOP_CLOSE   */
STUBFS_VNOP(access)      /* VNOP_ACCESS  */
STUBFS_VNOP(getattr)     /* VNOP_GETATTR */
STUBFS_VNOP(setattr)     /* VNOP_SETATTR */

/* data transfer operations */
STUBFS_VNOP(read)        /* VNOP_READ    */
STUBFS_VNOP(write)       /* VNOP_WRITE   */
STUBFS_VNOP(ioctl)       /* VNOP_IOCTL   */
STUBFS_VNOP(select)      /* VNOP_SELECT  */
STUBFS_VNOP(exchange)    /* VNOP_EXCHANGE */
STUBFS_VNOP(fsync)       /* VNOP_FSYNC   */

/* revocation / memory mapping operations */
STUBFS_VNOP(revoke)      /* VNOP_REVOKE  */
STUBFS_VNOP(mmap_check)  /* VNOP_MMAP_CHECK */
STUBFS_VNOP(mmap)        /* VNOP_MMAP    */
STUBFS_VNOP(mnomap)      /* VNOP_MNOMAP  */

/* directory namespace operations */
STUBFS_VNOP(remove)      /* VNOP_REMOVE  */
STUBFS_VNOP(link)        /* VNOP_LINK    */
STUBFS_VNOP(rename)      /* VNOP_RENAME  */
STUBFS_VNOP(renamex)     /* VNOP_RENAMEX */
STUBFS_VNOP(mkdir)       /* VNOP_MKDIR   */
STUBFS_VNOP(rmdir)       /* VNOP_RMDIR   */
STUBFS_VNOP(symlink)     /* VNOP_SYMLINK */
STUBFS_VNOP(readdir)     /* VNOP_READDIR */
STUBFS_VNOP(readdirattr) /* VNOP_READDIRATTR */
STUBFS_VNOP(getattrlistbulk) /* VNOP_GETATTRLISTBULK */
STUBFS_VNOP(readlink)    /* VNOP_READLINK */

/* vnode lifecycle operations */
STUBFS_VNOP(inactive)    /* VNOP_INACTIVE */
STUBFS_VNOP(reclaim)     /* VNOP_RECLAIM */

/* misc operations */
STUBFS_VNOP(pathconf)    /* VNOP_PATHCONF */
STUBFS_VNOP(advlock)     /* VNOP_ADVLOCK */
STUBFS_VNOP(allocate)    /* VNOP_ALLOCATE */
STUBFS_VNOP(pagein)      /* VNOP_PAGEIN  */
STUBFS_VNOP(pageout)     /* VNOP_PAGEOUT */
STUBFS_VNOP(searchfs)    /* VNOP_SEARCHFS */
STUBFS_VNOP(copyfile)    /* VNOP_COPYFILE */
STUBFS_VNOP(clonefile)   /* VNOP_CLONEFILE */

/* block mapping operations */
STUBFS_VNOP(blktooff)    /* VNOP_BLKTOOFF */
STUBFS_VNOP(offtoblk)    /* VNOP_OFFTOBLK */
STUBFS_VNOP(blockmap)    /* VNOP_BLOCKMAP */
STUBFS_VNOP(strategy)    /* VNOP_STRATEGY */
STUBFS_VNOP(bwrite)      /* VNOP_BWRITE  */

/* file monitoring / extended attributes */
#if STUBFS_HAVE_VNOP_MONITOR
STUBFS_VNOP(monitor)     /* VNOP_MONITOR */
#endif
STUBFS_VNOP(getxattr)    /* VNOP_GETXATTR */
STUBFS_VNOP(setxattr)    /* VNOP_SETXATTR */
STUBFS_VNOP(removexattr) /* VNOP_REMOVEXATTR */
STUBFS_VNOP(listxattr)   /* VNOP_LISTXATTR */

/*
 * Vnode operation vector.  VFS fills this in at registration time based on
 * the vnodeopv_desc below.
 */
int (**stubfs_vnodeop_p)(void *);

/*
 * Null-terminated list mapping each VNOP descriptor to its implementation.
 * Any operation not listed here would default to vn_default_error(), but we
 * list every supported operation explicitly.
 */
static const struct vnodeopv_entry_desc stubfs_vnodeop_entries[] = {
	{ &vnop_default_desc,        (int (*)(void *))vn_default_error },
	{ &vnop_lookup_desc,         (int (*)(void *))stubfs_lookup },
	{ &vnop_create_desc,         (int (*)(void *))stubfs_create },
	{ &vnop_whiteout_desc,       (int (*)(void *))stubfs_whiteout },
	{ &vnop_mknod_desc,          (int (*)(void *))stubfs_mknod },
	{ &vnop_open_desc,           (int (*)(void *))stubfs_open },
	{ &vnop_close_desc,          (int (*)(void *))stubfs_close },
	{ &vnop_access_desc,         (int (*)(void *))stubfs_access },
	{ &vnop_getattr_desc,        (int (*)(void *))stubfs_getattr },
	{ &vnop_setattr_desc,        (int (*)(void *))stubfs_setattr },
	{ &vnop_read_desc,           (int (*)(void *))stubfs_read },
	{ &vnop_write_desc,          (int (*)(void *))stubfs_write },
	{ &vnop_ioctl_desc,          (int (*)(void *))stubfs_ioctl },
	{ &vnop_select_desc,         (int (*)(void *))stubfs_select },
	{ &vnop_exchange_desc,       (int (*)(void *))stubfs_exchange },
	{ &vnop_revoke_desc,         (int (*)(void *))stubfs_revoke },
	{ &vnop_mmap_check_desc,     (int (*)(void *))stubfs_mmap_check },
	{ &vnop_mmap_desc,           (int (*)(void *))stubfs_mmap },
	{ &vnop_mnomap_desc,         (int (*)(void *))stubfs_mnomap },
	{ &vnop_fsync_desc,          (int (*)(void *))stubfs_fsync },
	{ &vnop_remove_desc,         (int (*)(void *))stubfs_remove },
	{ &vnop_link_desc,           (int (*)(void *))stubfs_link },
	{ &vnop_rename_desc,         (int (*)(void *))stubfs_rename },
	{ &vnop_renamex_desc,        (int (*)(void *))stubfs_renamex },
	{ &vnop_mkdir_desc,          (int (*)(void *))stubfs_mkdir },
	{ &vnop_rmdir_desc,          (int (*)(void *))stubfs_rmdir },
	{ &vnop_symlink_desc,        (int (*)(void *))stubfs_symlink },
	{ &vnop_readdir_desc,        (int (*)(void *))stubfs_readdir },
	{ &vnop_readdirattr_desc,    (int (*)(void *))stubfs_readdirattr },
	{ &vnop_getattrlistbulk_desc,(int (*)(void *))stubfs_getattrlistbulk },
	{ &vnop_readlink_desc,       (int (*)(void *))stubfs_readlink },
	{ &vnop_inactive_desc,       (int (*)(void *))stubfs_inactive },
	{ &vnop_reclaim_desc,        (int (*)(void *))stubfs_reclaim },
	{ &vnop_pathconf_desc,       (int (*)(void *))stubfs_pathconf },
	{ &vnop_advlock_desc,        (int (*)(void *))stubfs_advlock },
	{ &vnop_allocate_desc,       (int (*)(void *))stubfs_allocate },
	{ &vnop_pagein_desc,         (int (*)(void *))stubfs_pagein },
	{ &vnop_pageout_desc,        (int (*)(void *))stubfs_pageout },
	{ &vnop_searchfs_desc,       (int (*)(void *))stubfs_searchfs },
	{ &vnop_copyfile_desc,       (int (*)(void *))stubfs_copyfile },
	{ &vnop_clonefile_desc,      (int (*)(void *))stubfs_clonefile },
	{ &vnop_blktooff_desc,       (int (*)(void *))stubfs_blktooff },
	{ &vnop_offtoblk_desc,       (int (*)(void *))stubfs_offtoblk },
	{ &vnop_blockmap_desc,       (int (*)(void *))stubfs_blockmap },
	{ &vnop_strategy_desc,       (int (*)(void *))stubfs_strategy },
	{ &vnop_bwrite_desc,         (int (*)(void *))stubfs_bwrite },
#if STUBFS_HAVE_VNOP_MONITOR
	{ &vnop_monitor_desc,        (int (*)(void *))stubfs_monitor },
#endif
	{ &vnop_getxattr_desc,       (int (*)(void *))stubfs_getxattr },
	{ &vnop_setxattr_desc,       (int (*)(void *))stubfs_setxattr },
	{ &vnop_removexattr_desc,    (int (*)(void *))stubfs_removexattr },
	{ &vnop_listxattr_desc,      (int (*)(void *))stubfs_listxattr },
	{ NULL, NULL }
};

/*
 * Vnode operation vector descriptor registered with VFS.
 */
struct vnodeopv_desc stubfs_vnodeop_opv_desc = {
	&stubfs_vnodeop_p,       /* where VFS installs the operation vector */
	stubfs_vnodeop_entries   /* the operation -> implementation table   */
};
