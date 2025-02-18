#include <stdio.h>
#include <stdint.h>
#define LED_REG_OFF (0x10>>2)
#define DS_REG_OFF (0x30>>2)
#define LW_H2F_RST_BIT 0x2
#define WDT_EN_BIT 0x1

uint32_t *fpga_mem = (uint32_t*)0xff200000;
uint32_t *bridge_en = (uint32_t*)0xffd0502c;
uint32_t *wdt0_cr = (uint32_t*)0xffd00200;
uint32_t *wdt1_cr = (uint32_t*)0xffd00300;

int main(int argc, char **argv) {
	uint32_t ds_val, wdt0_en = *wdt0_cr, wdt1_en = *wdt1_cr;

	//CONFIG: ultimately should be done in the SPL
	//Check and disable watchdog
	if(wdt0_en & WDT_EN_BIT) *wdt0_cr &= ~WDT_EN_BIT;
	if(wdt1_en & WDT_EN_BIT) *wdt1_cr &= ~WDT_EN_BIT;
	//Enable lw_h2f bridge
	*bridge_en &= ~LW_H2F_RST_BIT;
	while(1)
	{
		ds_val = fpga_mem[DS_REG_OFF];
		fpga_mem[LED_REG_OFF] = ds_val;
	}


	return 0;
}
