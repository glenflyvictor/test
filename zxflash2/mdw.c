/************************************************************/
/*	Project		:	MMIO Read/Write for CHX001			 	*/
/*	Author		:	Janice Yi								*/
/*	Date		:	Oct. 25th, 2016							*/
/*	Rev.		:	1.0.0.2									*/
/*	Compiler	:	Watcom (DOS4GW)							*/
/************************************************************/

#include <conio.h>
#include <stdio.h>

#define PCI_VENDORID_S3		0x5333
#define PCI_VENDORID_VIA	0x1d17
#define PCI_VENDORID_716	0x0716

#define PCI_DEVICEID1_D4	0x9060
#define PCI_DEVICEID2_D4	0x9040
#define PCI_DEVICEID3_D4	0x9050
#define PCI_DEVICEID4_D4	0x9070
#define PCI_DEVICEID_UMA	0x6120
#define PCI_DEVICEID_VT3456	0x3a00
#define PCI_DEVICEID_CHX001	0x3a00
#define PCI_DEVICEID_GRAY	0x3D00
#define PCI_DEVICEID_716	0x0012
#define PCI_DEVICEID_Ari1020	0x3D02

unsigned char ReadPciCfgByte(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg);
unsigned short ReadPciCfgWord(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg);
unsigned int ReadPciCfgDword(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg);
void WritePciCfgByte(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned char v);
void WritePciCfgWord(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned short v);
void WritePciCfgDword(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned int v);
unsigned int GetMMIOBase();
int ReadMMIO(unsigned int addr,int size);
void WriteMMIO(unsigned int addr,unsigned int value,int size);
unsigned int StoH(unsigned char * s);
void Delay1us(unsigned long x);

typedef enum
{	
	S3X_BYTE =0,
	S3X_WORD =1,
	S3X_DWORD=2,
}SIZE;

unsigned int g_mmiobase = 0;

unsigned char ReadPciCfgByte(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg)
{
	unsigned char v;
	unsigned char flag = 0;

	_asm pushfd;
	_asm pop flag;
	_asm cli;
	outpd(0xCF8, (0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC)));
	v = inp(0xCFC+(reg&3));
	_asm push flag;
	_asm popfd;

	return v;
}

unsigned short ReadPciCfgWord(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg)
{
	unsigned short v;
	unsigned int flag = 0;

	_asm pushfd;
	_asm pop flag;
	_asm cli;
	outpd(0xCF8, (0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC)));
	v = inpw(0xCFC+(reg&3));
	_asm push flag;
	_asm popfd;

	return v;
}

unsigned int ReadPciCfgDword(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg)
{
	unsigned int v, flag = 0;

	_asm pushfd;
	_asm pop flag;
	_asm cli;
	outpd(0xCF8, (0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC)));
	v = inpd(0xCFC);
	_asm push flag;
	_asm popfd;

	return v;
}

void WritePciCfgByte(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned char v)
{
	unsigned int flag = 0;

	_asm pushfd;
	_asm pop flag;
	_asm cli;
	outpd(0xCF8, (0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC)));
	outp(0xCFC+(reg&3), v);
	_asm push flag;
	_asm popfd;
}

void WritePciCfgWord(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned short v)
{
	unsigned int flag = 0;

	_asm pushfd;
	_asm pop flag;
	_asm cli;
	outpd(0xCF8, (0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC)));
	outpw(0xCFC+(reg&3), v);
	_asm push flag;
	_asm popfd;
}

void WritePciCfgDword(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned int v)
{
	unsigned int flag = 0;

	_asm pushfd;
	_asm pop flag;
	_asm cli;
	outpd(0xCF8, (0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC)));
	outpd(0xCFC, v);
	_asm push flag;
	_asm popfd;
}

//get the first adapter's mmio base address from pci config space
unsigned int GetMMIOBase()
{
	unsigned char bus, dev;
	unsigned int MMIOBaseAddress;
	unsigned short vendorID;
	unsigned short deviceID;
	unsigned short PCIcmd;
	unsigned int  classCode;

		
	for ( bus = 0; bus < 0x3f; bus++)
	{
		for ( dev = 0; dev < 32; dev++)
		{
			vendorID = ReadPciCfgWord(bus, dev, 0, 0x00);
			
			if (vendorID != PCI_VENDORID_VIA && vendorID != PCI_VENDORID_S3&& vendorID != PCI_VENDORID_716)
			{
				continue;
			}

			classCode = ReadPciCfgDword(bus, dev, 0, 0x08);
			if ((classCode & 0xFF000000) == 0x03000000)
			{
				PCIcmd = ReadPciCfgDword(bus, dev, 0, 0x04);
				printf("PCIcmd:0x%04x\n",PCIcmd);
				if(PCIcmd&0x02)
				{
					MMIOBaseAddress = ReadPciCfgDword(bus, dev, 0, 0x10);
					MMIOBaseAddress &= 0xFFFFFF00;
					return MMIOBaseAddress;
				}
			}

		}
	}
	return -1;


//	MMIOBaseAddress = ReadPciCfgDword(0, 1, 0, 0x10);
//	MMIOBaseAddress &= 0xFFFF0000;
	return MMIOBaseAddress;
}
unsigned int GetFBBase()
{
	unsigned char bus, dev;
	unsigned int FBBaseAddress;
	unsigned short vendorID;
	unsigned short deviceID;
	unsigned short PCIcmd;
	unsigned int  classCode;

		
	for ( bus = 0; bus < 0x3f; bus++)
	{
		for ( dev = 0; dev < 32; dev++)
		{
			vendorID = ReadPciCfgWord(bus, dev, 0, 0x00);
			
			if (vendorID != PCI_VENDORID_VIA && vendorID != PCI_VENDORID_S3&& vendorID != PCI_VENDORID_716)
			{
				continue;
			}

			classCode = ReadPciCfgDword(bus, dev, 0, 0x08);
			if ((classCode & 0xFF000000) == 0x03000000)
			{
				PCIcmd = ReadPciCfgDword(bus, dev, 0, 0x04);
				printf("PCIcmd:0x%04x\n",PCIcmd);
				if(PCIcmd&0x02)
				{
					FBBaseAddress = ReadPciCfgDword(bus, dev, 0, 0x14);
					FBBaseAddress &= 0xFFFFFF00;
					return FBBaseAddress;
				}
			}

		}
	}
	return -1;
}
int ReadMMIO(unsigned int addr,int size)
{
	int value = -1;

	switch(size)
	{
		case 0:
			value = *(unsigned char*)(g_mmiobase + addr);
			break;
			
		case 1:
			value = *(unsigned short*)(g_mmiobase+addr);
			break;
			
		case 2:
			value = *(unsigned int*)(g_mmiobase+addr);
	}

	return value;

}

void WriteMMIO(unsigned int addr,unsigned int value,int size)
{

	switch(size)
	{
		case S3X_BYTE:
			*(unsigned char*)(g_mmiobase + addr) = (unsigned char)(value&0xFF);
			break;
			
		case S3X_WORD:
			*(unsigned short*)(g_mmiobase+addr) = (unsigned short)(value&0xFFFF);
			break;
			
		case S3X_DWORD:
			*(unsigned int*)(g_mmiobase+addr) = value;
	}
}

unsigned int StoH(unsigned char * s)
{
	unsigned int hdata;
	int i=0;
	hdata = 0;
	while (s[i] != '\0')
	{
		if (s[i] >= '0' && s[i] <= '9')
			hdata = hdata * 16 + ( s[i] - 0x30) ;
		else if (s[i] >= 'a' && s[i] <= 'f')
			hdata = hdata * 16 + (s[i] - 0x61 + 0x0a);
		else if (s[i] >= 'A' && s[i] <= 'F')
			hdata = hdata * 16 + (s[i] - 0x41 + 0x0a);
		
		i ++;
	}
	return hdata;
}

void Delay1us(unsigned long x)
{
	unsigned long i;

	for (i = 0; i < x; ++i){
		_asm{
			mov	dx, 0x0ed
//			mov dx, 0x12f
			out	dx, al
		}
	}
}
typedef enum 
{
	MMIO = 0,
	FRAME_BIFFER,
	CPU_MEM
}IOTYPE;
void asciiDisplay(unsigned char *pbuf,unsigned len)
{
	int i;
	printf("\t");
	for(i=0;i<len;i++)
	{
		if(pbuf[i]>32 && pbuf[i]<127)
		{
			putchar(pbuf[i]);
		}
		else
		{
			putchar('.'); 
		}
		
	}
}
int main(int argc, char *argv[])
{
	unsigned int addr;
	unsigned int data;
	unsigned int dataLen;
	unsigned int mask;
	unsigned int reg;
	unsigned int temp;
	unsigned int cmdErr=0;
	IOTYPE iotype = MMIO;
	unsigned int i;
	unsigned int addrIndex = 0;	
	unsigned char asciiBuf[20];
	if(argc>1)
	{
		if(strcmp(argv[1],"phys")==0)
		{
			addrIndex=2;
			iotype = MMIO;
		}
		else if(strcmp(argv[1],"fb")==0)
		{
			addrIndex=2;
			iotype = FRAME_BIFFER;
		}
		else if(strcmp(argv[1],"mem")==0)
		{
			addrIndex=2;
			iotype = CPU_MEM;
		}
		else
		{
			addrIndex=1;
		}		
	
		if(iotype==MMIO)
		{
			g_mmiobase = GetMMIOBase();
		}
		else if(iotype==FRAME_BIFFER)
		{
			g_mmiobase = GetFBBase();
		}
		else
		{
			g_mmiobase = 0;
		}
		printf("Base Addr = %8.8x\n", g_mmiobase);	

		if(argc == addrIndex+1)
		{
			addr = StoH(argv[addrIndex]);
			data = ReadMMIO(addr, S3X_DWORD);
			printf("mm %x = %08x\n", addr + g_mmiobase, data);
			return 0;	
		}
		else if (argc == addrIndex+2)
		{
			addr = StoH(argv[addrIndex]);
			dataLen = StoH(argv[addrIndex+1]);
			for(i=0;i<dataLen;i++)
			{
				if(i%4==0)
				{
					if(i==0)
						printf("%08x:",StoH(argv[addrIndex])+i*4 + g_mmiobase);
					else
					{
						asciiDisplay(asciiBuf,16);
						printf("\n%08x:",StoH(argv[addrIndex])+i*4 + g_mmiobase);
					}
				}
				data = ReadMMIO(StoH(argv[addrIndex])+i*4, S3X_DWORD);
				printf(" %08x",data);
				*((unsigned int*)(asciiBuf+(i%4)*4)) = data;
			}
			asciiDisplay(asciiBuf,16);
			printf("\n");
			return 0;	
		}
		
	}

	printf("Usage: Read MMIO register, Frame Buffer and CPU memory.\n");
	printf("mdw [phys] addr(HEX) [dataLenth(HEX)]		-Read MMIO Register\n");
	printf("mdw fb addr(HEX) [dataLenth(HEX)]    		-Read Frame Buffer\n");
	printf("mdw mem addr(HEX) [dataLenth(HEX)]   		-Read CPU Memory\n");

	
	return 0;
}

