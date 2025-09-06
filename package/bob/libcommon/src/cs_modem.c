#include "cs_common.h"
#include <sys/socket.h>
#include <sys/un.h>

int get_sim_status(void)
{

/*
	char tmp_buf[128],ubus_data[2048];
	char sim_state[8];

	cs_ubus_cli_call("sim", "get_sim_status",ubus_data);

	cJSON *ubus_root = cJSON_Parse(ubus_data);
	if(ubus_root == NULL)
	{
		return 0;
	}

	cJSON *appObj = cJSON_GetObjectItem(ubus_root, "pin_puk");
	if(appObj) {
		get_cjson_string(appObj, "sim_status", sim_state, sizeof(sim_state)); 	
	}	
	cJSON_Delete(ubus_root);
*/

	int sim_state=0;
	char cmd_result[128] = { 0 };
	get_cmd_result("cli_atc AT+CPIN? | awk 'NR==2{print}'", cmd_result, sizeof(cmd_result));

	if(strstr(cmd_result, "READY") != NULL)
		sim_state = 1;

	return sim_state;
}

int connectUnixSocket(const char* socketName)
{
	struct sockaddr_un addr;
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (sock < 0)
		return -1;

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strlcpy(addr.sun_path, socketName, sizeof(addr.sun_path));
	if (/*TEMP_FAILURE_RETRY*/(connect(sock,(const struct sockaddr*) &addr, sizeof(addr))) /*!=*/ < 0) {
		close(sock);
		return -1;
	}
	return sock;
}

void *receive_at_data_handle(struct atcmd_call_st *call_data)
{
	int tota,bytes,count,timeout;
	
	//pthread_detach(pthread_self());
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	if(call_data->serial_fd < 0 || !call_data->rsp_data)
		goto end;

	tota = 0;
	timeout = AT_TIMEOUT*10;//AT_TIMEOUT*1000*1000/100000
	count = 0;
	while (count < timeout ) {

		bytes = read(call_data->serial_fd, call_data->rsp_data+tota, call_data->rsp_len-tota-1);

		if (bytes < 1)
			continue;

		tota += bytes;
		if(call_data->rsp_len-tota<2)
		{
			call_data->rsp_data[tota] = '\0';
			break;
		}

		if (strstr(call_data->rsp_data, "OK") || strstr(call_data->rsp_data, "ERROR") ||
		   strstr(call_data->rsp_data, "CONNECT") || strstr(call_data->rsp_data, "NO CARRIER")) {
			call_data->rsp_data[tota] = '\0';
			break;
		}
		usleep(100000);
		count++;
	}

end:

	if(call_data->serial_fd > 0)
			close(call_data->serial_fd);

	pthread_exit((void *)0);
	call_data->serial_fd = -1;
}

int send_atcmd(char *at_cmd,char *rsp_data,int rsp_len)
{
	int rc,recieved;
	struct atcmd_call_st call_data;
	
	memset(&call_data,0,sizeof(struct atcmd_call_st));

	if (!at_cmd || !strlen(at_cmd) || !rsp_data || !rsp_len)
		return -1;

	call_data.serial_fd = -1;
	call_data.rsp_len = rsp_len;
	call_data.rsp_data = rsp_data;

	call_data.serial_fd = connectUnixSocket(SERIAL_PORT_NAME);

	if (call_data.serial_fd <	0)
		return -1;

	fcntl(call_data.serial_fd, F_SETFL, O_NONBLOCK);
	tcflush(call_data.serial_fd, 0);

	pthread_t InputThread;
	if (pthread_create(&InputThread, NULL,
		(void *)receive_at_data_handle, &call_data) != 0) {
		close(call_data.serial_fd);
		return -1;
	}

	char buf[513] = {0};
	size_t len = strlen(at_cmd);

	if (len < 512) {
		strcpy((char *)buf, at_cmd);
	}
	else {
		close(call_data.serial_fd);
		pthread_cancel(InputThread);
		return -1;
	}

	buf[len] = 0xd;
	rc = write(call_data.serial_fd, buf, len+1);

	if (rc < 0) {
		pthread_cancel(InputThread);
		close(call_data.serial_fd);
		call_data.serial_fd = -1;
	}
	else {
		pthread_join(InputThread, NULL);
	}
	return 0;
}

void set_modem_flight(void)
{
	doSystem("cli_atc at+cfun=0");
	usleep(500);
	doSystem("cli_atc at+cfun=1");
}

int get_modem_status(void)
{
	return MODEM_STATUS_4G;
}

