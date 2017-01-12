/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fdfs_client.h"
#include "logger.h"

#include "fdfs_upload_file.h"

static void usage(char *argv[])
{
	printf("Usage: %s <config_file> <local_filename> " \
		"[storage_ip:port] [store_path_index]\n", argv[0]);
}

/*
 * 这里是根据本地文件的路径和文件名进行上传
 * 将int main(int argc, char *argv[])转换成uploadMain(int argc, char *argv[])作为接口进行调用
 */
char* uploadMain(int argc, char *argv[])
{
	char *conf_filename;
	char *local_filename;
	char group_name[FDFS_GROUP_NAME_MAX_LEN + 1];
	ConnectionInfo *pTrackerServer;
	int result;
	int store_path_index;
	ConnectionInfo storageServer;
//	char file_id[128];
	char* file_id = (char*)malloc(sizeof(char)*128);
	
	if (argc < 3)	// 除了文件名，至少还需要2个参数: argv[1]为client.conf的完整路径，argv[2]为需要上传文件的路径;./fdfs_upload_file ../conf/client.conf {xxx}/.jpg
	{
		usage(argv);
		return "1";
	}

	log_init();
	g_log_context.log_level = LOG_ERR;
	ignore_signal_pipe();

	conf_filename = argv[1];
	if ((result=fdfs_client_init(conf_filename)) != 0)
	{
		return NULL;
	}

	pTrackerServer = tracker_get_connection();	// 连接tracker server，返回连接信息
	if (pTrackerServer == NULL)
	{
		fdfs_client_destroy();
		return NULL;
	}

	local_filename = argv[2];
	*group_name = '\0';
	if (argc >= 4)
	{
		const char *pPort;
		const char *pIpAndPort;

		pIpAndPort = argv[3];		// 第三个参数为storage的IP地址和端口号
		pPort = strchr(pIpAndPort, ':');
		if (pPort == NULL)
		{
			fdfs_client_destroy();
			fprintf(stderr, "invalid storage ip address and " \
				"port: %s\n", pIpAndPort);
			usage(argv);
			return "1";
		}

		storageServer.sock = -1;
		snprintf(storageServer.ip_addr, sizeof(storageServer.ip_addr), \
			 "%.*s", (int)(pPort - pIpAndPort), pIpAndPort);
		storageServer.port = atoi(pPort + 1);
		if (argc >= 5)
		{
			store_path_index = atoi(argv[4]);
		}
		else
		{
			store_path_index = -1;
		}
	}
	else if ((result=tracker_query_storage_store(pTrackerServer, \
	                &storageServer, group_name, &store_path_index)) != 0)
	{
		fdfs_client_destroy();
		fprintf(stderr, "tracker_query_storage fail, " \
			"error no: %d, error info: %s\n", \
			result, STRERROR(result));
		return NULL;
	}

	// 根据文件名上传
	// #define storage_upload_by_filename1(pTrackerServer, pStorageServer, \
	store_path_index, local_filename, file_ext_name, \
	meta_list, meta_count, group_name, file_id)
	result = storage_upload_by_filename1(pTrackerServer, \
			&storageServer, store_path_index, \
			local_filename, NULL, \
			NULL, 0, group_name, file_id);

	// 根据buffer上传: 将根据文件名上传的local_filename换成file_buff, file_size
	// #define storage_upload_by_filebuff1(pTrackerServer, pStorageServer, \
	store_path_index, file_buff, file_size, file_ext_name, \
	meta_list, meta_count, group_name, file_id)

	if (result == 0)
	{
		printf("%s\n", file_id);	// 上传成功，返回fileID
		tracker_disconnect_server_ex(pTrackerServer, true);
		fdfs_client_destroy();
		return file_id;
	}
	else
	{
		fprintf(stderr, "upload file fail, " \
			"error no: %d, error info: %s\n", \
			result, STRERROR(result));
	}

	tracker_disconnect_server_ex(pTrackerServer, true);
	fdfs_client_destroy();

	return NULL;
}


/*
 * 根据buffer上传文件
 *
 */
char* uploadByBuffer(char* confFileName, char *fileBuffer, int fileSize)
{
	char group_name[FDFS_GROUP_NAME_MAX_LEN + 1];
	ConnectionInfo *pTrackerServer;	// 连接tracker的信息
	int result;	// 上传返回的结果
	int store_path_index;
	ConnectionInfo storageServer;
	char* file_id = (char*)malloc(sizeof(char)*128);

	log_init();
	g_log_context.log_level = LOG_ERR;
	ignore_signal_pipe();

	if ((result=fdfs_client_init(confFileName)) != 0)	// 加载客户端的配置文件
	{
		return NULL;
	}

	pTrackerServer = tracker_get_connection();	// 连接tracker server，返回连接信息
	if (pTrackerServer == NULL)
	{
		fdfs_client_destroy();
		return NULL;
	}

	*group_name = '\0';

	if ((result=tracker_query_storage_store(pTrackerServer, \
	                &storageServer, group_name, &store_path_index)) != 0)	// 指定组的所有group列表,返回0表示成功
	{
		fdfs_client_destroy();	// 返回gropu信息失败
		fprintf(stderr, "tracker_query_storage fail, " \
			"error no: %d, error info: %s\n", \
			result, STRERROR(result));
		return NULL;
	}

	// 根据buffer上传: 将根据文件名上传的local_filename换成fileBuffer, fileSize,这里把文件后缀名看成NULL
	// #define storage_upload_by_filebuff1(pTrackerServer, pStorageServer, \
	store_path_index, file_buff, file_size, file_ext_name, \
	meta_list, meta_count, group_name, file_id)
	result = storage_upload_by_filebuff1(pTrackerServer, \
				&storageServer, store_path_index, \
				fileBuffer, fileSize, NULL, \
				NULL, 0, group_name, file_id);

	if (result == 0)
	{
		printf("%s\n", file_id);	// 上传成功，返回fileID
		tracker_disconnect_server_ex(pTrackerServer, true);
		fdfs_client_destroy();
		return file_id;
	}
	else
	{
		fprintf(stderr, "upload file fail, " \
			"error no: %d, error info: %s\n", \
			result, STRERROR(result));
	}

	tracker_disconnect_server_ex(pTrackerServer, true);
	fdfs_client_destroy();

	return NULL;
}

