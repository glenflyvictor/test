#ifndef _FLASH_H_
#define _FLASH_H_

#define NORMAL_EXIT 0
#define CHECKSUM_ERROR 1
#define HARDWARE_ERROR 9
#define SYNTAX_ERROR 10
#define GENERAL_FILE_ERROR 11
#define ROM_FILE_ERROR 12
#define FLASH_ERROR 20

#define MAX_VIDEO_ROM_SIZE 0x50000 // 64k+256k
#define FP_MD5_SIZE 16	//size of flash part page (in bytes)
#define FP_PAGE_SIZE 128	//size of flash part page (in bytes)
#define FP_SECTOR_SIZE 4096	//size of flash part sector (in bytes)
#define FP_BLOCK_SIZE 32768	//size of flash part block (in bytes)


int flash_main(int argc, char **argv);
int dump_main(int argc, char **argv);
int info_main(int argc, char **argv); 

#endif
