#
# Makefile for building stubfs.kext
#
# Target platform: macOS 26.5.2 (see Info.plist OSBundleLibraries).
#
# Kernel headers are resolved in this order:
#   1. KDK                 - Apple Kernel Development Kit
#      (https://developer.apple.com/download/all/)
#   2. MacKernelSDK/Headers - community SDK, clone with:
#      git clone --depth 1 https://github.com/acidanthera/MacKernelSDK
#   3. macOS SDK Kernel.framework (removed on recent SDKs)
#
# Examples:
#   make                                  # universal (x86_64 + arm64)
#   make KDK=/Library/Developer/KDKs/KDK_26.5.2_<build>.kdk
#   make ARCHS=x86_64
#

BUNDLE_ID       := com.example.stubfs
BUNDLE_NAME     := stubfs
BUNDLE_VERSION  := 1.0.0
PRODUCT         := $(BUNDLE_NAME).kext

# Architectures to build.  Override for a single-arch build.
ARCHS           ?= x86_64 arm64

KDK             ?= $(lastword $(wildcard /Library/Developer/KDKs/KDK_*))
MAC_KERNEL_SDK  ?= $(CURDIR)/MacKernelSDK/Headers

# Resolve kernel header root.
KERNFRAMEWORK := $(if $(strip $(KDK)),\
	$(KDK)/System/Library/Frameworks/Kernel.framework/Versions/A/Headers,\
	$(if $(wildcard $(MAC_KERNEL_SDK)/sys/vnode.h),\
		$(MAC_KERNEL_SDK),\
		$(shell xcrun --sdk macosx --show-sdk-path)/System/Library/Frameworks/Kernel.framework/Headers))

# Fail early with a helpful message when no kernel headers are available.
ifeq ($(wildcard $(KERNFRAMEWORK)/sys/vnode.h),)
$(error No kernel headers found at "$(KERNFRAMEWORK)". \
	Run: make kdk          (downloads + installs the KDK on macOS) \
	or:  ./scripts/download-kdk.sh 26.5.2 \
	or:  git clone --depth 1 https://github.com/acidanthera/MacKernelSDK \
	or set: KERNFRAMEWORK=/path/to/kernel/headers)
endif

CC      := xcrun --sdk macosx clang
LIPO    := xcrun lipo

SRCDIR  := Sources
SRCS    := $(SRCDIR)/stubfs.c \
           $(SRCDIR)/stubfs_vfsops.c \
           $(SRCDIR)/stubfs_vnops.c

# VNOP_MONITOR is absent from older kernel SDKs (e.g. MacKernelSDK).
# Disable the monitor stub when the headers do not declare vnop_monitor_desc.
HAVE_VNOP_MONITOR := $(shell grep -q 'vnop_monitor_desc' "$(KERNFRAMEWORK)/sys/vnode_if.h" 2>/dev/null && echo 1 || echo 0)

CFLAGS  := -std=gnu11 \
           -O2 \
           -Wall -Wextra \
           -fapple-kext \
           -fno-builtin \
           -nostdinc \
           -DKERNEL \
           -I$(KERNFRAMEWORK)

ifeq ($(HAVE_VNOP_MONITOR),0)
CFLAGS  += -DSTUBFS_HAVE_VNOP_MONITOR=0
endif

LDFLAGS := -nostdlib \
           -Xlinker -kext \
           -Xlinker -static

BINARIES := $(addprefix build/,$(addsuffix /$(BUNDLE_NAME),$(ARCHS)))

.PHONY: all clean load unload reload kdk

all: $(PRODUCT)

# Download and install the KDK (macOS only, requires sudo).
kdk:
	./scripts/download-kdk.sh 26.5.2

# Compile and link one architecture slice (single cc invocation, standard for kexts).
build/%/$(BUNDLE_NAME): $(SRCS) $(SRCDIR)/stubfs.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) -arch $* -o $@ $(SRCS)

# Merge architecture slices into a universal binary.
$(PRODUCT)/Contents/MacOS/$(BUNDLE_NAME): $(BINARIES)
	@mkdir -p $(dir $@)
	$(LIPO) -create $(BINARIES) -output $@

$(PRODUCT): $(PRODUCT)/Contents/MacOS/$(BUNDLE_NAME) Info.plist
	cp Info.plist $@/Contents/Info.plist
	@echo "Built $(PRODUCT) for: $(ARCHS)"

load: $(PRODUCT)
	sudo chown -R root:wheel $(PRODUCT)
	sudo chmod -R 755 $(PRODUCT)
	sudo kextutil -v $(PRODUCT)

unload:
	sudo kextunload -b $(BUNDLE_ID)

reload: unload load

clean:
	rm -rf build $(PRODUCT)
