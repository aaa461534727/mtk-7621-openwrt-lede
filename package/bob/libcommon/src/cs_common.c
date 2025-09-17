#include <libdatconf.h>

#include <sys/sysinfo.h>

#include "cs_common.h"
#include <sys/socket.h>
#include <sys/un.h>

WLAN_TABLE WL_IF[]={

	//2G
	{"wifi-device", "radio_2g",      "band",   "2.4G"},
	{"wifi-iface",  "main_2g",  	 "ifname", "ra0"},
	{"wifi-iface",  "mesh_2g",       "ifname", "ra2"},
	{"wifi-iface",  "guest_2g1",     "ifname", "ra1"},

	//5G
	{"wifi-device", "radio_5g",      "band",   "5G"},
	{"wifi-iface",  "main_5g",  	 "ifname", "rai0"},
	{"wifi-iface",  "mesh_5g",  	 "ifname", "rai2"},
	{"wifi-iface",  "guest_5g1",     "ifname", "rai1"},

	//RPT
	{"wifi-iface",  "apcli",  	     "ifname", "apcli0"},

	{"", 0, NULL, 0},
};

PRODUCT_PARAM_TABLE product_param[]={
	{PKG_PRODUCT_CONFIG, "sysinfo",          "soft_model"},
	{PKG_PRODUCT_CONFIG, "sysinfo",          "hard_model"},
	{PKG_PRODUCT_CONFIG, "custom",           "cloudupdate_domain"},
	{PKG_PRODUCT_CONFIG, "custom",           "telnetd_password"},
	{PKG_PRODUCT_CONFIG, "custom",           "hide_logo"},
	{PKG_PRODUCT_CONFIG, "custom",           "copyright"},
	{PKG_PRODUCT_CONFIG, "custom",           "domainaccess"},
	{PKG_PRODUCT_CONFIG, "custom",           "hostname"},
	{PKG_PRODUCT_CONFIG, "custom",           "web_title"},
	{PKG_PRODUCT_CONFIG, "custom",           "vendor"},
	{PKG_PRODUCT_CONFIG, "custom",           "helpurl_cn"},
	{PKG_PRODUCT_CONFIG, "custom",           "helpurl_en"},
	{PKG_PRODUCT_CONFIG, "custom",           "helpurl_ct"},
	{PKG_PRODUCT_CONFIG, "custom",           "helpurl_ru"},
	{PKG_PRODUCT_CONFIG, "custom",           "helpurl_vi"},
	
	//wireless
	{PKG_PRODUCT_CONFIG, "custom",           "fixed_mac"},
	{PKG_PRODUCT_CONFIG, "custom",           "ssid_2g"},
	{PKG_PRODUCT_CONFIG, "custom",           "wlankey_2g"},
	{PKG_PRODUCT_CONFIG, "custom",           "country_2g"},
	{PKG_PRODUCT_CONFIG, "custom",           "htmode_2g"},
	{PKG_PRODUCT_CONFIG, "custom",           "channel_2g"},
	{PKG_PRODUCT_CONFIG, "custom",           "maxsta_2g"},
	{PKG_PRODUCT_CONFIG, "custom",           "ssid_tail_2g"},
	{PKG_PRODUCT_CONFIG, "custom",           "ssid_5g"},
	{PKG_PRODUCT_CONFIG, "custom",           "wlankey_5g"},
	{PKG_PRODUCT_CONFIG, "custom",           "country_5g"},
	{PKG_PRODUCT_CONFIG, "custom",           "htmode_5g"},
	{PKG_PRODUCT_CONFIG, "custom",           "channel_5g"},
	{PKG_PRODUCT_CONFIG, "custom",           "maxsta_5g"},
	{PKG_PRODUCT_CONFIG, "custom",           "ssid_tail_5g"},
	//upnp
	{PKG_PRODUCT_CONFIG, "custom",           "manufacturer"},
	{PKG_PRODUCT_CONFIG, "custom",           "model_url"},
	{PKG_PRODUCT_CONFIG, "custom",           "manufacturer_url"},
	{PKG_PRODUCT_CONFIG, "custom",           "manufacturer_name"},

	{PKG_SYSTEM_CONFIG,  "main",             "username"},
	{PKG_SYSTEM_CONFIG,  "main",             "password"},
	{PKG_SYSTEM_CONFIG,  "main",             "lang_support"},
	{PKG_SYSTEM_CONFIG,  "main",             "lang_type"},
	{PKG_SYSTEM_CONFIG,  "main",             "lang_show_auto"},
	{PKG_SYSTEM_CONFIG,  "main",             "lang_auto_flag"},
	

	{PKG_SYSTEM_CONFIG,  "ntp",              "timezone"},
	{PKG_SYSTEM_CONFIG,  "statistics",       "statistics_model"},
	{PKG_SYSTEM_CONFIG,  "statistics",       "statistics_domain"},

	{-1, NULL, NULL},
};

static char g_strResultTmp[TEMP_STR_LEN] = { 0 };

int CsteSystem(char *command, int printFlag)
{
	int pid = 0, status = 0;

    if( !command )
    {
        printf("CsteSystem: Null Command, Error!");
        return -1;
    }

	pid = fork();
  	if ( pid == -1 )
  	{
		return -1;
	}

  	if ( pid == 0 )
  	{
        char *argv[4];
    	argv[0] = "sh";
    	argv[1] = "-c";
    	argv[2] = command;
    	argv[3] = 0;
    	if (printFlag)
    	{
	        printf("[system]: %s\r\n", command);
        }
    	execv("/bin/sh", argv);
    	exit(127);
	}

  	/* wait for child process return */
  	do
  	{
	  	if ( waitpid(pid, &status, 0) == -1 )
    	{
	    	if ( errno != EINTR )
    		{
            	return -1;
      	    }
	    }
    	else
    	{
	    	return status;
		}
	} while ( 1 );

	return status;
}


/*
 * Returns the process ID.
 *
 * @param	name	pathname used to start the process.  Do not include the
 *                      arguments.
 * @return	pid
 */
pid_t get_pid_by_name(char *name)
{
	pid_t           pid = -1;
	DIR             *dir;
	struct dirent   *next;

	if ((dir = opendir("/proc")) == NULL)
	{
		perror("Cannot open /proc");
		return -1;
	}

	while ((next = readdir(dir)) != NULL)
	{
		FILE *fp;
		char filename[256];
		char buffer[256];

		/* If it isn't a number, we don't want it */
		if (!isdigit(*next->d_name))
			continue;

		sprintf(filename, "/proc/%s/cmdline", next->d_name);
		fp = fopen(filename, "r");
		if (!fp)
		{
			continue;
		}
		buffer[0] = '\0';
		fgets(buffer, 256, fp);
		fclose(fp);

		if (!strcmp(name, buffer))
		{
			pid = strtol(next->d_name, NULL, 0);
			break;
		}
	}
	closedir(dir);
	return pid;
}


void set_lktos_effect(char *action)
{
	 char cmd[CMD_STR_LEN];
	 snprintf(cmd, CMD_STR_LEN, "lktos_reload %s >/dev/null 2>&1", action);
	 system(cmd);
	 return;
}

void logmessage(char *logheader, char *fmt, ...)
{
	va_list args;
	char buf[512];

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	openlog(logheader, 0, 0);
	syslog(0,"%s",buf);
	closelog();
	va_end(args);
}

#define TCP_TMPFILE "/tmp/.tcpcheck.tmp"

int tcpcheck_net(const char *host, int port, int timeout)
{
	FILE *f = NULL;
	char s[TEMP_STR_LEN] = { 0 }; 
	char cmd[CMD_STR_LEN] = { 0 }; 
	int ok = 0;

	if ((NULL == host) || (strlen(host)<7))
		return ok;
	
	sprintf(cmd, "tcpcheck %d %s:%d > %s", timeout, host, port, TCP_TMPFILE);
	system(cmd);
	sleep(1);
	if ((f = fopen(TCP_TMPFILE, "r")) != NULL)
	{
		if ( NULL != fgets(s, sizeof(s), f))
		{
			if (strstr(s, "alive") != NULL)
			{
				ok = 1;
			}
			else if (strstr(s, "timed out") != NULL)
			{
				ok = 0;
			}
			else
			{
				ok = 0;
			}
		}
		fclose(f);
	}
	unlink(TCP_TMPFILE);
	
	return ok;
}

#define PING_TMPFILE "/tmp/.ping_success"
int do_ping_detect(const char *host, char *ifname)
{
	char cmd[CMD_STR_LEN] = {0};

	if ((NULL == host) || (strlen(host)<7))
		goto fail;

	if (ifname == NULL)
	{
		snprintf(cmd, sizeof(cmd), "ping -c 1 -W %d %s > /dev/null && touch %s", 1, host, PING_TMPFILE);
	}
	else
	{
		snprintf(cmd, sizeof(cmd), "ping -c 1 -W %d %s -I %s > /dev/null && touch %s", 1, host, ifname, PING_TMPFILE);
	}
	system(cmd);

	if ( access(PING_TMPFILE, F_OK) == 0)
	{
		unlink(PING_TMPFILE);
		return 1;
	}
fail:
	return 0;
}


int get_split_nums(char *value, char delimit)
{
    char *pos = value;
    int count=1;
    if(!pos)
        return 0;
    while( (pos = strchr(pos, delimit)))
	{
        pos = pos+1;
        count++;
    }
    return count;
}


int get_nth_val_safe(int index, char *value, char delimit, char *result, int len)
{
    int i=0, result_len=0;
    char *begin, *end;

    if(!value || !result || !len)
        return -1;

    begin = value;
    end = strchr(begin, delimit);

    while(i<index && end)
	{
        begin = end+1;
        end = strchr(begin, delimit);
        i++;
    }

    //no delimit
    if(!end)
	{
		if(i == index)
		{
			end = begin + strlen(begin);
			result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);
		}
		else
		{
			return -1;
		}
	}
	else
	{
		result_len = (len-1) < (end-begin)? (len-1) : (end-begin);
	}

	memcpy(result, begin, result_len );
	*(result+ result_len ) = '\0';

	return 0;
}


long get_current_uptime_sec(void)
{
    struct sysinfo info;
    sysinfo(&info);
    return info.uptime;
}


int domain_to_ip(const char *domain, char *ip)
{
	char str[32];
	struct hostent *hptr;
	int count = 0;

//	memset(ip, 0, 32);
	if(isalpha(domain[0]))
	{
		while(count<=3)
		{
			if((hptr = gethostbyname(domain)) != NULL)
			{
				sprintf(ip, "%s", inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str)));
				return 0;
			}
			else
			{
				printf(" gethostbyname error for host:%s try again!\n", domain);
				count++;
			}
		}
		//fail
		strncpy(ip, domain, 32);
		return 0;
	}
	else
	{
		strncpy(ip, domain, 32);
		return 0;
	}
}


int get_cmd_result(char *cmd, char *resultbuf, size_t buf_size)
{
	char *pchar = NULL;
	FILE *fp = popen(cmd, "r");
	if(!fp)
	{
		return -1;
	}
	fgets(resultbuf, buf_size, fp);
	pclose(fp); 
	if((pchar = strstr(resultbuf, "\n")))
		*pchar = '\0';

	resultbuf[buf_size-1] = '\0';
	return 0;
}

int get_cmd_val(const char *cmd)
{
    char buf[TEMP_STR_LEN]= {0};
	
    get_cmd_result(cmd, buf, sizeof(buf));

    return atoi(buf);
}


int get_ifname_ipaddr(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		dbg("open socket error");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0)
	{
		close(skfd);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}

void getStrFromFile(char* path, char* tmpbuf)
{
	char tmp[2] = {0};
	FILE *fp;
	int i = 0;
	strcpy(tmpbuf,"");
	fp = fopen(path, "r");
	if (!fp) {
		fprintf(stderr, "Read file error:%s!\n",path);
		return ;
	}
	while(!feof(fp)){
		fread(tmp,1,1,fp);
		tmpbuf[i++] = tmp[0];
	}
	tmpbuf[i-2]='\0';//del "\n\r"
	
	fclose(fp);
	return;
}

int getIfBytes(const char *ifname, unsigned long long *rxb, unsigned long long *txb)
{
	char path[TEMP_STR_LEN] = {0};
	char bytes[OPTION_STR_LEN] = {0};

	snprintf(path, TEMP_STR_LEN, "/sys/class/net/%s/statistics/tx_bytes", ifname);
	getStrFromFile(path, bytes);
	*txb = strtoull (bytes, NULL, 10);

	snprintf(path, TEMP_STR_LEN, "/sys/class/net/%s/statistics/rx_bytes", ifname);
	getStrFromFile(path, bytes);
	*rxb = strtoull (bytes, NULL, 10);

	return 0;
}


int get_vpnserver_ipaddr(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		dbg("open socket error");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFDSTADDR, &ifr) < 0)
	{
		close(skfd);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}


int get_ifname_macaddr(char *ifname, char *if_hw)
{
	struct ifreq ifr;
	char *ptr;
	int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0)
	{
		close(skfd);
		return -1;
	}

	ptr = (char *)&ifr.ifr_addr.sa_data;
	sprintf(if_hw, "%02X:%02X:%02X:%02X:%02X:%02X",
			(ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
			(ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

	close(skfd);
	return 0;
}


int get_ifname_mask(char *ifname, char *if_addr)
{
    struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		dbg("open socket error");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0)
	{
		close(skfd);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}


int get_current_gateway(char  *sgw)
{
	char   buff[256];
	int    nl = 0 ;
	struct in_addr dest;
	struct in_addr gw;
	int    flgs, ref, use, metric;
	unsigned long int d, g, m;
	int    find_default_flag = 0;

	FILE *fp = fopen("/proc/net/route", "r");

	while (fgets(buff, sizeof(buff), fp) != NULL)
	{
		if (nl)
		{
			int ifl = 0;

			while (buff[ifl] != ' ' && buff[ifl] != '\t' && buff[ifl] != '\0')
				ifl++;

			buff[ifl] = 0;  /* interface */

			if (sscanf(buff + ifl + 1, "%lx%lx%X%d%d%d%lx",
				   &d, &g, &flgs, &ref, &use, &metric, &m) != 7) {
				fclose(fp);
				return 0;
			}

			if (flgs & 0x0001)
			{
				dest.s_addr = d;
				gw.s_addr   = g;
				strcpy(sgw, (gw.s_addr == 0 ? "" : inet_ntoa(gw)));

				if (dest.s_addr == 0)
				{
					find_default_flag = 1;
					break;
				}
			}
		}

		nl++;
	}

	fclose(fp);

	if (find_default_flag != 1)
	{
		strcpy(sgw, "");
	}

	return 0;
	
}


int get_current_dns(int dnsIdx, char *dns, int is_ipv6)
{
	FILE *fp;
	char buf[80] = {0}, ns_str[11];
	int idx = 0;

	fp = fopen("/tmp/resolv.conf.auto", "r");

	if (NULL == fp)
	{
		return -1;
	}

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		if (strncmp(buf, "nameserver", 10) != 0 || strstr(buf, "127.0.0.1") != NULL)
			continue;

		if(1 == is_ipv6 && strstr(buf, ":") == NULL)
			continue;

		sscanf(buf, "%s %s", ns_str, dns);
		idx++;

		if (idx == dnsIdx)
			break;
	}

	fclose(fp);

	if (idx != dnsIdx)
	{
		strcpy(dns, "");
	}

	return 0;
}

int get_sta_ipaddr_bymac(char *pmac, char *ipv4_addr, char *ipv6_addr)
{
	FILE *fp = NULL;

	int  is_get_ip=0;

	char line_str[TEMP_STR_LEN] = {0};

	char mac[RESULT_STR_LEN] = {0}, ip[RESULT_STR_LEN] = {0};

	fp = fopen("/tmp/dhcp.leases","r");
	if(fp!=NULL)
	{
		while(fgets(line_str,sizeof(line_str),fp))
		{
			memset(mac,0,sizeof(mac));
			memset(ip,0,sizeof(ip));
			sscanf(line_str, "%*s %s %s %*s %*s",mac, ip);

			if(strlen(mac)>0 && strlen(ip)>0 && !strcasecmp(pmac,mac))
			{
				strcpy(ipv4_addr,ip);

				//get_sta_ipv6addr(pmac, ipv6_addr);
				//strcpy(ipv6_addr,"");
				is_get_ip=1;
				break;
			}
			memset(line_str,0,sizeof(line_str));
		}
		fclose(fp);
	}

	if(0==is_get_ip)
	{
		strcpy(ipv4_addr,"0.0.0.0");
		strcpy(ipv6_addr,"");
	}
	return 0;
}

int get_sta_hostname_bymac(char *pmac, char *hostname)
{
	FILE *fp = NULL;

	int is_get_dev_name=0;

	char line_str[TEMP_STR_LEN] = {0};

	char mac[RESULT_STR_LEN] = {0},ip[RESULT_STR_LEN] = {0},dev_name[TEMP_STR_LEN]={0};

	//try get hostname from dhcp.leases
	fp = fopen("/tmp/dhcp.leases","r");
	if(fp)
	{
		while(fgets(line_str, sizeof(line_str)-1, fp))
		{
			bzero(mac,sizeof(mac));
			bzero(ip,sizeof(ip));
			bzero(dev_name,sizeof(dev_name));

			sscanf(line_str, "%*s %s %*s %s %*s",mac, dev_name);

			if(0==strcasecmp(mac,pmac) && strlen(dev_name) > 0)
			{
				strcpy(hostname,dev_name);
				is_get_dev_name=1;
				break;
			}

			bzero(line_str,sizeof(line_str));
		}
		fclose(fp);
		fp = NULL;
	}

	if(0==is_get_dev_name){
		strcpy(hostname,pmac);
	}

	return 0;
}

int get_sta_mac_byip(char *ipaddr, char *mac)
{
	char buf[256];
	char ip_entry[32],flags[8], hw_address[32];
	unsigned long i_flags = 0;

    FILE *fp = fopen("/proc/net/arp", "r");
    if(!fp){
        printf("no proc fs mounted!\n");
        return;
    }
    strcpy(mac, "00:00:00:00:00:00");

	fgets(buf, 256, fp); //header
    while(fgets(buf, 256, fp)){
        sscanf(buf, "%s %*s %s %s", ip_entry, flags, hw_address);

		i_flags = strtoul(flags, 0, 16);
		if ( 0 == i_flags ){
			continue;
		}

        if(!strcmp(ipaddr, ip_entry)){
            strcpy(mac, hw_address);
            break;
        }
    }

    fclose(fp);
    return;
}

int get_ifname_bytes(const char *ifname, unsigned long long *rxb, unsigned long long *txb)
{
	char path[TEMP_STR_LEN] = {0};

	snprintf(path, TEMP_STR_LEN, "/sys/class/net/%s/statistics/tx_bytes", ifname);
	*txb = f_read_long_long(path);

	snprintf(path, TEMP_STR_LEN, "/sys/class/net/%s/statistics/rx_bytes", ifname);
	*rxb = f_read_long_long(path);

	return 0;
}


int get_wanmode_int(char *proto)
{
	int ret = DHCP_DISABLED;

	if(0 == strcmp(proto, "static"))
		ret = DHCP_DISABLED;
	else if(0 == strcmp(proto, "pppoe"))
		ret = PPPOE;
	else if(0 == strcmp(proto, "pptp"))
		ret = PPTP;
	else if(0 == strcmp(proto, "l2tp"))
		ret = L2TP;
	else
		ret = DHCP_CLIENT;

	return ret;
}


int get_wan_ifname(char *ifanem)
{
	char wanMode[SHORT_STR_LEN] = { 0 }, wan_interface[SHORT_STR_LEN] = { 0 };
	int wanType = 0;

	Uci_Get_Str(PKG_NETWORK_CONFIG, "vpn", "proto",wanMode);
	strcpy(wan_interface,"vpn");
	if(strlen(wanMode) == 0)
	{
		memset(wan_interface,'\0',sizeof(wan_interface));
		strcpy(wan_interface,"wan");
		Uci_Get_Str(PKG_NETWORK_CONFIG, "wan", "proto",wanMode);
	}
	wanType = get_wanmode_int(wanMode);

	switch(wanType)
	{
		case DHCP_DISABLED:
		case DHCP_CLIENT:
			strcpy(ifanem, WAN_IFNAME);
			break;
		case PPPOE:
			strcpy(ifanem, "pppoe-wan");
			break;
		case PPTP:
			strcpy(ifanem, "pptp-vpn");
			break;
		case L2TP:
			strcpy(ifanem, "l2tp-vpn");
			break;
		default:
			strcpy(ifanem, WAN_IFNAME);
			break;
	}
	return 0;
}

void get_gateway_iface(char *interface)
{
	char buff[1024], iface[16];
	char net_addr[128], mask_addr[128], gw_addr[16];
	int num, iflags, metric, refcnt, use, mss, window, irtt;
	FILE *fp = fopen("/proc/net/route", "r");
	char *fmt;
	int found = 0;
	unsigned long addr;
	char tmpbuf[32] = {0};

	if (!fp) {
		//printf("Open %s file error.\n", "/proc/net/route");
		return -1;
	}

	fmt = "%16s %128s %128s %X %d %d %d %128s %d %d %d";

	while (fgets(buff, 1023, fp)) {
		num = sscanf(buff, fmt, iface, net_addr, gw_addr,
			     &iflags, &refcnt, &use, &metric, mask_addr, &mss, &window, &irtt);

		if (num < 10 || !(iflags & 0x0001) || !(iflags & 0x0002)) //UP GATEWAY
			continue;

		if(strcmp(iface, "br-lan") != 0) {
			found=1;
			strcpy(interface, iface);
			break;
		}
	}

	fclose(fp);

	if (found == 1)
		return ;
	else {
		strcpy(interface, "");
		return ;
	}
}


int get_mdoem_iface(const char *interface)
{
    char ifname[64];
    in_addr_t dest, gway, mask;
    int flags, refcnt, use, metric, mtu, win, irtt;
    FILE *fp;

    fp = fopen("/proc/net/route", "r");
    if (fp == NULL)
        return -1;
    /* Skip the header line */
    if (fscanf(fp, "%*[^\n]\n") < 0) {
        fclose(fp);
        return -1;
    }
    for (;;) {
        int nread = fscanf(fp, "%63s%X%X%X%d%d%d%X%d%d%d\n",
                           ifname, &dest, &gway, &flags, &refcnt, &use, &metric, &mask,
                           &mtu, &win, &irtt);
				if(nread < 1)
					break;

        if (nread != 11 || !(flags & 0x0001)) {
					continue;
        }

				if (strstr(ifname,MODEM_IF_X)) {
					strcpy(interface,ifname);
					break;
				}
    }
    fclose(fp);
    return 0;
}

int get_wire_wan_status(struct interface_status *status_paremeter)
{
	char opmode_custom[16] = { 0 };
	char wan_if[16] = { 0 }, vpn_mode[16] = { 0 }, wan_mode[8] = { 0 };

	memset(status_paremeter, 0, sizeof(struct interface_status));

	Uci_Get_Str(PKG_SYSTEM_CONFIG, "opmode", "opmode_custom", opmode_custom);

	if(strcmp(opmode_custom, "gw") == 0)
	{
		Uci_Get_Str(PKG_NETWORK_CONFIG, "vpn", "proto", vpn_mode);

		if(strlen(vpn_mode) == 0)
		{
			strcpy(wan_if, "wan");
		}
		else
		{
			strcpy(wan_if, "vpn");
		}
	}
	else if(strcmp(opmode_custom, "wisp")==0)
	{
		strcpy(wan_if, "wlan");
	}

	if(strlen(wan_if) > 0)
	{
		get_interface_status(status_paremeter, wan_if);
	}
	else
	{
		return -1;
	}

	return 1;
}

LINK_STATUS_T get_wan_status(struct interface_status *status_paremeter)
{
	int modem_prio = 0;
	char opmode_custom[16] = { 0 };
	char ubus_wan[16] = { 0 }, modem_if[16] = { 0 },gw_if[16] = { 0 } ,vpn_mode[16] = { 0 }, wan_mode[8] = { 0 };
	char ubus_modem_wan[16] = { 0 };

	memset(status_paremeter, 0, sizeof(struct interface_status));

	if(!get_mdoem_iface(modem_if) && strlen(modem_if))
		sprintf(ubus_modem_wan,"wan%d",atoi(modem_if+strlen(MODEM_IF_X)));

	Uci_Get_Str(PKG_SYSTEM_CONFIG, "opmode", "opmode_custom", opmode_custom);
	Uci_Get_Int(PKG_WAN_MODEM_CONFIG,"main","modem_priority", &modem_prio);

	if(modem_prio == MODEM_PRIO_ONLY_3G ||
		 modem_prio == MODEM_PRIO_ONLY_4G ||
		 modem_prio == MODEM_PRIO_ONLY_5G)
	{
		strcpy(ubus_wan, ubus_modem_wan);
		get_interface_status(status_paremeter, ubus_wan);
	}
	else {
		if(strcmp(opmode_custom, "gw") == 0)
		{
			Uci_Get_Str(PKG_NETWORK_CONFIG, "vpn", "proto", vpn_mode);

			if(strlen(vpn_mode) == 0)
			{
				strcpy(ubus_wan, "wan");
			}
			else
			{
				strcpy(ubus_wan, "vpn");
			}
		}
		else if(strcmp(opmode_custom, "wisp")==0)
		{
			strcpy(ubus_wan, "wlan");
		}

		if(strlen(ubus_wan) > 0)
		{
			get_gateway_iface(gw_if);
			if((modem_prio == MODEM_PRIO_WIRE_FRIST || modem_prio == MODEM_PRIO_LTE_FRIST) && 
					strstr(gw_if, MODEM_IF_X))
			{
				strcpy(ubus_wan, ubus_modem_wan);
			}

			if((!strcmp(ubus_wan,"wan") || !strcmp(ubus_wan,"vpn")) && !is_phyport_connected(4))
			{
				return LINK_STATUS_NO;
			}

			get_interface_status(status_paremeter, ubus_wan);
		}
		else
		{
			return LINK_STATUS_NO;
		}

	}
	if(status_paremeter->up) 
	{
		if(!strcmp(ubus_wan, ubus_modem_wan))
			return LINK_STATUS_MODEM;
		else
			return LINK_STATUS_WIRE;
	}

	return LINK_STATUS_NO;

}

int get_cjson_string(cJSON *object, char *key,  char *val, int len)
{
	cJSON	*sp;

    assert(key && *key);

	if ((sp = cJSON_GetObjectItem(object, key)) != NULL) {
		if (sp->type==cJSON_String)
		{
			snprintf(val, len, "%s", sp->valuestring);
		}
		else if (sp->type==cJSON_False)
		{
			snprintf(val, len, "%d", 0);
		}
		else if (sp->type==cJSON_True)
		{
			snprintf(val, len, "%d", 1);
		}
		else if (sp->type==cJSON_Number)
		{
			snprintf(val, len, "%d", sp->valueint);
		}
		else
		{
			return -1;
		}
	}
	return 0;
}

//ubus call network.interface.wan/lan status
int get_interface_status(struct interface_status *status_paremeter,char *interface)
{
	int ret, i, len;

	char *ptr;

	char tmp_buf[128] = {0}, p_json[1024*4] = {0},ubus_data[1024*4] = {0},str[32] = {0},wan_cnt[64]={0};

	unsigned int addr;

	cJSON *j_data, *j_obj, *tmp_obj, *ubus_root;


	if(!strlen(interface))
		return 0;

	snprintf(tmp_buf,sizeof(tmp_buf)-1,"network.interface.%s",interface);
	ret = cs_ubus_cli_call(tmp_buf, "status",p_json);

	if(!ret){
		j_data = cJSON_Parse(p_json);
		if(j_data)
		{

			get_cjson_string(j_data, "proto",  status_paremeter->proto, sizeof(status_paremeter->proto));
			get_cjson_string(j_data, "l3_device", status_paremeter->device, sizeof(status_paremeter->device));

			memset(tmp_buf,0,sizeof(tmp_buf));
			get_cjson_string(j_data, "up",  tmp_buf, sizeof(tmp_buf));
			status_paremeter->up = atoi(tmp_buf);
			if(status_paremeter->up){
				memset(tmp_buf,0,sizeof(tmp_buf));
				get_cjson_string(j_data, "uptime",  tmp_buf, sizeof(tmp_buf));
				status_paremeter->uptime = atoi(tmp_buf);

				j_obj = cJSON_GetObjectItem(j_data, "ipv4-address");
				if(j_obj)
				{
					if(cJSON_GetArraySize(j_obj)>0) {
						tmp_obj = cJSON_GetArrayItem(j_obj,0);
						get_cjson_string(tmp_obj, "address", status_paremeter->ipaddr_v4, sizeof(status_paremeter->ipaddr_v4));

						memset(tmp_buf,0,sizeof(tmp_buf));
						get_cjson_string(tmp_obj, "mask",  tmp_buf, sizeof(tmp_buf));
						i=atoi(tmp_buf);

						memset(tmp_buf,0,sizeof(tmp_buf));
						addr = LMOVE(0xffffffff,32-i);
						sprintf(tmp_buf,"%u.%u.%u.%u",RMOVE(addr,24),
							RMOVE(addr,16)&0xff,RMOVE(addr,8)&0xff,addr&0xff);
						strcpy(status_paremeter->mask_v4,tmp_buf);
					}
				}

				j_obj = cJSON_GetObjectItem(j_data, "route");
				if(j_obj)
				{
					len = cJSON_GetArraySize(j_obj);
					for(i = 0;i<len;i++) {
						tmp_obj = cJSON_GetArrayItem(j_obj,i);

						memset(tmp_buf,0,sizeof(tmp_buf));
						get_cjson_string(tmp_obj, "target",  tmp_buf, sizeof(tmp_buf));
						if(strlen(tmp_buf)>0 && !strcmp(tmp_buf,"0.0.0.0"))
						{
							memset(tmp_buf,0,sizeof(tmp_buf));
							get_cjson_string(tmp_obj, "nexthop",  tmp_buf, sizeof(tmp_buf));
							strcpy(status_paremeter->gateway_v4,tmp_buf);
							break;
						}
					}
				}

				j_obj = cJSON_GetObjectItem(j_data, "dns-server");
				if(j_obj)
				{
					len=cJSON_GetArraySize(j_obj);
					for(i = 0; i < len & i < 2; i++){
						tmp_obj = cJSON_GetArrayItem(j_obj,i);
						ptr=tmp_obj->valuestring ? tmp_obj->valuestring : "0.0.0.0";
						if(i==0){
							snprintf(status_paremeter->pri_dns_v4,sizeof(status_paremeter->pri_dns_v4),"%s",ptr);
						}else if(i==1){
							snprintf(status_paremeter->sec_dns_v4,sizeof(status_paremeter->sec_dns_v4),"%s",ptr);
						}
					}
				}
			}
			cJSON_Delete(j_data);
		}
	}

	ret = cs_ubus_cli_call("cm", "get_eng_info",ubus_data);
	if(ret != -1)
	{	
		ubus_root = cJSON_Parse(ubus_data);
		if(ubus_root) 
		{
			cJSON *object= cJSON_GetObjectItem(ubus_root, "response");
			if(object) {
				memset(str, '\0', sizeof(str));
				get_cjson_string(object, "response_status", str, sizeof(str));
				if(!strcmp(str,"OK")){
					status_paremeter->lte_info_state = 1;
				}else{
					status_paremeter->lte_info_state = 0;
				}
					
			}
			
			cJSON_Delete(ubus_root);
		}
		
	}else{
		status_paremeter->lte_info_state = 0;
	}
	
	return 0;
}

//exp. 24 to "255.255.255.0"
int mask_num2string(int num, char *mask_buf, int buf_len)
{

    int i_num = 0;
    int byte = 0;
    int bit = 0;
    int i, j;

	union mask_s
	{
			char ch[4];
			unsigned long ul;
	}mask;
		
    mask.ul = 0;

    i_num = num;
    if (i_num < 1 || i_num > 32)
    {
        return -1;
    }

    byte = i_num/8;
    bit = i_num%8;

    for (i = 0; i < byte; i++)
    {
        mask.ch[i] = 0xff;
    }
    for (j = 0; j < bit; j++)
    {
        mask.ch[i] |= (1 << (7 - j));
    }

    if (inet_ntop(AF_INET, (void*)&mask.ul, mask_buf, buf_len) < 0)
    {
        return -1;
    }

    return 0;
}

//exp. "255.255.255.0" to 24 
int mask_string2num(char *mask)
{
	int i = 0;
	struct  in_addr addr;

	if(!mask || strlen(mask) < 9) //128.0.0.0
		return i;

	addr.s_addr=inet_addr(mask);
	for(i = 0; i<32; i++)
	{
		if((RMOVE(htonl(addr.s_addr),i) & 1) == 0)
			continue;
		else
			break;
	}
	return 32-i;
}

/*
 * Convert Ethernet address string representation to binary data
 * @param	a	string in xx:xx:xx:xx:xx:xx notation
 * @param	e	binary data
 * @return	TRUE if conversion was successful and FALSE otherwise
 */
int ether_atoe(const char *a, unsigned char *e)
{
	char *c = (char *) a;
	int i = 0;

	memset(e, 0, ETHER_ADDR_LEN);
	for (;;)
	{
		e[i++] = (unsigned char) strtoul(c, &c, 16);
		if (!*c++ || i == ETHER_ADDR_LEN)
			break;
	}
	return (i == ETHER_ADDR_LEN);
}

/*
 * Convert Ethernet address binary data to string representation
 * @param	e	binary data
 * @param	a	string in xx:xx:xx:xx:xx:xx notation
 * @return	a
 */
int ether_etoa(const unsigned char *e, char *a)
{
	char *c = a;
	int i;

	for (i = 0; i < ETHER_ADDR_LEN; i++)
	{
		if (i)
			*c++ = ':';
		c += sprintf(c, "%02X", e[i] & 0xff);
	}
	return 0;
}

void inc_mac(unsigned char *mac, int offset)
{
	int t;

	if (!mac)
		return;

	t = (((unsigned int) mac[3]) << 16) + (((unsigned int) mac[4]) << 8) + ((unsigned int) mac[5]);
	t += offset;

	mac[0] = mac[0];
	mac[1] = mac[1];
	mac[2] = mac[2];
	mac[3] = (t >> 16) & 0xff;
	mac[4] = (t >> 8) & 0xff;
	mac[5] = t & 0xff;
}


#define BURSIZE 2048

int hex2dec(char c)
{
    if ('0' <= c && c <= '9')
    {
        return c - '0';
    }
    else if ('a' <= c && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else if ('A' <= c && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else
    {
        return -1;
    }
}


char dec2hex(short int c)
{
    if (0 <= c && c <= 9)
    {
        return c + '0';
    }
    else if (10 <= c && c <= 15)
    {
        return c + 'A' - 10;
    }
    else
    {
        return -1;
    }
}


void urldecode(char url[], char *result)
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;
    char res[BURSIZE];
    for (i = 0; i < len; ++i)
    {
        char c = url[i];
        if (c != '%')
        {
            res[res_len++] = c;
        }
        else
        {
            char c1 = url[++i];
            char c0 = url[++i];
            int num = 0;
            num = hex2dec(c1) * 16 + hex2dec(c0);
            res[res_len++] = num;
        }
    }
    res[res_len] = '\0';
    strcpy(result, res);
}


void urlencode(char url[],char *result)
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;
    char res[BURSIZE];
    for (i = 0; i < len; ++i)
    {
        char c = url[i];
        if (('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '/' || c == '.')
        {
            res[res_len++] = c;
        }
        else
        {
            int j = (short int)c;
            if (j < 0)
                j += 256;
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1 * 16;
            res[res_len++] = '%';
            res[res_len++] = dec2hex(i1);
            res[res_len++] = dec2hex(i0);
        }
    }
    res[res_len] = '\0';
    strcpy(result, res);
}


int get_mtd_idx_by_name(const char *mtd_name)
{
	FILE *fp;
	char line[128], bnm[64], *p;
	int idx, ret_idx = -1;

	if (!(fp = fopen("/proc/mtd", "r")))
		return -1;

	fgets(line, sizeof(line), fp); //skip the 1st line
	while (fgets(line, sizeof(line), fp))
	{
		if (sscanf(line, "mtd%d: %*s %*s \"%63s\"", &idx, bnm) != 2)
			continue;
		
		/* strip tailed " character, if present. */
		if ((p = strchr(bnm, '"')) != NULL)
			*p = '\0';
		
		if (strcmp(mtd_name, bnm) == 0)
		{
			ret_idx = idx;
			break;
		}
	}

	fclose(fp);

	return ret_idx;
}


int get_flash_total_size()
{
	FILE *fp;
	char line[128] = { 0 };
	int flash_size = 0;

	if (!(fp = fopen("/proc/mtd", "r")))
	{
		return 16;
	}

	while (fgets(line, sizeof(line), fp))
	{
		if(strstr(line, "flash size") == NULL)
		{
			continue;
		}

		sscanf(line, "flash size: %d %*s", &flash_size);

		break;
	}

	fclose(fp);

	if(flash_size == 0)
	{
		flash_size = 16;
	}

	return flash_size;
}


int get_flash_actual_size(void)
{
	int n;
	FILE *fp;
	char *ptr = NULL;
	int flash_size = 0;
	char buf[256] = { 0 };

	fp = fopen("/proc/mtd", "r");

	if (fp == NULL)
	{
		dbg("%s open error.\n", __func__);

		return -1;
	}

	while (fgets(buf, 256, fp))
	{
		if (buf[0] == 'm' && buf[1] == 't' && buf[2] == 'd')
		{
			/* mtd file include: dev size erasesize name */
			ptr = strtok(buf, ": ");

			/* get size */
			ptr = strtok(NULL, " ");

			if (ptr != NULL)
			{
				n = (int)strtol(ptr, (char **)NULL, 16);
				flash_size += n;
			}
		}
	}

	/* bytes to mbytes */
	flash_size = flash_size / (1024 * 1024);

	fclose(fp);

	return flash_size;
}


int get_apcli_connected(int wl_idx)
{
	char cmd[256] = {0},result[128] ={0};

	sprintf(cmd,"iwinfo %s info | grep Mode:",WL_IF[wl_idx].ifname);
	get_cmd_result(cmd, result,sizeof(result));

	if(strstr(result,"Client")!=NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int get_apcli_connect_ssid(int wl_idx,char *ssid,int len)
{
	char cmd[256] = {0},result[64] ={0};

	sprintf(cmd,"iw %s info | grep ssid | cut -d ' ' -f2",WL_IF[wl_idx].ifname);
	get_cmd_result(cmd, result,sizeof(result));

	strncpy(ssid,result,len);

	return 0;
}

int get_apcli_connect_bssid(const char *ifname,char *bssid,int len)
{
	char cmd[256] = {0},result[64] ={0};
	sprintf(cmd,"iwconfig %s |grep Point|cut -d ' ' -f17",ifname);
	get_cmd_result(cmd, result,sizeof(result));
	strncpy(bssid,result,len);
	return 0;
}


int get_apcli_idx(void)
{
	int ret=0;
	char tmp_buf[24];

	wificonf_get_by_key(WLAN_APCLI,"disabled", tmp_buf, sizeof(tmp_buf));
	if(1 == atoi(tmp_buf)){
		wificonf_get_by_key(WLAN_APCLI,"device", tmp_buf, sizeof(tmp_buf));
		if(strcmp(tmp_buf,WL_IF[W58G_RADIO].section_name )==0){
			ret=1;
		}
	}else{
		ret=2;
	}

	return ret;
}

int get_apcli_enable(int wl_idx)
{
	int ret=0;
	char tmp_buf[24];

	wificonf_get_by_key(WLAN_APCLI,"disabled", tmp_buf, sizeof(tmp_buf));
	if(1 == atoi(tmp_buf)){
		return 0;
	}else{
		wificonf_get_by_key(WLAN_APCLI,"device", tmp_buf, sizeof(tmp_buf));
		if(strcmp(tmp_buf,WL_IF[W58G_RADIO].section_name )==0 && wl_idx == 1){
			ret=1;
		}else if(strcmp(tmp_buf,WL_IF[W24G_RADIO].section_name )==0 && wl_idx == 0){
			ret=1;
		}
	}
	
	return ret;
}

int get_channel(int wl_idx, char *channel)
{
	char cmd[256] = {0},result[64] ={0};
	sprintf(cmd,"iwlist %s channel | grep \"Current Channel\" | cut -d ':' -f2",WL_IF[wl_idx].ifname);
	get_cmd_result(cmd, result,sizeof(result));

	sprintf(channel,"%d", atoi(result));

	return 0;
}

int get_wlan_merge(int wl_idx, int wl_odx)
{
	char ssid_i[64],ssid_o[64];
	char key_i[128],key_o[128];
	char encryption_i[24],encryption_o[24];
	char hidden_i[8], hidden_o[8];

	wificonf_get_by_key(wl_idx,  "ssid",   ssid_i, sizeof(ssid_i));
	wificonf_get_by_key(wl_odx,  "ssid",   ssid_o, sizeof(ssid_o));

	wificonf_get_by_key(wl_idx,  "key",   key_i, sizeof(key_i));
	wificonf_get_by_key(wl_odx,  "key",   key_o, sizeof(key_o));

	wificonf_get_by_key(wl_idx,  "encryption",   encryption_i, sizeof(encryption_i));
	wificonf_get_by_key(wl_odx,  "encryption",   encryption_o, sizeof(encryption_o));

	wificonf_get_by_key(wl_idx,  "hidden",   hidden_i, sizeof(hidden_i));
	wificonf_get_by_key(wl_odx,  "hidden",   hidden_o, sizeof(hidden_o));

	if(strcmp(ssid_i,ssid_o)==0 && strcmp(key_i,key_o)==0 \
		&& strcmp(encryption_i,encryption_o)==0 \
		&& strcmp(hidden_i, hidden_o)==0 \
		&&is_ssid_disabled(wl_idx)==is_ssid_disabled(wl_odx))
	{
		return 1;
	}

	return 0;
}

int get_encryption_ui(int wl_idx, char *encryption_ui, char *encryptype_ui)
{
	char authmode[64], encryption[15];

	wificonf_get_by_key(wl_idx, "authmode", authmode, sizeof(authmode));
	wificonf_get_by_key(wl_idx, "encryption", encryption, sizeof(encryption));

	if(!strcmp(authmode,"OPEN")){
		strcpy(encryption_ui,"0");
	}
	else if(!strcmp(authmode,"WPAPSK"))
	{
		strcpy(encryption_ui,"3");
	}
	else if(!strcmp(authmode,"WPA2PSK"))
	{
		strcpy(encryption_ui,"4");
	}
	else if(!strcmp(authmode,"WPAPSKWPA2PSK"))
	{
		strcpy(encryption_ui,"5");
	}
	else if(!strcmp(authmode,"WPA3PSK"))
	{
		strcpy(encryption_ui,"6");
	}
	else if(!strcmp(authmode,"WPA2PSKWPA3PSK"))
	{
		strcpy(encryption_ui,"7");
	}

	if(!strcmp(encryption,"NONE"))
	{
		strcpy(encryptype_ui,"0");
	}
	else if(!strcmp(encryption,"TKIP"))
	{
		strcpy(encryptype_ui,"3");
	}
	else if(!strcmp(encryption,"AES"))
	{
		strcpy(encryptype_ui,"4");
	}
	else if(!strcmp(encryption,"TKIPAES"))
	{
		strcpy(encryptype_ui,"5");
	}

	return 0;
}

int wificonf_set_disabled(int radio, int disabled)
{
	int wl_radio = W24G_RADIO;
	int wl_idx   = W24G_IF;

	if(radio!=W24G_RADIO){
		wl_radio = W58G_RADIO;
		wl_idx   = W58G_IF;
	}

	if(disabled == 1) {
		wificonf_set_by_key(wl_radio, "disabled", "1");
		wificonf_set_by_key(wl_idx,	  "disabled", "1");
	} else {
		wificonf_set_by_key(wl_radio, "disabled", "0");
		wificonf_set_by_key(wl_idx,	  "disabled", "0");
	}

	return 0 ;
}

void str_del_char_bak(char *a,char c)
{
	int i,j;
	for(i=0,j=0; *(a+i)!='\0'; i++)
	{
		if(*(a+i)==c)
			continue;
		else
		{
		 	*(a+j)=*(a+i);
			j++;
		}
	}
   *(a+j)='\0';
   
}

void add_mac_split(const char *mac_org, char *mac_new)
{
	int i, j;
	mac_new[0] = 0;
	if (strlen(mac_org) == 12)
	{
		for (i = 0, j = 0; i < 12; i++)
		{
			if (i != 0 && (i%2) == 0)
				mac_new[j++] = ':';
			mac_new[j++] = mac_org[i];
		}
		mac_new[j] = 0;	// oleg patch
	}

	if (strcasecmp(mac_new, "FF:FF:FF:FF:FF:FF") == 0 || strcmp(mac_new, "00:00:00:00:00:00") == 0)
		mac_new[0] = 0;

	return ;
}

void str_tolower(char *str)
{
	while(*str != '\0'){
		*str = tolower(*str);
		str++;
	}
}

void str_toupper(char *str)
{
	while(*str != '\0'){
		*str = toupper(*str);
		str++;
	}
}

void str_escape(char *src,char *dec)
{

	char *ptr,tmp[8] = {0};
	ptr = src;
	
	if(!src || !dec)
		return;
	
	while(*ptr)
	{
		memset(tmp,0,sizeof(tmp));
		if(*ptr != '`' && *ptr != '"')
		{
			sprintf(tmp,"%c",*ptr);
		}
		else
		{
			sprintf(tmp,"\\%c",*ptr);
		}
		strcat(dec,tmp);
		ptr++;
	}
}


int datconf_set_by_key(char *path,char *key,char *value)
{
	struct kvc_context *ctx;
	int ret;
	char cmd[256]={0},value_esc[512] = {0};

	ctx = kvc_load_opt(path, DATCONF_LF_FLAGS, dat_nostrip_list);
	if (!ctx)
	{
		return 0;
	}
	str_escape(value,value_esc);

	ret = kvc_set(ctx, key, value_esc);

	if (ret) goto out;
	
	kvc_commit(ctx);
	
out:
	kvc_unload(ctx);
	return 0;
}


int datconf_get_by_key(char *path,char *key,char *value,int len)
{
	char *value_get;
	struct kvc_context *ctx;

	ctx = kvc_load_opt(path, DATCONF_LF_FLAGS, dat_nostrip_list);

	if (!ctx) return 0;

	value_get = kvc_get(ctx, key);

	if (!value_get) goto out;

	snprintf(value, len, "%s", value_get);

out:
	kvc_unload(ctx);

	return 0;
}

int wificonf_set_by_key(int idx, char *key, char *value)
{
	struct uci_context *uci_ctx = NULL;
	struct uci_package *pkg = NULL;
	struct uci_ptr uci_ptr;

	char uci_path[256];
	int ret;

	memset(uci_path,0,sizeof(uci_path));

	if(idx>=WLAN_MAX){
		return -1;
	}

	uci_ctx = uci_alloc_context();

	if(UCI_OK != uci_load(uci_ctx, "/etc/config/wireless", &pkg))
	{
		uci_free_context(uci_ctx);
		return -1;
	}

	snprintf(uci_path, sizeof(uci_path)-1,"wireless.%s.%s", WL_IF[idx].section_name, key);

	if(UCI_OK == uci_lookup_ptr(uci_ctx, &uci_ptr, uci_path, true))
	{
		uci_ptr.value = value;
		ret=uci_set(uci_ctx, &uci_ptr);
		if (ret == UCI_OK){
			ret = uci_save(uci_ctx, uci_ptr.p);
		}
	}

	uci_unload(uci_ctx, pkg);
	uci_free_context(uci_ctx);
	uci_ctx = NULL;

	return 0;
}

int wificonf_get_by_key(int idx, char *key, char *value, int len)
{
	struct uci_context *uci_ctx = NULL;
	struct uci_package *pkg = NULL;
	struct uci_element *e;

	bool sep = false;

	char *p_value = value;

	char uci_path[256]={0};

	struct uci_ptr uci_ptr;

	memset(value,0,len);
	if(idx>=WLAN_MAX){
		return -1;
	}

	uci_ctx = uci_alloc_context();
	if(UCI_OK != uci_load(uci_ctx, "/etc/config/wireless", &pkg))
	{
		strcpy(value,"");
		uci_free_context(uci_ctx);
		return -1;
	}

	snprintf(uci_path, sizeof(uci_path)-1,"wireless.%s.%s",WL_IF[idx].section_name, key);

  uci_lookup_ptr(uci_ctx, &uci_ptr, uci_path, true);
	if (!(uci_ptr.flags & UCI_LOOKUP_COMPLETE)) {
		goto end_label;
	}

	e = uci_ptr.last;
	switch (e->type) {
		case UCI_TYPE_OPTION:
			if (uci_ptr.o->type == UCI_TYPE_STRING) {
				snprintf(value, len, "%s", uci_ptr.o->v.string);
			} else if (uci_ptr.o->type == UCI_TYPE_LIST) {
				*p_value = '\0';
				uci_foreach_element(&(uci_ptr.o->v.list), e) {
					if ((strlen(value) + strlen(e->name) + 2) < len) {
						p_value += sprintf(p_value, "%s%s", sep?" ":"", e->name);
						sep = true;
					}
				}
			} else {
				goto end_label;
			}
			break;
		default:
			goto end_label;
	}

end_label:
	uci_unload(uci_ctx, pkg);
	uci_free_context(uci_ctx);
	uci_ctx = NULL;

	return 0;
}


int wificonf_add_by_key(int idx, char *key, char *value)
{
	char uci_path[256]={0}, cmd_line[256]={0};

	snprintf(uci_path, sizeof(uci_path)-1,"wireless.%s.%s", WL_IF[idx].section_name, key);

	memset(cmd_line,0,sizeof(cmd_line));
	snprintf(cmd_line, sizeof(cmd_line), "uci add_list %s=\"%s\"", uci_path,value);

	CsteSystem(cmd_line,0);

	return 0;
}

int wificonf_del_by_key(int idx, char *key, char *value)
{
	char uci_path[256], cmd_line[256]={0};

	snprintf(uci_path, sizeof(uci_path)-1,"wireless.%s.%s", WL_IF[idx].section_name, key);

	if(value && strlen(value)>0){
		snprintf(cmd_line,sizeof(cmd_line),"uci -q del_list %s=\"%s\"",uci_path, value);
	}else{
		snprintf(cmd_line,sizeof(cmd_line),"uci -q delete %s", uci_path);
	}
	system(cmd_line);

	return 0;
}

int get_mem_ratio(void)
{
	FILE *fp;
	int memoryUseRate = 0, memtotal = 0, memfree = 0;
	char line_buffer[TEMP_STR_LEN];
	char *pchar = NULL, *token = NULL;

	if((fp=fopen(MEMRATIO_FILE, "r")) == NULL)
	{
		memoryUseRate = 10;
		return memoryUseRate;
	}
	while(fgets(line_buffer, sizeof(line_buffer), fp))
	{
		line_buffer[strlen(line_buffer)-1]='\0';
		if((pchar=strstr(line_buffer, "MemTotal:"))!=NULL)
		{
			pchar+=strlen("MemTotal:");
			token = strtok(pchar, " ");
			memtotal = atoi(token);
		}
		//if((pchar=strstr(line_buffer, "MemFree:"))!=NULL)
		if((pchar=strstr(line_buffer, "MemAvailable:"))!=NULL)
		{
			pchar+=strlen("MemAvailable:");
			token = strtok(pchar, " ");
			memfree = atoi(token);
			break;
		}
	}
	
	memoryUseRate= 100-(memfree*100/memtotal);
	fclose(fp);
	return memoryUseRate;
}


int get_wan_mode(char *proto)
{
	int ret = DHCP_DISABLED;

	if(strcmp(proto, "static") == 0)
	{
		ret = DHCP_DISABLED;
	}
	else if(strcmp(proto, "pppoe") == 0)
	{
		ret = PPPOE;
	}
	else if(strcmp(proto, "pptp") == 0)
	{
		ret = PPTP;
	}
	else if(strcmp(proto, "l2tp") == 0)
	{
		ret = L2TP;
	}
	else
	{
		ret = DHCP_CLIENT;
	}

	return ret;
}


void get_wan_linktime(unsigned long seconds, char *tmp_buf)
{
	unsigned long sec = seconds;
	unsigned long d, h, m;

	d = sec / 86400;
	sec %= 86400;
	h = sec / 3600;
	sec %= 3600;
	m = sec / 60;
	sec %= 60;
	sprintf(tmp_buf, "%ld;%ld;%ld;%ld", d, h, m, sec);
	return ;
}

void get_sys_uptime(char *tmpBuf)
{
	unsigned long sec, mn, hr, day;
	long uptime = 0;
	
	struct sysinfo info;
	
	sysinfo(&info);
	sec = (unsigned long) info.uptime ;
		
	day = sec / 86400;
	//day -= 10957; // day counted from 1970-2000

	sec %= 86400;
	hr = sec / 3600;
	sec %= 3600;
	mn = sec / 60;
	sec %= 60;
	sprintf(tmpBuf, "%ld;%ld;%ld;%ld", day, hr, mn, sec);

	return ;
}

void takeeffect_ntp_zonename()
{
	char zone_name[8]={0}, timezone[8]={0};
	char cmd[128]={0};
	

	Uci_Get_Str(PKG_SYSTEM_CONFIG, "ntp", "timezone", timezone);
	sprintf(cmd, "echo %s > /etc/TZ", timezone);
	system(cmd);
	
	return ;
}


void set_timezone_to_kernel(void)
{
	system("date -k");
}

int getOpmodeVal()
{
	int val=1;
	char tmpBuf[8]={0};
	
	Uci_Get_Str(PKG_SYSTEM_CONFIG,"opmode","opmode_custom",tmpBuf);
	if(strcmp(tmpBuf, "gw") == 0)
		val=1;
	else if(strcmp(tmpBuf, "br") == 0)
		val=0;
	else if(strcmp(tmpBuf, "rpt") == 0)
		val=2;
	else if(strcmp(tmpBuf, "wisp") == 0)
		val=3;
	else
		val=1;
	
	return val;
}

int getInAddr( char *interface, int type, void *pAddr )
{
    struct ifreq ifr;
    int skfd, found=0;
	struct sockaddr_in *addr;
    skfd = socket(AF_INET, SOCK_DGRAM, 0);

    strcpy(ifr.ifr_name, interface);
    if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
	{
    		close( skfd );
		return (0);
	}
    if (type == HW_ADDR_T)
	{
    	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) >= 0)
		{
			memcpy(pAddr, &ifr.ifr_hwaddr, sizeof(struct sockaddr));
			found = 1;
		}
    }
    else if (type == IP_ADDR_T)
	{
		if (ioctl(skfd, SIOCGIFADDR, &ifr) == 0)
		{
			addr = ((struct sockaddr_in *)&ifr.ifr_addr);
			*((struct in_addr *)pAddr) = *((struct in_addr *)&addr->sin_addr);
			found = 1;
		}
    }
    else if (type == NET_MASK_T)
	{
		if (ioctl(skfd, SIOCGIFNETMASK, &ifr) >= 0)
		{
			addr = ((struct sockaddr_in *)&ifr.ifr_addr);
			*((struct in_addr *)pAddr) = *((struct in_addr *)&addr->sin_addr);
			found = 1;
		}
    }
	else 
    {
    	if (ioctl(skfd, SIOCGIFDSTADDR, &ifr) >= 0)
		{
			addr = ((struct sockaddr_in *)&ifr.ifr_addr);
			*((struct in_addr *)pAddr) = *((struct in_addr *)&addr->sin_addr);
			found = 1;
		}
    }
    close( skfd );
    return found;

}

int getCmdResult(char *cmd, char *resultbuf, size_t buf_size)
{
	char *pchar = NULL;
	FILE *fp = popen(cmd, "r");
	if(!fp) {
		return -1;
	}
	fgets(resultbuf, buf_size, fp);
	pclose(fp); 
	if((pchar = strstr(resultbuf, "\n")))
		*pchar = '\0';

	resultbuf[buf_size-1] = '\0';
	return 0;
}

int getCmdStr(const char *cmd, char *strVal, int len)
{
	char *p;
	int ret = 0;

	FILE *fp = popen(cmd, "r");
	if(!fp) 
		return -1;

	if(fgets(strVal, len, fp) != NULL){
		if(p=strstr(strVal, "\n"))
			p[0]='\0';
	}else{
		*strVal = '\0';
		ret = -1;
	}
	pclose(fp);

	return ret;
}

/*
src1="key : val \n"
src2="key : val,"
*/
void get_sub_value(char *val, const char *src, int size, char delimit, char end )
{
	//printf("src=%s. end=%c.\n", src, end);
	char *s1 = val;
	char *s2 = src;

	if ( src == NULL || val == NULL ) goto noval;

	if ( strchr(src, delimit) == NULL )
		goto noval;

	memset(val, 0, size);

	while (*s2 != delimit)
		s2++;

	s2++;

	while (*s2 == ' ')
		s2++;

	while (*s2 != '\0' && *s2 != end) {
		*s1 = *s2;
		s1++;
		s2++;
	}

noval:
	*s1 = '\0';

	//printf("val=%s.\n", val);
	return;
}


int getNthValueSafe(int index, char *value, char delimit, char *result, int len)
{
    int i=0, result_len=0;
    char *begin, *end;

    if(!value || !result || !len)
        return -1;

    begin = value;
    end = strchr(begin, delimit);

    while(i<index && end){
        begin = end+1;
        end = strchr(begin, delimit);
        i++;
    }

    //no delimit
    if(!end){
		if(i == index){
			end = begin + strlen(begin);
			result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);
		}else
			return -1;
	}else
		result_len = (len-1) < (end-begin)? (len-1) : (end-begin);

	memcpy(result, begin, result_len );
	*(result+ result_len ) = '\0';

	return 0;
}

int getIfMac(char *ifname, char *if_hw)
{
	struct ifreq ifr;
	char *ptr;
	int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		//error(E_L, E_LOG, T("getIfMac: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		//error(E_L, E_LOG, T("getIfMac: ioctl SIOCGIFHWADDR error for %s"), ifname);
		return -1;
	}

	ptr = (char *)&ifr.ifr_addr.sa_data;
	sprintf(if_hw, "%02X:%02X:%02X:%02X:%02X:%02X",
			(ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
			(ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

	close(skfd);
	return 0;
}

int getIfIp(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("lktFirewallConfig getIfIp: open socket error");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}

int getIfIsUp(char *ifname)
{
	struct ifreq ifr;
	int skfd;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd == -1) {
		perror("socket");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		perror("ioctl");
		close(skfd);
		return -1;
	}
	close(skfd);
	if (ifr.ifr_flags & IFF_UP)
		return 1;
	else
		return 0;
}


void getRealGateway(char  *sgw)
{
	char   buff[256];
	int    nl = 0 ;
	struct in_addr dest;
	struct in_addr gw;
	int    flgs, ref, use, metric;
	unsigned long int d, g, m;
	int    find_default_flag = 0;

	FILE *fp = fopen("/proc/net/route", "r");

	while (fgets(buff, sizeof(buff), fp) != NULL) {
		if (nl) {
			int ifl = 0;

			while (buff[ifl] != ' ' && buff[ifl] != '\t' && buff[ifl] != '\0')
				ifl++;

			buff[ifl] = 0;  /* interface */

			if (sscanf(buff + ifl + 1, "%lx%lx%X%d%d%d%lx",
				   &d, &g, &flgs, &ref, &use, &metric, &m) != 7) {
				fclose(fp);
				return ;
			}

			if (flgs & 0x0001) {
				dest.s_addr = d;
				gw.s_addr   = g;
				strcpy(sgw, (gw.s_addr == 0 ? "" : inet_ntoa(gw)));

				if (dest.s_addr == 0) {
					find_default_flag = 1;
					break;
				}
			}
		}

		nl++;
	}

	fclose(fp);

	if (find_default_flag == 1)
		return ;
	else {
		strcpy(sgw, "");
		return ;
	}
}

int getDns(int dnsIdx, char *dns, int is_ipv6)
{
	FILE *fp;
	char buf[80] = {0}, ns_str[11];
	int idx = 0;

	fp = fopen("/etc/resolv.conf", "r");

	if (NULL == fp) {
		return -1;
	}

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (strncmp(buf, "nameserver", 10) != 0 || strstr(buf, "127.0.0.1") != NULL)
			continue;

		if(1 == is_ipv6 && strstr(buf, ":") == NULL)
			continue;

		sscanf(buf, "%s %s", ns_str, dns);
		idx++;

		if (idx == dnsIdx)
			break;
	}

	fclose(fp);

	if (idx != dnsIdx) {
		strcpy(dns, "");
	}

	return 0;
}

int getIfMask(char *ifname, char *if_addr)
{
    struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("lktFirewallConfig getIfIp: open socket error");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}


char *websGetVar(cJSON *object, char *var, char *defaultGetValue)
 {
	 cJSON	 *sp;
 
	if(var == NULL)
	{
		return defaultGetValue;
	}
  
	 if ((sp = cJSON_GetObjectItem(object, var)) != NULL) {
		 if (sp->valuestring) 
		 {
			return sp->valuestring;
		 }
		 else if (sp->type==cJSON_False)
		 {
			return "0";
		 }
		 else if (sp->type==cJSON_True)
		 {
			return "1";
		 }
		 else if (sp->type==cJSON_Number)
		 {
		 	memset(g_strResultTmp, 0x00, sizeof(g_strResultTmp));
			snprintf(g_strResultTmp, sizeof(g_strResultTmp), "%d", sp->valueint);
			return g_strResultTmp;
		 }		 
		 else 
		 {
			return "";
		 }
	 }
	 return defaultGetValue;
 }

 void setLktosEffect(char *action)
 {
	 char cmd[CMD_STR_LEN];
	 snprintf(cmd, CMD_STR_LEN, "lktos_reload %s", action);
	 system(cmd);
	 return;
 }

unsigned long file_size(const char *filepath)
{
	 struct stat stat_buf;

	 if (!stat(filepath, &stat_buf) && S_ISREG(stat_buf.st_mode))
		 return ((unsigned long) stat_buf.st_size);

	 return 0;
}

/**
**Get user's software version.
**@@Such as web ...
**Don't use statistical services.
**@@Such as cloud upgrades, statistical ..
**/

void get_user_software_version(cJSON *root,char *key)
{
	char tmp[128] = {0};
	char soft_ver[64] = {0},commit_num[32] = {0};
	char custom_ver_code[64] = {0},custom_soft_ver[64] = {0};

	Uci_Get_Str(PKG_PRODUCT_CONFIG,"sysinfo","soft_version",soft_ver);
	Uci_Get_Str(PKG_PRODUCT_CONFIG,"sysinfo","svn_num",commit_num);
	Uci_Get_Str(PKG_PRODUCT_CONFIG,"custom","customVerCode",custom_ver_code);
	Uci_Get_Str(PKG_PRODUCT_CONFIG,"custom","customSoftVersion",custom_soft_ver);
	if(strlen(custom_ver_code) > 0) {
		strcat(tmp, soft_ver);
		strcat(tmp, ".");
		strcat(tmp, custom_ver_code);
		cJSON_AddStringToObject(root, key, tmp);
	}
	else if(strlen(custom_soft_ver) > 0) {
		cJSON_AddStringToObject(root, key, custom_soft_ver);
	}
	else {
		snprintf(tmp, sizeof(tmp), "%s.%s", soft_ver, commit_num);
		cJSON_AddStringToObject(root, key, tmp);
	}
}


int get_soft_version(char *soft_version, int len)
{
	char commit_num[16], pre_verion[24];
	
	memset(commit_num, 0, sizeof(commit_num));
	memset(pre_verion, 0, sizeof(pre_verion));

	Uci_Get_Str(PKG_PRODUCT_CONFIG, "sysinfo", "svn_num", commit_num);
	Uci_Get_Str(PKG_PRODUCT_CONFIG, "sysinfo", "soft_version", pre_verion);

	snprintf(soft_version, len, "%s.%s", pre_verion, commit_num);

	return 0;
}


void del_char(char str[], char target)
{
	int i, j;

	for(i = j = 0; str[i] != '\0'; i++)
    {
		if(str[i] != target)
		{
			str[j++] = str[i];
		}
    }

   str[j] = '\0';
}

int check_console_getty(void)
{
	char buff[128];
	int ret = 0;
	
	FILE *fp = fopen("/etc/inittab", "r");

	if(!fp)
		return ret;
	
	while (fgets(buff, sizeof(buff), fp) != NULL)
	{
		if(!strstr(buff,"askconsole"))
			continue;
		if(buff[0] == '#')
			ret = 1;
	}
	fclose(fp);

	return ret;
}


void data_to_hex(unsigned char *src, int len, char *dst)
{
	int i=0;
    int index=0;
    char buff[1024]={0};

    for(i=0;i<len;i++){
			if(i % 2 == 0)
	            index+= sprintf(buff + index, "%02x", src[i+1]);
			else 
				index+= sprintf(buff + index, "%02x", src[i-1]);
    }

    if(strlen(buff) > 0)
            strcpy(dst, buff);
}

void string_to_unicode(char *src, unsigned *dst)
{
	wchar_t wstr[256]={0};
	char buff[1024]={0};
    int index=0, i=0, max_len=60, count=0, print_len=0,offset=0;

    setlocale(LC_ALL, "");
    mbstowcs(wstr, src, 256);
    for(i=0; i < wcslen(wstr); i++){
        memcpy(buff + index, &wstr[i], 2);
        index += 2;
    }

	if(index <= max_len){
		data_to_hex(buff, index, dst);
	}else{
		while(index > 0){
            count++;
            if(index > max_len)
                    print_len = max_len;
            else
                    print_len = index;

            data_to_hex(buff+offset, print_len, dst);
            offset += print_len;
            index -= print_len;
        }
	}
	
}

int config_sms_temp()
{
	int sms_reboot=0, i=0, count=0;
	char cmd[64], sms_content[32]={0}, unicode[128]={0};
	char param[8]={0}, buf[32]={0};

	if(!f_exists(TEMP_SMS_FILE)){
		snprintf(cmd,sizeof(cmd),"touch %s",TEMP_SMS_FILE);
		CsteSystem(cmd, 0);
	}
	
	Uci_Get_Int(PKG_WAN_MODEM_CONFIG, "smsInfo", "rebootEnable", &sms_reboot);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d", sms_reboot);
	datconf_set_by_key(TEMP_SMS_FILE,"enable", buf);
	if(sms_reboot){
		Uci_Get_Str(PKG_WAN_MODEM_CONFIG, "smsInfo", "rebootText", sms_content);
		string_to_unicode(sms_content, unicode);
		datconf_set_by_key(TEMP_SMS_FILE,"sms_content", unicode);

		datconf_set_by_key(TEMP_SMS_FILE, "phone_num", "0");
		
		for(i=0; i<6; i++){
			sprintf(param, "phone%d", i);
			memset(buf, 0, sizeof(buf));
			Uci_Get_Str(PKG_WAN_MODEM_CONFIG, "smsInfo", param, buf);
			if(strcmp(buf, "") != 0){
				memset(param, 0, sizeof(param));
				sprintf(param, "phone%d", count);
				datconf_set_by_key(TEMP_SMS_FILE, param, buf);
				count++;
			}
		}
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%d", count);
		datconf_set_by_key(TEMP_SMS_FILE, "phone_num", buf);
	}
	
	return 0;
}

