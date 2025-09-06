#ifndef __CS_FLASH_BIN_HEADER__
#define __CS_FLASH_BIN_HEADER__

extern int write_base_mac(unsigned char *mac);
extern int read_base_mac(unsigned char *mac);
extern int is_burn_flash(void);
extern int burn_flash_disabled(void);

#endif
