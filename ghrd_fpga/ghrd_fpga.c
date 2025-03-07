#include <stdio.h>
#include <stdint.h>
#define LED_REG_OFF (0x10>>2)
#define DS_REG_OFF (0x30>>2)

uint32_t *fpga_mem = (uint32_t*)0xff200000;

void ghrd_fpga_led(uint32_t val)
{
	fpga_mem[LED_REG_OFF] = val;
}

uint32_t ghrd_fpga_sw()
{
	return fpga_mem[DS_REG_OFF];
}

int ghrd_fpga_sw2led() {
	uint32_t ds_val;

	ds_val = ghrd_fpga_sw();
	ghrd_fpga_led(ds_val);
	
	return 0;
}
