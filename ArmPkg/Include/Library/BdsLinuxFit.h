#ifndef __BDS_LINUX_FIT__
#define __BDS_LINUX_FIT__

#include <Library/IoLib.h>

#define FIT_DESC_PROP "description"
#define FIT_IMAGES_PATH "/images"
#define FIT_KERNEL_IMAGE "kernel"
#define FIT_INITRD_IMAGE "ramdisk"
#define FIT_FIRMWARE_IMAGE "firmware"
#define FIT_FDT_IMAGE "fdt"
#define FIT_CONFS_PATH "/configurations"
#define FIT_DEFAULT_PROP "default"
#define FIT_IMAGE_DATA "data"
#define FIT_IMAGE_LOAD "load"

EFI_STATUS
FitCheckHeader(
	EFI_PHYSICAL_ADDRESS	FitImage
);

EFI_STATUS
FitGetConfNode (
	EFI_PHYSICAL_ADDRESS  FitImage,
	VOID*									ConfigPtr,
	INT32*								NodeOffset
);

EFI_STATUS
FitGetNodeFromConf (
	EFI_PHYSICAL_ADDRESS  FitImage,
	INT32									CfgNodeOffset,
	CHAR8*								ConfPropName,
	INT32*								NodeOffset
);

EFI_STATUS
FitGetNodeData (
	EFI_PHYSICAL_ADDRESS  FitImage,
	INT32									NodeOffset,
	EFI_PHYSICAL_ADDRESS* Addr,
	INT32*								Size
);

EFI_STATUS
FitGetNodeLoad (
	EFI_PHYSICAL_ADDRESS  FitImage,
	INT32									NodeOffset,
	EFI_PHYSICAL_ADDRESS* Addr
);

#endif
