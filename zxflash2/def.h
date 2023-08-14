#ifndef _DEF_H_
#define _DEF_H_


typedef int BOOL;
typedef int INT;
typedef unsigned char S3_U8;
typedef unsigned short S3_U16;
typedef unsigned long S3_U32;
typedef unsigned int S3_UINT, S3_UPTR;
typedef void * S3_PTR;
typedef unsigned char S3_BOOL;

typedef char CHAR;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef long LONG;
typedef unsigned long ULONG;

typedef void * PVOID;

typedef unsigned char * PUCHAR;
typedef unsigned short * PUSHORT;
typedef unsigned long * PULONG;

#define TRUE 1
#define FALSE 0

#define DebugPrint printf

struct test_vector {
	char vector_name[64];
	char usage_str[200];
	int(*test_func)(int argc, char **argv);
};

#endif