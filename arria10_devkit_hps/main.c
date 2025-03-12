#include <stdio.h>
#include <stdint.h>

#include "alt_watchdog.h"
#include "soc_a10/alt_bridge_manager.h"
#include "ghrd_fpga/ghrd_fpga.h"

#define LED_REG_OFF (0x10>>2)
#define DS_REG_OFF (0x30>>2)
#define LW_H2F_RST_BIT 0x2
#define WDT_EN_BIT 0x1
#define GIC_WDOG0_IRQ 151
#define GIC_WDOG1_IRQ 152

int __auto_semihosting;

//uint32_t *fpga_mem = (uint32_t*)0xff200000;
//uint32_t *bridge_en = (uint32_t*)0xffd0502c;
//uint32_t *wdt0_cr = (uint32_t*)0xffd00200;
//uint32_t *wdt1_cr = (uint32_t*)0xffd00300;
//uint32_t *mpu_gic = (uint32_t*)0xffffc1ff;

int system_setup()
{
	// uint32_t wdt0_en = *wdt0_cr, wdt1_en = *wdt1_cr;
	//Check and disable watchdog
	//if(wdt0_en & WDT_EN_BIT) *wdt0_cr &= ~WDT_EN_BIT;
	//if(wdt1_en & WDT_EN_BIT) *wdt1_cr &= ~WDT_EN_BIT;
	alt_wdog_stop(ALT_WDOG_CPU);
	alt_wdog_stop(ALT_WDOG0);
	alt_wdog_stop(ALT_WDOG1);

	//Enable lw_h2f bridge
	//*bridge_en &= ~LW_H2F_RST_BIT;
	alt_bridge_init(ALT_BRIDGE_LWH2F);
	return 0;
}

int system_exit()
{
	return 0;
}

int main(int argc, char **argv) {

	system_setup();

	while(1)
	{
		ghrd_fpga_sw2led();
	}

	system_exit();

	return 0;
}
