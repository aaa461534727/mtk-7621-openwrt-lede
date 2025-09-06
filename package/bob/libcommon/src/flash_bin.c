#include "cs_common.h"

#define ETHER_ADDR_LEN		6

#define MTD_FACTORY 	"Factory"

//WLAN
#define WLAN_OFFSET   0x04
#define W24_OFFSET    0x04

//cs custom
#define USER_DATA_BASIC_OFFSET    0x10000
#define BURN_FLASH_OFFSET         USER_DATA_BASIC_OFFSET+0x00


#define MACADDR_LEN 	6

int write_base_mac(unsigned char *mac)
{
	return flash_mtd_write(MTD_FACTORY,WLAN_OFFSET,mac,MACADDR_LEN);
}

int read_base_mac(unsigned char *mac)
{
	return flash_mtd_read(MTD_FACTORY,WLAN_OFFSET,mac,MACADDR_LEN);
}

int is_burn_flash(void)
{
	int ret=0;
	unsigned char buffer[2]={0};

	flash_mtd_read(MTD_FACTORY, BURN_FLASH_OFFSET, buffer, 1);
	if(buffer[0]!=0x55)
		ret=1;
	return ret;
}

int burn_flash_disabled(void)
{
	int ret=0;
	char buffer[2]={0x55,0x00};
	
	ret=flash_mtd_write(MTD_FACTORY, BURN_FLASH_OFFSET, buffer, 1);
	return ret;
}
