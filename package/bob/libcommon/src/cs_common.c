#include "cs_common.h"
#include "oid.h"
#include <sys/mman.h>


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
 * Concatenates NULL-terminated list of arguments into a single
 * commmand and executes it
 * @param	argv	argument list
 * @param	path	NULL, ">output", or ">>output"
 * @param	timeout	seconds to wait before timing out or 0 for no timeout
 * @param	ppid	NULL to wait for child termination or pointer to pid
 * @return	return value of executed command or errno
 */
int _eval(char *const argv[], char *path, int timeout, int *ppid)
{
	pid_t pid;
	int status;
	int fd;
	int flags;
	int sig;

	switch (pid = fork()) {
	case -1:	/* error */
		perror("fork");
		return errno;
	case 0:		/* child */
		/* Reset signal handlers set for parent process */
		for (sig = 0; sig < (_NSIG-1); sig++)
			signal(sig, SIG_DFL);

		/* Clean up */
		ioctl(0, TIOCNOTTY, 0);
		setsid();

		/* Redirect stdout to <path> */
		if (path) {
			flags = O_WRONLY | O_CREAT;
			if (!strncmp(path, ">>", 2)) {
				/* append to <path> */
				flags |= O_APPEND;
				path += 2;
			} else if (!strncmp(path, ">", 1)) {
				/* overwrite <path> */
				flags |= O_TRUNC;
				path += 1;
			}
			if ((fd = open(path, flags, 0644)) < 0)
				perror(path);
			else {
				dup2(fd, STDOUT_FILENO);
				close(fd);
			}
		}
		
		/* execute command */
		printf("%s\n", argv[0]);
		setenv("PATH", "/sbin:/bin:/usr/sbin:/usr/bin", 1);
		alarm(timeout);
		execvp(argv[0], argv);
		perror(argv[0]);
		exit(errno);
	default:	/* parent */
		*ppid = pid;
		if (waitpid(pid, &status, WNOHANG) == -1) {
			if (errno == ECHILD){
				printf("%s %d: waitpid error, the errno is ECHILD\n", __func__, __LINE__);
				return 0;
			}
			perror("waitpid");
			return errno;
		}
		if (WIFEXITED(status)){
			printf("%s %d: child process status is %d\n", __func__, __LINE__, WEXITSTATUS(status));
			return WEXITSTATUS(status);
		}
		else{
			printf("%s %d: child process create success\n", __func__, __LINE__);
			return status;
		}
	}
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

	if ((dir = opendir("/proc")) == NULL) {
		perror("Cannot open /proc");
		return -1;
	}

	while ((next = readdir(dir)) != NULL) {
		FILE *fp;
		char filename[256];
		char buffer[256];

		/* If it isn't a number, we don't want it */
		if (!isdigit(*next->d_name))
			continue;

		sprintf(filename, "/proc/%s/cmdline", next->d_name);
		fp = fopen(filename, "r");
		if (!fp) {
			continue;
		}
		buffer[0] = '\0';
		fgets(buffer, 256, fp);
		fclose(fp);

		if (!strcmp(name, buffer)) {
			pid = strtol(next->d_name, NULL, 0);
			break;
		}
	}
	closedir(dir);
	return pid;
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


/*=========================================================================*/
/*  函数名称: isMacValid                                                   */
/*  函数功能: 判断MAC的合法性                                              */
/*  输  入  : char * MAC                                                   */
/*  输  出  : int   0:非法，1：合法                                        */
/*  创  建  : CaryStudio / 2014-8-26                                       */
/*=========================================================================*/
int isMacValid(char *str)
{
	int i, len = strlen(str);
	if(len != 17)
		return 0;

	for(i=0; i<5; i++){
		if( (!isxdigit( str[i*3])) || (!isxdigit( str[i*3+1])) || (str[i*3+2] != ':') )
			return 0;
	}
	return (isxdigit(str[15]) && isxdigit(str[16])) ? 1: 0;
}

/*=========================================================================*/
/*  函数名称: isIpValid																	  */
/*  函数功能:验证是否是IP地址										   */
/*  输  入  : IP地址														  */
/*  输  出  : 是IP地址返回1，否则返回0 									*/
/*  创  建  : CaryStudio / 2014-9-19																*/
/*=========================================================================*/
int isIpValid(char *str)
{
  struct in_addr addr;	 
  int len = strlen(str);
  if(len < 7)
	  return 0;
  if( (! strcmp(T("any"), str)) || (! strcmp(T("any/0"), str)))
	  return 1;

  if(! (inet_aton(str, &addr))){
	  printf("isIpValid(): %s is not a valid IP address.\n", str);
	  return 0;
  }
  return 1;
}

int isOnlyOneSlash(char *str)
{
	int i, count=0;
	int len = strlen(str);
	for(i=0; i<len; i++)
		if( str[i] == '/')
			count++;
	return count <= 1 ? 1 : 0;
}

int isNumOnly(char *str)
{
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if((str[i] >= '0' && str[i] <= '9'))
			continue;
		return 0;
	}
	return 1;
}

int isAllNumAndSlash(char *str)
{
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if( (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '/' )
			continue;
		return 0;
	}
	return 1;
}

int isIpNetmaskValid(char *s)
{
	char str[RESULT_STR_LEN];
	char *slash;
	struct in_addr addr;    // for examination

	if(!s || !strlen(s)){
		return 0;
	}

	strncpy(str, s, sizeof(str));

    	if( (!strcmp("any", str)) || (!strcmp("any/0", str)))
        	return 1;

	if (!isAllNumAndSlash(str)){
		return 0;
	}

	if(!isOnlyOneSlash(str)){
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

		if(!isNumOnly(slash)){
			return 0;
		}

		mask = atoi(slash);
		if(mask < 0 || mask > 32){
			return 0;
		}
	}

	if(! (inet_aton(str, &addr))){
        	printf("isIpNetmaskValid(): %s is not a valid IP address.\n", str);
        	return 0;
    	}
    	return 1;
}

/*=========================================================================*/
/*  函数名称: getNums                                                      */
/*  函数功能: 获取元素个数                                                 */
/*  输  入  : char * 元素字符串                                            */
/*            char   分隔符                                                */
/*  输  出  : int    元素个数                                              */
/*  创  建  : CaryStudio / 2014-8-26                                       */
/*=========================================================================*/
int getNums(char *value, char delimit)
{
    char *pos = value;
    int count=1;
    if(!pos)
        return 0;
    while( (pos = strchr(pos, delimit))){
        pos = pos+1;
        count++;
    }
    return count;
}

int getRuleNums(char *rules)
{
	return getNums(rules, ';');
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

long get_current_uptime_sec(void)
{
    struct sysinfo info;
    sysinfo(&info);
    return info.uptime;
}


void setLktosEffect(char *action)
{
	char cmd[CMD_STR_LEN];
	snprintf(cmd, CMD_STR_LEN, "lktos_reload %s", action);
	system(cmd);
	return;
}

/*
* Bootloader & Factory & Kernel is locked, from linux/driver/mtd/ralink/x
*/
int lock_cs_mtd(void)
{
	system("echo 0 > /proc/MtdEnable/Enable");
	return 0;
}

int unlock_cs_mtd(void)
{
	system("echo 1 > /proc/MtdEnable/Enable");
	return 0;
}

int lock_file(const char * lock_file)
{
	if( access(lock_file, R_OK) == 0 )
	{
		//printf("%s had locked .....\n", lock_file);
		return 0;
	}
	else 
	{
		creat(lock_file, S_IREAD);
		return 1;
	}
}

int unlock_file(const char * lock_file)
{
	unlink(lock_file);
	//printf("%s had remove .....\n", lock_file);
	return 1;
}

int is_phyconnected(int portNum)
{
	FILE *fp=NULL;
	int portLink=0;
	char *p=NULL, *q=NULL;
	char valBuf[TEMP_STR_LEN]={0}, tmpBuf[TEMP_STR_LEN]={0};

	sprintf(tmpBuf, "mii_mgr -g -p %d -r 1", portNum);
	//CsteSystem(tmpBuf, CSTE_PRINT_CMD);		
	
	fp = popen (tmpBuf, "r" );
	if (!fp) 
		return 0;
	if(fgets(valBuf, 64, fp)) 
	{
#if defined(CONFIG_RALINK_SOC_MT7621)
		p = strstr(valBuf, "79");
#else
		p = strstr(valBuf, "78");
#endif
		if(p!=NULL) q = strstr(p, "49");
		
		if(p==NULL) portLink=0;
		if(q!=NULL) portLink=0;
		else portLink=1;
	}			
	pclose(fp);

	//unlink("/tmp/phyLink");
	
	return portLink;
}

int tcpcheck_net(const char *host, int port, int timeout)
{
	FILE *f = NULL;
	char s[TEMP_STR_LEN] = { 0 }; 
	char cmd[CMD_STR_LEN] = { 0 }; 
	int ok = 0;

	if ((NULL == host) || (strlen(host)<7))/* 地址合法性检测 */
		return ok;
	
	sprintf(cmd, "tcpcheck %d %s:%d > %s", timeout, host, port, TCP_TMPFILE);
	system(cmd);
	sleep(1);
	if ((f = fopen(TCP_TMPFILE, "r")) != NULL) {
		if ( NULL != fgets(s, sizeof(s), f)) {
			if (strstr(s, "alive") != NULL) {
				ok = 1;
			}
			else if (strstr(s, "timed out") != NULL) {
				ok = 0;
			}
			else {
				ok = 0;
			}
		}
		fclose(f);
	}
	unlink(TCP_TMPFILE);
	
	return ok;
}

int do_ping_detect(const char *host, char *ifname)
{
	char cmd[CMD_STR_LEN] = {0};

	if ((NULL == host) || (strlen(host)<7))/* 地址合法性检测 */
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

int getCmdVal(const char *cmd)
{
    char buf[TEMP_STR_LEN]= {0};
    char *p;

    FILE *fp = popen(cmd, "r");
    if(!fp) return 0;

    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        if((p=strstr(buf, "\n")))
            p[0]='\0';
    }
    pclose(fp);

    return atoi(buf);
}

char* getLanIfName(void)
{
    static char if_name[RESULT_STR_LEN] = LAN_DEV_NAME;

    return if_name;
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


int getVpnServerIp(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("lktFirewallConfig getIfIp: open socket error");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFDSTADDR, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
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

/*=========================================================================*/
/*  函数名称: getIfIsUp                                                  */
/*  函数功能:  判断指定的接口是否是up  或者down                         			              */
/*  输  入  : 接口名称                         														*/
/*  输  出  :接口UP 返回1，接口down 返回0                                                                        */
/*  创  建  : CaryStudio / 2014-9-17                                     */
/*=========================================================================*/
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

unsigned int getFlashSizeCs(void)
{
	unsigned int fsize = 0;

	char buf[TEMP_STR_LEN]={0},tmp[TEMP_STR_LEN];
	char *p;
	FILE *fp = fopen("/proc/mtd", "r");
	if(!fp) 
		return 0;
	
	while(fgets(buf, sizeof(buf), fp) != NULL){
		if( strstr(buf, "ALL") != NULL )
		{
			p=strstr(buf,"mtd0:");
			snprintf(tmp,4,"%s",(p+6));
			sscanf(tmp,"%x",&fsize);
			break;
		}
	}
	fclose(fp);
	
	return fsize;
}

int domain_to_ip(unsigned char *domain, unsigned char *ip)
{
	unsigned char str[32];
	struct hostent *hptr;
	int count=0;
	
	memset(ip, 0, 32);
	if ( isalpha(domain[0]) )
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

void STATUS_WAN_LINK_GET(int idx, int *value)
{
#if defined(CONFIG_USER_4G)
	int Wanmetric;

	Uci_Get_Int_By_Idx(PKG_NETWORK_CONFIG,"wan", idx, "metric",&Wanmetric);
	if (Wanmetric == 3)
	{
		Uci_Get_Int(PKG_NETWORK_STATUS, "sys", "wan4g_link",value);
	}
	else
#endif
	{
		STATUS_WAN_LINK_UCI_GET(idx, value);
	}
}

void STATUS_WAN_IP_ADDRESS_GET(int idx, char *value)
{
#if defined(CONFIG_USER_4G)
	int Wanmetric;

	Uci_Get_Int_By_Idx(PKG_NETWORK_CONFIG,"wan", idx, "metric",&Wanmetric);
	if (Wanmetric == 3)
	{
		Uci_Get_Str(PKG_NETWORK_STATUS, "sys", "wan4g_ip_address",value);
	}
	else
#endif
	{
		STATUS_WAN_IP_ADDRESS_UCI_GET(idx, value);
	}
}

void STATUS_SUBNET_MASK_GET(int idx, char *value)
{
#if defined(CONFIG_USER_4G)
	int Wanmetric;

	Uci_Get_Int_By_Idx(PKG_NETWORK_CONFIG,"wan", idx, "metric",&Wanmetric);
	if (Wanmetric == 3)
	{
		Uci_Get_Str(PKG_NETWORK_STATUS, "sys", "wan4g_subnet_mask",value);
	}
	else
#endif
	{
		STATUS_SUBNET_MASK_UCI_GET(idx, value);
	}
}

void STATUS_WAN_DEFAULT_GW_GET(int idx, char *value)
{
#if defined(CONFIG_USER_4G)
	int Wanmetric;

	Uci_Get_Int_By_Idx(PKG_NETWORK_CONFIG,"wan", idx, "metric",&Wanmetric);
	if (Wanmetric == 3)
	{
		Uci_Get_Str(PKG_NETWORK_STATUS, "sys", "wan4g_default_gw",value);
	}
	else
#endif
	{
		STATUS_WAN_DEFAULT_GW_UCI_GET(idx, value);
	}
}

void STATUS_WAN_DNS_SERVER_GET(int idx, char *value)
{
#if defined(CONFIG_USER_4G)
	int Wanmetric;

	Uci_Get_Int_By_Idx(PKG_NETWORK_CONFIG,"wan", idx, "metric",&Wanmetric);
	if (Wanmetric == 3)
	{
		Uci_Get_Str(PKG_NETWORK_STATUS, "sys", "wan4g_dns_server",value);
	}
	else
#endif
	{
		STATUS_WAN_DNS_SERVER_UCI_GET(idx, value);
	}
}

void STATUS_WAN_IF_NAME_GET(int idx, char *value)
{
#if defined(CONFIG_USER_4G)
	int Wanmetric;

	Uci_Get_Int_By_Idx(PKG_NETWORK_CONFIG,"wan", idx, "metric",&Wanmetric);
	if (Wanmetric == 3)
	{
		Uci_Get_Str(PKG_NETWORK_STATUS, "sys", "wan4g_if_name",value);
	}
	else
#endif
	{
		STATUS_WAN_IF_NAME_UCI_GET(idx, value);
	}
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
		else {
			sprintf(tmp,"\\%c",*ptr);
		}
		strcat(dec,tmp);
		ptr++;
	}
}

int wificonf_set_by_key(char *path,char *key,char *value)
{
	struct kvc_context *ctx;
	int ret;
	char cmd[256]={0},value_esc[512] = {0};

	ctx = kvc_load_opt(path, DATCONF_LF_FLAGS, dat_nostrip_list);
	if (!ctx) {
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


int wificonf_get_by_key(char *path,char *key,char *value,int len)
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

int wificonf_get_by_ifname_key(char *ifname,char *key,char *value,int len)
{
	char *value_get, *idxval;
	struct kvc_context *ctx;
	int findex = -1;
	char cmd[256]={0},path[128]={0};

	if(strstr(ifname,"rax")){//5G is rax0,rax1...
		strcpy(path,W58G_PATH);
	}else{
		strcpy(path,W24G_PATH);
	}

	char strCmdBuf[CMD_STR_LEN] = { 0 };

	findex = atoi(ifname + (strlen(ifname)-1));
	ctx = kvc_load_opt(path, DATCONF_LF_FLAGS, dat_nostrip_list);

	if (!ctx) return 0;

	value_get = kvc_get(ctx, key);

	if (!value_get) goto out;

 	if(!strstr(value_get,";")){
		findex = -1;
	}

	if (findex < 0) {
		snprintf(value, len, "%s", value_get);
	} else {
		idxval = dat_get_indexed_value(value_get, findex);
		if (idxval) {
			snprintf(value, len, "%s", idxval);
			dat_free_value(idxval);
		}
	}

out:
	kvc_unload(ctx);

	return 0;
}

int wificonf_set_by_ifname_key(char *ifname,char *key,char *value)
{
	struct kvc_context *ctx;
	char *oldval, *newval;
	int ret,findex = -1;
	char cmd[256]={0},path[128]={0};

	findex = atoi(ifname + (strlen(ifname)-1));

	if(strstr(ifname,"andlink")){
		strcpy(path,ANDLINK_PATH);
	}else if(strstr(ifname,"rax")){//5G is rax0,rax1...
		strcpy(path,W58G_PATH);
	}else{
		strcpy(path,W24G_PATH);
	}

	ctx = kvc_load_opt(path, DATCONF_LF_FLAGS, dat_nostrip_list);
	if (!ctx) {
		return 0;
	}

	newval = value;

	oldval = kvc_get(ctx, key);
	
	if (!oldval){
		oldval = "";
		findex = -1;

	}else if(!strstr(oldval,";")){
		findex = -1;
	}
	
	if (findex >= 0) {
		oldval = kvc_get(ctx, key);
		if (!oldval)
			oldval = "";
		
		newval = dat_set_indexed_value(oldval, findex, value);
		if (!newval) {
			goto out;
		}

	}

	ret = kvc_set(ctx, key, newval);

	if (findex >= 0)
		dat_free_value(newval);

	if (ret) 
		goto out;
	
	kvc_commit(ctx);
	
out:
	kvc_unload(ctx);
	return 0;
}

int get_interface_status(struct interface_status *status_paremeter,char *interface)
{
	int ret,i = 0,array_len = 0,mask_i = 0,mask_t = 0;
	char path[128] = {0},p_json[1024*4] = {0};
	char mask[32] = {0};
	char *ptr=NULL, *target = NULL, *address=NULL,/* *mask=NULL, */*dns[2]={NULL,NULL};
	struct in_addr addr;

	cJSON *root,*root_obj = NULL,*ipv4_obj = NULL,*route_obj = NULL,*dns_obj = NULL,*tmp_obj = NULL;

	snprintf(path,sizeof(path)-1,"network.interface.%s",interface);
	ret = cs_ubus_cli_call(path, "status",p_json);
	
	if(!ret){
		root_obj = cJSON_Parse(p_json);
		if(root_obj)
		{
			char *up = websGetVar(root_obj, T("up"), T("0"));
			char *uptime = websGetVar(root_obj, T("uptime"), T(""));
			char *proto = websGetVar(root_obj, T("proto"), T("0"));
			char *device =  websGetVar(root_obj, T("device"), T("0"));
			
			status_paremeter->up = atoi(up);
			status_paremeter->uptime = atoi(uptime);
			
			strcpy(status_paremeter->proto,proto);
			strcpy(status_paremeter->device,device);
		
			ipv4_obj = cJSON_GetObjectItem(root_obj, "ipv4-address");
			route_obj = cJSON_GetObjectItem(root_obj, "route");
			dns_obj = cJSON_GetObjectItem(root_obj, "dns-server");
			
			if(ipv4_obj)
			{
				array_len=cJSON_GetArraySize(ipv4_obj);
				if(array_len>0) {
					tmp_obj = cJSON_GetArrayItem(ipv4_obj,0);
					address = websGetVar(tmp_obj, "address", T("0.0.0.0"));
					strcpy(status_paremeter->ipaddr_v4,address);
					mask_i = atoi(websGetVar(tmp_obj, "mask", T("0")));
					unsigned int addr;
					
					addr = LMOVE(0xffffffff,32-mask_i);
					sprintf(mask,"%u.%u.%u.%u",RMOVE(addr,24),
						RMOVE(addr,16)&0xff,RMOVE(addr,8)&0xff,addr&0xff);
#if 0
					mask_i = 32-mask_i;
					for(i= 31;i>=0;i--){
						if(mask_i>0)
							mask_t |= (0<<i);
						else
							mask_t |= (1<<i);
						mask_i--;
					}
					memcpy(&addr, &mask_t, 4);
					mask = inet_ntoa(addr);		
#endif
				if(mask) 
				{
					strcpy(status_paremeter->mask_v4,mask);
				}
				}
			}
			if(route_obj)
			{
				array_len = cJSON_GetArraySize(route_obj);
				for(i = 0;i<array_len;i++) {
					tmp_obj = cJSON_GetArrayItem(route_obj,i);
					target = websGetVar(tmp_obj, "target", T(""));
					ptr = websGetVar(tmp_obj, "nexthop", T("0.0.0.0"));
					if(strlen(target)>0 && !strcmp(target,"0.0.0.0"))
					{
						strcpy(status_paremeter->gateway_v4,ptr);
						break;
					}
				}
			}
			if(dns_obj)
			{
				array_len=cJSON_GetArraySize(dns_obj);
				if(array_len>0)
				{
					for(i = 0; i<array_len & i <2; i++){
						tmp_obj = cJSON_GetArrayItem(dns_obj,i);
						dns[i] = tmp_obj->valuestring ? tmp_obj->valuestring : "0.0.0.0";
					}
					if (dns[0])
					{	
						strcpy(status_paremeter->pri_dns_v4,dns[0]);
						if (dns[1])
							strcpy(status_paremeter->sec_dns_v4,dns[1]);
					}
				}
			}
			cJSON_Delete(root_obj);
		}
	}
	return 0;
}

static int get_prefix6_len(struct sockaddr_in6 *mask6)
{
	int i, j, prefix = 0;
	unsigned char *netmask = (unsigned char *) &(mask6)->sin6_addr;

	for (i = 0; i < 16; i++, prefix += 8)
		if (netmask[i] != 0xff)
			break;

	if (i != 16 && netmask[i])
		for (j = 7; j > 0; j--, prefix++)
			if ((netmask[i] & (1 << j)) == 0)
				break;

	return prefix;
}
char *get_ifaddr6(const char *ifname, int linklocal, char *p_addr6s)
{
	char *ret = NULL;
	int prefix;
	struct ifaddrs *ifap, *ife;
	const struct sockaddr_in6 *addr6;
	char addr6s_new[INET6_ADDRSTRLEN] = {0};

	if (getifaddrs(&ifap) < 0)
		return NULL;

	for (ife = ifap; ife; ife = ife->ifa_next)
	{
		if (strcmp(ifname, ife->ifa_name) != 0)
			continue;
		if (ife->ifa_addr == NULL)
			continue;
		if (ife->ifa_addr->sa_family == AF_INET6)
		{
			addr6 = (const struct sockaddr_in6 *)ife->ifa_addr;
			if (IN6_IS_ADDR_LINKLOCAL(&addr6->sin6_addr) ^ linklocal)
				continue;
			if (inet_ntop(ife->ifa_addr->sa_family, &addr6->sin6_addr, addr6s_new, INET6_ADDRSTRLEN) != NULL) {
				prefix = get_prefix6_len((struct sockaddr_in6 *)ife->ifa_netmask);
				if (prefix > 0 && prefix < 128)
					sprintf(p_addr6s, "%s/%d", addr6s_new, prefix);
				ret = p_addr6s;
				break;
			}
		}
	}
	freeifaddrs(ifap);
	return ret;
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

int is_wan_up(void)
{
	int up = 0,wan_phy=4;
	char wan_mode[8] = {0},wan_interface[8] = {0},opmode[8] = {0} ;	

	struct interface_status status_paremeter;
	memset(&status_paremeter,0,sizeof(struct interface_status));

	Uci_Get_Int(PKG_PRODUCT_CONFIG, "hardware", "WanPhyNum", &wan_phy);
	Uci_Get_Str(PKG_NETWORK_CONFIG, "vpn", "proto", wan_mode);
	strcpy(wan_interface,"vpn");
	if(strlen(wan_mode) == 0)
	{
		memset(wan_interface,'\0',sizeof(wan_interface));
		strcpy(wan_interface,"wan");
		Uci_Get_Str(PKG_NETWORK_CONFIG, "wan", "proto",wan_mode);
	}
	
	get_interface_status(&status_paremeter,wan_interface);
	if(!strcmp(wan_mode,"static"))
		up = is_phyconnected(wan_phy);	
	else 
		up = status_paremeter.up;	
	
	return up;
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

	switch(wanType){
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

int wl_ioctl(const char *ifname, int cmd, struct iwreq *pwrq)
{
	int ret = 0;
	int s;

	/* open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return errno;
	}

	/* do it */
	strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);

	if ((ret = ioctl(s, cmd, pwrq)) < 0)
		perror(pwrq->ifr_name);

	/* cleanup */
	close(s);
	return ret;
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
	for (;;) {
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
char *ether_etoa(const unsigned char *e, char *a)
{
	char *c = a;
	int i;

	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		if (i)
			*c++ = ':';
		c += sprintf(c, "%02X", e[i] & 0xff);
	}
	return a;
}

/*
 * Convert Ethernet address binary data to string representation
 * @param       e       binary data
 * @param       a       string in xxxxxxxxxxxx notation
 * @return      a
 */
char *ether_etoa2(const unsigned char *e, char *a)
{
	char *c = a;
	int i;
	
	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		c += sprintf(c, "%02x", e[i] & 0xff);
	}
	return a;
}

char *ether_etoa3(const unsigned char *e, char *a)
{
	char *c = a;
	int i;
	
	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		c += sprintf(c, "%02X", e[i] & 0xff);
	}
	return a;
}

void inc_mac(unsigned char *mac, int offset)
{
	int t;

	if (!mac)
		return;

#if 0
	if (!is_valid_ether_addr(mac)) {
		return;
	}
#endif

	t = (((unsigned int) mac[3]) << 16) + (((unsigned int) mac[4]) << 8) + ((unsigned int) mac[5]);
	t += offset;

	mac[0] = mac[0];
	mac[1] = mac[1];
	mac[2] = mac[2];
	mac[3] = (t >> 16) & 0xff;
	mac[4] = (t >> 8) & 0xff;
	mac[5] = t & 0xff;
}


/**
* @note loginAuth - Auth login
*
* @param  loginAuthUrl - login url.
<pre>
{
"loginAuthUrl":	"username=admin&password=admin&http_host=192.168.30.101"
}
</pre>
* @return json str format
<pre>
{
"httpStatus":"302",
"host":"192.168.30.101",
"redirectURL":"http://192.168.30.101/formLoginAuth.htm?authCode=1&userName=admin&password=admin&goURL=home.asp&action=login"
}
httpStatus: 302 redirect
host: device ip.
redirectURL: redirect URL.
</pre>
* @author felix
* @date    2017-10-23
*/
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
        if (    ('0' <= c && c <= '9') ||
                ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z') ||
                c == '/' || c == '.')
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

//mtd handle
int mtd_dev_idx(const char *mtd_part)
{
	FILE *fp;
	char line[128], bnm[64], *p;
	int idx, ret_idx = -1;

	if (!(fp = fopen("/proc/mtd", "r")))
		return -1;

	fgets(line, sizeof(line), fp); //skip the 1st line
	while (fgets(line, sizeof(line), fp)) {
		if (sscanf(line, "mtd%d: %*s %*s \"%63s\"", &idx, bnm) != 2)
			continue;
		
		/* strip tailed " character, if present. */
		if ((p = strchr(bnm, '"')) != NULL)
			*p = '\0';
		
		if (strcmp(mtd_part, bnm) == 0) {
			ret_idx = idx;
			break;
		}
	}

	fclose(fp);

	return ret_idx;
}

int get_apcli_connected(const char *ifname)
{
	char cmd[256] = {0},result[128] ={0};
	sprintf(cmd,"iwconfig %s | grep 'Access Point' ",ifname);
	getCmdResult(cmd, result,sizeof(result));

	if(strstr(result,"Not-Associated")==NULL){
		return 1;
	}else{
		return 0;
	}
}

int get_apcli_connect_ssid(const char *ifname,char *ssid,int len)
{
	char cmd[256] = {0},result[64] ={0};
	sprintf(cmd,"iwconfig %s |grep ESSID|cut -d '\"' -f2",ifname);
	getCmdResult(cmd, result,sizeof(result));
	strncpy(ssid,result,len);
	return 0;
}

int get_apcli_connect_bssid(const char *ifname,char *bssid,int len)
{
	char cmd[256] = {0},result[64] ={0};
	sprintf(cmd,"iwconfig %s |grep Point|cut -d ' ' -f17",ifname);
	getCmdResult(cmd, result,sizeof(result));
	strncpy(bssid,result,len);
	return 0;
}

int get_apcli_connect_noise(const char *ifname,char *noise,int len)
{
	char cmd[256] = {0},result[64] ={0};
	sprintf(cmd,"iwconfig %s |grep 'Signal level' |cut -d ':' -f3 |cut -d ' ' -f1",ifname);
	getCmdResult(cmd, result,sizeof(result));
	strncpy(noise,result,len);
	return 0;
}

int get_apcli_connect_rssi(const char *ifname,char *rssi,int len)
{
	char cmd[256] = {0},result[64] ={0};
	sprintf(cmd,"iwconfig %s |grep 'Signal level' |cut -d ':' -f2 |cut -d ' ' -f1",ifname);
	getCmdResult(cmd, result,sizeof(result));
	strncpy(rssi,result,len);
	return 0;
}

#define SIOCGIWAP	0x8B15
int get_apcli_ioctl_connected(char *apcli_x)
{
	struct iwreq wrq;
	char ifname[24] = {0};

	strcpy(ifname, apcli_x);
	memset(&wrq, 0, sizeof(struct iwreq));
	wrq.u.ap_addr.sa_family = ARPHRD_ETHER;

	if (wl_ioctl(ifname, SIOCGIWAP, &wrq) >= 0) {
		if (wrq.u.ap_addr.sa_data[0] ||
		    wrq.u.ap_addr.sa_data[1] ||
		    wrq.u.ap_addr.sa_data[2] ||
		    wrq.u.ap_addr.sa_data[3] ||
		    wrq.u.ap_addr.sa_data[4] ||
		    wrq.u.ap_addr.sa_data[5])
			return 1;
	}
	return 0;
}

/* "\ "-->"SUB" */
int parsing_str(char *str,char *result,int len)
{
	char backslash = 0x5C;
	char space = 0x20;
	char SUB = 0x1A;
	char buf[1024] = {0},buffer[1024] = {0};
	int i,j;
	char ch,sch;
	strncpy(buf,str,len);

	i = 0;j = 0;
	do{
		ch = buf[i++];
		if(ch == backslash){
			sch = buf[i++];
			if(sch == space){
				ch = SUB;
			}else if(sch == backslash){
				ch = sch;
			}else{
				buffer[j++] = ch;
				ch =sch;
			}
		}
		buffer[j++] = ch;
	}while(ch != '\0');

	strncpy(result,buffer,strlen(buffer));
	return 0;
}

/* " "-->"\ " */
int parsing_ssid(char *ssid,int len)
{
	int i,j;
	char ch;
	char ssid_t[64] = {0},buf[64] = {0};
	char space = 0x20;
	strncpy(ssid_t,ssid,len);
	i = 0;j = 0;
	for(i = 0;i<len;i++){
		ch = ssid_t[i];
		if(ch == '\0')
			break;
		if(ch == space){
			buf[j++] = '\\';
			buf[j++] = ch;
		}else{
			buf[j++] = ch;
		}
	}
	memset(ssid,0,len);
	strncpy(ssid,buf,strlen(buf));
	return 0;
}

/*
  al_mac	radio_band	ssid	auth_mode	encry_type	passwd	bh_support fh_support hidden vlan_id primary_vlan default_pcp
#ucc_bss_info
1,ff:ff:ff:ff:ff:ff 11x Multi-AP-5LG-1 0x0020 0x0008 maprocks1 1 1 hidden-N 4095 pvid 5
2,ff:ff:ff:ff:ff:ff 11x Multi-AP-5LG-2 0x0020 0x0008 maprocks2 0 1 hidden-N 4095 N/A N/A

#ucc_bss_info
1,ff:ff:ff:ff:ff:ff 11x Multi-AP-5LG-1 0x0020 0x0008 maprocks1 1 1 hidden-N
2,ff:ff:ff:ff:ff:ff 11x Multi-AP-5LG-2 0x0020 0x0008 maprocks2 0 1 hidden-N
*/
/*
setBssConfig(3,"8x","EasyMesh_2.4g","OPEN","NONE",NULL,0);
setBssConfig(0,"11x","EasyMesh_5g","WPA2","AES","123456",0);
*/
int setBssConfig(int idx,char *band,char *ssid,char *auth,char *encryption,char *passwd,int hidden)
{
	int count = 0;
	char al_mac[32] = {0},radio_band[8] = {0},ssid_t[32] = {0},auth_mode[32] = {0},encry_type[32] = {0};
	char passwd_t[64] = {0},hidden_t[16] = {0},primary_vlan[32] = {0},default_pcp[32] = {0};
	int bh_support,fh_support,vlan_id,idx_i,flag,real_idx = -1;
	FILE *fp;
	char buffer[1024*5] = {0};
	char buf[256]={0},tmp[256]={0},idx_t[16]={0};
	char buf_t[256] = {0};
	char auth_f[32] = {0},encry_f[32] = {0},hidden_f[32] = {0},passwd_f[64] = {0};

	if(ssid == NULL || auth == NULL || encryption == NULL ||!( hidden == 0||hidden == 1)){
		printf("error");
		return -1;
	}

	fp = fopen("/etc/map/wts_bss_info_config", "r");
	if(!fp){
		return -1;
	}
	flag= 1;

	while(fgets(buf,256,fp)){
		if(strstr(buf,"ucc_bss_info") && count == 0)
		{
			strcat(buffer,buf);
			memset(buf,0,sizeof(buf));
			continue;
		}
		memset(buf_t,0,sizeof(buf_t));
		parsing_str(buf,buf_t,strlen(buf));
		sscanf(buf_t,"%d,%s%s%s%s%s%s%d%d%s%d%s%s",
				&idx_i,al_mac,radio_band,ssid_t,auth_mode,encry_type,passwd_t,&bh_support,&fh_support,hidden_t,&vlan_id,primary_vlan,default_pcp);

		if(flag){
			if(strcmp(radio_band,band) == 0){/*8x--2G,11x--5LG,12x--5HG*/
				real_idx = idx_i + idx;
				flag = 0;
			}
		}

		if(idx_i == real_idx){
			//printf("[%d][%s][%s][%s][%s][%s][%s][%d][%d][%s][%d][%s][%s]\n",
			//	idx_i,al_mac,radio_band,ssid_t,auth_mode,encry_type,passwd_t,bh_support,fh_support,hidden_t,vlan_id,primary_vlan,default_pcp);

			if(strcmp(auth,"WPA2") == 0 && strcmp(encryption,"AES") == 0){
				strcpy(auth_f,"0x0020");/*WPA2*/
				strcpy(encry_f,"0x0008");/*AES*/
				strcpy(passwd_f,passwd);
			}else if(strcmp(auth,"WPA2WPA3") == 0 && strcmp(encryption,"AES") == 0){
				strcpy(auth_f,"0x0060");/*WPA2WPA3*/
				strcpy(encry_f,"0x0008");/*AES*/
				strcpy(passwd_f,passwd);
			}
#if defined(CONFIG_KL_SUPPORT_MIXEDWPA)
			else if(strcmp(auth,"OPEN") == 0 && strcmp(encryption,"WEP") == 0){
				strcpy(auth_f,"0x0001");/*OPEN*/
				strcpy(encry_f,"0x0002");/*WEP*/
				strcpy(passwd_f,passwd);
			}else if(strcmp(auth,"SHARED") == 0 && strcmp(encryption,"WEP") == 0){
				strcpy(auth_f,"0x0004");/*SHARED*/
				strcpy(encry_f,"0x0002");/*WEP*/
				strcpy(passwd_f,passwd);
			}else if(strcmp(auth,"WPA") == 0 && strcmp(encryption,"TKIP") == 0){
				strcpy(auth_f,"0x0002");/*WPA*/
				strcpy(encry_f,"0x0004");/*TKIP*/
				strcpy(passwd_f,passwd);
			}else if(strcmp(auth,"WPA2") == 0 && strcmp(encryption,"TKIPAES") == 0){
				strcpy(auth_f,"0x0020");/*WPA2*/
				strcpy(encry_f,"0x000c");/*TKIPAES*/
				strcpy(passwd_f,passwd);
			}else if(strcmp(auth,"WPAWPA2") == 0 && strcmp(encryption,"AES") == 0){
				strcpy(auth_f,"0x0022");/*WPAWPA2*/
				strcpy(encry_f,"0x0008");/*AES*/
				strcpy(passwd_f,passwd);
			}else if(strcmp(auth,"WPAWPA2") == 0 && strcmp(encryption,"TKIPAES") == 0){
				strcpy(auth_f,"0x0020");/*WPAWPA2*/
				strcpy(encry_f,"0x000c");/*TKIPAES*/
				strcpy(passwd_f,passwd);
			}else if(strcmp(auth,"WPA3") == 0 && strcmp(encryption,"AES") == 0){
				strcpy(auth_f,"0x0040");/*WPA3*/
				strcpy(encry_f,"0x0008");/*AES*/
				strcpy(passwd_f,passwd);
			}
#endif
			else{
				strcpy(auth_f,"0x0001");/*OPEN*/
				strcpy(encry_f,"0x0001");/*NONE*/
				strcpy(passwd_f,passwd_t);
			}

			if(hidden == 1){
				strcpy(hidden_f,"hidden-Y");
			}else{
				strcpy(hidden_f,"hidden-N");
			}
			sprintf(tmp,"%d,%s %s %s %s %s %s %d %d %s %d %s %s",
				idx_i,al_mac,radio_band,ssid,auth_f,encry_f,passwd_f,bh_support,fh_support,hidden_f,vlan_id,primary_vlan,default_pcp);
			strcat(buffer,tmp);
			strcat(buffer,"\n");
#if defined(DEFAULT_NO_LOW_CHANNEL)
			if(strcmp(band,"11x") == 0)
			{
				sprintf(tmp,"%d,%s %s %s %s %s %s %d %d %s %d %s %s",
					(idx_i+12),al_mac,"12x",ssid,auth_f,encry_f,passwd_f,bh_support,fh_support,hidden_f,vlan_id,primary_vlan,default_pcp);
				strcat(buffer,tmp);
				strcat(buffer,"\n");
			}
#endif
		}
		else{
#if defined(DEFAULT_NO_LOW_CHANNEL)
			if(idx_i < 13 ||  idx_i >  15)
				strcat(buffer,buf);

			if(strcmp(band,"8x") == 0 && (idx_i >12 && idx_i < 16))
				strcat(buffer,buf);
			
			if(strcmp(band,"11x") == 0 && (idx_i >0 && idx_i < 4))
			{
				sprintf(tmp,"%d,%s %s %s %s %s %s %d %d %s %d %s %s",
					(idx_i+12),al_mac,"12x",ssid_t,auth_mode,encry_type,passwd_t,bh_support,fh_support,hidden_t,vlan_id,primary_vlan,default_pcp);
				strcat(buffer,tmp);
				strcat(buffer,"\n");
			}
#else
			strcat(buffer,buf);	
#endif
		}
		count++;
		memset(tmp,0,sizeof(tmp));
		memset(buf,0,sizeof(buf));
	}
	fclose(fp);
	f_write_string("/etc/map/wts_bss_info_config", buffer, 0, 0);
	return 0;
}

int getChannel(char *ifname, char *channel)
{
	char cmd[128] = {0}, buf[32] = {0}, tmp[8] = {0};
	char *p = NULL;
	int ret = 0;;
	sprintf(cmd, "iwconfig %s 2>/dev/null|grep Channel|awk '{print $2}'", ifname);
	FILE *fp = popen(cmd, "r");
	
	if(!fp) {
		printf("getAutoChannel:popen fail\n");
		return -1;
	}
	if(fgets(buf, sizeof(buf)-1, fp) != NULL){
		if(p=strchr(buf, '=')){
			getNthValueSafe(1, buf, '=', tmp, sizeof(tmp));
			p = strchr(tmp, '\n');
			*p = '\0';
			strcpy(channel, tmp);
			ret=0;
		}
		else{
			strcpy(channel, "");
			ret = -1;
		}
	}
	pclose(fp);

	return ret;
}

void getVersion(char *version)
{
	char buf[32] = {0},custom_ver_code[64] = {0};
	Uci_Get_Str(PKG_PRODUCT_CONFIG,"custom","customVerCode",custom_ver_code);
	Uci_Get_Str(PKG_PRODUCT_CONFIG,"custom","customVersion",buf);
	if(strlen(buf) >0){
		strcpy(version,buf);
	}
	else{
		memset(buf,0,sizeof(buf));
		Uci_Get_Str(PKG_PRODUCT_CONFIG,"sysinfo","softVersion",buf);
		if(strlen(custom_ver_code) > 0) {
			strcat(buf, ".");
			strcat(buf, custom_ver_code);
		}
		strcpy(version,buf);
	}
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
	if (strlen(mac_org) == 12) {
		for (i = 0, j = 0; i < 12; i++) {
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

void get_brlan_prefix(char *link_prefix, char *global_prefix){

	system("ip -6  route show dev br-lan > /tmp/brlan_ip6_info");

	FILE *fp = NULL;
	char line_str[TEMP_STR_LEN] = {0};
	char buf[32] = {0};
	char prefix1[32] = {0}, prefix2[32] = {0};
	
	fp = fopen("/tmp/brlan_ip6_info", "r");
	if(fp!=NULL)
	{
		while(fgets(line_str,sizeof(line_str),fp))
		{
			if(strstr(line_str, "kernel") != NULL) //link
				getNthValueSafe(0, line_str, ' ', prefix1, sizeof(prefix1));
			else
				getNthValueSafe(0, line_str, ' ', prefix2, sizeof(prefix2));

			memset(line_str, 0, sizeof(line_str));
		}
		fclose(fp);
		unlink("/tmp/brlan_ip6_info");
	}

	memset(buf, 0, sizeof(buf));
	getNthValueSafe(0, prefix1, '/', buf, sizeof(buf));
	strcpy(link_prefix, buf);

	if(strlen(prefix2) > 0)
	{
		memset(buf, 0, sizeof(buf));
		getNthValueSafe(0, prefix2, '/', buf, sizeof(buf));
		if(buf[strlen(buf) - 1] == ':')
			buf[strlen(buf) - 1] = '\0';
		
		sprintf(global_prefix, "%s", buf );
	}
	
}
void get_sta_ipv6addr(char *pmac, char *ipv6_addr)
{
	FILE *fp = NULL;
	char mac[32] = {0}, status[32] = {0};
	char line_str[TEMP_STR_LEN] = {0};
	char link_prefix[32] = {0}, global_prefix[32] = {0};
	char buf[64] = {0}, link_addr[64] = {0};
	char *suffix = NULL;
	int is_online = 0;
	
	system("ip -6 n > /tmp/sta_ip6");
	get_brlan_prefix(link_prefix, global_prefix);

	fp = fopen("/tmp/sta_ip6", "r");
	if(fp!=NULL)
	{
		while(fgets(line_str,sizeof(line_str),fp))
		{
			memset(buf, 0, sizeof(buf));
			memset(mac, 0, sizeof(mac));
			memset(status, 0, sizeof(status));
			getNthValueSafe(0, line_str, ' ', buf, sizeof(buf));
			getNthValueSafe(4, line_str, ' ', mac, sizeof(mac));
			getNthValueSafe(5, line_str, ' ', status, sizeof(status));
			str_del_char_bak(mac,':');

			if(0==strcasecmp(pmac,mac)){
				if(NULL != strstr(buf, link_prefix))
					strcpy(link_addr, buf);

				if(strstr(status, "REACHABLE"))
					is_online = 1;
			}

			memset(line_str, 0, sizeof(line_str));
		}
		fclose(fp);
		unlink("/tmp/sta_ip6");
	}

	if(is_online == 1 && strlen(link_addr) > 0)
	{
		/* 240e:3b7:3244:db60:
	    		 fe80:: */
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%s", link_addr + strlen(link_prefix));
		sprintf(ipv6_addr, "%s%s", global_prefix , buf);
	}
}

int get_sta_ipaddr_bymac(char *pmac, char *ipv4_addr, char *ipv6_addr)
{
	FILE *fp = NULL;
	int is_get_ip=0;
	char line_str[TEMP_STR_LEN] = {0};
	char mac[RESULT_STR_LEN] = {0},ip[RESULT_STR_LEN] = {0};

	fp = fopen("/tmp/static_ip.inf", "r");
	if(fp!=NULL)
	{
		while(fgets(line_str,sizeof(line_str),fp))
		{				
			memset(mac,0,sizeof(mac));
			memset(ip,0,sizeof(ip));
			getNthValueSafe(0, line_str, ',', mac, sizeof(mac));
			getNthValueSafe(1, line_str, ',', ip, sizeof(ip));
			str_del_char_bak(mac,':');
			str_del_char_bak(ip,'\n');

			if(0==strcmp(pmac,mac)){
				//wifi sta, add ipaddr
				strcpy(ipv4_addr,ip);
				get_sta_ipv6addr(pmac, ipv6_addr);
				//strcpy(ipv6_addr,"");
				is_get_ip=1;
				break;
			}
			memset(line_str,0,sizeof(line_str));
		}
		fclose(fp);
	}
	
	if(0 == is_get_ip)	
	{
		fp = fopen("/tmp/dhcp.leases","r");
		if(fp!=NULL) 
		{
			while(fgets(line_str,sizeof(line_str),fp))
			{
				memset(mac,0,sizeof(mac));
				memset(ip,0,sizeof(ip));
		        sscanf(line_str, "%*s%s%s%*s%*s",mac,ip);
				str_del_char_bak(mac,':');
							
				if(strlen(mac)>0 && strlen(ip)>0 && !strcasecmp(pmac,mac))
				{				
					//wifi sta, add ipaddr
					strcpy(ipv4_addr,ip);
					get_sta_ipv6addr(pmac, ipv6_addr);
					//strcpy(ipv6_addr,"");
					is_get_ip=1;
					break;
				}
				memset(line_str,0,sizeof(line_str));
		    }
			fclose(fp);	
		}
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
	char time[RESULT_STR_LEN]={0},mac[RESULT_STR_LEN] = {0},ip[RESULT_STR_LEN] = {0},dev_name[TEMP_STR_LEN]={0},none[TEMP_STR_LEN]={0};

	//try get hostname from /proc/net/xt_hostname/dhcp
	fp = fopen("/proc/net/xt_hostname/dhcp","r");
	if(fp) 
	{
		while(fgets(line_str, sizeof(line_str)-1, fp))
		{
			bzero(mac,sizeof(mac));
			bzero(dev_name,sizeof(dev_name));

			sscanf(line_str, "%s %s ", mac, dev_name);
			str_del_char_bak(mac,':');

			if(0==strcasecmp(mac,pmac) && strlen(dev_name) > 2)
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

	if(1==is_get_dev_name){
		return 0;
	}
	
	//try get hostname from dhcp.leases
	fp = fopen("/tmp/dhcp.leases","r");
	if(fp) 
	{
		 while(fgets(line_str, sizeof(line_str)-1, fp))
		 {
			bzero(mac,sizeof(mac));
			bzero(ip,sizeof(ip));
			bzero(dev_name,sizeof(dev_name));
			
			sscanf(line_str, "%s %s %s %s %s", time, mac, ip, dev_name,none);
			str_del_char_bak(mac,':');
			//printf("time=%s, mac=%s, ip=%s, dev_name=%s,none=%s\n",time, mac, ip, dev_name,none);
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

int getClientLinkTime(char *devmac,char *time)
{
	cJSON *Devices,*item;
	char mac[24]={0},macBuf[512]={0};
	int  client_num,i=0;
	unsigned long sec, mn, hr, day;

	size_t file_len = f_size("/tmp/client_info");
	client_num=0;
	if(file_len > 0) {
		char *buffer = (char *)malloc(sizeof(char)*file_len+1);
		if(buffer) {
			memset(buffer,'\0',sizeof(char)*file_len+1);
			f_read_excl("/tmp/client_info", buffer, file_len);

			Devices = cJSON_Parse(buffer);
			if(Devices!=NULL) {
				client_num=cJSON_GetArraySize(Devices);
			}
			else {
				Devices = cJSON_CreateArray();
				client_num=0;
			}
			free(buffer);
		}
	}

	for(i=0;i<client_num;i++)
	{
		item = cJSON_GetArrayItem(Devices,i);

		char *mac_ptr = websGetVar(item, "MacAddress", "");
		int sec = atoi(websGetVar(item, "UpTime", "0"));

		if(!strlen(mac_ptr))
			continue;

		memset(mac,0,sizeof(mac));
		add_mac_split(mac_ptr, mac);
		if(strlen(macBuf) > 0)
				strcat(macBuf,",");
			strcat(macBuf,mac);

		if(strcmp(mac,devmac) != 0)
			continue;

		day = sec / 86400;
		sec %= 86400;
		hr = sec / 3600;
		sec %= 3600;
		mn = sec / 60;
		sec %= 60;
		sprintf(time, "%d;%d;%d;%d", day, hr, mn, sec);
	}
	if(strstr(macBuf,devmac) == NULL){
		return 1;
	}

	return 0;
}

int Validity_check(char *tmpbuf)
{
	if(strstr(tmpbuf, ";")|| strstr(tmpbuf, ".sh")||strstr(tmpbuf, "iptables")\
	||strstr(tmpbuf, "telnetd")||strstr(tmpbuf, "&")||strstr(tmpbuf, "|")\
	||strstr(tmpbuf, "`")||strstr(tmpbuf, "$")||strstr(tmpbuf, "\n")){
		return 1;
	}

	return 0;
}

int str_toupper(char *src)
{
	int i;
	for(i=0;i<strlen(src);i++){
		 if(src[i]>='a'&&src[i]<='z')
   			 src[i]-='a'-'A';
	}
	return 0;
}

void set_timezone_to_kernel(void)
{
	char time_zone[8]={0}, cmd_line[128]={0};
	int tz;

	Uci_Get_Str(PKG_SYSTEM_CONFIG, "main", "timezone", time_zone);

	if(strlen(time_zone)<4){
		return;
	}

	sscanf(time_zone,"UTC%d", &tz);

	snprintf(cmd_line, sizeof(cmd_line), "echo 'tz=%d' > /proc/mtd", tz);

	system(cmd_line);
}

int check_img_valid(char *imagefile, int offset, int len, char *err_msg)
{
	struct stat sbuf;

	int  data_len,custom_len=0;
	char *data,product[64]={0}, cmd[265]={0},*p;
	char cur_csid[16]={0},cur_bak_csid[16]={0}, cur_svn[16]={0}, fw_csid[16]={0},fw_custom_csid[16]={0}, fw_svn[16]={0};
	unsigned char *ptr,*ptr_tmp;
	unsigned long checksum;

	image_header_t header;
	image_header_t *hdr = &header;
	image3_header_t header3;
	int headerLen=0;

	int ifd;

	if ((unsigned)len < sizeof(image_header_t)) {
		sprintf (err_msg, "Bad size: \"%s\" is no valid image\n", imagefile);
		return 0;
	}

	ifd = open(imagefile, O_RDONLY);
	if(!ifd){
		sprintf (err_msg, "Can't open %s: %s\n", imagefile, strerror(errno));
		return 0;
	}

	if (fstat(ifd, &sbuf) < 0) {
		close(ifd);
		sprintf (err_msg, "Can't stat %s: %s\n", imagefile, strerror(errno));
		return 0;
	}
	
	ptr = (unsigned char *) mmap(0, sbuf.st_size, PROT_READ, MAP_SHARED, ifd, 0);
	if ((caddr_t)ptr == (caddr_t)-1) {
		close(ifd);
		sprintf (err_msg, "Can't mmap %s: %s\n", imagefile, strerror(errno));
		return 0;
	}
	ptr_tmp=ptr + offset;

	char nvram_buff[16]={0};
	FILE *fp = NULL;
	char tmpcsid[32] = {0};
	custom_header_t c_header;
	custom_header_t *c_hdr = &c_header;
	memcpy (c_hdr, ptr_tmp, sizeof(custom_header_t));
	Uci_Get_Str(PKG_PRODUCT_CONFIG,"custom","csid",nvram_buff);
	strcpy(cur_csid, nvram_buff);

	memset(&header3, 0x00, sizeof(image3_header_t));
	memcpy(&header3, ptr_tmp, sizeof(image3_header_t));

	memset(nvram_buff, 0, sizeof(nvram_buff));
	Uci_Get_Str(PKG_PRODUCT_CONFIG,"sysinfo","svnNum",nvram_buff);
	strcpy(cur_svn, nvram_buff);
	Uci_Get_Str(PKG_PRODUCT_ROM,"custom","csid",cur_bak_csid);

	//check userdate head
	if(strstr(c_hdr->ih_name,"CUSTOM"))// for early custom web
	{
		memcpy(fw_custom_csid,&c_hdr->ih_name[7],sizeof(fw_custom_csid));
		if(0 != strcmp(cur_csid, cur_bak_csid))
		{
			if ( 0 != strcmp(cur_csid, fw_custom_csid) )
			{
				munmap(ptr, len);
				close(ifd);
				sprintf (err_msg, "MM_cloud_fw2flash2");
				return 0;
			}
		}
		data = (char *)c_hdr;
		data = (char *)(ptr_tmp + sizeof(custom_header_t));
		data_len  = ntohl(c_hdr->ih_size) ; //support upgrade openwrt image
		custom_len = data_len + sizeof(custom_header_t);
		//Cut out userdate part
		sprintf(cmd, "dd if=%s of=/tmp/custom bs=%d count=1", imagefile,custom_len);
		CsteSystem(cmd,CSTE_PRINT_CMD);
		sleep(1);

		//remove head
		sprintf(cmd, "dd if=/tmp/custom of=/tmp/userdata.tar.gz bs=%d skip=1", sizeof(custom_header_t));
		CsteSystem(cmd,CSTE_PRINT_CMD);
		sleep(1);
		
		if((len-custom_len)<1024000) //only upgrade userdata
		{
			munmap(ptr, len);
			close(ifd);
			sprintf(cmd, "rm -f %s", DL_IMAGE_FILE);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			return 1;
		}
		else
		{
			sprintf(cmd, "dd if=%s of=/tmp/firmware_bk.img bs=%d skip=1", imagefile, custom_len);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			sprintf(cmd, "rm -f %s", DL_IMAGE_FILE);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			sprintf(cmd, "mv /tmp/firmware_bk.img %s", DL_IMAGE_FILE);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			ptr_tmp = ptr_tmp + custom_len;
		}
	}
	else if(strstr(c_hdr->ih_name,"USERDATABIN"))// for early custom web
	{
		memcpy(fw_custom_csid,&c_hdr->ih_name[12],sizeof(fw_custom_csid));
		if(0 != strcmp(cur_csid, cur_bak_csid))
		{
			if ( 0 != strcmp(cur_csid, fw_custom_csid) )
			{
				munmap(ptr, len);
				close(ifd);
				sprintf (err_msg, "MM_cloud_fw2flash2");
				return 0;
			}
		}
		
		data = (char *)c_hdr;
		data = (char *)(ptr_tmp + sizeof(custom_header_t));
		data_len  = ntohl(c_hdr->ih_size) ; //support upgrade openwrt image
		custom_len = data_len + sizeof(custom_header_t);
		//Cut out userdate part
		sprintf(cmd, "dd if=%s of=/tmp/custom bs=%d count=1", imagefile,custom_len);
		CsteSystem(cmd,CSTE_PRINT_CMD);
		sleep(1);

		//remove head
		sprintf(cmd, "dd if=/tmp/custom of=/tmp/userdata.bin bs=%d skip=1", sizeof(custom_header_t));
		CsteSystem(cmd,CSTE_PRINT_CMD);
		sleep(1);
		
		if((len-custom_len)<1024000)
		{
			munmap(ptr, len);
			close(ifd);
			sprintf(cmd, "rm -f %s", DL_IMAGE_FILE);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			return 1;
		}
		else
		{
			sprintf(cmd, "dd if=%s of=/tmp/firmware_bk.img bs=%d skip=1", imagefile, custom_len);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			sprintf(cmd, "rm -f %s", DL_IMAGE_FILE);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			sprintf(cmd, "mv /tmp/firmware_bk.img %s", DL_IMAGE_FILE);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			ptr_tmp = ptr_tmp + custom_len;
		}
	}
	else if(htonl(header3.magic) == IH3_MAGIC)
	{
		memcpy(fw_custom_csid,&header3.name,sizeof(fw_custom_csid));
		if(0 != strcmp(cur_csid, cur_bak_csid))
		{
			if ( 0 != strcmp(cur_csid, fw_custom_csid) )
			{
				munmap(ptr, len);
				close(ifd);
				sprintf (err_msg, "MM_cloud_fw2flash2");
				return 0;
			}
		}
		headerLen = sizeof(image3_header_t);
		ptr_tmp += headerLen;
		checksum=crc32(0, ptr_tmp, htonl(header3.dataSize));
		if (checksum != ntohl(header3.dcrc))
		{
			printf("FW crc32 checksum error, reboot....\n");
			return(-1);
		}

		//Cut out userdate part
		custom_len=headerLen+htonl(header3.dataSize);
		sprintf(cmd, "dd if=%s of=/tmp/custom bs=%d count=1", imagefile,custom_len);
		CsteSystem(cmd,CSTE_PRINT_CMD);
		sleep(1);

		//remove head
		sprintf(cmd, "dd if=/tmp/custom of=/tmp/head3_userdata.bin bs=%d skip=1", headerLen);
		CsteSystem(cmd,CSTE_PRINT_CMD);
		sleep(1);
		if((len-custom_len)<1024000)
		{
			munmap(ptr, len);
			close(ifd);
			sprintf(cmd, "rm -f %s", DL_IMAGE_FILE);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			return 1;
		}
		else
		{
			sprintf(cmd, "dd if=%s of=/tmp/firmware_bk.img bs=%d skip=1", imagefile, custom_len);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			sprintf(cmd, "rm -f %s", DL_IMAGE_FILE);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			sprintf(cmd, "mv /tmp/firmware_bk.img %s", DL_IMAGE_FILE);
			CsteSystem(cmd,CSTE_PRINT_CMD);
			ptr_tmp += htonl(header3.dataSize);
		}
	}

	 /*  handle Header CRC32 */
#if defined(CONFIG_BOARD_IP04433_NAND) || defined(CONFIG_BOARD_AP0185_NAND)
	char adr[32]={0},board[32]={0},cur_hardmode[16]={0},fw_hardmode[16]={0};
	char *padr=adr;
	memcpy (padr, ptr_tmp, sizeof(adr));
	
	memcpy(board, (char *)padr , 32);
	getNthValueSafe(1, board, '-', fw_hardmode, sizeof(fw_hardmode));

	Uci_Get_Str(PKG_PRODUCT_ROM,"sysinfo","hardModel",cur_hardmode);
	if (0 != strncmp(cur_hardmode, fw_hardmode,strlen(cur_hardmode))) {
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "MM_cloud_fw2flash2");
		return 0;
	}
#else
	memcpy (hdr, ptr_tmp, sizeof(image_header_t));

	if (ntohl(hdr->ih_magic) != IH_MAGIC) {
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "Bad Magic Number: \"%s\" is no valid image\n", imagefile);
		return 0;
	}

	data = (char *)hdr;
	

	checksum = ntohl(hdr->ih_hcrc);
	hdr->ih_hcrc = htonl(0);	/* clear for re-calculation */

	if (crc32 (0, data, sizeof(image_header_t)) != checksum) {
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "*** Warning: \"%s\" has bad header checksum!\n", imagefile);
		return 0;
	}

	 /*  handle Data CRC32 */
	 
	data = (char *)(ptr_tmp + sizeof(image_header_t));
	data_len  = ntohl(hdr->ih_size) ; //support upgrade openwrt image

	if (crc32 (0, data, data_len) != ntohl(hdr->ih_dcrc)) {
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "*** Warning: \"%s\" has corrupted data!\n", imagefile);
		return 0;
	}

	memcpy(product, (char *)hdr->ih_name, 32);
	getNthValueSafe(0, product, '-', fw_csid, sizeof(fw_csid));
	getNthValueSafe(1, product, '-', fw_svn, sizeof(fw_svn));

	if(0 == strcmp(cur_csid, cur_bak_csid))
	{
		if ( 0 != strcmp(cur_csid, fw_csid) )
		{
			munmap(ptr, len);
			close(ifd);
			sprintf (err_msg, "MM_cloud_fw2flash2");
			return 0;
		}
	}else{
		if(strlen(fw_custom_csid)>0)
		{
			if ( 0 != strcmp(cur_csid, fw_custom_csid))
			{
				munmap(ptr, len);
				close(ifd);
				sprintf (err_msg, "MM_cloud_fw2flash2");
				return 0;
			}
		}else{
			if ( 0 != strcmp(cur_bak_csid, fw_csid))
			{
				munmap(ptr, len);
				close(ifd);
				sprintf (err_msg, "MM_cloud_fw2flash2");
				return 0;
			}
		}
	}
#endif
	munmap(ptr, len);
	close(ifd);
	return 1;
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


