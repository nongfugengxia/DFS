CC      = gcc


all:
	cd DES && make && cd ..							#编译DES加密的代码，生成.o文件
	cd third/jpegoptim-master && make && cd ../..	# 编译jpegoptim
	@sh ./make.sh	# 编译FastDFS源代码（包括client）
	cd tools && make all && cd ..	#编译tools包里的代码
	cd middle && make all && cd ..	# 编译middle里的代码
	
	cd ./third/iniparser-master && make check && make && make example && cd ../..	#编译./thrid/iniparser-master中的代码
	
	cd server && make && cd ..		# 编译Server
	
	cd test && make && cd ..		# 编译FastDFS的性能测试程序
	

clean:
	@sh ./make.sh clean		#清除FastDFS编译的结果
	cd server && make clean && cd ..	#清除server的编译结果
	cd server/DFSClient && make clean && cd ..	#清除server/DFSClient的编译结果
	cd test && make clean && cd ..				# 清除FastDFS性能测试的编译结果
	cd DES && make clean && cd ..				# 清除DES编译的结果
	cd middle && make clean && cd ..			# 清除middle的编译结果
	cd tools && make clean && cd ..
	
	
# 通过查找gcc的环境变量C_INCLUDE_PATH/CPLUS_INCLUDE_PATH/OBJC_INCLUDE_PATH来搜索头文件位置