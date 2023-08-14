#include "pciif.h"

#include <conio.h>
extern  vendorID;
extern  deviceID;

BYTE pci_config_reg_rd_byte(int bus, int dev, int func, int reg)
{
    outpd(PCI_CONFIG_ADDRESS, MK_PCICFGADDR(bus,dev,func)|(reg&0xfc));
    return inp(PCI_CONFIG_DATA+(reg&0x3));
}
WORD pci_config_reg_rd_word(int bus, int dev, int func, int reg)
{
    outpd(PCI_CONFIG_ADDRESS, MK_PCICFGADDR(bus,dev,func)|(reg&0xfc));
    return inpw(PCI_CONFIG_DATA+(reg&0x2));
}
DWORD pci_config_reg_rd_dword(int bus, int dev, int func, int reg)
{
    DWORD dwData;
    outpd(PCI_CONFIG_ADDRESS, MK_PCICFGADDR(bus,dev,func)|(reg&0xfc));
    dwData=inpd(PCI_CONFIG_DATA);
    return dwData;
}

void pci_config_reg_wr_byte(int bus, int dev, int func, int reg, BYTE value)
{
    outpd(PCI_CONFIG_ADDRESS, MK_PCICFGADDR(bus,dev,func)|(reg&0xfc));
    outp(PCI_CONFIG_DATA+(reg&0x3),value);
}
void pci_config_reg_wr_word(int bus, int dev, int func, int reg, WORD value)
{
    outpd(PCI_CONFIG_ADDRESS, MK_PCICFGADDR(bus,dev,func)|(reg&0xfc));
    outpw(PCI_CONFIG_DATA+(reg&0x2),value);
}
void pci_config_reg_wr_dword(int bus, int dev, int func, int reg, DWORD value)
{
    outpd(PCI_CONFIG_ADDRESS, MK_PCICFGADDR(bus,dev,func)|(reg&0xfc));
    outpd(PCI_CONFIG_DATA,value);
}




void read_config_byte(int bus, int dev, int func, int reg, unsigned char *value)
{
    outpd(PCI_CONFIG_ADDRESS, MK_PCICFGADDR(bus,dev,func)|(reg&0xfc));
    *value=inp(PCI_CONFIG_DATA+(reg&0x3));
}
void get_class_name(unsigned int class,char *str)
{
    memset(str,0,sizeof(str));
    switch((class>>16)&0xff)
    {
        case 0x00:
            sprintf(str,"Devices built before class codes");
        break;
        case 0x01:
            sprintf(str,"Mass Storage Controller");
        break;
        case 0x02:
            sprintf(str,"Network Controller");
        break;
        case 0x03:
            sprintf(str,"Display Controller");
        break;
        case 0x04:
            sprintf(str,"Multimedia Device");
        break;
        case 0x05:
            sprintf(str,"Memory Controller");
        break;
        case 0x06:
            sprintf(str,"Bridge Device");
        break;
        case 0x07:
            sprintf(str,"Simple communications controllers");
        break;
        case 0x08:
            sprintf(str,"Base system peripherals");
        break;
        case 0x09:
            sprintf(str,"Inupt devices");
        break;
        case 0x0A:
            sprintf(str,"Docking Stations");
        break;
        case 0x0B:
            sprintf(str,"Processorts");
        break;
        case 0x0C:
            sprintf(str,"Serial Bus Controller");
        break;
        case 0xff:
            sprintf(str,"Misc");
        break;         
        default:
            sprintf(str,"Unknown device");
    }
}
void pciscan(void)
{
    int bus, dev, func;
    char className[50];
    DWORD dwData;
    printf("\n");
    printf("Bus#\tDevice#\tFunc#\tVendor\tDevice\tClass\tIRQ\tIntPin\n");
    /* 枚举PCI设备 */
    for (bus = 0; bus <= PDI_BUS_MAX; ++bus)
    {
        for (dev = 0; dev <= PDI_DEVICE_MAX; ++dev)
        {
            for (func = 0; func <= PDI_FUNCTION_MAX; ++func)
            {
                // /* 获取厂商ID */
                dwData = pci_config_reg_rd_dword(bus,dev,func,0);
                /* 判断设备是否存在。FFFFh是非法厂商ID */
                if ((WORD)dwData != 0xFFFFFFFF)
                {
                    /* bus/dev/func */
                    printf("%2.2X\t%2.2X\t%1X\t", bus, dev, func);
                    /* Vendor/Device */
                    printf("%4.4X\t%4.4X\t", (WORD)dwData&0xffff, dwData >> 16);
                    /* Class Code */
                    dwData = pci_config_reg_rd_dword(bus,dev,func,0x8);
                    printf("%6.6lX\t", dwData >> 8);
                    get_class_name(dwData>>8,className);
                    
                    // /* IRQ/intPin */

                    dwData = pci_config_reg_rd_dword(bus,dev,func,0x3C);
                    printf("%d\t", (BYTE)dwData);
                    printf("%d\t", (BYTE)(dwData >> 8));
                    printf("%s", className);
                    printf("\n");
                    
                }
            }
        }
    }
}

DWORD find_pci(DWORD device)
{
    int bus, dev, func;
    DWORD dwData;
    /* 枚举PCI设备 */
    for (bus = 0; bus <= PDI_BUS_MAX; ++bus)
    {
        for (dev = 0; dev <= PDI_DEVICE_MAX; ++dev)
        {
            for (func = 0; func <= PDI_FUNCTION_MAX; ++func)
            {
                // /* 获取厂商ID */
                dwData = pci_config_reg_rd_dword(bus,dev,func,0);
                /* 判断设备是否存在。FFFFh是非法厂商ID */
                if ((WORD)dwData != 0xFFFFFFFF)
                {
                   if(dwData==device)
                    return MK_PCICFGADDR(bus,dev,func);
                }
            }
        }
    }
    return 0xffffffff;
}
DWORD find_zx_graphics_card()
{
    int bus, dev, func;
    DWORD dwData;
    unsigned int  classCode;
    unsigned short PCIcmd;
    /* 枚举PCI设备 */
    for (bus = 0; bus <= PDI_BUS_MAX; ++bus)
    {
        for (dev = 0; dev <= PDI_DEVICE_MAX; ++dev)
        {
            vendorID = pci_config_reg_rd_word(bus, dev, 0, 0x00);            
            //check vendor ID
			if (vendorID != PCI_VENDORID_ZX && vendorID != PCI_VENDORID_716 && vendorID != PCI_VENDORID_JC && vendorID != PCI_VENDORID_GLF)
			{
				continue;
			}
            deviceID = pci_config_reg_rd_word(bus, dev, 0, 0x02);
            // check device ID            
            if(deviceID != PCI_DEVICEID_ZX && deviceID != PCI_DEVICEID_716 && deviceID != PCI_DEVICEID_JC1 && deviceID != PCI_DEVICEID_JC2 && deviceID != PCI_DEVICEID_JC3&& deviceID != PCI_DEVICEID_GLF&& deviceID != PCI_DEVICEID_Ari1020)
            {
                continue;
            }
            classCode = pci_config_reg_rd_dword(bus, dev, 0, 0x08);
			if ((classCode & 0xFF000000) == 0x03000000)
			{
				PCIcmd = pci_config_reg_rd_word(bus, dev, 0, 0x04);
                //printf("vendorID: %04x\n",vendorID);
				//printf("PCIcmd:0x%04x\n",PCIcmd);
				PCIcmd|=(1<<1);
                pci_config_reg_wr_word(bus, dev, 0, 0x04,PCIcmd);
                return MK_PCICFGADDR(bus,dev,0);
			}
        }
    }
    return 0xffffffff;
}
void get_pci_register(char bus, char dev, char func,PCI_CFG_HEAD_TYPE0 *pPciCfgData)
{
	unsigned char reg=0;
	unsigned char value;
	int i, j;
    for(i=0;i<64;i++)
        pPciCfgData->data[i]=pci_config_reg_rd_byte(bus,dev,func,i);
    // printf("\nPCI bus:%02x dev:%02x func:%02x Configuration Space:\n",bus,dev,func);
	// printf("REG");
	// for (i=0; i < 16; i++)
	// 	printf("  %01X",i);
	// for (j=0; j < 4; j++) {
	// 	printf("\n%02X ",reg);
	// 	for (i=0; i < 16; i++,reg++) {
			
    //         //value=pci_config_reg_rd_dword(bus,dev,func,reg);
    //         //value>>=(reg%4);
	// 		printf(" %02X",pci_cfg_head.data[reg]);
	// 	}
	// }

}
void disp_pci_register(char bus, char dev, char func)
{
	unsigned char reg=0;
	unsigned char value;
	int i, j;
    printf("\nPCI bus:%02x dev:%02x func:%02x Configuration Space:\n",bus,dev,func);
	printf("REG");
	for (i=0; i < 16; i++)
		printf("  %01X",i);
	for (j=0; j < 16; j++) {
		printf("\n%02X ",reg);
		for (i=0; i < 16; i++,reg++) {
			read_config_byte(bus,dev,func,reg,&value);
            //value=pci_config_reg_rd_dword(bus,dev,func,reg);
            //value>>=(reg%4);
			printf(" %02X",value);
		}
	}
    printf("\n");
}



/*
find zxdev and display config space 
 */
extern PCI_CFG_ADDR pci_cfg_addr;
int zxdev_main(int argc, char **argv)
{
    pci_cfg_addr.uint = find_zx_graphics_card();
    disp_pci_register(pci_cfg_addr.reg.bus,pci_cfg_addr.reg.dev,pci_cfg_addr.reg.func);
    get_pci_register(pci_cfg_addr.reg.bus,pci_cfg_addr.reg.dev,pci_cfg_addr.reg.func,&pci_cfg_head);
    printf("bar0\t\t\t:0x%08x\n",pci_cfg_head.cfg.bar0);
    printf("bar1\t\t\t:0x%08x\n",pci_cfg_head.cfg.bar1);
    printf("expansion rom bar\t:0x%08x\n",pci_cfg_head.cfg.expansion_rom_bar);
    return 0;
}
