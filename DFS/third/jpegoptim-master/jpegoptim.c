#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <dirent.h>
#if HAVE_GETOPT_H && HAVE_GETOPT_LONG
#include <getopt.h>
#else
#include "getopt.h"
#endif
#include <signal.h>
#include <string.h>
#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>
#include <time.h>
#include <math.h>

#include "jpegoptim.h"

#define VERSIO "1.4.4"
#define COPYRIGHT  "Copyright (c) 1996-2016, Timo Kokkonen"

#define LOG_FH (logs_to_stdout ? stdout : stderr)

#define FREE_LINE_BUF(buf,lines)  {				\
    int j;							\
    for (j=0;j<lines;j++) free(buf[j]);				\
    free(buf);							\
    buf=NULL;							\
  }

#define STRNCPY(dest,src,n) { strncpy(dest,src,n); dest[n-1]=0; }

struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
	int jump_set;
};
typedef struct my_error_mgr * my_error_ptr;

const char *rcsid = "$Id: 478198edd3c2a9f96326151c4648dd44118ae09c $";

int verbose_mode = 0;
int quiet_mode = 0;
int global_error_counter = 0;
int preserve_mode = 0;
int preserve_perms = 0;
int overwrite_mode = 0;
int totals_mode = 0;
int stdin_mode = 0;		// 默认不是从标准输入读入需要压缩的数据
int stdout_mode = 0;
int noaction = 0;		// 否认，拒绝
int quality = -1;
int retry = 0;
int dest = 0;
int force = 0;
int save_exif = 1;
int save_iptc = 1;
int save_com = 1;
int save_icc = 1;
int save_xmp = 1;
int strip_none = 0;
int threshold = -1;
int csv = 0;
int all_normal = 0;
int all_progressive = 0;
int target_size = 0;
int logs_to_stdout = 1;

struct option long_options[] = { { "verbose", 0, 0, 'v' },
		{ "help", 0, 0, 'h' }, { "quiet", 0, 0, 'q' }, { "max", 1, 0, 'm' }, {
				"totals", 0, 0, 't' }, { "noaction", 0, 0, 'n' }, { "dest", 1,
				0, 'd' }, { "force", 0, 0, 'f' }, { "version", 0, 0, 'V' }, {
				"preserve", 0, 0, 'p' }, { "preserve-perms", 0, 0, 'P' }, {
				"strip-all", 0, 0, 's' }, { "strip-none", 0, &strip_none, 1 }, {
				"strip-com", 0, &save_com, 0 },
		{ "strip-exif", 0, &save_exif, 0 }, { "strip-iptc", 0, &save_iptc, 0 },
		{ "strip-icc", 0, &save_icc, 0 }, { "strip-xmp", 0, &save_xmp, 0 }, {
				"threshold", 1, 0, 'T' }, { "csv", 0, 0, 'b' }, { "all-normal",
				0, &all_normal, 1 },
		{ "all-progressive", 0, &all_progressive, 1 }, { "size", 1, 0, 'S' }, {
				"stdout", 0, &stdout_mode, 1 }, { "stdin", 0, &stdin_mode, 1 },
		{ 0, 0, 0, 0 } };

/*****************************************************************/

METHODDEF(void) my_error_exit(j_common_ptr cinfo) {
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	(*cinfo->err->output_message)(cinfo);
	if (myerr->jump_set)
		longjmp(myerr->setjmp_buffer, 1);
	else
		fatal("fatal error");
}

METHODDEF(void) my_output_message(j_common_ptr cinfo) {
	char buffer[JMSG_LENGTH_MAX + 1];

	if (verbose_mode) {
		(*cinfo->err->format_message)((j_common_ptr) cinfo, buffer);
		buffer[sizeof(buffer) - 1] = 0;
		fprintf(LOG_FH, " (%s) ", buffer);
	}
	global_error_counter++;
}

void print_usage(void) {
	fprintf(stderr, PROGRAMNAME " v" VERSIO "  " COPYRIGHT "\n");

	fprintf(stderr,
			"Usage: " PROGRAMNAME " [options] <filenames> \n\n"
			"  -d<path>, --dest=<path>\n"
			"                    specify alternative destination directory for \n"
			"                    optimized files (default is to overwrite originals)\n"
			"  -f, --force       force optimization\n"
			"  -h, --help        display this help and exit\n"
			"  -m<quality>, --max=<quality>\n"
			"                    set maximum image quality factor (disables lossless\n"
			"                    optimization mode, which is by default on)\n"
			"                    Valid quality values: 0 - 100\n"
			"  -n, --noaction    don't really optimize files, just print results\n"
			"  -S<size>, --size=<size>\n"
			"                    Try to optimize file to given size (disables lossless\n"
			"                    optimization mode). Target size is specified either in\n"
			"                    kilo bytes (1 - n) or as percentage (1%% - 99%%)\n"
			"  -T<threshold>, --threshold=<threshold>\n"
			"                    keep old file if the gain is below a threshold (%%)\n"
			"  -b, --csv         print progress info in CSV format\n"
			"  -o, --overwrite   overwrite target file even if it exists (meaningful\n"
			"                    only when used with -d, --dest option)\n"
			"  -p, --preserve    preserve file timestamps\n"
			"  -P, --preserve-perms\n"
			"                    preserve original file permissions by overwriting it\n"
			"  -q, --quiet       quiet mode\n"
			"  -t, --totals      print totals after processing all files\n"
			"  -v, --verbose     enable verbose mode (positively chatty)\n"
			"  -V, --version     print program version\n\n"
			"  -s, --strip-all   strip all markers from output file\n"
			"  --strip-none      do not strip any markers\n"
			"  --strip-com       strip Comment markers from output file\n"
			"  --strip-exif      strip Exif markers from output file\n"
			"  --strip-iptc      strip IPTC/Photoshop (APP13) markers from output file\n"
			"  --strip-icc       strip ICC profile markers from output file\n"
			"  --strip-xmp       strip XMP markers markers from output file\n"
			"\n"
			"  --all-normal      force all output files to be non-progressive\n"
			"  --all-progressive force all output files to be progressive\n"
			"  --stdout          send output to standard output (instead of a file)\n"
			"  --stdin           read input from standard input (instead of a file)\n"
			"\n\n");
}

void print_version() {
	struct jpeg_error_mgr jcerr, *err;

//  printf(PROGRAMNAME " v%s  %s\n",VERSIO,HOST_TYPE);
	printf(COPYRIGHT "\n");

	if (!(err = jpeg_std_error(&jcerr)))
		fatal("jpeg_std_error() failed");

	printf("\nlibjpeg version: %s\n%s\n", err->jpeg_message_table[JMSG_VERSION],
			err->jpeg_message_table[JMSG_COPYRIGHT]);
}

void own_signal_handler(int a) {
	if (verbose_mode > 1)
		fprintf(stderr, PROGRAMNAME ": signal: %d\n", a);
	exit(1);
}

void write_markers(struct jpeg_decompress_struct *dinfo,
		struct jpeg_compress_struct *cinfo) {
	jpeg_saved_marker_ptr mrk;
	int write_marker;

	if (!cinfo || !dinfo)
		fatal("invalid call to write_markers()");

	mrk = dinfo->marker_list;
	while (mrk) {
		write_marker = 0;
		/* 检查所需要保存的图片标签信息 check for markers to save... */
		if (save_com && mrk->marker == JPEG_COM)
			write_marker++;

		if (save_iptc && mrk->marker == IPTC_JPEG_MARKER)
			write_marker++;

		if (save_exif && mrk->marker == EXIF_JPEG_MARKER
				&& !memcmp(mrk->data, EXIF_IDENT_STRING,
						EXIF_IDENT_STRING_SIZE))
			write_marker++;

		if (save_icc && mrk->marker == ICC_JPEG_MARKER
				&& !memcmp(mrk->data, ICC_IDENT_STRING, ICC_IDENT_STRING_SIZE))
			write_marker++;

		if (save_xmp && mrk->marker == XMP_JPEG_MARKER
				&& !memcmp(mrk->data, XMP_IDENT_STRING, XMP_IDENT_STRING_SIZE))
			write_marker++;

		if (strip_none)
			write_marker++;

		/* libjpeg emits some markers automatically so skip these to avoid duplicates... */

		/* skip JFIF (APP0) marker */
		if (mrk->marker == JPEG_APP0 && mrk->data_length >= 14
				&& mrk->data[0] == 0x4a && mrk->data[1] == 0x46
				&& mrk->data[2] == 0x49 && mrk->data[3] == 0x46
				&& mrk->data[4] == 0x00)
			write_marker = 0;

		/* skip Adobe (APP14) marker */
		if (mrk->marker == JPEG_APP0 + 14 && mrk->data_length >= 12
				&& mrk->data[0] == 0x41 && mrk->data[1] == 0x64
				&& mrk->data[2] == 0x6f && mrk->data[3] == 0x62
				&& mrk->data[4] == 0x65)
			write_marker = 0;

		if (write_marker)
			jpeg_write_marker(cinfo, mrk->marker, mrk->data, mrk->data_length);

		mrk = mrk->next;
	}
}

/*****************************************************************/
unsigned char* jpegoptimMain(unsigned char * fileBuffer, int * compressedFileSize, int quality) {
//char* jpegoptimMain(unsigned char * fileBuffer, unsigned long fileSize, int quality) {
	unsigned long fileSize = *compressedFileSize;

	struct jpeg_decompress_struct dinfo;	// 解压缩实例的主记录
	struct jpeg_compress_struct cinfo;		// 压缩实例的主记录
	struct my_error_mgr jcerr, jderr;		// 错误信息
	JSAMPARRAY buf = NULL;					// JPEG图像的样本(二维矩阵unsigned char)
	jvirt_barray_ptr *coef_arrays = NULL;
	char marker_str[256];
	char tmpfilename[MAXPATHLEN], tmpdir[MAXPATHLEN];
	char newname[MAXPATHLEN], dest_path[MAXPATHLEN];
	volatile int i;
	int c, j, tmpfd, searchcount, searchdone;
	int opt_index = 0;
	long insize = 0, outsize = 0, lastsize = 0;	// insize全部换成fileSize
	int oldquality;
	double ratio;
	struct stat file_stat;
	jpeg_saved_marker_ptr cmarker;
	unsigned char *outbuffer = NULL;
	size_t outbuffersize;
	char *outfname = NULL;
	FILE *infile = NULL, *outfile = NULL;
	int marker_in_count, marker_in_size;
	int compress_err_count = 0;
	int decompress_err_count = 0;
	long average_count = 0;
	double average_rate = 0.0, total_save = 0.0;

	if (rcsid)
		; /* 不做任何事情表明编译器不会任何未使用过的rcsid字符串；so compiler won't complain about "unused" rcsid string */

	umask(077);	// 在创建文件时设置或者屏蔽掉文件的一些权限
	signal(SIGINT, own_signal_handler);
	signal(SIGTERM, own_signal_handler);

	/* 初始化压缩对象 initialize decompression object */
	dinfo.err = jpeg_std_error(&jderr.pub);
	jpeg_create_decompress(&dinfo);
	jderr.pub.error_exit = my_error_exit;
	jderr.pub.output_message = my_output_message;
	jderr.jump_set = 0;

	/* initialize compression object */
	cinfo.err = jpeg_std_error(&jcerr.pub);
	jpeg_create_compress(&cinfo);
	jcerr.pub.error_exit = my_error_exit;
	jcerr.pub.output_message = my_output_message;
	jcerr.jump_set = 0;

	if (quality < 0)
		quality = 0;
	if (quality > 100)
		quality = 100;

	/* 解析命令行参数 parse command line parameters */

	/* 检查有两个-的选项（意味着是从标准输入读入数据） check for '-' option indicating input is from stdin... */


	/* 循环处理输入文件 loop to process the input files */
	printf("循环开始\n");
	i = 1;
	do {
//		if (stdin_mode) {		// 如果是从标准输入中读入数据
//		} else {
//			// 重入点，后面跳转到这里
			retry_point:
//
//			if ((infile = fopen("./data/terrain.jpg", "rb")) == NULL) {	// 打开输入的图片数据文件
//				warn("cannot open file ./data/terrain.jpg");
//				continue;
//			}
//		}

		if (setjmp(jderr.setjmp_buffer)) {
			/* 处理解压缩错误 error handler for decompress */
			jpeg_abort_decompress(&dinfo);
			fclose(infile);
			if (buf)
				FREE_LINE_BUF(buf, dinfo.output_height);
			if (!quiet_mode || csv)
				fprintf(LOG_FH, csv ? ",,,,,error\n" : " [ERROR]\n");
			decompress_err_count++;
			jderr.jump_set = 0;
			continue;
		} else {
			jderr.jump_set = 1;
		}

//		if (!retry && (!quiet_mode || csv)) {
//			fprintf(LOG_FH, csv ? "%s," : "%s ",
//					(stdin_mode ? "stdin\n" : "argv[i]\n"));
//			fflush(LOG_FH);
//		}

		/* 准备解压缩 prepare to decompress */
		global_error_counter = 0;	// 全局错误计数器
		jpeg_save_markers(&dinfo, JPEG_COM, 0xffff);	// 保存JPEG图像的标记信息
		for (j = 0; j <= 15; j++)
			jpeg_save_markers(&dinfo, JPEG_APP0 + j, 0xffff);
//		jpeg_stdio_src(&dinfo, infile);
		jpeg_mem_src (&dinfo, fileBuffer, fileSize);	// 将原来的标准输入输出源，改为内存源
		jpeg_read_header(&dinfo, TRUE);					// 已经经过优化后的图像读不到头
//
//		/* 检查是否需要除去标记 check for Exif/IPTC/ICC/XMP markers */
		marker_str[0] = 0;
		marker_in_count = 0;
		marker_in_size = 0;
		cmarker = dinfo.marker_list;

		while (cmarker) {
			marker_in_count++;
			marker_in_size += cmarker->data_length;

			if (cmarker->marker == EXIF_JPEG_MARKER
					&& !memcmp(cmarker->data, EXIF_IDENT_STRING,
							EXIF_IDENT_STRING_SIZE))
				strncat(marker_str, "Exif ",
						sizeof(marker_str) - strlen(marker_str) - 1);

			if (cmarker->marker == IPTC_JPEG_MARKER)
				strncat(marker_str, "IPTC ",
						sizeof(marker_str) - strlen(marker_str) - 1);

			if (cmarker->marker == ICC_JPEG_MARKER
					&& !memcmp(cmarker->data, ICC_IDENT_STRING,
							ICC_IDENT_STRING_SIZE))
				strncat(marker_str, "ICC ",
						sizeof(marker_str) - strlen(marker_str) - 1);

			if (cmarker->marker == XMP_JPEG_MARKER
					&& !memcmp(cmarker->data, XMP_IDENT_STRING,
							XMP_IDENT_STRING_SIZE))
				strncat(marker_str, "XMP ",
						sizeof(marker_str) - strlen(marker_str) - 1);

			cmarker = cmarker->next;
		}

//		if (!retry && (!quiet_mode || csv)) {	// 第一次进入循环（不是重入），打出文件信息
//			fprintf(LOG_FH, csv ? "%dx%d,%dbit,%c," : "%dx%d %dbit %c ",
//					(int) dinfo.image_width, (int) dinfo.image_height,
//					(int) dinfo.num_components * 8,
//					(dinfo.progressive_mode ? 'P' : 'N'));
//
//			if (!csv) {
//				fprintf(LOG_FH, "%s", marker_str);
//				if (dinfo.saw_Adobe_marker)
//					fprintf(LOG_FH, "Adobe ");
//				if (dinfo.saw_JFIF_marker)
//					fprintf(LOG_FH, "JFIF ");
//			}
//			fflush(LOG_FH);
//		}
		printf("文件信息\n");

//		if ((insize = file_size(infile)) < 0)
//			fatal("failed to stat() input file");

		printf("开始解压缩文件\n");
		/* 解压缩文件 decompress the file */
		if (quality >= 0 && !retry) {
			jpeg_start_decompress(&dinfo);

			/* 分配行缓冲来存储解压缩的图像 allocate line buffer to store the decompressed image */
			buf = malloc(sizeof(JSAMPROW) * dinfo.output_height);
			if (!buf)
				fatal("分配行缓冲来存储解压缩的图像not enough memory");
			for (j = 0; j < dinfo.output_height; j++) {
				buf[j] = malloc(
						sizeof(JSAMPLE) * dinfo.output_width
								* dinfo.out_color_components);
				if (!buf[j])
					fatal("分配行缓冲来存储解压缩的图像2not enough memory");
			}

			while (dinfo.output_scanline < dinfo.output_height) {
				jpeg_read_scanlines(&dinfo, &buf[dinfo.output_scanline],
						dinfo.output_height - dinfo.output_scanline);
			}
		} else {
			coef_arrays = jpeg_read_coefficients(&dinfo);
		}
		printf("解压缩文件完成\n");

		if (!retry && !quiet_mode) {
			if (global_error_counter == 0)
				fprintf(LOG_FH, " [OK] ");
			else
				fprintf(LOG_FH, " [WARNING] ");
			fflush(LOG_FH);
		}
		printf("\n");

		if (dest && !noaction) {
			if (file_exists(newname) && !overwrite_mode) {
				warn("target file already exists: %s\n", newname);
				jpeg_abort_decompress(&dinfo);
				fclose(infile);
				if (buf)
					FREE_LINE_BUF(buf, dinfo.output_height);
				continue;
			}
		}

		if (setjmp(jcerr.setjmp_buffer)) {
			/* 处理错误或者压缩失败 error handler for compress failures */
			jpeg_abort_compress(&cinfo);
			jpeg_abort_decompress(&dinfo);
			fclose(infile);
			if (!quiet_mode)
				fprintf(LOG_FH, " [Compress ERROR]\n");
			if (buf)
				FREE_LINE_BUF(buf, dinfo.output_height);
			compress_err_count++;
			jcerr.jump_set = 0;
			continue;
		} else {
			jcerr.jump_set = 1;
		}
		printf("set jmp\n");

		lastsize = 0;
		searchcount = 0;
		searchdone = 0;
		oldquality = 200;

		binary_search_loop:
//
		/* 分配大的内存用于存储输出的JPEG图像； allocate memory buffer that should be large enough to store the output JPEG... */
		if (outbuffer)
			free(outbuffer);	// 输出图片的缓冲
		outbuffersize = fileSize + 32768;
		outbuffer = (unsigned char*)malloc((int)(outbuffersize));
		printf("用于存储输出的JPEG图像大小：%d\n", outbuffersize);
		if (!outbuffer)
			fatal("分配大的内存用于存储输出的JPEG图像not enough memory");
//
//		/* 建立定制的“目的管理器”用于libjpeg库写入我们的缓冲区; setup custom "destination manager" for libjpeg to write to our buffer */
		jpeg_memory_dest(&cinfo, &outbuffer, &outbuffersize, 65536);
		printf("分配缓冲成功222\n");

		if (quality >= 0 && !retry) {
			/* 有损优化 lossy "optimization" ... */
			cinfo.in_color_space = dinfo.out_color_space;
			cinfo.input_components = dinfo.output_components;
			cinfo.image_width = dinfo.image_width;
			cinfo.image_height = dinfo.image_height;
			jpeg_set_defaults(&cinfo);
			jpeg_set_quality(&cinfo, quality, TRUE);
			if (all_normal) {
				cinfo.scan_info = NULL; // 显示地关闭强制(如果libjpeg打开了它) Explicitly disables progressive if libjpeg had it on by default
				cinfo.num_scans = 0;
			} else if (dinfo.progressive_mode || all_progressive) {
				jpeg_simple_progression(&cinfo);
			}
			cinfo.optimize_coding = TRUE;

			j = 0;
			jpeg_start_compress(&cinfo, TRUE);	// 开始图片的压缩

			/* 写入标记信息 write markers */
			write_markers(&dinfo, &cinfo);

			/* 向缓冲写入图片 write image */
			while (cinfo.next_scanline < cinfo.image_height) {
				jpeg_write_scanlines(&cinfo, &buf[cinfo.next_scanline],
						dinfo.output_height);
			}

		} else {
			/* 无损优化 lossless "optimization" ... */

			jpeg_copy_critical_parameters(&dinfo, &cinfo);
			if (all_normal) {
				cinfo.scan_info = NULL; // Explicitly disables progressive if libjpeg had it on by default
				cinfo.num_scans = 0;
			} else if (dinfo.progressive_mode || all_progressive) {
				jpeg_simple_progression(&cinfo);
			}
			cinfo.optimize_coding = TRUE;

			/* write image */
			jpeg_write_coefficients(&cinfo, coef_arrays);

			/* write markers */
			write_markers(&dinfo, &cinfo);

		}
		printf("压缩并写入缓冲成功\n");


		jpeg_finish_compress(&cinfo);
		outsize = outbuffersize;
		printf("输出大小： %d\n", outsize);

		if (target_size != 0 && !retry) {
			/* 执行二进制搜索，尝试到达目标文件大小 perform (binary) search to try to reach target file size... */
			long osize = outsize / 1024;
			long isize = fileSize / 1024;
			long tsize = target_size;

			if (tsize < 0) {
				tsize = ((-target_size) * fileSize / 100) / 1024;
				if (tsize < 1)
					tsize = 1;
			}

			if (osize == tsize || searchdone || searchcount >= 8
					|| tsize > isize) {
				if (searchdone < 42 && lastsize > 0) {
					if (labs(osize - tsize) > labs(lastsize - tsize)) {
						if (verbose_mode)
							fprintf(LOG_FH, "(revert to %d)", oldquality);
						searchdone = 42;
						quality = oldquality;
						goto binary_search_loop;
					}
				}
				if (verbose_mode)
					fprintf(LOG_FH, " ");

			} else {
				int newquality;
				int dif = floor((abs(oldquality - quality) / 2.0) + 0.5);
				if (osize > tsize) {
					newquality = quality - dif;
					if (dif < 1) {
						newquality--;
						searchdone = 1;
					}
					if (newquality < 0) {
						newquality = 0;
						searchdone = 2;
					}
				} else {
					newquality = quality + dif;
					if (dif < 1) {
						newquality++;
						searchdone = 3;
					}
					if (newquality > 100) {
						newquality = 100;
						searchdone = 4;
					}
				}
				oldquality = quality;
				quality = newquality;

				if (verbose_mode)
					fprintf(LOG_FH, "(try %d)", quality);

				lastsize = osize;
				searchcount++;
				goto binary_search_loop;
			}
		}
		printf("执行二进制搜索成功\n");

		if (buf)
			FREE_LINE_BUF(buf, dinfo.output_height);
		jpeg_finish_decompress(&dinfo);
		printf("完成解压缩\n");

		if (quality >= 0 && outsize >= fileSize && !retry && !stdin_mode) {
			if (verbose_mode)
				fprintf(LOG_FH, "(retry w/lossless) ");
			retry = 1;
			goto retry_point;
		}
		printf("重入点跳回\n");

		retry = 0;
		ratio = (fileSize - outsize) * 100.0 / fileSize;
		if (!quiet_mode || csv)
			fprintf(LOG_FH,
					csv ? "%u,%ld,%0.2f," : "%u --> %ld bytes (%0.2f%%), ",
					fileSize, outsize, ratio);
		average_count++;
		average_rate += (ratio < 0 ? 0.0 : ratio);

		printf("异常处理开始\n");
//		if ((outsize < fileSize && ratio >= threshold) || force) {
//			total_save += (fileSize - outsize) / 1024.0;
//			if (!quiet_mode || csv)
//				fprintf(LOG_FH, csv ? "optimized\n" : "optimized.\n");
//			if (noaction)
//				continue;
//
//			if (stdout_mode) {
//			} else {
//				if (preserve_perms && !dest) {
//					/* make backup of the original file */
//					snprintf(tmpfilename, sizeof(tmpfilename),
//							"%s.jpegoptim.bak", newname);
//					if (verbose_mode > 1 && !quiet_mode)
//					if (file_exists(tmpfilename))
//					if (copy_file(newname, tmpfilename))
//					if ((outfile = fopen(newname, "wb")) == NULL)
//					outfname = newname;
//				} else {
//#ifdef HAVE_MKSTEMPS
//					/* rely on mkstemps() to create us temporary file safely... */
//					snprintf(tmpfilename,sizeof(tmpfilename),
//							"%sjpegoptim-%d-%d.XXXXXX.tmp", tmpdir, (int)getuid(), (int)getpid());
//					if ((tmpfd = mkstemps(tmpfilename,4)) < 0)
//					fatal("%s, error creating temp file %s: mkstemps() failed",(stdin_mode?"stdin":argv[i]),tmpfilename);
//					if ((outfile=fdopen(tmpfd,"wb"))==NULL)
//#else
//					/* if platform is missing mkstemps(), try to create at least somewhat "safe" temp file... */
//					snprintf(tmpfilename, sizeof(tmpfilename),
//							"%sjpegoptim-%d-%d.%ld.tmp", tmpdir, (int) getuid(),
//							(int) getpid(), (long) time(NULL));
//					tmpfd = 0;
//					if ((outfile = fopen(tmpfilename, "wb")) == NULL)
//#endif
//						fatal("error opening temporary file: %s", tmpfilename);
//					outfname = tmpfilename;
//				}
//
//				if (verbose_mode > 1 && !quiet_mode)
//					fprintf(LOG_FH, "writing %lu bytes to file: %s\n",
//							(long unsigned int) outbuffersize, outfname);
//				if (fwrite(outbuffer, outbuffersize, 1, outfile) != 1)
//					fatal("write failed to file: %s", outfname);
//				fclose(outfile);
//			}

//			if (outfname) {
//
//				if (preserve_mode) {
//					/* 保持文件修改时间不变preserve file modification time */
//					struct utimbuf time_save;
//					time_save.actime = file_stat.st_atime;
//					time_save.modtime = file_stat.st_mtime;
//					if (utime(outfname, &time_save) != 0)
//						warn("重置输出文件的时间失败failed to reset output file time/date");
//				}

//				if (preserve_perms && !dest) {
//					/* original file was already replaced, remove backup... */
//					if (delete_file(tmpfilename))
//						warn("failed to remove backup file: %s", tmpfilename);
//				} else {
//					/* make temp file to be the original file... */
//
//					/* preserve file mode */
//					if (chmod(outfname, (file_stat.st_mode & 0777)) != 0)
//						warn("failed to set output file mode");
//
//					/* preserve file group (and owner if run by root) */
//					if (chown(outfname,
//							(geteuid() == 0 ? file_stat.st_uid : -1),
//							file_stat.st_gid) != 0)
//						warn("重置输出文件的所有者失败failed to reset output file group/owner");
//
//					if (verbose_mode > 1 && !quiet_mode)
//						fprintf(LOG_FH, "renaming: %s to %s\n", outfname,
//								newname);
//					if (rename_file(outfname, newname))
//						fatal("cannot rename temp file");
//				}
//			}
//		} else {
//			if (!quiet_mode || csv)
//				fprintf(LOG_FH, csv ? "skipped\n" : "skipped.\n");
//		}

	} while (i<1);
	printf("完成异常判断\n");

	if (totals_mode && !quiet_mode)
		fprintf(LOG_FH,
				"Average " "compression" " (%ld files): %0.2f%% (%0.0fk)\n",
				average_count, average_rate / average_count, total_save);
	jpeg_destroy_decompress(&dinfo);
	jpeg_destroy_compress(&cinfo);

	printf("返回的文件大小: %u\n", outsize);
//	compressedFileSize = &outsize;
	*compressedFileSize = (unsigned long)outsize;
	printf("压缩函数返回\n");
//	*tmpP = outbuffer;
	return outbuffer;
}

/* :-) */
