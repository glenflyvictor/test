#ifndef _PCI_IF_H_
#define _PCI_IF_H_

#define PCI_VENDORID_716		0x0716
#define PCI_VENDORID_JC		    0x1900
#define PCI_VENDORID_GLF		0x6766

#ifdef Vendor_ID_ZX
#define PCI_VENDORID_ZX	Vendor_ID_ZX
#else
#define PCI_VENDORID_ZX	0x1d17
#endif

#define PCI_DEVICEID_716        0x0012
#define PCI_DEVICEID_ZX         0x3d00 
#define PCI_DEVICEID_JC1         0x3d2a 
#define PCI_DEVICEID_JC2         0x3d2b 
#define PCI_DEVICEID_JC3         0x3d2c
#define PCI_DEVICEID_GLF         0x3d00
#define PCI_DEVICEID_Ari1020         0x3D02


typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef union
{
    BYTE data[64];
    struct 
    {
        //00h
        WORD vendor_id;
        WORD device_id;
        //04h
        WORD command;
        WORD status;
        //08h
        BYTE revision_id;
        //class code
        BYTE class_code_programming_if;
        BYTE class_code_sub;
        BYTE class_code_base;
        //0Ch
        BYTE catche_line_size;
        BYTE latency_timer;
        BYTE header_type;
        BYTE bist;
        //10h
        DWORD bar0;
        //14h
        DWORD bar1;
        //18h
        DWORD bar2;
        //1ch
        DWORD bar3;
        //20h
        DWORD bar4;
        //24h
        DWORD bar5;
        //28h
        DWORD cardbus_cis_pointer;
        //2Ch
        WORD subsystem_vendor_id;
        WORD subsystem_id;
        //30h
        DWORD expansion_rom_bar;
        //34h
        BYTE capabilities_pointer;
        BYTE reserved1[3];
        //38h
        BYTE reserved2[4];
        //3ch
        BYTE interrupt_line;
        BYTE interrupt_pin;
        BYTE min_gnt;
        BYTE max_lat;
    }cfg;
}PCI_CFG_HEAD_TYPE0;
extern PCI_CFG_HEAD_TYPE0 pci_cfg_head;
/* PCI设备索引。bus/dev/func 共16位，为了方便处理可放在一个WORD中 */
#define PDI_BUS_SHIFT 8
#define PDI_BUS_SIZE 8
#define PDI_BUS_MAX 0xFF
#define PDI_BUS_MASK 0xFF00
#define PDI_DEVICE_SHIFT 3
#define PDI_DEVICE_SIZE 5
#define PDI_DEVICE_MAX 0x1F
#define PDI_DEVICE_MASK 0x00F8
#define PDI_FUNCTION_SHIFT 0
#define PDI_FUNCTION_SIZE 3
#define PDI_FUNCTION_MAX 0x7
#define PDI_FUNCTION_MASK 0x0007

typedef union 
{
    unsigned int uint;
    struct 
    {
       unsigned int reserved    :2; //00
       unsigned int reg         :6; //reg;
       unsigned int func        :3; //function
       unsigned int dev         :5; //device
       unsigned int bus         :8; //bus
       unsigned int reserved2   :7; //reserved
       unsigned int enable      :1; //enable  
    }reg; 
}PCI_CFG_ADDR;

/*******
0xcf8 information format
31       30~24       23~16   15~11   10~8        7~2         1~0
Enable  |reserve    |BUS    |Device |Function   |Register   |00

写入到0xCF8寄存器的格式如下:
低八位(0~7):            (寄存器地址)&0xFC.低二位为零
8~10:功能位.            有时候,一个pci设备对应多个功能.将每个功能单元分离出来,对应一个独立的pci device
11~15位:设备号        对应该pci总线上的设备序号
16~23位:总线号        根总线的总线号为0.每遍历到下层总线,总线号+1
31:有效位                    如果该位为1.则说明写入的数据有效,否则无效
 *******/
#define MK_PDI(bus, dev, func) (WORD)((bus & PDI_BUS_MAX) << PDI_BUS_SHIFT | (dev & PDI_DEVICE_MAX) << PDI_DEVICE_SHIFT | (func & PDI_FUNCTION_MAX))

/* PCI配置空间寄存器 */
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC


/* 填充PCI_CONFIG_ADDRESS */
#define MK_PCICFGADDR(bus, dev, func) (DWORD)(0x80000000L | (DWORD)MK_PDI(bus, dev, func) << 8)

//function define
BYTE pci_config_reg_rd_byte(int bus, int dev, int func, int reg);
WORD pci_config_reg_rd_word(int bus, int dev, int func, int reg);
DWORD pci_config_reg_rd_dword(int bus, int dev, int func, int reg);
void pci_config_reg_wr_byte(int bus, int dev, int func, int reg, BYTE value);
void pci_config_reg_wr_word(int bus, int dev, int func, int reg, WORD value);
void pci_config_reg_wr_dword(int bus, int dev, int func, int reg, DWORD value);
void pciscan(void);
void get_pci_register(char bus, char dev, char func,PCI_CFG_HEAD_TYPE0 *pPciCfgData);
void disp_pci_register(char bus, char dev, char func);
DWORD find_zx_graphics_card();

int zxdev_main(int argc, char **argv);

#endif
