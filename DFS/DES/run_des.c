#include "run_des.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * des.h provides the following functions and constants:
 *
 * generate_key, generate_sub_keys, process_message, ENCRYPTION_MODE, DECRYPTION_MODE
 *
 */
#include "des.h"

#include "run_des.h"


// Declare file handlers
static FILE *key_file, *input_file, *output_file;	// 文件指针

// Declare action parameters
#define ACTION_GENERATE_KEY "-g"	// 产生密钥
#define ACTION_ENCRYPT "-e"			// 加密
#define ACTION_DECRYPT "-d"			// 解密

// DES key is 8 bytes long
#define DES_KEY_SIZE 8				// 密钥的长度


/*
 * 从字符数组str中读取size个字节到data中
 * 返回:读取到的字节个数
 */
size_t stringRead (void *data, int size, char* str, int pos) {
	memcpy(data, str+pos, size);
//	int i;
//	for (i = 0; i < size; i++, pos++) {
//		if (pos < strlen(str)) {
//			data[i] = str[pos];
//		} else {
//			return i;
//		}
//	}
	return size;
}


size_t stringWrite (const void *processed_block, int size, char* processedString, int posInProcessString) {
//	int i;
//	for (i = 0; i < size; i++) {
//		processedString[posInProcessString++] = processed_block[i];
//	}
	memcpy(processedString, processed_block, size);
	return size;
}


/*
 * 产生密钥文件并写入磁盘 ./DES/data/DES.key
 */
int generateKey(char* keyPath)
{
	key_file = fopen(keyPath, "wb");		// 新建并打开密钥文件
	if (!key_file) {
		printf("Could not open file to write key.");
		return 1;
	}

	unsigned int iseed = (unsigned int)time(NULL);		// 返回结果写入time(&t)所在的地址，这里不需要写入到某个变量的地址中，直接返回
	srand (iseed);

	unsigned char* des_key = (unsigned char*) malloc(8*sizeof(char));
	generate_key(des_key);

	short int bytes_written;
	bytes_written = fwrite(des_key, 1, DES_KEY_SIZE, key_file);
	if (bytes_written != DES_KEY_SIZE) {
		printf("Error writing key to output file.");
		fclose(key_file);
		free(des_key);
		return 1;
	}
	free(des_key);
	fclose(key_file);

	return 1;
}


/*
 * string: process string
 * acttion=0 encrypy, action=1 decrypt
 */
char* processMessages(char* processString, int action)
{
	// Read key file
	char* keyPath = "";
	key_file = fopen("/home/ubuntu/install/eclipse/workspace/DFS/DES/data/DES.key", "rb");
	if (!key_file) {
		printf("Could not open key file to read key.");
		return "Could not open key file to read key.";
	}

	unsigned long file_size;
	unsigned short int padding;		// DES加密的时候，如果不是8的整倍数，则会以某种机制补足到8字节长度

	short int bytes_read;
	unsigned char* des_key = (unsigned char*) malloc(8*sizeof(char));
	bytes_read = fread(des_key, sizeof(unsigned char), DES_KEY_SIZE, key_file);	// 将文件名为变量key_file的文件读入到变量des_key中，返回读取元素的个数
	if (bytes_read != DES_KEY_SIZE) {
		printf("Key read from key file does not have valid key size.");
		fclose(key_file);
		return "Key read from key file does not have valid key size.";
	}
	fclose(key_file);

	// Generate DES key set
	short int bytes_written, process_mode;
	unsigned long block_count = 0, number_of_blocks;
	unsigned char* data_block = (unsigned char*) malloc(8*sizeof(char));		// 每次加密的数据块（8个字节大小）
	unsigned char* processed_block = (unsigned char*) malloc(8*sizeof(char));	// 用于保存加密后的字符串8B
	key_set* key_sets = (key_set*)malloc(17*sizeof(key_set));					// ( 8(k) + 4(c) + 4(d) ) * 17个密钥集

//	unsigned char* des_key = (unsigned char*) malloc(8*sizeof(char));
//	generate_key(des_key);					// 产生密钥,暂时不这样做,需要从磁盘中读取密钥
	generate_sub_keys(des_key, key_sets);	// 产生子密钥

	// Determine process mode
	if (action == 0) {
		process_mode = ENCRYPTION_MODE;
		printf("Encrypting..\n");
	} else {
		action = DECRYPTION_MODE;
		printf("Decrypting..\n");
	}

	file_size = strlen(processString);
	number_of_blocks = file_size/8 + ((file_size%8)?1:0);	// 计算文件块的数目

	unsigned char* processedString = (unsigned char*) malloc(8*sizeof(char) * number_of_blocks);	// 用于保存加密后的字符串
	int posInProcessString = 0;
	while(block_count <= number_of_blocks) {	// 从input_file中每次读取1个字节，读取8次到data_block中
		stringRead(data_block, 8, processString, posInProcessString);
		posInProcessString += 8;
		block_count++;
		if (block_count == number_of_blocks) {
			if (process_mode == ENCRYPTION_MODE) {		// 加密的情况
				padding = 8 - file_size%8;
				if (padding < 8) { // Fill empty data block bytes with padding
					memset((data_block + 8 - padding), (unsigned char)padding, padding);
				}

				process_message(data_block, processed_block, key_sets, process_mode);	// data_block --> processed_block
				bytes_written = stringWrite(processed_block, 8, processedString, posInProcessString);

				if (padding == 8) { // Write an extra block for padding
					memset(data_block, (unsigned char)padding, 8);
					process_message(data_block, processed_block, key_sets, process_mode);
					bytes_written = stringWrite(processed_block, 8, processedString, posInProcessString);
				}
			} else {
				process_message(data_block, processed_block, key_sets, process_mode);
				padding = processed_block[7];

				if (padding < 8) {
					bytes_written = stringWrite(processed_block, 8, processedString, posInProcessString);
				}
			}
		} else {
			process_message(data_block, processed_block, key_sets, process_mode);
			bytes_written = stringWrite(processed_block, 8, processedString, posInProcessString);
		}
		memset(data_block, 0, 8);
	}

	return processedString;
}


// -e /home/ubuntu/install/eclipse/workspace/DFS/DES/data/keyfile.key /home/ubuntu/install/eclipse/workspace/DFS/DES/data/sample.txt /home/ubuntu/install/eclipse/workspace/DFS/DES/data/sample.enc
int runDESMain(int argc, char* argv[]) {
//int main(int argc, char* argv[]) {	// 利用主函数先生成可执行文件run_des.o,后面用来直接调用
//	char* keyPath = "/home/ubuntu/install/eclipse/workspace/DFS/DES/data/DES.key";
//	generateKey(keyPath);
//
//	char* fileID = "group1/M01/05/59/ChMTFlhg0vyAVH4PAAcV_GyiCUY984.jpg\\haidong.wang";
//	char* encryptedFileID = processMessages(fileID, 0);
//	printf("%d\n", strlen(encryptedFileID));
//	printf("%s\n", encryptedFileID);
//	char* decryptedFileID = processMessages(fileID, 1);
//	printf("%d\n", strlen(decryptedFileID));
//	printf("%s\n", decryptedFileID);
//
//	printf("---------------------------------\n");


	clock_t start, finish;
	double time_taken;
	unsigned long file_size;
	unsigned short int padding;		// DES加密的时候，如果不是8的整倍数，则会以某种机制补足到8字节长度

	if (argc < 2) {
		printf("You must provide at least 1 parameter, where you specify the action.");
		return 1;
	}

	if (strcmp(argv[1], ACTION_GENERATE_KEY) == 0) { // Generate key file
		if (argc != 3) {
			printf("Invalid # of parameter specified. Usage: run_des -g keyfile.key");
			return 1;
		}

		key_file = fopen(argv[2], "wb");		// 新建并打开密钥文件
		if (!key_file) {
			printf("Could not open file to write key.");
			return 1;
		}

		unsigned int iseed = (unsigned int)time(NULL);		// 返回结果写入time(&t)所在的地址，这里不需要写入到某个变量的地址中，直接返回
		srand (iseed);

		short int bytes_written;
		unsigned char* des_key = (unsigned char*) malloc(8*sizeof(char));
		generate_key(des_key);
		bytes_written = fwrite(des_key, 1, DES_KEY_SIZE, key_file);
		if (bytes_written != DES_KEY_SIZE) {
			printf("Error writing key to output file.");
			fclose(key_file);
			free(des_key);
			return 1;
		}

		free(des_key);
		fclose(key_file);
	} else if ((strcmp(argv[1], ACTION_ENCRYPT) == 0) || (strcmp(argv[1], ACTION_DECRYPT) == 0)) { // Encrypt or decrypt 加密或者解密
		if (argc != 5) {
			printf("Invalid # of parameters (%d) specified. Usage: run_des [-e|-d] keyfile.key input.file output.file", argc);
			return 1;
		}

		// Read key file 读取密钥文件
		key_file = fopen(argv[2], "rb");
		if (!key_file) {
			printf("Could not open key file to read key.");
			return 1;
		}

		short int bytes_read;
		unsigned char* des_key = (unsigned char*) malloc(8*sizeof(char));
		bytes_read = fread(des_key, sizeof(unsigned char), DES_KEY_SIZE, key_file);	// 将文件名为变量key_file的文件读入到变量des_key中，返回读取元素的个数
		if (bytes_read != DES_KEY_SIZE) {
			printf("Key read from key file does nto have valid key size.");
			fclose(key_file);
			return 1;
		}
		fclose(key_file);

		// Open input file 验证输入文件
		input_file = fopen(argv[3], "rb");
		if (!input_file) {
			printf("Could not open input file to read data.");
			return 1;
		}

		// Open output file 验证输出文件
		output_file = fopen(argv[4], "wb");
		if (!output_file) {
			printf("Could not open output file to write data.");
			return 1;
		}

		// Generate DES key set
		short int bytes_written, process_mode;
		unsigned long block_count = 0, number_of_blocks;
		unsigned char* data_block = (unsigned char*) malloc(8*sizeof(char));		// 每次加密的数据块（8个字节大小）
		unsigned char* processed_block = (unsigned char*) malloc(8*sizeof(char));
		key_set* key_sets = (key_set*)malloc(17*sizeof(key_set));					// ( 8(k) + 4(c) + 4(d) ) * 17个密钥集

		start = clock();
		generate_sub_keys(des_key, key_sets);	// 产生子密钥
		finish = clock();
		time_taken = (double)(finish - start)/(double)CLOCKS_PER_SEC;

		// Determine process mode
		if (strcmp(argv[1], ACTION_ENCRYPT) == 0) {
			process_mode = ENCRYPTION_MODE;
			printf("Encrypting..\n");
		} else {
			process_mode = DECRYPTION_MODE;
			printf("Decrypting..\n");
		}

		// Get number of blocks in the file 得到文件的块数（有多少个8字节）
		fseek(input_file, 0L, SEEK_END);
		file_size = ftell(input_file);	// 返回当前文件指针的位置

		fseek(input_file, 0L, SEEK_SET);	// 得到文件的大小后，将文件的指针复原到文件的开头
		number_of_blocks = file_size/8 + ((file_size%8)?1:0);	// 计算文件块的数目 file_size%8 != 0

		start = clock();

		// Start reading input file, process and write to output file
		while(fread(data_block, 1, 8, input_file)) {	// 从input_file中每次读取1个字节，读取8次到data_block中
			block_count++;
			if (block_count == number_of_blocks) {		// 处理最后一块
				if (process_mode == ENCRYPTION_MODE) {	// 加密
					padding = 8 - file_size%8;			// 填充的字符为:有几个未满
					if (padding < 8) { // Fill empty data block bytes with padding
						memset((data_block + 8 - padding), (unsigned char)padding, padding);	// 把后面未满的填充为padding个padding
					}

					process_message(data_block, processed_block, key_sets, process_mode);
					bytes_written = fwrite(processed_block, 1, 8, output_file);

					if (padding == 8) { // Write an extra block for padding
						memset(data_block, (unsigned char)padding, 8);
						process_message(data_block, processed_block, key_sets, process_mode);
						bytes_written = fwrite(processed_block, 1, 8, output_file);
					}
				} else {
					process_message(data_block, processed_block, key_sets, process_mode);
					padding = processed_block[7];

					if (padding < 8) {
						bytes_written = fwrite(processed_block, 1, 8 - padding, output_file);
					}
				}
			} else {
				process_message(data_block, processed_block, key_sets, process_mode);
				bytes_written = fwrite(processed_block, 1, 8, output_file);
			}
			memset(data_block, 0, 8);
		}

		finish = clock();

		// Free up memory
		free(des_key);
		free(data_block);
		free(processed_block);
		fclose(input_file);
		fclose(output_file);

		// Provide feedback
		time_taken = (double)(finish - start)/(double)CLOCKS_PER_SEC;
		printf("Finished processing %s. Time taken: %lf seconds.\n", argv[3], time_taken);
		return 0;
	} else {
		printf("Invalid action: %s. First parameter must be [ -g | -e | -d ].", argv[1]);
		return 1;
	}

	return 0;
}
