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

#include "fdfs_download_file.h"

int downloadMain(int argc, char *argv[])
{
	char *conf_filename;
	char *local_filename;
	ConnectionInfo *pTrackerServer;
	int result;
	char file_id[128];
	int64_t file_size;
	int64_t file_offset;
	int64_t download_bytes;
	
	if (argc < 3)
	{
		printf("Usage: %s <config_file> <file_id> " \
			"[local_filename] [<download_offset> " \
			"<download_bytes>]\n", argv[0]);
		return 1;
	}

	log_init();
	g_log_context.log_level = LOG_ERR;
	ignore_signal_pipe();

	conf_filename = argv[1];
	if ((result=fdfs_client_init(conf_filename)) != 0)
	{
		return result;
	}

	pTrackerServer = tracker_get_connection();
	if (pTrackerServer == NULL)
	{
		fdfs_client_destroy();
		return errno != 0 ? errno : ECONNREFUSED;
	}

	snprintf(file_id, sizeof(file_id), "%s", argv[2]);

	file_offset = 0;
	download_bytes = 0;
	if (argc >= 4)
	{
		local_filename = argv[3];	// 默认下载保存的文件名为file_id，这里是自定义下载的文件名
		if (argc >= 6)
		{
			file_offset = strtoll(argv[4], NULL, 10);
			download_bytes = strtoll(argv[5], NULL, 10);
		}
	}
	else		// 这里是没有指定下载后保存的文件名，就取file_id中的后面一段
	{
		local_filename = strrchr(file_id, '/');	// '/'在file_id中从右侧开始首次出现的位置
		if (local_filename != NULL)
		{
			local_filename++;  //skip /
		}
		else
		{
			local_filename = file_id;
		}
	}

	result = storage_do_download_file1_ex(pTrackerServer, \
                NULL, FDFS_DOWNLOAD_TO_FILE, file_id, \
                file_offset, download_bytes, \
                &local_filename, NULL, &file_size);	// 下载保存的文件名为local_filename
	if (result != 0)
	{
		printf("download file fail, " \
			"error no: %d, error info: %s\n", \
			result, STRERROR(result));
	}

	tracker_disconnect_server_ex(pTrackerServer, true);
	fdfs_client_destroy();

	return 0;
}


/*
 * 下载文件，返回char*缓存
 * configFile: 客户端配置文件目录
 * fileID:     下载文件的文件ID
 * ReturnBuffer
 */
void downloadReturnBuffer(char* configFile, char* fileID, char** downloadBuffer, int64_t* fileLen)
{
	char *downloadTempBuffer = NULL;
	ConnectionInfo *pTrackerServer;
	int result;
	int64_t file_size;		// 调用函数中用于保存下载文件的大小
	int64_t file_offset;
	int64_t download_bytes;

	log_init();
	g_log_context.log_level = LOG_ERR;
	ignore_signal_pipe();

	if ((result=fdfs_client_init(configFile)) != 0)
	{
		return;
	}

	pTrackerServer = tracker_get_connection();
	if (pTrackerServer == NULL)
	{
		fdfs_client_destroy();
		return;
	}

	file_offset = 0;	// 下载文件时候的偏移量，默认为0
	download_bytes = 0;	// 0意味着从文件头到文件尾

	// 下载的数据保存到缓存中，第三个参数 download_type: FDFS_DOWNLOAD_TO_BUFF
	// file_offset: the start offset to download 文件偏移量：下载文件开始的偏移量
	// download_bytes: download bytes, 0 means from start offset to the file end 下载的字节数目，0意味着从文件头到文件尾
	// file_buff: return file content/buff, must be freed	文件缓冲：返回文件内容/缓冲（必须被释放）
	//	file_size: return file size (bytes) 返回文件的大小(B)
	result = storage_do_download_file1_ex(pTrackerServer, \
				NULL, FDFS_DOWNLOAD_TO_BUFF, fileID, \
				file_offset, download_bytes, \
				&downloadTempBuffer, NULL, &file_size);	// 下载保存的文件名为local_filename
//	char *downloadRetBuffer = (char*)malloc(sizeof(char)*(file_size));	// 编译器就是把大小的数值放在分配地址开始的之前位置
	memcpy(*downloadBuffer, downloadTempBuffer, file_size);
	*fileLen = file_size;
	printf("下载到缓冲完成，文件大小： %d\n", file_size);
//	FILE *fp = fopen("./data/download/test.jpg","w");
//	fwrite(downloadTempBuffer, file_size,1, fp);
//	fclose(fp);

	if (result != 0)
	{
		printf("download file fail, " \
			"error no: %d, error info: %s\n", \
			result, STRERROR(result));
	}

	tracker_disconnect_server_ex(pTrackerServer, true);
	fdfs_client_destroy();
}

