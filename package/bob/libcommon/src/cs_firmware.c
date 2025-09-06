#include <sys/mman.h>
#include "cs_common.h"


int firmware_check(char *imagefile, int offset, int len, char *err_msg, char *re_csid)
{
	int count_members, index, results = 0;
	cJSON *root = NULL, *json_node = NULL, *item = NULL;
	char meta_info[1024] = { 0 }, actual_csid[128] = { 0 }, current_csid[16] = { 0 };
	
	
	doSystem("/usr/libexec/validate_firmware_image %s",imagefile);
	f_read("/tmp/sysupgrade.meta", meta_info, sizeof(meta_info));
	
	root = cJSON_Parse(meta_info);

	if(root == NULL)
	{
		strcpy(err_msg,"MM_cloud_fw2flash1");

		return 0;
	}

	json_node = cJSON_GetObjectItem(root,"supported_devices");

	
	if(json_node == NULL)
	{
		cJSON_Delete(root);
		strcpy(err_msg, "MM_cloud_fw2flash1");
	
		return 0;
	}

	/* check csid */
	Uci_Get_Str(PKG_PRODUCT_CONFIG, "custom", "csid", current_csid);

	count_members = cJSON_GetArraySize(json_node);

	for(index = 0; index < count_members; index++)
	{
		
		item = cJSON_GetArrayItem(json_node, index);
		memset(actual_csid, 0, sizeof(actual_csid));
	
		snprintf(actual_csid, sizeof(actual_csid), "%s", item->valuestring);
	
		if(strstr(actual_csid, current_csid) != NULL)
		{
			results = 1;
			break;
		}
	}
	
	cJSON_Delete(root);
	
	return results;
}

