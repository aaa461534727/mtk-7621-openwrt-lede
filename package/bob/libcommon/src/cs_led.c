#include <fcntl.h>
#include <sys/sysinfo.h>


#include "cs_uci.h"
#include "cs_common.h"

#define TMP_LED_SH	"/tmp/tmp_led.sh"

void led_start_mesh_wps(void)
{
	CsteSystem("echo none > /sys/devices/platform/gpio-leds/leds/led_sys/trigger", 0);
	CsteSystem("echo 1 > /sys/devices/platform/gpio-leds/leds/led_sys/brightness", 0);
}

void led_system_init(void)
{
	int led_status;

	Uci_Get_Int(PKG_SYSTEM_CONFIG, "main", "ledStatus", &led_status);

	set_led_status(led_status);
}

void led_reset_blink(void)
{
	CsteSystem("touch /tmp/restore_default", 0);
	CsteSystem("echo timer > /sys/class/leds/led_sys/trigger", 0);
	CsteSystem("echo 100   > /sys/class/leds/led_sys/delay_on", 0);
	CsteSystem("echo 100   > /sys/class/leds/led_sys/delay_off", 0);
}

void set_led_status(int led_status)
{
	if(led_status == 1)
	{
		CsteSystem("echo timer > /sys/class/leds/led_sys/trigger", 0);
		CsteSystem("echo 1000 > /sys/devices/platform/gpio-leds/leds/led_sys/delay_on", 0);
		CsteSystem("echo 1000 > /sys/devices/platform/gpio-leds/leds/led_sys/delay_off", 0);
		CsteSystem("/etc/init.d/led restart", 0);

		CsteSystem("switch reg w 7d00 77777", 0);
	}
	else
	{
		CsteSystem("echo 0 > /sys/devices/platform/gpio-leds/leds/led_sys/delay_on", 0);
		CsteSystem("echo 1 > /sys/devices/platform/gpio-leds/leds/led_sys/delay_off", 0);
		CsteSystem("switch reg w 7d00 0", 0);

#if defined(CONFIG_BOARD_IP04520)
		CsteSystem("echo 0 > /sys/class/leds/led_sys/brightness", 0);
		CsteSystem("echo 0 > /sys/class/leds/led_wlan2g/brightness", 0);
		CsteSystem("echo 0 > /sys/class/leds/led_wlan5g/brightness", 0);
		CsteSystem("echo 0 > /sys/class/leds/led_usb/brightness", 0);
#endif
	}
}


//一些状态灯位需要在定时器里进行控制
void led_schedule_control()
{
	int led_status;
	char led_status_buf[8], wps_onboarding_trigger_flag[8]={0};


	Uci_Get_Int(PKG_SYSTEM_CONFIG, "main", "ledStatus", &led_status);

	//datconf_get_by_key(TEMP_STATUS_FILE, "wps_onboarding_trigger_flag", wps_onboarding_trigger_flag, sizeof(wps_onboarding_trigger_flag));

	if( led_status==0 || atoi(wps_onboarding_trigger_flag)==1 || f_exists("/tmp/restore_default"))
	{
		return;
	}
}


