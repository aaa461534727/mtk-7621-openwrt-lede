#ifndef __CS_UCI_FUN_H__
#define __CS_UCI_FUN_H__


#include <string.h>
#include <stdlib.h>
#include "cs_uci.h"
#include <cJSON.h>
#include "cs_common.h"


typedef enum
{
	UCI_FUN_FALSE = 0,
	UCI_FUN_TRUE=1,
}UCI_FUN_BOOL;

/*--------system status -----------------*/
#define STATUS_CPU_TOTAL_TIME_GET(value)\
		do {\
			Uci_Get_Long(PKG_SYSTEM_STATUS,"sys", "cpu_total_time",value);\
		}while(0)

#define STATUS_CPU_IDLE_TIME_GET(value)\
		do {\
			Uci_Get_Long(PKG_SYSTEM_STATUS,"sys", "cpu_idle_time",value);\
		}while(0)

#define STATUS_CPU_RT_USAGE_GET(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "cpu_rt_usage",value);\
		}while(0)

#define STATUS_CPU_AVG_USAGE_GET(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "cpu_avg_usage",value);\
		}while(0)				

#define STATUS_MEM_RT_USAGE_GET(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "mem_rt_usage",value);\
		}while(0)

#define STATUS_MEM_AVG_USAGE_GET(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "mem_avg_usage",value);\
		}while(0)
		
#define STATUS_MEM_TOTAL_GET(value)\
		do {\
			Uci_Get_Long(PKG_SYSTEM_STATUS,"sys", "mem_total",value);\
		}while(0)

#define STATUS_MEM_USED_GET(value)\
		do {\
			Uci_Get_Long(PKG_SYSTEM_STATUS,"sys", "mem_used",value);\
		}while(0)

#define STATUS_MEM_CACHED_GET(value)\
		do {\
			Uci_Get_Long(PKG_SYSTEM_STATUS,"sys", "mem_cached",value);\
		}while(0)

#define STATUS_MEM_FREE_GET(value)\
		do {\
			Uci_Get_Long(PKG_SYSTEM_STATUS,"sys", "mem_free",value);\
		}while(0)

#define STATUS_FLASH_SIZE_GET(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "flash_size",value);\
		}while(0)

#define STATUS_LANGUAGE_TYPE_GET(value)\
		do {\
			Uci_Get_Str(PKG_SYSTEM_STATUS,"sys", "language_type",value);\
		}while(0)

#define STATUS_USB_LINK_GET(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "usb_link",value);\
		}while(0)

#define STATUS_WEB_SITE_GET(value)\
		do {\
			Uci_Get_Str(PKG_SYSTEM_STATUS,"sys", "web_site",value);\
		}while(0)				

#define STATUS_UPLOAD_SETTINGS_PATH_GET(value)\
		do {\
			Uci_Get_Str(PKG_SYSTEM_STATUS,"sys", "upload_settings_path",value);\
		}while(0)

#define STATUS_FIRMWARE_GET(value)\
		do {\
			Uci_Get_Str(PKG_SYSTEM_STATUS,"sys", "firmware",value);\
		}while(0)

#define STATUS_UPGRADE_CUSTOM_FIRMWARE_GET(value)\
		do {\
			Uci_Get_Str(PKG_SYSTEM_STATUS,"sys", "custom_firmware",value);\
		}while(0)

#define STATUS_UPGRADE_OPTION_GET(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "upgrade_option",value);\
		}while(0)
#define STATUS_UPGRADE_STATUS_GET(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "upgrade_status",value);\
		}while(0)			
#define STATUS_UPTIME_GET(value)\
		do {\
			Uci_Get_Long(PKG_SYSTEM_STATUS,"sys", "uptime",value);\
		}while(0)

#define STATUS_STATISTICS_AUTH_GET(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "statistics_auth",value);\
		}while(0)
#define STATUS_STATISTICS_AUTH1_GET(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "statistics_auth1",value);\
		}while(0)
#define STATUS_STATISTICS_AUTH2_GET(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "statistics_auth2",value);\
		}while(0)

#define STATUS_LOGIN_FLAG(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "login_flag",value);\
		}while(0)

#define STATUS_OPENVPND_CERT_STATUS(value)\
		do {\
			Uci_Get_Int(PKG_SYSTEM_STATUS,"sys", "openvpnd_cert_status",value);\
		}while(0)


/*--------network status -----------------*/
#define STATUS_LAN_IF_NAME_GET(value)\
		do {\
			Uci_Get_Str(PKG_NETWORK_STATUS,"sys", "lan_if_name",value);\
		}while(0)

#define STATUS_LAN_MAC_ADDRESS_GET(value)\
		do {\
			Uci_Get_Str(PKG_NETWORK_STATUS,"sys", "lan_mac_address",value);\
		}while(0)

#define STATUS_LAN_IP_ADDRESS_GET(value)\
		do {\
			Uci_Get_Str(PKG_NETWORK_STATUS,"sys", "lan_ip_address",value);\
		}while(0)

#define STATUS_LAN_DEFAULT_GW_GET(value)\
		do {\
			Uci_Get_Str(PKG_NETWORK_STATUS,"sys", "lan_default_gw",value);\
		}while(0)

#define STATUS_LAN_SUBNET_MASK_GET(value)\
		do {\
			Uci_Get_Str(PKG_NETWORK_STATUS,"sys", "lan_subnet_mask",value);\
		}while(0)
		
#define STATUS_WAN_IF_NAME_UCI_GET(idx, value)\
		do {\
			Uci_Get_Str_By_Idx(PKG_NETWORK_STATUS, "wan", idx, "if_name",value);\
		}while(0)

#define STATUS_WAN_MAC_ADDRESS_GET(idx, value)\
		do {\
			Uci_Get_Str_By_Idx(PKG_NETWORK_CONFIG,"wan", idx, "macaddr",value);\
		}while(0)

#define STATUS_WAN_IP_ADDRESS_UCI_GET(idx, value)\
		do {\
			Uci_Get_Str_By_Idx(PKG_NETWORK_STATUS, "wan", idx, "ip_address",value);\
		}while(0)

#define STATUS_WAN_DEFAULT_GW_UCI_GET(idx, value)\
		do {\
			Uci_Get_Str_By_Idx(PKG_NETWORK_STATUS, "wan", idx, "default_gw",value);\
		}while(0)

#define STATUS_SUBNET_MASK_UCI_GET(idx, value)\
		do {\
			Uci_Get_Str_By_Idx(PKG_NETWORK_STATUS, "wan", idx, "subnet_mask",value);\
		}while(0)

#define STATUS_LAN_DNS_SERVER_GET(value)\
		do {\
			Uci_Get_Str(PKG_NETWORK_STATUS,"sys", "lan_dns_server",value);\
		}while(0)

#define STATUS_WAN_DNS_SERVER_UCI_GET(idx, value)\
		do {\
			Uci_Get_Str_By_Idx(PKG_NETWORK_STATUS, "wan", idx, "dns_server",value);\
		}while(0)

#define STATUS_WAN_LINK_UCI_GET(idx, value)\
		do {\
			Uci_Get_Int_By_Idx(PKG_NETWORK_STATUS, "wan", idx, "link",value);\
		}while(0)

#define STATUS_WAN_LINK_GET_STR(idx, value)\
		do {\
			Uci_Get_Str_By_Idx(PKG_NETWORK_STATUS, "wan", idx, "link",value);\
		}while(0)

#define STATUS_WAN_UPTIME_GET(idx, value)\
		do {\
			Uci_Get_Str_By_Idx(PKG_NETWORK_STATUS, "wan", idx, "uptime",value);\
		}while(0)

#define STATUS_DDNS_LINK_GET(idx, value)\
		do {\
			Uci_Get_Int_By_Idx(PKG_NETWORK_STATUS,"wan", idx, "ddns_link",value);\
		}while(0)
						
#define STATUS_DDNS_BIND_IP_GET(idx, value)\
		do {\
			Uci_Get_Str_By_Idx(PKG_NETWORK_STATUS,"wan", idx,"ddns_bind_ip",value);\
		}while(0)

#define STATUS_DHCP_CHECK_STATE_GET(value)\
		do {\
			Uci_Get_Int(PKG_NETWORK_STATUS,"sys", "dhcp_check_state",value);\
		}while(0)
		
#define STATUS_WAN_VPN_INTERFACE_GET(value)\
		do {\
			Uci_Get_Str(PKG_NETWORK_STATUS,"sys", "wan_vpn_interface",value);\
		}while(0)
#define STATUS_OPENVPNC_INTERFACE_GET(value)\
		do {\
			Uci_Get_Str(PKG_NETWORK_STATUS,"sys", "openvpnc_interface",value);\
		}while(0)

/*--------wireless status -----------------*/
#define STATUS_CHANNEL_SCAN_STATE_GET(value)\
		do {\
			Uci_Get_Int(PKG_WIRELESS_STATUS,"sys", "channel_scan_state",value);\
		}while(0)


UCI_FUN_BOOL Uci_Get_Str(int title,char *section, char *option,char *value);
UCI_FUN_BOOL Uci_Get_Int(int title,char *section, char *option,int *value);
UCI_FUN_BOOL Uci_Get_Long(int title,char *section, char *option,long *value);
UCI_FUN_BOOL Uci_Get_Long_Long(int title,char *section, char *option,long *value);
UCI_FUN_BOOL Uci_Set_Str(int title,char *section, char *option,char *value);
UCI_FUN_BOOL Uci_Add_List(int title,char *section, char *option,char *value);
UCI_FUN_BOOL Uci_Del_List(int title,char *section, char *option,char *value);
UCI_FUN_BOOL Uci_Add_Section(int title,char *section_type);
UCI_FUN_BOOL Uci_Del_Section(int title,char *section_name);
UCI_FUN_BOOL Uci_Commit(int title);
UCI_FUN_BOOL Uci_Get_Str_By_Idx(int title, const char *section, int idx, const char *option, char *value);
UCI_FUN_BOOL Uci_Get_Int_By_Idx(int title,char *section, int idx, char *option,int *value);
UCI_FUN_BOOL Uci_Set_Str_By_Idx(int title, const char *section, int idx, const char *option, char *value);
UCI_FUN_BOOL Uci_Get2Json(cJSON* root, int title, const char *section, const char *option, char *objname);
UCI_FUN_BOOL Uci_Get2Json_By_Idx(cJSON* root, int title, const char *section, int idx, const char *option, char *objname);
UCI_FUN_BOOL Uci_Get_Str_By_Section_Index(int title,char *section_type,int index, char *option,char *value);
UCI_FUN_BOOL Uci_Get_Int_By_Section_Index(int title,char *section_type,int index, char *option,int *value);
UCI_FUN_BOOL get_num_uci2json(cJSON* root,int title, char *section, char *option, char *key);
UCI_FUN_BOOL get_uci2json(cJSON* root,int title, char *section, char *option, char *key);


#endif


