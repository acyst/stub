/*
 * stubfs_vfsops.c
 *
 * VFS (filesystem-level) operation stubs and VFS registration.
 *
 * This file defines the struct vfsops table (mount/root/sync/...), the
 * struct vfs_fsentry used to register the filesystem type, and the
 * register/unregister helpers invoked from kern_start()/kern_stop().
 *
 * Like the vnode operations, every VFS operation is a stub returning
 * ENOTSUP, except vfs_init which has nothing to do and returns 0.
 */

#include <sys/param.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <sys/vnode_if.h>
#include <sys/errno.h>
#include <sys/kauth.h>

#include "stubfs.h"

/* -------------------------------------------------------------------------
 * VFS operation stubs
 * -----------------------------------------------------------------------*/

static int
stubfs_mount(struct mount *mp, vnode_t devvp, user_addr_t data, vfs_context_t ctx)
{
	(void)mp; (void)devvp; (void)data; (void)ctx;
	return ENOTSUP;
}

static int
stubfs_start(struct mount *mp, int flags, vfs_context_t ctx)
{
	(void)mp; (void)flags; (void)ctx;
	return ENOTSUP;
}

static int
stubfs_unmount(struct mount *mp, int mntflags, vfs_context_t ctx)
{
	(void)mp; (void)mntflags; (void)ctx;
	return ENOTSUP;
}

static int
stubfs_root(struct mount *mp, struct vnode **vpp, vfs_context_t ctx)
{
	(void)mp; (void)vpp; (void)ctx;
	return ENOTSUP;
}

static int
stubfs_getattr(struct mount *mp, struct vfs_attr *vfa, vfs_context_t ctx)
{
	(void)mp; (void)vfa; (void)ctx;
	return ENOTSUP;
}

static int
stubfs_sync(struct mount *mp, int waitfor, vfs_context_t ctx)
{
	(void)mp; (void)waitfor; (void)ctx;
	return ENOTSUP;
}

static int
stubfs_setattr(struct mount *mp, struct vfs_attr *vfa, vfs_context_t ctx)
{
	(void)mp; (void)vfa; (void)ctx;
	return ENOTSUP;
}

/*
 * vfs_init is called once before any instance is mounted.  The stub has no
 * global state to initialize, so it simply succeeds.
 */
static int
stubfs_init(struct vfsconf *vfsc)
{
	(void)vfsc;
	return 0;
}

/* -------------------------------------------------------------------------
 * VFS operation table
 * -----------------------------------------------------------------------*/

static struct vfsops stubfs_vfsops = {
	.vfs_mount       = stubfs_mount,
	.vfs_start       = stubfs_start,
	.vfs_unmount     = stubfs_unmount,
	.vfs_root        = stubfs_root,
	.vfs_quotactl    = NULL,          /* unsupported */
	.vfs_getattr     = stubfs_getattr,
	.vfs_sync        = stubfs_sync,
	.vfs_vget        = NULL,          /* unsupported */
	.vfs_fhtovp      = NULL,          /* unsupported */
	.vfs_vptofh      = NULL,          /* unsupported */
	.vfs_init        = stubfs_init,
	.vfs_sysctl      = NULL,          /* unsupported */
	.vfs_setattr     = stubfs_setattr,
	.vfs_ioctl       = NULL,          /* unsupported */
	.vfs_vget_snapdir = NULL,         /* unsupported */
};

/* -------------------------------------------------------------------------
 * Filesystem registration table
 * -----------------------------------------------------------------------*/

static struct vnodeopv_desc *stubfs_opv_desc_list[] = {
	&stubfs_vnodeop_opv_desc,
	NULL
};

static struct vfs_fsentry stubfs_vfsconf = {
	.vfe_vfsops     = &stubfs_vfsops,
	.vfe_vopcnt     = 1,               /* number of vnodeopv_desc entries */
	.vfe_opvdescs   = stubfs_opv_desc_list,
	.vfe_fstypenum  = -1,              /* let VFS assign a type number */
	.vfe_fsname     = STUBFS_NAME,
	.vfe_flags      = (VFS_TBLTHREADSAFE | VFS_TBLNOTYPENUM |
	                   VFS_TBL64BITREADY | VFS_TBLREADDIR_EXTENDED),
	.vfe_reserv     = { NULL, NULL },
};

static vfstable_t stubfs_vfshandle = NULL;

int
stubfs_register(void)
{
	int error;

	error = vfs_fsadd(&stubfs_vfsconf, &stubfs_vfshandle);
	if (error != 0) {
		return error;
	}

	return 0;
}

int
stubfs_unregister(void)
{
	int error;

	if (stubfs_vfshandle == NULL) {
		return 0;
	}

	error = vfs_fsremove(stubfs_vfshandle);
	if (error == 0) {
		stubfs_vfshandle = NULL;
	}

	return error;
}
