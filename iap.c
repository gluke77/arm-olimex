#include "iap.h"

#include "rtc.h"
#include "timer.h"

// This is for LPC2294

unsigned int iap_cmd[IAP_CMD_SIZE];
unsigned int iap_res[IAP_RES_SIZE];

typedef void (* IAP)(unsigned int [], unsigned int []);
IAP iap_proc=(IAP)0x7FFFFFF1;

int iap_copy_ram_to_flash(void * buf, int size)
{
	iap_cmd[0] = IAP_PREPARE_SECTORS_FOR_WRITE;
	iap_cmd[1] = IAP_FLASH_SECTOR;
	iap_cmd[2] = IAP_FLASH_SECTOR;

	timer_suspend();
	iap_proc(iap_cmd, iap_res);
	timer_resume();

	if (IAP_CMD_SUCCESS != iap_res[0])
		return iap_res[0];

	iap_cmd[0] = IAP_COPY_RAM_TO_FLASH;
	iap_cmd[1] = (unsigned int)IAP_FLASH_BEGIN;
	iap_cmd[2] = (unsigned int)buf;
	iap_cmd[3] = size;
	iap_cmd[4] = processor_clock() / 1000;
	
	timer_suspend();
	iap_proc(iap_cmd, iap_res);
	timer_resume();
	
	return iap_res[0];
}

int iap_clear_flash(void)
{
	iap_cmd[0] = IAP_PREPARE_SECTORS_FOR_WRITE;
	iap_cmd[1] = IAP_FLASH_SECTOR;
	iap_cmd[2] = IAP_FLASH_SECTOR;

	timer_suspend();
	iap_proc(iap_cmd, iap_res);
	timer_resume();

	if (IAP_CMD_SUCCESS != iap_res[0])
		return iap_res[0];

	iap_cmd[0] = IAP_ERASE_SECTORS;
	iap_cmd[1] = IAP_FLASH_SECTOR;
	iap_cmd[2] = IAP_FLASH_SECTOR;
	iap_cmd[3] = processor_clock() / 1000;
	
	timer_suspend();
	iap_proc(iap_cmd, iap_res);
	timer_resume();
	
	if (IAP_CMD_SUCCESS != iap_res[0])
		return iap_res[0];

	iap_cmd[0] = IAP_BLANK_CHECK_SECTORS;
	iap_cmd[1] = IAP_FLASH_SECTOR;
	iap_cmd[2] = IAP_FLASH_SECTOR;

	timer_suspend();
	iap_proc(iap_cmd, iap_res);
	timer_resume();

	return iap_res[0];
}

int iap_compare(void * adr1, void * adr2, int size)
{
	iap_cmd[0] = IAP_COMPARE;
	iap_cmd[1] = (unsigned int)adr1;
	iap_cmd[2] = (unsigned int)adr2;
	iap_cmd[3] = size;

	timer_suspend();
	iap_proc(iap_cmd, iap_res);
	timer_resume();

	return iap_res[0];
}
