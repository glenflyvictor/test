#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "pciif.h"
#include "def.h"
#include "flash.h"
#include "spi_flash.h"
 
#define VENDOR_ID_ZX 0x1D17
#define DEVICE_ID_ZX 0x3D00
#define VENDOR_ID 0x6766
#define DEVICE_ID 0x3D00
#define BIOS_SIZE 0xFFFF
PCI_CFG_ADDR pci_cfg_addr;
PCI_CFG_HEAD_TYPE0 pci_cfg_head;
char exeName[100];
unsigned short vendorID;
unsigned short deviceID;


struct test_vector test_vector_buf[] = {
        {
                "/p",
                "/p filename    --flash vbios to spi rom",
                flash_main,
        },
        {
                "/d",
                "/d filename    --dump vbios to filename from spi rom",
                dump_main,
        },
        {
                "/i",
                "/i [filename]    --dump vbios info from card or rom file",
                info_main,
        },
};

void mem_verify_with_file(unsigned int mem_addr,char *file_name)
{
    FILE *hF;
    unsigned int i=0;
    unsigned int file_size;
    unsigned char byte_r;
    hF = fopen(file_name, "rb");
    if(hF==NULL)
        return;
    fseek (hF, 0, SEEK_END); 
    file_size=ftell(hF);
    fseek (hF, 0, SEEK_SET); 
    printf("file size:%d Byte\n",file_size);

    for(i=0;i<file_size;i++)
    {
        fread(&byte_r,1,1,hF);
        if(*(unsigned char *)(mem_addr+i)!=byte_r)
        {
            printf("addr:0x%08x:required:0x%02x, read:0x%02x\n",i,byte_r,*(unsigned char *)(mem_addr+i));
        }
    }
    fclose(hF);
}

void bios_verify_expansion_rom(unsigned int shadow_address)
{
    unsigned int rombase;
    unsigned int i=0;
    rombase = pci_config_reg_rd_dword(0x1,0,0,0x30);
    printf("ROM base is 0x%08x\n",rombase);
    pci_config_reg_wr_dword(0x1,0,0,0x30,rombase+1);
    for (i=0;i<BIOS_SIZE;i++)
    {
        if(*(unsigned char *)(rombase+i)!=*(unsigned char *)(shadow_address+i))
        {
            printf("addr:0x%08x:required:0x%02x, read:0x%02x\n",i,*(unsigned char *)(rombase+i),*(unsigned char *)(shadow_address+i));
        }
    }
}
// index <0  print all help
//       >=0 print single help
void help(int index)
{
    int vec_num;
    int i;
    vec_num = sizeof(test_vector_buf) / sizeof(test_vector_buf[0]);
    //printf("test num:%d\n",vec_num);
    printf("\n");
    if(index>=0 &&index<vec_num)
    {
        printf("command format error!\n");
        printf("%s %s\n",exeName,test_vector_buf[index].usage_str);
    }
    else
    {
        for (i = 0; i < vec_num; i++)
        {
            printf("%s %s\n",exeName,test_vector_buf[i].usage_str);
        }
    }
    
    

}

void main(int argc,char* argv[])
{
    
    int vec_num;
	int vec_index=0;
    int i;
    int cmd_find=0;
    char *FileName = strrchr(argv[0], '\\') + 1;
    strncpy(exeName,FileName,strlen(FileName)-4);
    for(i=0;i<strlen(exeName);i++)
    {
        if(exeName[i]>='A' && exeName[i]<='Z')
            exeName[i]+=32;
    }

    //printf("file name:%s\n",exeName);
    //get zx graphics card
    pci_cfg_addr.uint = find_zx_graphics_card();
    if(pci_cfg_addr.uint == 0xffffffff)
    {
        printf("can't find GF adapter!!!\n");
        return ;
    }
    //printf("find zx graphics card at: bus:%02x,dev:%02x,func:%02x\n",pci_cfg_addr.reg.bus,pci_cfg_addr.reg.dev,pci_cfg_addr.reg.func);
    //disp_pci_register(pci_cfg_addr.reg.bus,pci_cfg_addr.reg.dev,pci_cfg_addr.reg.func);
    get_pci_register(pci_cfg_addr.reg.bus,pci_cfg_addr.reg.dev,pci_cfg_addr.reg.func,&pci_cfg_head);
    //init spi 
    sf_init(pci_cfg_head.cfg.bar0);
    
    if (argc < 2)
	{
		help(-1);
		return ;
	}
	vec_num = sizeof(test_vector_buf) / sizeof(test_vector_buf[0]);
	for(i=0;i<strlen(argv[1]);i++)
	{
		if(argv[1][i]>='0' && argv[1][i]<='9')
		{
			vec_index  = vec_index * 16 + ( argv[1][i] - '0') ;
				continue;
		}
		else
			break;
	}
	if(i==strlen(argv[1]))
	{
		//  argv[1] is digital
		if(vec_index<vec_num)
		{
			vec_index;
			printf("%s\n",test_vector_buf[vec_index].vector_name);			
			cmd_find=1;
		}
	}
	else
	{
		for (i = 0; i < vec_num; i++)
		{
			if (strcmp(test_vector_buf[i].vector_name, argv[1]) == 0)
			{
				vec_index=i;
				cmd_find=1;
			}
		}
	}	
	if(cmd_find)
	{
		if (test_vector_buf[vec_index].test_func != 0)
		{
			//printf("enter test %s\n",test_vector_buf[vec_index].vector_name);	
			if(test_vector_buf[vec_index].test_func(argc,argv)==-1)
            {
                help(vec_index);
            }
			cmd_find=1;
		}
		else
		{
			printf("test function of %s is NULL\n",test_vector_buf[i].vector_name);
		}
	}
	else
		help(-1);	
    
}
