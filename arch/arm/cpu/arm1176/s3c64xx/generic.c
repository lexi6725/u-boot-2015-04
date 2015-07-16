#include "asm/types.h"
#include "linux/types.h"
#include "configs/smdk6410.h"
#include "asm/u-boot.h"
#include "asm/global_data.h"
#include "asm-generic/global_data.h"
#include "asm/spl.h"
//#include "spl.h"

u32 spl_boot_device(void)
{
	return BOOT_DEVICE_NAND;
}
