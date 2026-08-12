/*
 * stubfs.c
 *
 * Kernel extension entry points.
 *
 * kern_start() registers the stubfs filesystem type with the VFS layer;
 * kern_stop() unregisters it.  These are the modern (macOS 10.9+) kext
 * entry point names, linked into the auto-generated kmod_info by the
 * `-Xlinker -kext` linker flag.
 */

#include <mach/mach_types.h>
#include <libkern/libkern.h>
#include <libkern/OSKextLib.h>

#include "stubfs.h"

kern_return_t kern_start(kmod_info_t *ki, void *data);
kern_return_t kern_stop(kmod_info_t *ki, void *data);

kern_return_t
kern_start(kmod_info_t *ki, void *data)
{
	(void)ki;
	(void)data;

	if (stubfs_register() != 0) {
		printf("%s: failed to register filesystem\n", STUBFS_NAME);
		return KERN_FAILURE;
	}

	printf("%s %s: registered stub filesystem (all operations return ENOTSUP)\n",
	    STUBFS_NAME, STUBFS_VERSION);
	return KERN_SUCCESS;
}

kern_return_t
kern_stop(kmod_info_t *ki, void *data)
{
	(void)ki;
	(void)data;

	if (stubfs_unregister() != 0) {
		printf("%s: failed to unregister filesystem\n", STUBFS_NAME);
		return KERN_FAILURE;
	}

	printf("%s: unregistered stub filesystem\n", STUBFS_NAME);
	return KERN_SUCCESS;
}
