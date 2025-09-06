#include <fcntl.h>

#include "cs_uci.h"
#include "cs_common.h"

#define SYTSEM_RESETINT              "/tmp/restore_default"
#define TMP_LED_SH                   "/tmp/tmp_led.sh"
#define LED_USE_OT_FLAG              "/tmp/.led_use_for_flag" //led user for check,wps,reset and ...
#define LED_RESET_FLAG               "/tmp/.led_reset_flag" //reset led status


typedef enum {
	NO_NETWORK = 0,
	WIRE_OR_4G,
	LOW_SIGNAL,
	NO_SIM_NO_NETWORK,
	SIM_NO_NETWORK,
	NO_NETWORK_LOW_SIGNAL,
}LED_STATUS;

LED_STATUS old_led_status = NO_NETWORK;
int old_cur_wan_netstatus=2;
int old_i_signal=0;



#define PRINTF_LED_STATUS_FORMAT(status) \
 ((status == NO_NETWORK) ? "NO_NETWORK" : \
  (status == WIRE_OR_4G)? "WIRE_OR_4G" : \
  (status == LOW_SIGNAL)? "LOW_SIGNAL" : \
  (status == NO_SIM_NO_NETWORK)? "NO_SIM_NO_NETWORK" : \
  (status == SIM_NO_NETWORK)? "SIM_NO_NETWORK" : \
	 "Unknown Status")

int led_self_check(void)
{
	int ret = 0;
	char cmd[128]={0};
	FILE *fp = NULL;
	fp = fopen(TMP_LED_SH,"w+");
	if(fp)
	{
#if defined(BOARD_IP04513)
		doSystem("touch %s", LED_USE_OT_FLAG);
		fprintf(fp, "%s\n", "#!/bin/sh");
		fprintf(fp, "%s\n", "echo none > /sys/class/leds/green/trigger");
		fprintf(fp, "%s\n", "echo none > /sys/class/leds/red/trigger");
		fprintf(fp, "%s\n", "blink=0");
		fprintf(fp, "%s\n", "while [ y=y ]");
		fprintf(fp, "%s\n", "do");
		fprintf(fp, "%s\n", "[ -f "SYTSEM_RESETINT" ] && break");
		fprintf(fp, "%s\n", "if [ $blink -eq 1 ];then");
		fprintf(fp, "%s\n", "blink=0");
		fprintf(fp, "%s\n", "echo 1       > /sys/class/leds/green/brightness");
		fprintf(fp, "%s\n", "echo 0       > /sys/class/leds/red/brightness");
		fprintf(fp, "%s\n", "else");
		fprintf(fp, "%s\n", "blink=1");
		fprintf(fp, "%s\n", "echo 0       > /sys/class/leds/green/brightness");
		fprintf(fp, "%s\n", "echo 1       > /sys/class/leds/red/brightness");
		fprintf(fp, "%s\n", "fi");
		fprintf(fp, "%s\n", "sleep 1");
		fprintf(fp, "%s\n", "done");

		fclose(fp);
		snprintf(cmd, sizeof(cmd)-1, "chmod 777 %s",TMP_LED_SH);
		ret = system(cmd);
		if(ret == -1)
			return 0;
		
		snprintf(cmd, sizeof(cmd)-1, "%s 1>/dev/null 2>&1 &", TMP_LED_SH);
		system(cmd);
		if(ret == -1)
			return 0;
		
		ret = 1;
#endif
	}
	return ret;
}


void led_system_init(void)
{
	int led_status,flash_boot;

	Uci_Get_Int(PKG_SYSTEM_CONFIG, "main", "led_status", &led_status);
	Uci_Get_Int(PKG_SYSTEM_CONFIG, "main", "flash_boot", &flash_boot);

	if(flash_boot) {
		if(led_self_check())
			return;
	}

	if(led_status == 1){
#if defined(BOARD_IP04513)
		CsteSystem("echo none > /sys/class/leds/red/trigger", 0);
		CsteSystem("echo 0 > /sys/class/leds/red/brightness", 0);
		CsteSystem("echo timer > /sys/class/leds/green/trigger", 0);
		CsteSystem("echo 1000 > /sys/class/leds/green/delay_on", 0);
		CsteSystem("echo 1000 > /sys/class/leds/green/delay_off", 0);

		CsteSystem("/etc/init.d/led restart", 0);
#elif defined(BOARD_IP04506)

		CsteSystem("echo none > /sys/class/leds/link/trigger", 0);
		CsteSystem("echo 0 > /sys/class/leds/link/brightness", 0);
		CsteSystem("echo none > /sys/class/leds/signal/link/trigger", 0);
		CsteSystem("echo 0 > /sys/class/leds/signal/brightness", 0);
		CsteSystem("echo timer > /sys/class/leds/sys/trigger", 0);
		CsteSystem("echo 1000 > /sys/class/leds/sys/delay_on", 0);
		CsteSystem("echo 1000 > /sys/class/leds/sys/delay_off", 0);

		CsteSystem("/etc/init.d/led restart", 0);
#endif

	}else {
#if defined(BOARD_IP04513)
			CsteSystem("echo none > /sys/class/leds/red/trigger", 0);
			CsteSystem("echo 0		> /sys/class/leds/red/brightness", 0);
			CsteSystem("echo none > /sys/class/leds/green/trigger", 0);
			CsteSystem("echo 0		> /sys/class/leds/green/brightness", 0);
#elif defined(BOARD_IP04506)
	
			CsteSystem("echo none > /sys/class/leds/link/trigger", 0);
			CsteSystem("echo 0 > /sys/class/leds/link/brightness", 0);
			CsteSystem("echo none > /sys/class/leds/signal/link/trigger", 0);
			CsteSystem("echo 0 > /sys/class/leds/signal/brightness", 0);
			CsteSystem("echo none > /sys/class/leds/sys/trigger", 0);
			CsteSystem("echo 0 > /sys/class/leds/sys/brightness", 0);
	
			CsteSystem("/etc/init.d/led restart", 0);
#endif


	}

}

void reset_led_blink(void)
{

	doSystem("touch %s", SYTSEM_RESETINT);
	doSystem("touch %s", LED_USE_OT_FLAG);

#if defined(BOARD_IP04513)

	CsteSystem("echo none  > /sys/class/leds/red/trigger", 0);
	CsteSystem("echo 0	 > /sys/class/leds/red/brightness", 0);

	CsteSystem("echo timer > /sys/class/leds/green/trigger", 0);
	CsteSystem("echo 100  > /sys/class/leds/green/delay_on", 0);
	CsteSystem("echo 100  > /sys/class/leds/green/delay_off", 0);
	
#elif defined(BOARD_IP04506)

	CsteSystem("echo timer > /sys/class/leds/sys/trigger", 0);
	CsteSystem("echo 100  > /sys/class/leds/sys/delay_on", 0);
	CsteSystem("echo 100  > /sys/class/leds/sys/delay_off", 0);
#endif

}

void set_led_status(int led_status)
{

	if(led_status == 1) {
		doSystem("touch %s", LED_RESET_FLAG);
	}
	else {
#if defined(BOARD_IP04513)
		CsteSystem("echo none >/sys/class/leds/green/trigger;	\
					echo 0 	  >/sys/class/leds/green/brightness;	\
					echo none >/sys/class/leds/red/trigger; \
					echo 0 	  >/sys/class/leds/red/brightness", 0);
#endif
	}
	
}

void is_network(int cur_wan_netstatus)
{	
	if(cur_wan_netstatus==old_cur_wan_netstatus)
	{
		return ;
	}

	old_cur_wan_netstatus=cur_wan_netstatus;
	
	if(cur_wan_netstatus==1)
	{
		CsteSystem("echo none  > /sys/class/leds/link/trigger; \
					echo 1	   > /sys/class/leds/link/brightness", 0);
	}else
	{
		CsteSystem("echo none  > /sys/class/leds/link/trigger; \
					echo 0	   > /sys/class/leds/link/brightness", 0);
	}
	
			
}

void is_monde_signal(int i_signal)
{

	//跟上一次状态值一样就退出
	if(1 <= old_i_signal && old_i_signal <40 && 1<=i_signal && i_signal<40)
	{
		return ;
	}else if( 40<= old_i_signal && old_i_signal <99 && 40<=i_signal && i_signal<90)
	{
		return ;
	}
	old_i_signal = i_signal;
	
	if((1 <= i_signal && i_signal < 40) || i_signal == 99)//信号低于两格
	{
		CsteSystem("echo none  > /sys/class/leds/signal/trigger; \
				echo 1 > /sys/class/leds/signal/brightness", 0);
	}
	else
	{
		CsteSystem("echo none  > /sys/class/leds/signal/trigger; \
				echo 0 > /sys/class/leds/signal/brightness", 0);
	}
	
}

//一些状态灯位需要在定时器里进行控制
void led_style_ctl(LED_STATUS led_status)
{
#if defined(BOARD_IP04513)
		
		if(led_status==WIRE_OR_4G )// 有网络或LTE信号好 green led 慢闪
		{ 
			CsteSystem("echo none  > /sys/class/leds/red/trigger; \
						echo 0		 > /sys/class/leds/red/brightness; \
						echo timer > /sys/class/leds/green/trigger; \
						echo 1000  > /sys/class/leds/green/delay_on;\
						echo 1000  > /sys/class/leds/green/delay_off", 0);
		}
		else if(led_status==LOW_SIGNAL)// 有网络信号弱 双色led慢闪
		{
			CsteSystem("echo none > /sys/class/leds/green/trigger;	\
						echo 0 > /sys/class/leds/green/brightness;	\
						echo none > /sys/class/leds/red/trigger; \
						echo 0 > /sys/class/leds/red/brightness", 0);

			CsteSystem("echo timer > /sys/class/leds/green/trigger; \
						echo timer > /sys/class/leds/red/trigger; \
						echo 1000  > /sys/class/leds/green/delay_on; \
						echo 1000  > /sys/class/leds/red/delay_on; \
						echo 1000  > /sys/class/leds/green/delay_off; \
						echo 1000  > /sys/class/leds/red/delay_off", 0);
		}
		else if(led_status==NO_SIM_NO_NETWORK)// 无网络无sim卡 red led慢闪
		{

			CsteSystem("echo none  > /sys/class/leds/green/trigger; \
						echo 0		 > /sys/class/leds/green/brightness; \
						echo none  > /sys/class/leds/red/trigger; \
						echo 0		 > /sys/class/leds/red/brightness; \
						echo timer > /sys/class/leds/red/trigger; \
						echo 1000  > /sys/class/leds/red/delay_on;\
						echo 1000  > /sys/class/leds/red/delay_off", 0);
		}
		else if(led_status==SIM_NO_NETWORK)// 无网络有sim卡 red led 常亮
		{

			CsteSystem("echo none  > /sys/class/leds/green/trigger; \
						echo 0		 > /sys/class/leds/green/brightness; \
						echo none  > /sys/class/leds/red/trigger; \
						echo 1		 > /sys/class/leds/red/brightness;", 0);
		}
		else{

			CsteSystem("echo none  > /sys/class/leds/green/trigger; \
						echo 0		 > /sys/class/leds/green/brightness; \
						echo none  > /sys/class/leds/red/trigger; \
						echo 0		 > /sys/class/leds/red/brightness; \
						echo timer > /sys/class/leds/red/trigger; \
						echo 100  > /sys/class/leds/red/delay_on;\
						echo 100  > /sys/class/leds/red/delay_off", 0);
		}
/*
#elif defined(BOARD_IP04506)
		if(led_status==WIRE_OR_4G )// 有网络
		{ 
			CsteSystem("echo none  > /sys/class/leds/link/trigger; \
						echo 1		 > /sys/class/leds/link/brightness", 0);
			CsteSystem("echo none  > /sys/class/leds/signal/trigger; \
						echo 0		 > /sys/class/leds/signal/brightness", 0);
		}
		else if(led_status==LOW_SIGNAL)// 有网络信号弱 低于两格
		{
			CsteSystem("echo none  > /sys/class/leds/link/trigger; \
						echo 1		 > /sys/class/leds/link/brightness", 0);
			CsteSystem("echo none  > /sys/class/leds/signal/trigger; \
						echo 1 > /sys/class/leds/signal/brightness", 0);
			
		}
		else if(led_status==NO_NETWORK_LOW_SIGNAL)// 无网络信号弱 
		{
			CsteSystem("echo none  > /sys/class/leds/signal/trigger; \
						echo 1		 > /sys/class/leds/signal/brightness", 0);
			CsteSystem("echo none  > /sys/class/leds/link/trigger; \
						echo 0		 > /sys/class/leds/link/brightness", 0);
		}
		else if(led_status==SIM_NO_NETWORK || led_status==NO_SIM_NO_NETWORK || led_status==NO_NETWORK)// 无网络
		{
			CsteSystem("echo none  > /sys/class/leds/link/trigger; \
						echo 0		 > /sys/class/leds/link/brightness", 0);
			CsteSystem("echo none  > /sys/class/leds/signal/trigger; \
						echo 0		 > /sys/class/leds/signal/brightness", 0);
		}
		else{

			CsteSystem("echo none  > /sys/class/leds/signal/trigger; \
						echo 0		 > /sys/class/leds/signal/brightness", 0);
		}		
*/
#endif
}

void schedule_led_control()
{
	LINK_STATUS_T led_sign;

	int led_status,cur_wan_netstatus,wan_status;
	char at_cmd[128]={0},at_rsp[128]={0},signal[32]={0};
	int i_signal = 0,sim_status=0;
	
	struct interface_status status_paremeter;

	if(f_exists(LED_USE_OT_FLAG)) {
		old_led_status = -1;
		return;
	}
	else if(f_exists(LED_RESET_FLAG)) {
		doSystem("rm -f %s",LED_RESET_FLAG);
		old_led_status = -1;
	}

	Uci_Get_Int(PKG_SYSTEM_CONFIG,"main","led_status",&led_status);
	if(!led_status)
	{
		return;
	}

	wan_status = get_wan_status(&status_paremeter);
	cur_wan_netstatus = f_read_int("/tmp/linkInternet");
	sim_status = get_sim_status();
	
	if(sim_status == 1) {
		snprintf(at_cmd, sizeof(at_cmd), "at+csq\r\n");
		send_atcmd(at_cmd,at_rsp,sizeof(at_rsp)-1);
		get_sub_value(signal, at_rsp, sizeof(signal), ':', ',');
		i_signal = atoi(signal);
		if ( 0 < i_signal && i_signal <= 31 )
			i_signal = i_signal * 100 / 31;
		else if ( 100 <= i_signal && i_signal < 199 )//+CSQ: 159,199
			i_signal = i_signal - 100;
	}
	else {
		i_signal = -1;
	}

//	dbg("sim_status-%d,i_signal-%d\n",sim_status,i_signal);
//	dbg("status_paremeter.up-%d,wan_status-%d,cur_wan_netstatus-%d\n",status_paremeter.up, wan_status,cur_wan_netstatus);
//dbg("status_paremeter.up[%d],wan_status[%d],cur_wan_netstatus[%d]=sim_status[%d]==\n",status_paremeter.up, wan_status,cur_wan_netstatus,sim_status);

	
	if( wan_status == LINK_STATUS_WIRE && status_paremeter.up && cur_wan_netstatus ==1) {

		led_sign=WIRE_OR_4G;

	}
	else if( ( wan_status == LINK_STATUS_MODEM) \
	  && status_paremeter.up && cur_wan_netstatus ==1 && (i_signal >48 && i_signal != 99 ))
	{

		led_sign=WIRE_OR_4G;

	}
	else if ((wan_status == LINK_STATUS_MODEM ) \
		&& status_paremeter.up && cur_wan_netstatus == 1 && (i_signal < 48 || i_signal == 99 ) )
	{
	          
		led_sign=LOW_SIGNAL;
	        
	}
	else if( (wan_status == LINK_STATUS_MODEM ) \
		&& cur_wan_netstatus == 0 && sim_status == 1 && (i_signal < 48 || i_signal == 99 ))
	{ 
		led_sign=NO_NETWORK_LOW_SIGNAL;

	}	
	else if(cur_wan_netstatus == 0 && sim_status != 1 )
	{      
		led_sign=NO_SIM_NO_NETWORK; 
	        
	}
	else if( (wan_status == LINK_STATUS_WIRE ) \
		&& cur_wan_netstatus == 0 && sim_status == 1 )
	{  
		led_sign=SIM_NO_NETWORK;

	}	
	else { 
		led_sign=NO_NETWORK;
	}
	
//	dbg("led_sign-[%s],old_led_status[%s]\n",PRINTF_LED_STATUS_FORMAT(led_sign),PRINTF_LED_STATUS_FORMAT(old_led_status));

#if defined(BOARD_IP04506)
		is_network(cur_wan_netstatus);//有网
		is_monde_signal(i_signal);
#endif

	if(led_sign == old_led_status)
		return;

	old_led_status = led_sign;
	led_style_ctl(led_sign);
}

void wps_start_led()
{

	doSystem("touch %s", LED_USE_OT_FLAG);

}

void wps_stop_led()
{
	doSystem("rm %s", LED_USE_OT_FLAG);
	doSystem("touch %s", LED_RESET_FLAG);
	
	schedule_led_control();
}

