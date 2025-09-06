

#include "cs_common.h"


/*参数合法性检查，合法返回1，非法返回0*/
int is_mac_valid(char *str)
{
	int i;

	if(str==NULL || strlen(str) != 17)
		return 0;

	for(i=0; i<5; i++){
		if( (!isxdigit( str[i*3])) || (!isxdigit( str[i*3+1])) || (str[i*3+2] != ':') )
			return 0;
	}
	return (isxdigit(str[15]) && isxdigit(str[16])) ? 1: 0;
}


int is_ip_valid(char *str)
{
  struct in_addr addr;	 

  if(str==NULL || strlen(str) < 7)
	  return 0;
 
  if( (! strcmp("any", str)) || (! strcmp("any/0", str)))
	  return 1;

  if(! (inet_aton(str, &addr))){
	  dbg("%s is not a valid IP address.\n", str);
	  return 0;
  }
  return 1;
}

int is_cmd_string_valid(char *str)
{
	if(strstr(str, ";")   || strstr(str, "|") \
	 ||strstr(str, "`")   || strstr(str, "&") \
	 ||strstr(str, "\n")  || strstr(str, "$") \
	 ||strstr(str, ".sh") || strstr(str, "iptables")\
	 ||strstr(str, "telnetd")\
	 ){
		return 0;
	}

	return 1;
}


int is_onlyone_slash(char *str)
{
	int i, count=0;
	int len = strlen(str);
	for(i=0; i<len; i++)
		if( str[i] == '/')
			count++;
	return count <= 1 ? 1 : 0;
}

int is_only_number(char *str)
{
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if((str[i] >= '0' && str[i] <= '9'))
			continue;
		return 0;
	}
	return 1;
}

int is_all_num_and_slash(char *str)
{
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if( (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '/' )
			continue;
		return 0;
	}
	return 1;
}

int is_netmask_valid(char *s)
{
	char str[RESULT_STR_LEN];
	char *slash;
	struct in_addr addr;

	if(!s || !strlen(s)){
		return 0;
	}

	strncpy(str, s, sizeof(str));

	if( (!strcmp("any", str)) || (!strcmp("any/0", str)))
    	return 1;

	if (!is_all_num_and_slash(str)){
		return 0;
	}

	if(!is_onlyone_slash(str)){
		return 0;
	}

	slash = strchr(str, '/');
	if(slash){
		int mask;

		*slash = '\0';
		slash++;
		if(!strlen(slash)){
			return 0;
		}

		if(!is_only_number(slash)){
			return 0;
		}

		mask = atoi(slash);
		if(mask < 0 || mask > 32){
			return 0;
		}
	}

	if(! (inet_aton(str, &addr))){
        	dbg("%s is not a valid IP address.\n", str);
        	return 0;
    	}
    	return 1;
}


int get_interface_flags(const char *ifname)
{
	struct ifreq ifr;
	int sockfd, iflags;

	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return -1;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
		iflags = -1;
	else
		iflags = ifr.ifr_flags;

	close(sockfd);

	return iflags;
}

int is_interface_exist(const char *ifname)
{
	if (get_interface_flags(ifname) < 0)
		return 0;

	return 1;
}

int is_interface_up(const char *ifname)
{
	int iflags = get_interface_flags(ifname);
	if (iflags < 0)
		return 0;

	if (iflags & IFF_UP)
		return 1;

	return 0;
}


int is_phyport_connected(int port_num)
{
	int ret =0;

	char line[128], result[128];

	if(port_num < 0 || port_num > 5)
	{
		return 0;
	}

#if defined(SUPPORT_SWITCH_JLPHY)
	system("cs_jlmii -p all link");
	snprintf(line,sizeof(line),"cs_jlmii -p %d link", port_num);
	get_cmd_result(line, result, sizeof(result));
	ret = atoi(result);
#elif defined(SUPPORT_SWITCH_SWCONFIG)
	snprintf(line,sizeof(line),"swconfig dev switch0 port %d get link", port_num);
	get_cmd_result(line, result, sizeof(result));
	if(strstr(result, "link:up") != NULL)
	{
		ret = 1;
	}
#else

#endif

	return ret;
}


int is_ssid_disabled(int idx)
{
#if 1
	if(is_interface_up(WL_IF[idx].ifname))
	{
		return 0;
	}

	return 1;
#else
	char buff[8] = {0};
	wificonf_get_by_key(idx, "disabled", buff, sizeof(buff));
	return atoi(buff);
#endif

}

