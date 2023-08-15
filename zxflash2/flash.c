#include "def.h"
#include "flash.h"
#include <i86.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <conio.h>
#include <stdarg.h>
#include "spi_flash.h"
#include <malloc.h>
#include "md5.h"
#include <memory.h>



char Value_MD5[20];
extern  vendorID;
extern  deviceID;


// md5.c

unsigned char PADDING[]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
void MD5Init(MD5_CTX *context)
{
    context->count[0] = 0;
    context->count[1] = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
}
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen)
{
    unsigned int i = 0,index = 0,partlen = 0;
    index = (context->count[0] >> 3) & 0x3F;
    partlen = 64 - index;
    context->count[0] += inputlen << 3;
    if(context->count[0] < (inputlen << 3))
        context->count[1]++;
    context->count[1] += inputlen >> 29;
    if(inputlen >= partlen)
    {
        memcpy(&context->buffer[index],input,partlen);
        MD5Transform(context->state,context->buffer);
        for(i = partlen;i+64 <= inputlen;i+=64)
            MD5Transform(context->state,&input[i]);
        index = 0;      
    }
    else
    {
        i = 0;
    }
    memcpy(&context->buffer[index],&input[i],inputlen-i);
}
void MD5Final(MD5_CTX *context,unsigned char digest[16])
{
    unsigned int index = 0,padlen = 0;
    unsigned char bits[8];
    index = (context->count[0] >> 3) & 0x3F;
    padlen = (index < 56)?(56-index):(120-index);
    MD5Encode(bits,context->count,8);
    MD5Update(context,PADDING,padlen);
    MD5Update(context,bits,8);
    MD5Encode(digest,context->state,16);
}
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len)
{
    unsigned int i = 0,j = 0;
    while(j < len)
    {
        output[j] = input[i] & 0xFF;
        output[j+1] = (input[i] >> 8) & 0xFF;
        output[j+2] = (input[i] >> 16) & 0xFF;
        output[j+3] = (input[i] >> 24) & 0xFF;
        i++;
        j+=4;
    }
}
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len)
{
    unsigned int i = 0,j = 0;
    while(j < len)
    {
        output[i] = (input[j]) |
            (input[j+1] << 8) |
            (input[j+2] << 16) |
            (input[j+3] << 24);
        i++;
        j+=4;
    }
}
void MD5Transform(unsigned int state[4],unsigned char block[64])
{
    unsigned int a = state[0];
    unsigned int b = state[1];
    unsigned int c = state[2];
    unsigned int d = state[3];
    unsigned int x[64];
    MD5Decode(x,block,64);
    FF(a, b, c, d, x[ 0], 7, 0xd76aa478); /* 1 */
    FF(d, a, b, c, x[ 1], 12, 0xe8c7b756); /* 2 */
    FF(c, d, a, b, x[ 2], 17, 0x242070db); /* 3 */
    FF(b, c, d, a, x[ 3], 22, 0xc1bdceee); /* 4 */
    FF(a, b, c, d, x[ 4], 7, 0xf57c0faf); /* 5 */
    FF(d, a, b, c, x[ 5], 12, 0x4787c62a); /* 6 */
    FF(c, d, a, b, x[ 6], 17, 0xa8304613); /* 7 */
    FF(b, c, d, a, x[ 7], 22, 0xfd469501); /* 8 */
    FF(a, b, c, d, x[ 8], 7, 0x698098d8); /* 9 */
    FF(d, a, b, c, x[ 9], 12, 0x8b44f7af); /* 10 */
    FF(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */
    FF(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */
    FF(a, b, c, d, x[12], 7, 0x6b901122); /* 13 */
    FF(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */
    FF(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */
    FF(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */
    /* Round 2 */
    GG(a, b, c, d, x[ 1], 5, 0xf61e2562); /* 17 */
    GG(d, a, b, c, x[ 6], 9, 0xc040b340); /* 18 */
    GG(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */
    GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa); /* 20 */
    GG(a, b, c, d, x[ 5], 5, 0xd62f105d); /* 21 */
    GG(d, a, b, c, x[10], 9,  0x2441453); /* 22 */
    GG(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */
    GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8); /* 24 */
    GG(a, b, c, d, x[ 9], 5, 0x21e1cde6); /* 25 */
    GG(d, a, b, c, x[14], 9, 0xc33707d6); /* 26 */
    GG(c, d, a, b, x[ 3], 14, 0xf4d50d87); /* 27 */
    GG(b, c, d, a, x[ 8], 20, 0x455a14ed); /* 28 */
    GG(a, b, c, d, x[13], 5, 0xa9e3e905); /* 29 */
    GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8); /* 30 */
    GG(c, d, a, b, x[ 7], 14, 0x676f02d9); /* 31 */
    GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */
    /* Round 3 */
    HH(a, b, c, d, x[ 5], 4, 0xfffa3942); /* 33 */
    HH(d, a, b, c, x[ 8], 11, 0x8771f681); /* 34 */
    HH(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */
    HH(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */
    HH(a, b, c, d, x[ 1], 4, 0xa4beea44); /* 37 */
    HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9); /* 38 */
    HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60); /* 39 */
    HH(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */
    HH(a, b, c, d, x[13], 4, 0x289b7ec6); /* 41 */
    HH(d, a, b, c, x[ 0], 11, 0xeaa127fa); /* 42 */
    HH(c, d, a, b, x[ 3], 16, 0xd4ef3085); /* 43 */
    HH(b, c, d, a, x[ 6], 23,  0x4881d05); /* 44 */
    HH(a, b, c, d, x[ 9], 4, 0xd9d4d039); /* 45 */
    HH(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */
    HH(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */
    HH(b, c, d, a, x[ 2], 23, 0xc4ac5665); /* 48 */
    /* Round 4 */
    II(a, b, c, d, x[ 0], 6, 0xf4292244); /* 49 */
    II(d, a, b, c, x[ 7], 10, 0x432aff97); /* 50 */
    II(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */
    II(b, c, d, a, x[ 5], 21, 0xfc93a039); /* 52 */
    II(a, b, c, d, x[12], 6, 0x655b59c3); /* 53 */
    II(d, a, b, c, x[ 3], 10, 0x8f0ccc92); /* 54 */
    II(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */
    II(b, c, d, a, x[ 1], 21, 0x85845dd1); /* 56 */
    II(a, b, c, d, x[ 8], 6, 0x6fa87e4f); /* 57 */
    II(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */
    II(c, d, a, b, x[ 6], 15, 0xa3014314); /* 59 */
    II(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */
    II(a, b, c, d, x[ 4], 6, 0xf7537e82); /* 61 */
    II(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */
    II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); /* 63 */
    II(b, c, d, a, x[ 9], 21, 0xeb86d391); /* 64 */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

BOOL FileExist(char *szFname, unsigned short nType)
{
    struct _stat fbuf;

    if (_stat(szFname, &fbuf)) {
        return FALSE;
    }
    if (!(fbuf.st_mode & nType)) {
        //if not an ordinary file
        return FALSE;
    } else
        return TRUE;
}

long GetFileSizeUser(char *szFname)
{
    struct _stat fbuf;

    if (_stat(szFname, &fbuf)) {
        return 0;
    } else
        return fbuf.st_size;
}
FILE *OpenFileUser(char *szFname, char *szMode)
{
    return fopen(szFname, szMode);
}


int FlashRom(FILE *pStream, LONG dwFileSize)
{
    int i,j;
    BOOL bWriteOk=TRUE;
    UCHAR pBuffer[FP_SECTOR_SIZE];
    unsigned int readdata;
    ULONG sectorNum;
    
    int erase_pmp_index=0;
    
    fseek(pStream, 0, SEEK_SET);
    
    //  base on file size, round up to an integer of sector num  
    sectorNum = (dwFileSize / FP_SECTOR_SIZE);
    if(dwFileSize>sectorNum*FP_SECTOR_SIZE)
        sectorNum++;
    
    //write hex file to rom
    //printf("sector offset:%d ,file size:%d,sectorNum:%d\n",pChipInfo->fpInfo.dwSector,dwFileSize,sectorNum);
    for(j=0; j<sectorNum; j++)
    {
        // erase rom
        sf_sector_erase(j*FP_SECTOR_SIZE);        
        //flash rom         
        // read a sector from file
        if (fread(pBuffer, sizeof(UCHAR), FP_SECTOR_SIZE, pStream)
            != FP_SECTOR_SIZE && (j<(dwFileSize / FP_SECTOR_SIZE))) {
                
                DebugPrint("\nVideo BIOS image File reading failure\n");
                break;
        }
        //write
        for(i=0;i<FP_SECTOR_SIZE;i+=4)
        {
            sf_write_data(j*FP_SECTOR_SIZE +i,*(unsigned int *)(pBuffer + i),4);
        }  
        //read out and verify
        for(i=0;i<FP_SECTOR_SIZE;i+=4)
        {
            readdata = sf_read_data(j*FP_SECTOR_SIZE +i,4);
            if(readdata!=*(unsigned int *)(pBuffer + i))
            {
                printf("Write Data error: addr:%08x, write data:%08x, read back:%08x\n",j*FP_SECTOR_SIZE +i,*(unsigned int *)(pBuffer + i),readdata);
                return -1;
            }
            //sf_write_data(j*FP_SECTOR_SIZE +i,*(unsigned int *)(pBuffer + i),4);
        }

        if((j*100/sectorNum)%10==0)
            printf("%d%%\n",(j*100/sectorNum));
        //bWriteOk = SectorWriteSerialChip(&pChipInfo->fpInfo, pBuffer);
    }
    printf("%d%%\n",100);
    return 0;
}
//flash vbios
int flash_main(int argc, char **argv)
{
    unsigned char romImageName[128];
    unsigned char romImage_Name2[128];
    long i;
    long fileSize;
    S3_U8 *pmpBuf[FP_MD5_SIZE];
    FILE *pStream_M = NULL;
    FILE *pStream = NULL;
    char *Ptr =NULL;
    unsigned int checksum_flag=0;
    Ptr =(char *)malloc(MAX_VIDEO_ROM_SIZE*sizeof(char));
    if (Ptr==NULL){
	 printf("ptr malloc is wrong");
        exit(GENERAL_FILE_ERROR);
    }

    if(argc!=3)
    {
        exit(GENERAL_FILE_ERROR);
    }
    strcpy(romImageName,argv[2]);
    if (!FileExist(romImageName, _S_IFREG)) 
    {
        DebugPrint("\nBIOS image file \"%s\" doesn't exist\n", romImageName);
        exit(GENERAL_FILE_ERROR);
    }
/*
    fileSize = GetFileSizeUser(romImageName);
    if ((pStream = OpenFileUser(romImageName, "rb")) == NULL)
    {
        printf("Can't open rom %s\n",romImageName);
        exit(ROM_FILE_ERROR);
    }
*/	//********check device id***************
    Check_Did(romImageName);
    //********added by cheerychen ;for check md5;2022.3.1**************************
    //1��find firmware MD5 for 0XF0B4;pmpbuff is MD5;//check 0Xf003==0X02;0X02 is for MD5;
    checksum_flag=checksum(romImageName);   
    if (checksum_flag==1){
    FM_md5_Info(romImageName);
    //2\   change  MD5 =>0 in init file rom;
    strcpy(romImage_Name2,"GFINIT.ROM");
    Change_Rom(romImageName,romImage_Name2);
    //3\MD5 caculate;
    MD5_Caculate(romImage_Name2,Ptr);
    }
    else{
       printf("use checksum verification\n");
    }
    
   //for debug;
   fileSize = GetFileSizeUser(romImageName);
    if ((pStream = OpenFileUser(romImageName, "rb")) == NULL)
    {
        printf("Can't open rom %s\n",romImageName);
        exit(ROM_FILE_ERROR);
    }
    if(FlashRom(pStream,fileSize)==0)
    {
        printf("Flash successfully!\n");
    }
    else
    {
        printf("Flash failed\n");
    }
   
   
    fclose(pStream);

    return 0;
}
int MD5_Caculate(char* romImage_Name2,char *Ptr)
{
       long fileSize;
	 FILE *pStream_M = NULL;
	 S3_U8 *pmpBuf_checksum[1];
	 unsigned char decrypt[16]; 
	 int i;
	 MD5_CTX md5;
        MD5Init(&md5); 
	 //fileSize = GetFileSizeUser(romImageName);
       if ((pStream_M = OpenFileUser(romImage_Name2, "rb")) == NULL)
      {
        printf("Can't open rom %s\n",romImage_Name2);
        exit(ROM_FILE_ERROR);
      }
    fseek(pStream_M , 0x0, SEEK_SET); 
    fread(Ptr,sizeof(UCHAR), MAX_VIDEO_ROM_SIZE, pStream_M );
    //printf("\n%s",(char*)(Ptr + 0x24));
    MD5Update(&md5,Ptr,MAX_VIDEO_ROM_SIZE);
    MD5Final(&md5,decrypt);
    fclose(pStream_M);
    for(i=0;i<16;i++)
    {
        //printf("%02x",decrypt[i]);
	 if (Value_MD5[i]!=decrypt[i]){
           printf("MD5 Error: ROM file caculte not match MD5 !\n");
	     exit(ROM_FILE_ERROR);
        }
    }
    printf("MD5 match: ROM file match MD5 !\n");
    
}
int Check_Did(char* romImageName)
{
       long fileSize;
	 FILE *pStream = NULL;
	 S3_U8 *pmpBuf_checkDid[2];
	 fileSize = GetFileSizeUser(romImageName);
       if ((pStream = OpenFileUser(romImageName, "rb")) == NULL)
      {
        printf("Can't open rom %s\n",romImageName);
        exit(ROM_FILE_ERROR);
       }
	fseek(pStream, 0XF00a, SEEK_SET); 
       fread(pmpBuf_checkDid,sizeof(UCHAR), 2, pStream);
	 //printf("check device is %8.8x",pmpBuf_checkDid[0]);
	 //printf("check device is %8.8x",pmpBuf_checkDid[1]);
	///strcpy(check_deviceid, pmpBuf_checkDid);
       if(pmpBuf_checkDid[0]!=deviceID)
	 {
		printf("Device ID not match ROM FILE in offset 0xf00a\n");
		printf("check device_id is %4.4x",deviceID);
		exit(GENERAL_FILE_ERROR);
	 }
	 else{
            printf("DID match :Device ID match ROM FILE in offset 0xf00a\n");
	}
	
	fclose(pStream);
}
int checksum(char* romImageName)
{
       long fileSize;
	 FILE *pStream = NULL;
	 S3_U8 *pmpBuf_checksum[1];
	 char buildTime[1];
	 unsigned int checksum_flag=0;
	 fileSize = GetFileSizeUser(romImageName);
       if ((pStream = OpenFileUser(romImageName, "rb")) == NULL)
      {
        printf("Can't open rom %s\n",romImageName);
        exit(ROM_FILE_ERROR);
       }
	fseek(pStream, 0XF002, SEEK_SET); 
       fread(pmpBuf_checksum,sizeof(UCHAR), 1, pStream);
	strcpy(buildTime, pmpBuf_checksum);
       if ( buildTime[0]!= 0x02)
	 {
		printf("NO md5!");
		
		//exit(GENERAL_FILE_ERROR);
	 }
	 else{
            checksum_flag=1;
	 }
	fclose(pStream);
	return checksum_flag;
}
int Change_Rom(char *romImageName1,char* romImageName2)
{
    UCHAR pBuffer[FP_SECTOR_SIZE];
    UCHAR pBuffer_md5[16]={0};
   // UCHAR pBuffer_all[MAX_VIDEO_ROM_SIZE];
    long i,j;
    ULONG sectorNum;
    FILE *pStream_w;
    FILE *pStream_2;

    if ((pStream_w = OpenFileUser(romImageName2, "wb+")) == NULL)
    {
        printf("Can't creat file %s\n",romImageName2);
        exit(GENERAL_FILE_ERROR);
    } 
    if ((pStream_2 = OpenFileUser(romImageName1, "rb")) == NULL)
    {
        printf("Can't creat file %s\n",romImageName1);
        exit(GENERAL_FILE_ERROR);
    } 

    sectorNum = (MAX_VIDEO_ROM_SIZE / FP_SECTOR_SIZE);
    if(MAX_VIDEO_ROM_SIZE>sectorNum*FP_SECTOR_SIZE)
        sectorNum++;
    for(i=0;i<sectorNum;i++) 
    {
        fseek(pStream_2, i*FP_SECTOR_SIZE, SEEK_SET); 
	  fread(pBuffer,sizeof(UCHAR), FP_SECTOR_SIZE, pStream_2);
	  /*
        for(j=0;j<FP_SECTOR_SIZE;j++)
        {
            //pBuffer[j]=sf_read_data(i*FP_SECTOR_SIZE+j,1);
             
            
		strcpy(pBuffer_all,pBuffer);
        }
        */
        fseek(pStream_w, i*FP_SECTOR_SIZE, SEEK_SET);
        fwrite(pBuffer, sizeof(UCHAR), FP_SECTOR_SIZE, pStream_w);   
    }
    fseek(pStream_w, 0XF0B4, SEEK_SET); 
    fwrite(pBuffer_md5, sizeof(UCHAR), 16, pStream_w);   
    fclose(pStream_2);
    fclose(pStream_w);
}
int FM_md5_Info(char* romImageName)    //check   for vs
{
	
	int i;
	FILE *pStream_MD5INIF;
	S3_U8 *pmpBuf[FP_MD5_SIZE];
	 if ((pStream_MD5INIF = OpenFileUser(romImageName, "rb")) == NULL)
      {
        printf("Can't open rom %s\n",romImageName);
        exit(ROM_FILE_ERROR);
       }
	fseek(pStream_MD5INIF, 0XF0B4, SEEK_SET); 
       fread(pmpBuf,sizeof(UCHAR), FP_MD5_SIZE, pStream_MD5INIF);
	strcpy(Value_MD5, pmpBuf);
	for ( i =0;i<16;i++){
	    //printf("\t%2.2x",  Value_MD5[i]);
	}
	fclose(pStream_MD5INIF);
}

int DumpRom(char* romImageName)
{
    UCHAR pBuffer[FP_SECTOR_SIZE];
    long i,j;
    ULONG sectorNum;
    FILE *pStream;

    if ((pStream = OpenFileUser(romImageName, "wb")) == NULL)
    {
        printf("Can't creat file %s\n",romImageName);
        return FALSE;
    } 

    sectorNum = (MAX_VIDEO_ROM_SIZE / FP_SECTOR_SIZE);
    if(MAX_VIDEO_ROM_SIZE>sectorNum*FP_SECTOR_SIZE)
        sectorNum++;
    for(i=0;i<sectorNum;i++) 
    {
        for(j=0;j<FP_SECTOR_SIZE;j++)
        {
            pBuffer[j]=sf_read_data(i*FP_SECTOR_SIZE+j,1);
        }
        fwrite(pBuffer, sizeof(UCHAR), FP_SECTOR_SIZE, pStream);
        
    }
    fclose(pStream);
}
//dump vbios
int dump_main(int argc, char **argv) 
{
    unsigned char romImageName[128];
 

    if(argc!=3)
    {
        return -1;
    }
    strcpy(romImageName,argv[2]);

    DumpRom(romImageName);
    
    return 0;
}   

//dump rom info
/*
pmp string 
start addr: 0x20
first string:pmp
second str: version
third  str: build time
the string length is not fixed
*/
int VerifyPmpInfo(char* pRom)
{
	char buildDate[20],buildTime[20];
	char strIndex = 0x20;
	//verify pmp
	//1. 0x20开始有一个字符串"pmp",通过该字符串判断是不是pmp.bin
	if (strcmp(pRom + strIndex, "pmp") != 0)
	{
		printf("PMP Format Error: Cannot find string \"pmp\" at 0x20 !\n");
		return -1;
	}	
	//2. print version
	strIndex = strIndex+strlen(pRom + strIndex) + 1;
	if (strlen(pRom + strIndex) > 0)
	{
		printf("PMP Version:\t%s\n",(char*)(pRom + strIndex));
	}
	//3. print build time
	strIndex += strlen(pRom + strIndex) + 1;
	strcpy(buildDate, pRom + strIndex);
	strIndex += strlen(pRom + strIndex) + 1;
	strcpy(buildTime, pRom + strIndex);
	printf("Build Time:\t%s - %s\n", buildDate, buildTime);
	return 0;
}
void dump_rom_part(char *filename,unsigned int offset, unsigned char*buff,unsigned int length)
{
    unsigned int len;
    int fileSize;
    FILE *pStream = NULL;
    int i;
    len = length+(length%4);
    if(strlen(filename)>0)
    {
        //load form rom file

        if (!FileExist(filename, _S_IFREG)) 
        {
            DebugPrint("\nBIOS image file \"%s\" doesn't exist\n", filename);
            exit(GENERAL_FILE_ERROR);
        }
        fileSize = GetFileSizeUser(filename);
        if ((pStream = OpenFileUser(filename, "rb")) == NULL)
        {
            printf("Can't open rom %s\n",filename);
            exit(ROM_FILE_ERROR);
        }  
        fseek(pStream, offset, SEEK_SET); 
        fread(buff,sizeof(UCHAR), len, pStream);
        
    }
    else
    {
        //load form card
        for(i=0;i<len;i+=4)
        {
            *(unsigned int*)(buff + i)= sf_read_data(offset+i,4);
        }
    }
    
}
void dump_info(char *filename)
{
    S3_U8 *pmpBuf[FP_PAGE_SIZE];


    //print pmp info
    printf("\n\nPMP Info:\n");
    dump_rom_part(filename,0,pmpBuf,FP_PAGE_SIZE);
    //PageReadFlashChip(&pPrimary->fpInfo, pmpBuf);
    VerifyPmpInfo(pmpBuf);
    printf("\n");

    //print vbios info
}
int info_main(int argc, char **argv) 
{
    unsigned char romImageName[128]={'\0'};
 
    
    if(argc!=3 && argc != 2)
    {
        printf("argc:%d\n",argc);
        return -1;
    }
    if(argc == 3)
    {
       strcpy(romImageName,argv[2]);
    }
    dump_info(romImageName);
    
    return 0;
} 

