/*
 * Include Files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include<sys/time.h>

#include <rtk/ldd.h>
#include <rtk/ponmac.h>
#include <rtk/switch.h>
#include <rtk/gpio.h>
#include <common/util/rt_util.h>

//#include "epon_oam_config.h"
//#include "epon_oam_err.h"
//#include "epon_oam_msgq.h"
#include "europa_cli.h"
#include "europa_8291.h"
//#include "rtl8291_reg_definition.h"

#include <osal/time.h>
#include <math.h>

#if CONFIG_GPON_VERSION < 2
#include <rtk/gpon.h>
#else
#include <module/gpon/gpon.h>
#endif

//For 8291
#define RTL8291_LASER_LUT_SZ 324
#define RTL8291_TXLUT_SZ 128
#define RTL8291_TXLUT_OFFSET 64
#define RTL8291_IBCONV(b) ((b)*2047/100)
#define RTL8291_IMCONV(m) ((m)*2047/100)

#define RTL8291_TEMP_ZERO        0x4449A
//19bits, 9 bits integer, 10 bits fraction.
#define RTL8291_TEMP_MASK        0x7FFFF 

#if 1
double _8291_dBm2uw(double dBm_pwr)
{
    double uW_pwr;

    uW_pwr = 1000*pow(10,dBm_pwr/10);
    
    return uW_pwr;
}

float _8291_dBm2001uw(double dBm_pwr)
{
    double uW_pwr;
    float out_pwr;

    uW_pwr = 100000*pow(10,dBm_pwr/10);
    out_pwr = (float)uW_pwr;  
    return out_pwr;
}

float _8291_dBm201uw(double dBm_pwr)
{
    double uW_pwr;
    float out_pwr;

    uW_pwr = 10000*pow(10,dBm_pwr/10);
    out_pwr = (float)uW_pwr;  
    return out_pwr;
}

#endif

void _8291_short_data_set(uint8 *ptr_data, uint32 length, uint32 value)
{
    uint8  i, temp8;
    uint32 temp32;

    if ((length==0)||(length>4))
    {
         printf("Data Length Error!!!!!!!\n");
         return;
    }
    temp8 = 0;
    temp32 = value;
    for(i=0;i<length;i++)
    {
        *ptr_data = (uint8)(temp32>>(8*((length-1)-i)));
       // printf("%s %d: 0x%x!!!!!!!\n", __FUNCTION__, i, *ptr_data);
        ptr_data++;
    }
    return;
}

void _8291_short_data_get(uint8 *ptr_data, uint32 length, uint32 *value)
{
    uint8  i, temp8;
    uint32 temp32;

    if ((length==0)||(length>4))
    {
         printf("Data Length Error!!!!!!!\n");
         return;
    }
    temp32 = 0;
    for(i=0;i<length;i++)
    {
        temp8 = *ptr_data;
        temp32 = temp32 | ((uint32)temp8<<(8*((length-1)-i)));
        ptr_data++;
    }

    //printf("%s = 0x%x!!!!!!!\n", __FUNCTION__, temp32);

    *value = temp32;
    return;
}


/* given (X1,Y1) (X2,Y2) (X3,Y3), using polynomial fitting to get
 * coefficients a, b and c of the equation y = a x^2 + b x + c
 *    Xn are RSSI current in nA
 *    Yn are known Rx power in nW */
uint32 _8291_cal_regression(
    uint32  X1,
    uint32  Y1,
    uint32  X2,
    uint32  Y2,
    uint32  X3,
    uint32  Y3,
    double    *A,
    double    *B,
    double    *C)
{
    double x1, x2, x3, y1, y2, y3;
    double a, b, c;

    x1 = (double)X1; /* unit is uA/32 */
    y1 = (double)Y1;
    x2 = (double)X2;
    y2 = (double)Y2; /* unit is 0.01uW */
    x3 = (double)X3;
    y3 = (double)Y3;


    //printf("%s: (%f,%lf) (%f, %lf) \n",  __FUNCTION__, (float)y1, Y1, (float)y2, Y2);

    //
    // TODO : validation the input value ?
    //

    a = (((y1*x2 - y2*x1)/(x1 - x2)) - ((y2*x3 - y3*x2)/(x2 - x3)) )/(x1*x2 - x2*x3);
    c = (((y1*x2 - y2*x1)/(x1 - x2)*x3) - ((y2*x3 - y3*x2)/(x2 - x3)*x1))/(x1 - x3);
    b = (y1 - a*x1*x1-c)/x1;


    *A = a;
    *B = b;
    *C = c;

    return 0;
}

/* given (X1,Y1) (X2,Y2), using linear fitting to get
 * coefficients a and b of the equation y = a x + b
 *
 * if (X2,Y2 = (0, 0), thne use y = a x only.
 *
 *    Xn are MPD current in nA
 *    Yn are known Rx power in nW */
uint32 _8291_cal_lin_regression(
    uint32  X1,
    uint32  Y1,
    uint32  X2,
    uint32  Y2,
    double    *A,
    double    *B)
{
    double x1, x2, x3, y1, y2, y3;
    double a, b, c;

    x1 = (double)X1; /* unit is uA/32 */
    y1 = (double)Y1;
    x2 = (double)X2;
    y2 = (double)Y2; /* unit is 0.1uW */

    //printf("%s: (%f,%lf) (%f, %lf) \n",  __FUNCTION__, (float)y1, Y1, (float)y2, Y2);

    //
    // TODO : validation the input value ?
    //

    if (0==x2 && 0==y2)   /* single point data */
    {
       // avoid divide by zero
       if (0==x1)
       {
          printf("%s: (%f,%f) (%f, %f) fail.\n",  __FUNCTION__,
              x1, y2, x2, y2);
      *A = 0;
      *B = 0;
      return 1;
       }
       a = y1 / x1;
       b = 0;
    }
    else
    {
       // avoid divide by zero
       if (0==(x2-x1))
       {
          printf("%s: (%f,%f) (%f, %f) fail.\n",  __FUNCTION__,
              x1, y2, x2, y2);
      *A = 0;
      *B = 0;
      return 1;
       }
       a = (y2 - y1) / (x2 - x1);
       b = y2 - a * x2;
    }

    *A = a;
    *B = b;

    return 0;
}

int _8291_cal_shift(double input, int *pShft, double *output)
{
    double tmp1;
    int  bit_shift;

    if (input == 0)
    {
        *pShft    = 0;
        *output = 0;
        return 0;
    }

    tmp1 = input;
    bit_shift = 0;

    while ((bit_shift<=40)&&(abs(tmp1)<=0x7FFF))
    {
        bit_shift++;
        tmp1 = tmp1*2;        
        printf(" bit_shift = %4d   tmp1 = %f\n", bit_shift, tmp1);        
    }

    *pShft  = bit_shift-1;
    *output = tmp1/2;

    return 0;
}


int _8291_cal_shift2(double input, int shift, double *output)
{
    double tmp1;
    int  bit_shift;

    if (input == 0)
    {
        *output = 0;
        return 0;
    }

    tmp1 = input;
    bit_shift = shift;

    while (bit_shift!=0)
    {

        tmp1 = tmp1*2;
        bit_shift--;        
        //printf(" bit_shift = %4d   tmp1 = %f\n", bit_shift, tmp1);        
    }

    *output = tmp1;

    return 0;
}


uint32 _8291_cal_reverse(
    double    A,
    double    B,
    double    C)
{
    uint32 i;
    double tmp1, tmp2, tmp3, i_rssi, pwr;
    double a, b, c, shf_a, shf_b;
    double tmp_a, tmp_b, tmp_c;
    int  curr;

    a = A;
    b = B;
    c = C;

    //(1*b + sqrt(b^2 - 4*a*c))(2*a)
    for(i=1;i<=100;i++)
    {
        tmp1 = tmp2 = tmp3 = 0;
        tmp_a = tmp_b = tmp_c = 0;        
        pwr = (double)i/10; /* 0.1uW to uW */
        tmp_c = c - pwr;
        //tmp1 = b^2 - 4*a*c;
        tmp_a = 4*a*tmp_c;
        tmp_b = b*b;
        //tmp_c = tmp_b - tmp_a;
        
        tmp1 = b*b - 4*a*tmp_c;        
        if (tmp1>=0)
        {
            tmp2 = sqrt(tmp1);
            tmp3 = (-1*b + tmp2)/(2*a);
        }
        i_rssi = tmp3;
        tmp3 = tmp3*1000;
        curr = (int)tmp3;
        printf(" pwr = %f(uW) tmp_a = %f  tmp_c = %f  i_rssi = %f(uA) i_rssi = %d(nA)\n", pwr, tmp_a, tmp_c, i_rssi, curr);        
    }

    return 0;
}

int _rtl8291_flash_init(uint32 devId)
{
    FILE *fp;
    int wfd = -1;
    int length, start_addr, file_len;
    unsigned int input;
    unsigned int i;
    int ret=0;
           
    if (devId == 0x5)
        fp = fopen(RTL8291_DEVICE_0_FILE, "wb");  
    else if (devId == 0x6)
        fp = fopen(RTL8291_DEVICE_1_FILE, "wb");  
    else if (devId == 0x7)
        fp = fopen(RTL8291_DEVICE_2_FILE, "wb");  
    else if (devId == 0x8)
        fp = fopen(RTL8291_DEVICE_3_FILE, "wb");  
    else if (devId == 0x9)
        fp = fopen(RTL8291_DEVICE_4_FILE, "wb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return -1;    
    }

    if (NULL ==fp)
    {
        printf("Create file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 
    
    ret=fseek(fp, 0, SEEK_SET);
    if (ret<0)
    {
        fclose(fp);
        return ret;
    }
    fwrite(rtl8291_a0_reg, sizeof(unsigned char), sizeof(rtl8291_a0_reg),fp);
    fwrite(rtl8291_a2l_reg, sizeof(unsigned char), sizeof(rtl8291_a2l_reg),fp);
    ret=fseek(fp, RTL8291_WB_ADDR_BASE, SEEK_SET);
    if (ret<0)
    {
        fclose(fp);
        return ret;
    }    
    fwrite(rtl8291_wb_reg, sizeof(unsigned char), sizeof(rtl8291_wb_reg),fp);    
    ret=fseek(fp, RTL8291_DIG_ADDR_BASE, SEEK_SET);
    if (ret<0)
    {
        fclose(fp);
        return ret;
    }
    fwrite(rtl8291_digital_reg, sizeof(unsigned char), sizeof(rtl8291_digital_reg),fp);       
    fwrite(rtl8291_txlut_reg, sizeof(unsigned char), sizeof(rtl8291_txlut_reg),fp);  
    fwrite(rtl8291_apdlut_reg, sizeof(unsigned char), sizeof(rtl8291_apdlut_reg),fp);  
    fwrite(rtl8291_rsvlut1_reg, sizeof(unsigned char), sizeof(rtl8291_rsvlut1_reg),fp);
    fwrite(rtl8291_rsvlut2_reg, sizeof(unsigned char), sizeof(rtl8291_rsvlut2_reg),fp);    
    ret=fseek(fp, RTL8291_DIGEX_ADDR_BASE, SEEK_SET);
    if (ret<0)
    {
        fclose(fp);
        return ret;
    }
    fwrite(rtl8291_digitalext_reg, sizeof(unsigned char), sizeof(rtl8291_digitalext_reg),fp);
    fwrite(rtl8291_digitalext2_reg, sizeof(unsigned char), sizeof(rtl8291_digitalext2_reg),fp);    
    fwrite(rtl8291_user_parameter, sizeof(unsigned char), sizeof(rtl8291_user_parameter),fp);      
    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);
    }
    
    fclose(fp);
    printf("Create File Success!!!\n"); 

    return 0;    
}

int _rtl8291_flash_delete(uint32 devId)
{
           
    if (devId == 0x5)
        unlink(RTL8291_DEVICE_0_FILE);   
    else if (devId == 0x6)
        unlink(RTL8291_DEVICE_1_FILE);   
    else if (devId == 0x7)
        unlink(RTL8291_DEVICE_2_FILE);   
    else if (devId == 0x8)
        unlink(RTL8291_DEVICE_3_FILE);   
    else if (devId == 0x9)
        unlink(RTL8291_DEVICE_4_FILE);   
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return -1;    
    }

    printf("Delete RTL8291 devId %d flash File Success!!!\n", devId); 

    return 0;    
}

int _rtl8291_load_data(
    unsigned char *ptr,
    uint32 devId,
    int *file_len)
{
    int i, output;
    FILE *fp;
    int ret=0;

    if (devId == 0x5)
        fp = fopen(RTL8291_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8291_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8291_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8291_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8291_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return -1;    
    }

    if (NULL ==fp)
    {
        printf("Open devId 0x%x file  error!!!!!!!\n", devId); 
        return -1;           
    } 
    
    ret=fseek(fp, 0, SEEK_SET);
    if (ret<0)
    {
        fclose(fp);
        return ret;
    }
    memset(ptr, 0x0, sizeof(char)*RTL8291_PARAMETER_SIZE);  
    for(i = 0; i < RTL8291_PARAMETER_SIZE; i++)
    {
        output = fgetc(fp);
        *file_len = i;                    
        if (output == EOF)
        {
            printf("Original file length = %d\n", *file_len);
            break;    
        }
        else
        {
            *ptr = (unsigned char)(output&0xff);
            //printf("temp[%d] = 0x%x\n", i, *ptr);
        }
        ptr++;       
    }        
#if 0                     
        for(i = 0; i < EUROPA_PARAMETER_SIZE; i++)
        {
            file_len = i;
            if (feof(fp))
            {
                printf("file length = %d\n", i);
                break;  
            }
            else
            {            
                input=fgetc(fp);
                temp[i] = input&0xff;
                printf("temp[%d] = %x\n", i, temp[i]);
            }
        } 
            
#endif        
    fclose(fp); 
    
    return 0;    
}


int _rtl8291_save_data(
    unsigned char *ptr,
    uint32 devId,
    int *file_len)
{
    int i, output;
    int wfd = -1;    
    FILE *fp;
    int ret = 0;

    if (devId == 0x5)
        fp = fopen(RTL8291_DEVICE_0_FILE, "wb");  
    else if (devId == 0x6)
        fp = fopen(RTL8291_DEVICE_1_FILE, "wb");  
    else if (devId == 0x7)
        fp = fopen(RTL8291_DEVICE_2_FILE, "wb");  
    else if (devId == 0x8)
        fp = fopen(RTL8291_DEVICE_3_FILE, "wb");  
    else if (devId == 0x9)
        fp = fopen(RTL8291_DEVICE_4_FILE, "wb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return -1;    
    }

    if (NULL ==fp)
    {
        printf("Open devId 0x%x file  error!!!!!!!\n", devId); 
        return -1;           
    } 

    ret=fseek(fp, 0, SEEK_SET);
    if (ret<0)
    {
        fclose(fp);
        return ret;
    }
    fwrite(ptr,sizeof(char),*file_len, fp);
    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);
    }
    
    fclose(fp);
    
    return 0;    
}

/* W01<7>: REG_IBX50U2, W01<6>: REG_IBX50U1, W01<5>: REG_IBX25U2, W01<4>: REG_IBX25U1
Total 4bits, the combination are 
0uA(idx=0), 
25uA(idx=1, REG_IBX25U1), 
50uA(idx=2, REG_IBX50U1), 
75uA(idx=3, REG_IBX25U1+REG_IBX50U1), 
100uA(idx=4, REG_IBX50U1+REG_IBX50U2), 
125uA(idx=5, REG_IBX50U1+REG_IBX50U2+REG_IBX25U1), 
150uA(idx=5, REG_IBX50U1+REG_IBX50U2++REG_IBX25U1+REG_IBX25U2)*/
int _8291_dac_offset2idx(uint32 offset)
{
    uint32 offset_idx=0;
    uint32 offset_code[7] = {0, 0x1, 0x4, 0x5, 0xc, 0xd, 0xf};

    if (offset>0xf)
    {
        printf("Offset input error");
        return -1;
    }
    else if (offset==0)
    {
        offset_idx = 0;
    }
    else if (offset==0x1)
    {
        //25uA(idx=1, REG_IBX25U1), 
        offset_idx = 1;
    }    
    else if (offset==0x4)
    {
    
        offset_idx = 2;
    }
    else if (offset==0x5)
    {
        offset_idx = 3;
    }
    else if (offset==0xc)
    {
        //25uA(idx=1, REG_IBX25U1), 
        offset_idx = 4;
    }    
    else if (offset==0xd)
    {
    
        offset_idx = 5;
    }
    else if (offset==0xf)
    {
        offset_idx = 6;
    }
    else 
    {
        printf("Not in index assign plan!");
        offset_idx = 0xFF;
    }
    
    return offset_idx;
}

uint32 _8291_dac_idx2offset(int idx)
{
    uint32 offset_code[7] = {0, 0x1, 0x4, 0x5, 0xc, 0xd, 0xf};

    if (idx>6)
    {
        printf("Offset index input error");
        return 0;
    }
    
    return offset_code[idx];
}

#if 0
void _8291_dac_offset2idx(uint32 offset, uint32 *idx)
{

    //W01<7>: REG_IBX50U2, W01<6>: REG_IBX50U1, W01<5>: REG_IBX25U2, W01<4>: REG_IBX25U1
    //Total 4bits, the combination are 
    //0uA(idx0), 
    //25uA(idx1, REG_IBX25U1), 
    //50uA(idx2, REG_IBX50U1), 
    //75uA(idx3, REG_IBX25U1+REG_IBX50U1), 
    //100uA(idx4, REG_IBX50U1+REG_IBX50U2), 
    //125uA(idx5, REG_IBX50U1+REG_IBX50U2+REG_IBX25U1), 
    //150uA(idx5, REG_IBX50U1+REG_IBX50U2++REG_IBX25U1+REG_IBX25U2)
    uint32 offset_code[7] = {0, 0x1, 0x4, 0x5, 0xc, 0xd, 0xf};


    return;
}

#endif
/* 
   set ASCII string type parameters in A0
   eg. A0/20~35 : Vendor Name
       A0/40~55 : Vendor Part Name
       A0/56~59 : Vendor Rev */
int _8291_cli_string_set(uint32 devId, int start_addr, uint32 size, char *str)
{
    int ret;
    uint32 temp;
    FILE *fp;

    if (devId == 0x5)
        fp = fopen(RTL8291_DEVICE_0_FILE, "rb+");  
    else if (devId == 0x6)
        fp = fopen(RTL8291_DEVICE_1_FILE, "rb+");  
    else if (devId == 0x7)
        fp = fopen(RTL8291_DEVICE_2_FILE, "rb+");  
    else if (devId == 0x8)
        fp = fopen(RTL8291_DEVICE_3_FILE, "rb+");  
    else if (devId == 0x9)
        fp = fopen(RTL8291_DEVICE_4_FILE, "rb+");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return -1;    
    }
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 

    //printf("%s( %d, %d, \"%s\" )\n", __FUNCTION__, start_addr, size, str);
 
    ret = fseek(fp, start_addr, SEEK_SET); 
    if (ret<0)
    {
        fclose(fp);
        return ret;
    }
    ret = fwrite((void*)str, sizeof(char), size, fp);

    fclose(fp);
        
    return 0;    
}


/* 
   get ASCII string type parameters in A0
       A0/56~59 : Vendor Rev */
int _8291_cli_string_get(uint32 devId, int start_addr, uint32 size, char *str)
{
    uint32 temp;
    uint32 europa_id;
    uint32 version;
    FILE *fp;
    int ret=0;

    if (devId == 0x5)
        fp = fopen(RTL8291_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8291_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8291_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8291_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8291_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return -1;    
    }
    
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 
 
    ret=fseek(fp, start_addr, SEEK_SET);
    if (ret<0)
    {
        fclose(fp);
        return ret;
    }
    ret=fread((void*)str, sizeof(char), size, fp);
 
    fclose(fp);
        
    return ret;    
}

int _8291_cli_flashBlock_get(uint32 devId, int start_addr, int length)
{
    int output,i;
    FILE *fp;
    int ret=0;

    if (devId == 0x5)
        fp = fopen(RTL8291_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8291_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8291_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8291_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8291_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return -1;    
    }
    
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 
                     
    /* set the file pointer to start address */
    ret=fseek(fp, start_addr, SEEK_SET); 
    if (ret<0)
    {
        fclose(fp);
        return ret;
    }
     
    for(i=0;i<length;i++)
    {
        output = fgetc(fp);
        if (output == EOF)
        {
            break;    
        }
        else
        {
            printf("0x%02x ", output);
            if(i%8 == 7)
                printf("   ");
            if(i%16 == 15)
                printf("\n");                    
        }
    }

    fclose(fp);
    printf("\n");    

    return 0; 
}

int _8291_cli_flashBlock_set( uint32 devId, int start_addr, unsigned int size)
{
    int ret=0;
    int wfd = -1;
    int file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[RTL8291_PARAMETER_SIZE];
    
    _rtl8291_load_data(temp,devId,&file_len);  
           
    printf("The maximum file size is %d. If you want to end the data inupt less than maximum , please key in abcd.  \n", size);              
    printf("Please insert data:\n");       
       
    length = 0;
    for(i = start_addr; i < (start_addr + size); i++)
    {
        ret=scanf("%x",&input);
        if (ret!=0)
        {
            return -1;
        }        
        if (0xabcd != input)
        { 
            length++;               
            temp[i] = input&0xFF;
        }
        else
        {
            break;         
        }
    }      
    printf("Input Europa  data with length %d:\n", length);         
 
    if (file_len<(length+start_addr))
    {
        file_len = length+start_addr;
    }
    
    _rtl8291_save_data(temp,devId,&file_len);    

    return 0;    
}

void _8291_cli_flash_array_set(uint32 devId, uint32 address, uint32 length, uint8 *data)
{
    FILE *fp;
    int wfd = -1;
    uint8 *init_data, *ptr_data, *dbg_ptr;
    uint32 i;
    uint8 output;
    int ret=0;

    if (devId == 0x5)
        fp = fopen(RTL8291_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8291_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8291_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8291_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8291_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return;    
    }
    if (NULL ==fp)
    {
        printf("Open file error!!!!!!!\n");
        return;
    }        
    //printf("devId = 0x%x.\n", devId); 
    
    init_data = (uint8 *)malloc(RTL8291_PARAMETER_SIZE*sizeof(uint8));

    ret=fseek(fp, 0, SEEK_SET);
    if (ret<0)
    {
        free(init_data);
        fclose(fp);
        return;
    }    
    memset(init_data, 0x0, sizeof(uint8)*RTL8291_PARAMETER_SIZE);
    ret=fread(init_data, 1, RTL8291_PARAMETER_SIZE, fp);
    fclose(fp);

    ptr_data = init_data;
    ptr_data = ptr_data + address;
    memcpy(ptr_data, data, length);

    if (devId == 0x5)
        fp = fopen(RTL8291_DEVICE_0_FILE, "wb");  
    else if (devId == 0x6)
        fp = fopen(RTL8291_DEVICE_1_FILE, "wb");  
    else if (devId == 0x7)
        fp = fopen(RTL8291_DEVICE_2_FILE, "wb");  
    else if (devId == 0x8)
        fp = fopen(RTL8291_DEVICE_3_FILE, "wb");  
    else if (devId == 0x9)
        fp = fopen(RTL8291_DEVICE_4_FILE, "wb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return;    
    }

    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        free(init_data);        
        return;
    }
    ret=fseek(fp, 0, SEEK_SET);
    if (ret<0)
    {
        free(init_data);
        fclose(fp);
        return;
    }

    fwrite(init_data, sizeof(char), RTL8291_PARAMETER_SIZE, fp);

    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);
    }
    
    fclose(fp);
    free(init_data);
    return;
}

void _8291_cal_flash_data_get(uint32 devId, uint32 address, uint32 length, uint32 *value)
{
    FILE *fp;
    uint8 *init_data, *ptr_data;
    int ret =0;
    
    if (devId == 0x5)
        fp = fopen(RTL8291_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8291_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8291_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8291_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8291_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return -1;    
    }

    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return;
    }

    init_data = (uint8 *)malloc(RTL8291_PARAMETER_SIZE*sizeof(uint8));

    ret = fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        free(init_data);
        fclose(fp);
        return;
    }    
    memset(init_data, 0x0, sizeof(uint8)*RTL8291_PARAMETER_SIZE);
    ret = fread(init_data, 1, RTL8291_PARAMETER_SIZE, fp);
    ptr_data = init_data;

    ptr_data = ptr_data + address;
    _8291_short_data_get(ptr_data, length, value);

    free(init_data);
    fclose(fp);

    return;
}

void _8291_cal_flash_data_set(uint32 devId, uint32 address, uint32 length, uint32 value)
{
    FILE *fp;
    int wfd = -1;
    uint8 *init_data, *ptr_data;
    uint32 i;
    uint8 output;
    int ret = 0;

    if (devId == 0x5)
        fp = fopen(RTL8291_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8291_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8291_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8291_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8291_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return;    
    }

    if (NULL ==fp)
    {
        printf("Open 8291 DEVICE FILE error!!!!!!!\n");
        return;
    }

    init_data = (uint8 *)malloc(RTL8291_PARAMETER_SIZE*sizeof(uint8));

    ret = fseek(fp, 0, SEEK_SET);
    if (ret<0)
    {
        free(init_data);
        fclose(fp);
        return;
    }    
    memset(init_data, 0x0, sizeof(uint8)*RTL8291_PARAMETER_SIZE);
    ret=fread(init_data, 1, RTL8291_PARAMETER_SIZE, fp);
    fclose(fp);

    ptr_data = init_data;
    ptr_data = ptr_data + address;
    _8291_short_data_set(ptr_data, length, value);

    if (devId == 0x5)
        fp = fopen(RTL8291_DEVICE_0_FILE, "wb");  
    else if (devId == 0x6)
        fp = fopen(RTL8291_DEVICE_1_FILE, "wb");  
    else if (devId == 0x7)
        fp = fopen(RTL8291_DEVICE_2_FILE, "wb");  
    else if (devId == 0x8)
        fp = fopen(RTL8291_DEVICE_3_FILE, "wb");  
    else if (devId == 0x9)
        fp = fopen(RTL8291_DEVICE_4_FILE, "wb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8291.\n", devId);     
        return;    
    }

    if (NULL ==fp)
    {
        printf("Open 8291 DEVICE FILE error!!!!!!!\n");
        free(init_data);        
        return;
    }
    ret = fseek(fp, 0, SEEK_SET);
    if (ret<0)
    {
        free(init_data);
        fclose(fp);
        return;
    }
    fwrite(init_data,sizeof(char), RTL8291_PARAMETER_SIZE,fp);
        
    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);
    }
        
    fclose(fp);
    free(init_data);
    return;
}

int rtl8291_cli_reset(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret;

    if(argc < 2)
    { 
        printf("%s <chip/digital/8051/analog> \n", argv[0]);
        return -1;
    }

    if(0 == strcmp(argv[1], "chip"))
    {
        /*Europa Reset Total Chip*/
        printf("Reset Total Chip\n");
        ret = rtk_ldd_reset2(devId, LDD_RESET_CHIP);
    }
    else if(0 == strcmp(argv[1], "digital"))
    {
        /*Europa Reset Digital*/
        printf("Reset Digital\n");
        ret = rtk_ldd_reset2(devId, LDD_RESET_DIGITAL)  ;
    }
    else if(0 == strcmp(argv[1], "8051"))
    {
        /*Europa Reset 8051*/
        printf("Reset 8051\n");
        ret = rtk_ldd_reset2(devId, LDD_RESET_8051)  ;
    }
    else if(0 == strcmp(argv[1], "analog"))
    {
        /*Europa Reset Analog*/
        printf("Reset Analog\n");
        ret = rtk_ldd_reset2(devId, LDD_RESET_ANALOG)  ;
    }
    else
    {
        printf("no such mode\n");
        return -1;
    }

    return 0;
}    

int rtl8291_cli_register_set(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int32 addr, data;
    uint32 regData;
    uint32 fin_addr;

    if(argc >= 3)
    {
        if(0 == strcmp(argv[1], "wb"))
        {
            if(argc == 3)
            {
                printf("%s %s %s <address: 0~0x97> \n", argv[0], argv[1], argv[2]);
                return -1;
            }
        
            addr = _vlaue_translate(argv[2]);
            data = _vlaue_translate(argv[3]); 

            if((addr<0)||(addr>0x97))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb <address: 0~0x97> <data> \n", argv[0]);
                return -1;
            }
            if (data >0xFF)
            {
                printf("data should be 0~0xFF\n");
                
            }

            if(addr<0x80)
            {
                fin_addr = RTL8291_WB_ADDR_BASE + addr;
                regData = (uint32)(data&0xFF);
            }
            else
            {
                fin_addr = RTL8291_DIGEX_ADDR_BASE + (addr-0x80);
                regData = (uint32)(data&0xFF);
            }
            ret = rtk_ldd_parameter2_set(devId, fin_addr, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_parameter2_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set WB%02X (0x%x) data = 0x%x\n", addr, fin_addr, regData);
        } 
        else if(0 == strcmp(argv[1], "dig"))
        {
            if(argc == 3)
            {
                printf("%s %s %s <address: 0~0x7F> \n", argv[0], argv[1], argv[2]);
                return -1;
            }
        
            addr = _vlaue_translate(argv[2]);
            data = _vlaue_translate(argv[3]); 

            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/dig/digex <address: 0~0x7F> <data>\n", argv[0]);
                return -1;
            }
            if (data >0xFF)
            {
                printf("data should be 0~0xFF\n");
                
            }

            fin_addr = RTL8291_DIG_ADDR_BASE + addr;
            regData = (uint32)(data&0xFF);
            ret = rtk_ldd_parameter2_set(devId, fin_addr, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_parameter2_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set DIG%02X (0x%x) data = 0x%x\n", addr, fin_addr, regData);            
        } 
        else if(0 == strcmp(argv[1], "digex"))
        {
            if(argc == 3)
            {
                printf("%s %s %s <address: 0~0x7F> \n", argv[0], argv[1], argv[2]);
                return -1;
            }
        
            addr = _vlaue_translate(argv[2]);
            data = _vlaue_translate(argv[3]); 

            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/dig/digex <address: 0~0x7F> <data>\n", argv[0]);
                return -1;
            }
            if (data >0xFF)
            {
                printf("data should be 0~0xFF\n");
                
            }

            fin_addr = RTL8291_DIGEX_ADDR_BASE + addr;
            regData = (uint32)(data&0xFF);
            ret = rtk_ldd_parameter2_set(devId, fin_addr, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_parameter2_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set DIGEX%02X (0x%x) data = 0x%x\n", addr, fin_addr, regData);            
        } 
        else
        {
            addr = _vlaue_translate(argv[1]);  
            data = _vlaue_translate(argv[2]); 
            if (data >0xFF)
            {
                printf("dsta should be 0~0xFF\n");    
            }
            regData = (uint32)(data&0xFF);
            ret = rtk_ldd_parameter2_set(devId, addr, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_parameter2_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set: devId = 0x%x, addr = 0x%x, data = 0x%x\n", devId, addr, regData);          
        }
    }
    else
    {
        printf("%s <address: 0~0x6FF> <data> or %s wb/dig/digex <address: 0~0x7F> \n", argv[0],  argv[0]);
        return -1;
    }

    return 0;    
}


int rtl8291_cli_register_get(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int32 addr;
    uint32 fin_addr, value;

    if(argc >= 2)
    {
        if(0 == strcmp(argv[1], "wb"))
        {
            if(argc == 2)
            {
                printf("%s %s <address: 0~0x7F> \n", argv[0], argv[1]);
                return -1;
            }
            addr = _vlaue_translate(argv[2]);
            if((addr<0)||(addr>0x97))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb <address: 0~0x97> \n", argv[0]);
                return -1;
            }
            if(addr<0x80)
            {
                fin_addr = RTL8291_WB_ADDR_BASE + addr;
            }
            else
            {
                fin_addr = RTL8291_DIGEX_ADDR_BASE + (addr-0x80);
            }
            ret = rtk_ldd_parameter2_get(devId, fin_addr, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
                return -1;
            }            
            printf("WB%02X (0x%x) value = 0x%x\n", addr, (uint32)fin_addr, value);            
        }
        else if(0 == strcmp(argv[1], "rb"))
        {
            if(argc == 2)
            {
                printf("%s %s <address: 0~0x7F> \n", argv[0], argv[1]);
                return -1;
            }        
            addr = _vlaue_translate(argv[2]);
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb/dig/digex <address: 0~0x7F> \n", argv[0]);
                return -1;
            }
            fin_addr = RTL8291_RB_ADDR_BASE + addr;
            ret = rtk_ldd_parameter2_get(devId, fin_addr, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
                return -1;
            }
            printf("RB%02X (0x%x) value = 0x%x\n", addr, (uint32)fin_addr, value);           
        }    
        else if(0 == strcmp(argv[1], "dig"))
        {
            if(argc == 2)
            {
                printf("%s %s <address: 0~0x7F> \n", argv[0], argv[1]);
                return -1;
            }        
            addr = _vlaue_translate(argv[2]);
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb/dig/digex <address: 0~0x7F> \n", argv[0]);
                return -1;
            }
            fin_addr = RTL8291_DIG_ADDR_BASE + addr;
            ret = rtk_ldd_parameter2_get(devId, fin_addr, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
                return -1;
            }
            printf("DIG%02X (0x%x) value = 0x%x\n", addr, (uint32)fin_addr, value);           
        }   
        else if(0 == strcmp(argv[1], "digex"))
        {
            if(argc == 2)
            {
                printf("%s %s <address: 0~0x7F> \n", argv[0], argv[1]);
                return -1;
            }        
            addr = _vlaue_translate(argv[2]);
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb/dig/digex <address: 0~0x7F> \n", argv[0]);
                return -1;
            }
            fin_addr = RTL8291_DIGEX_ADDR_BASE + addr;
            ret = rtk_ldd_parameter2_get(devId, fin_addr, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
                return -1;
            }
            printf("DIGEX%02X (0x%x) value = 0x%x\n", addr, (uint32)fin_addr, value);           
        } 
        else
        {
            addr = _vlaue_translate(argv[1]);          
            ret = rtk_ldd_parameter2_get(devId, addr, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
                return -1;
            }
            printf("devId = 0x%x, addr = 0x%x, data = 0x%x\n", devId, addr, value);
        }
    }
    else
    {
        printf("%s <address: 0~0x6FF> or %s wb/rb/dig/digex <address: 0~0x7F> \n", argv[0], argv[0]);
        return -1;
    }

    return 0;    
}

int rtl8291_cli_regBit_set(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int32 addr, data, bit;
    uint32 regData;
    uint32 fin_addr;

    if(argc >= 3)
    {
        if(0 == strcmp(argv[1], "wb"))
        {
            if(argc == 4)
            {
                printf("%s %s %s %s <address: 0~0x7F> \n", argv[0], argv[1], argv[2], argv[3]);
                return -1;
            }
        
            addr = _vlaue_translate(argv[2]);
            bit = _vlaue_translate(argv[3]); 
            data = _vlaue_translate(argv[4]);

            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/dig/digex <address: 0~0x7F> <bit> <data> \n", argv[0]);
                return -1;
            }
            if ((bit >7)||(bit<0))
            {
                printf("bit should be 0~7\n");
                return -1;                
            }
            if ((data>1)||(data<0))
            {
                printf("data should be 0 or 1\n");
                return -1;                
            }

            fin_addr = RTL8291_WB_ADDR_BASE + addr;
            regData = (uint32)(data&0xFF);
            ret = rtk_ldd_regBit_set(devId, fin_addr, bit, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBit_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set WB%02X (0x%x) Bit%d data = 0x%x\n", addr, fin_addr, bit, regData);            
        } 
        else if(0 == strcmp(argv[1], "dig"))
        {
            if(argc == 4)
            {
                printf("%s %s %s %s<address: 0~0x7F> \n", argv[0], argv[1], argv[2], argv[3]);
                return -1;
            }
        
            addr = _vlaue_translate(argv[2]);
            bit = _vlaue_translate(argv[3]); 
            data = _vlaue_translate(argv[4]);

            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/dig/digex <address: 0~0x7F> <bit> <data>\n", argv[0]);
                return -1;
            }
            if ((bit >7)||(bit<0))
            {
                printf("bit should be 0~7\n");
                return -1;                
            }
            if ((data>1)||(data<0))
            {
                printf("data should be 0 or 1\n");
                return -1;                
            }

            fin_addr = RTL8291_DIG_ADDR_BASE + addr;
            regData = (uint32)(data&0xFF);
            ret = rtk_ldd_regBit_set(devId, fin_addr, bit, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBit_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set DIG%02X (0x%x) Bit%d data = 0x%x\n", addr, fin_addr, bit, regData);            
        } 
        else if(0 == strcmp(argv[1], "digex"))
        {
            if(argc == 4)
            {
                printf("%s %s %s %s<address: 0~0x7F> \n", argv[0], argv[1], argv[2], argv[3]);
                return -1;
            }
        
            addr = _vlaue_translate(argv[2]);
            bit = _vlaue_translate(argv[3]); 
            data = _vlaue_translate(argv[4]); 

            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/dig/digex <address: 0~0x7F> <bit> <data>\n", argv[0]);
                return -1;
            }
            if ((bit >7)||(bit<0))
            {
                printf("bit should be 0~7\n");
                return -1;                
            }
            if ((data>1)||(data<0))
            {
                printf("data should be 0 or 1\n");
                return -1;               
            }

            fin_addr = RTL8291_DIGEX_ADDR_BASE + addr;
            regData = (uint32)(data&0xFF);
            ret = rtk_ldd_regBit_set(devId, fin_addr, bit, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBit_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set DIGEX%02X (0x%x) Bit%d data = 0x%x\n", addr, fin_addr, bit, regData);            
        } 
        else
        {
            addr = _vlaue_translate(argv[1]);
            bit = _vlaue_translate(argv[2]); 
            data = _vlaue_translate(argv[3]);
            
            if ((bit >7)||(bit<0))
            {
                printf("bit should be 0~7\n");
                return -1;                
            }
            if ((data>1)||(data<0))
            {
                printf("data should be 0 or 1\n");
                return -1;                
            }

            regData = (uint32)(data&0xFF);
            ret = rtk_ldd_regBit_set(devId, addr, bit, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBit_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set: devId = 0x%x, addr = 0x%x, bit = %d, data = 0x%x\n", devId, addr, bit, regData);          
        }
    }
    else
    {
        printf("%s <address: 0~0x6FF> <bit: 0~7> <data> or %s wb/dig/digex <address: 0~0x7F> <bit: 0~7> <data>\n", argv[0],  argv[0]);
        return -1;
    }

    return 0;    
}


int rtl8291_cli_regBit_get(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int32 addr, bit;
    uint32 fin_addr, value;

    if(argc >= 2)
    {
        if(0 == strcmp(argv[1], "wb"))
        {
            if(argc == 3)
            {
                printf("%s %s %s <address: 0~0x7F> <bit: 0~7> \n", argv[0], argv[1], argv[2]);
                return -1;
            }
            addr = _vlaue_translate(argv[2]);
            bit = _vlaue_translate(argv[3]); 
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb/dig/digex <address: 0~0x7F> <bit: 0~7>\n", argv[0]);
                return -1;
            }
            if ((bit >7)||(bit<0))
            {
                printf("bit should be 0~7\n");
                return -1;                
            }
            fin_addr = RTL8291_WB_ADDR_BASE + addr;
            ret = rtk_ldd_regBit_get(devId, fin_addr, bit, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBit_get failed (%d)\n", ret);
                return -1;
            }            
            printf("WB%02X (0x%x) Bit%d value = 0x%x\n", addr, (uint32)fin_addr, bit, value);            
        }
        else if(0 == strcmp(argv[1], "rb"))
        {
            if(argc == 3)
            {
                printf("%s %s %s <address: 0~0x7F> <bit: 0~7> \n", argv[0], argv[1], argv[2]);
                return -1;
            }        
            addr = _vlaue_translate(argv[2]);
            bit = _vlaue_translate(argv[3]); 
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb/dig/digex <address: 0~0x7F> <bit: 0~7> \n", argv[0]);
                return -1;
            }
            if ((bit >7)||(bit<0))
            {
                printf("bit should be 0~7\n");
                return -1;                
            }
            fin_addr = RTL8291_RB_ADDR_BASE + addr;
            ret = rtk_ldd_regBit_get(devId, fin_addr, bit, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBit_get failed (%d)\n", ret);
                return -1;
            }
            printf("RB%02X (0x%x) Bit%d value = 0x%x\n", addr, (uint32)fin_addr, bit, value);           
        }    
        else if(0 == strcmp(argv[1], "dig"))
        {
            if(argc == 3)
            {
                printf("%s %s %s <address: 0~0x7F> <bit: 0~7> \n", argv[0], argv[1], argv[2]);
                return -1;
            }       
            addr = _vlaue_translate(argv[2]);
            bit = _vlaue_translate(argv[3]); 
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb/dig/digex <address: 0~0x7F> <bit: 0~7> \n", argv[0]);
                return -1;
            }
            if ((bit >7)||(bit<0))
            {
                printf("bit should be 0~7\n");
                return -1;                
            }
            fin_addr = RTL8291_DIG_ADDR_BASE + addr;
            ret = rtk_ldd_regBit_get(devId, fin_addr, bit, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBit_get failed (%d)\n", ret);
                return -1;
            }
            printf("DIG%02X (0x%x) Bit%d value = 0x%x\n", addr, (uint32)fin_addr, bit, value);           
        }   
        else if(0 == strcmp(argv[1], "digex"))
        {
            if(argc == 3)
            {
                printf("%s %s %s <address: 0~0x7F> <bit: 0~7> \n", argv[0], argv[1], argv[2]);
                return -1;
            }      
            addr = _vlaue_translate(argv[2]);
            bit = _vlaue_translate(argv[3]); 
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb/dig/digex <address: 0~0x7F> <bit: 0~7> \n", argv[0]);
                return -1;
            }
            if ((bit >7)||(bit<0))
            {
                printf("bit should be 0~7\n");
                return -1;                
            }
            fin_addr = RTL8291_DIGEX_ADDR_BASE + addr;
            ret = rtk_ldd_regBit_get(devId, fin_addr, bit, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBit_get failed (%d)\n", ret);
                return -1;
            }
            printf("DIGEX%02X (0x%x) Bit%d value = 0x%x\n", addr, (uint32)fin_addr, bit, value);           
        } 
        else
        {
            addr = _vlaue_translate(argv[1]);   
            bit = _vlaue_translate(argv[2]); 
            if ((bit >7)||(bit<0))
            {
                printf("bit should be 0~7\n");
                return -1;                
            }
            value = 0;
            ret = rtk_ldd_regBit_get(devId, addr, bit, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBit_get failed (%d)\n", ret);
                return -1;
            }
            printf("devId = 0x%x, addr = 0x%x, bit = %d, data = 0x%x\n", devId, addr, bit, value);
        }
    }
    else
    {
        printf("%s <address: 0~0x6FF> <bit: 0~7> or %s wb/rb/dig/digex <address: 0~0x7F> <bit: 0~7> \n", argv[0], argv[0]);
        return -1;
    }

    return 0;    
}

int rtl8291_cli_regBits_set(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int32 addr, data, bits;
    uint32 regData;
    uint32 fin_addr;

    if(argc >= 3)
    {
        if(0 == strcmp(argv[1], "wb"))
        {      
            addr = _vlaue_translate(argv[2]);
            bits = _vlaue_translate(argv[3]); 
            data = _vlaue_translate(argv[4]);

            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/dig/digex <address: 0~0x7F> <bits: 0~0xFF> <data> \n", argv[0]);
                return -1;
            }
            if ((bits >0xFF)||(bits<0))
            {
                printf("bit mask should be 0~0xFF\n");
                
            }
            if ((data >0xFF)||(data<0))
            {
                printf("data should be 0~0xFF\n");
                
            }
            fin_addr = RTL8291_WB_ADDR_BASE + addr;
            regData = (uint32)(data&0xFF);
            ret = rtk_ldd_regBits_set(devId, fin_addr, bits, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBits_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set WB%02X (0x%x) Bits 0x%x data = 0x%x\n", addr, fin_addr, bits, regData);            
        } 
        else if(0 == strcmp(argv[1], "dig"))
        {
            addr = _vlaue_translate(argv[2]);
            bits = _vlaue_translate(argv[3]); 
            data = _vlaue_translate(argv[4]);

            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/dig/digex <address: 0~0x7F> <bits: 0~0xFF> <data>\n", argv[0]);
                return -1;
            }
            if ((bits >0xFF)||(bits<0))
            {
                printf("bit mask should be 0~0xFF\n");
                
            }
            if ((data >0xFF)||(data<0))
            {
                printf("data should be 0~0xFF\n");
                
            }
            fin_addr = RTL8291_DIG_ADDR_BASE + addr;
            regData = (uint32)(data&0xFF);
            ret = rtk_ldd_regBits_set(devId, fin_addr, bits, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBits_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set DIG%02X (0x%x) Bits 0x%x data = 0x%x\n", addr, fin_addr, bits, regData);            
        } 
        else if(0 == strcmp(argv[1], "digex"))
        {
            addr = _vlaue_translate(argv[2]);
            bits = _vlaue_translate(argv[3]); 
            data = _vlaue_translate(argv[4]); 

            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/dig/digex <address: 0~0x7F> <bit> <data>\n", argv[0]);
                return -1;
            }
            if ((bits >0xFF)||(bits<0))
            {
                printf("bit mask should be 0~0xFF\n");
                return -1;                
            }
            if ((data >0xFF)||(data<0))
            {
                printf("data should be 0~0xFF\n");
                return -1;                
            }
            fin_addr = RTL8291_DIGEX_ADDR_BASE + addr;
            regData = (uint32)(data&0xFF);
            ret = rtk_ldd_regBits_set(devId, fin_addr, bits, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBits_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set DIGEX%02X (0x%x) Bits 0x%x data = 0x%x\n", addr, fin_addr, bits, regData);            
        } 
        else
        {
            addr = _vlaue_translate(argv[1]);
            bits = _vlaue_translate(argv[2]); 
            data = _vlaue_translate(argv[3]);
            
            if ((bits >0xFF)||(bits<0))
            {
                printf("bit mask should be 0~0xFF\n");
                return -1;                
            }
            if ((data >0xFF)||(data<0))
            {
                printf("data should be 0~0xFF\n");
                return -1;               
            }
            regData = (uint32)(data&0xFF);
            ret = rtk_ldd_regBits_set(devId, addr, bits, regData);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBits_set failed (%d)\n", ret);
                return -1;
            }
            printf("Set: devId = 0x%x, addr = 0x%x, bits = 0x%x, data = 0x%x\n", devId, addr, bits, regData);          
        }
    }
    else
    {
        printf("%s <address: 0~0x6FF> <bits: 0~0xFF> <data> or %s wb/dig/digex <address: 0~0x7F> <bits: 0~0xFF> <data>\n", argv[0],  argv[0]);
        return -1;
    }

    return 0;    
}


int rtl8291_cli_regBits_get(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int32 addr, bits;
    uint32 fin_addr, value;

    if(argc >= 2)
    {
        if(0 == strcmp(argv[1], "wb"))
        {
            addr = _vlaue_translate(argv[2]);
            bits = _vlaue_translate(argv[3]); 
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb/dig/digex <address: 0~0x7F>  <bits: 0~0xFF>\n", argv[0]);
                return -1;
            }
            if ((bits >0xFF)||(bits<0))
            {
                printf("bit mask should be 0~0xFF\n");
                return -1;                
            }
            fin_addr = RTL8291_WB_ADDR_BASE + addr;
            ret = rtk_ldd_regBits_get(devId, fin_addr, bits, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBits_get failed (%d)\n", ret);
                return -1;
            }            
            printf("WB%02X (0x%x) Bits 0x%x value = 0x%x\n", addr, (uint32)fin_addr, bits, value);            
        }
        else if(0 == strcmp(argv[1], "rb"))
        {       
            addr = _vlaue_translate(argv[2]);
            bits = _vlaue_translate(argv[3]); 
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb/dig/digex <address: 0~0x7F>  <bits: 0~0xFF> \n", argv[0]);
                return -1;
            }
            if ((bits >0xFF)||(bits<0))
            {
                printf("bit mask should be 0~0xFF\n");
                return -1;                
            }
            fin_addr = RTL8291_RB_ADDR_BASE + addr;
            ret = rtk_ldd_regBits_get(devId, fin_addr, bits, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBits_get failed (%d)\n", ret);
                return -1;
            }
            printf("RB%02X (0x%x) Bits 0x%x value = 0x%x\n", addr, (uint32)fin_addr, bits, value);           
        }    
        else if(0 == strcmp(argv[1], "dig"))
        {    
            addr = _vlaue_translate(argv[2]);
            bits = _vlaue_translate(argv[3]); 
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb/dig/digex <address: 0~0x7F>  <bits: 0~0xFF> \n", argv[0]);
                return -1;
            }
            if ((bits >0xFF)||(bits<0))
            {
                printf("bit mask should be 0~0xFF\n");
                return -1;                
            }
            fin_addr = RTL8291_DIG_ADDR_BASE + addr;
            ret = rtk_ldd_regBits_get(devId, fin_addr, bits, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBits_get failed (%d)\n", ret);
                return -1;
            }
            printf("DIG%02X (0x%x) Bits 0x%x value = 0x%x\n", addr, (uint32)fin_addr, bits, value);           
        }   
        else if(0 == strcmp(argv[1], "digex"))
        {   
            addr = _vlaue_translate(argv[2]);
            bits = _vlaue_translate(argv[3]); 
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb/dig/digex <address: 0~0x7F>  <bits: 0~0xFF> \n", argv[0]);
                return -1;
            }
            if ((bits >0xFF)||(bits<0))
            {
                printf("bit mask should be 0~0xFF\n");
                return -1;                
            }
            fin_addr = RTL8291_DIGEX_ADDR_BASE + addr;
            ret = rtk_ldd_regBits_get(devId, fin_addr, bits, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBits_get failed (%d)\n", ret);
                return -1;
            }
            printf("DIGEX%02X (0x%x) Bits 0x%x value = 0x%x\n", addr, (uint32)fin_addr, bits, value);           
        } 
        else
        {
            addr = _vlaue_translate(argv[1]);   
            bits = _vlaue_translate(argv[2]); 
            if ((bits >0xFF)||(bits<0))
            {
                printf("bit mask should be 0~0xFF\n");
                return -1;                
            }
            ret = rtk_ldd_regBits_get(devId, addr, bits, &value);
            printf("\n");
            if(ret)
            {
                printf("rtk_ldd_regBits_get failed (%d)\n", ret);
                return -1;
            }
            printf("devId = 0x%x, addr = 0x%x, bitmask = 0x%x, data = 0x%x\n", devId, addr, bits, value);
        }
    }
    else
    {
        printf("%s <address: 0~0x6FF>  <bits: 0~0xFF> or %s wb/rb/dig/digex <address: 0~0x7F>  <bits: 0~0xFF> \n", argv[0], argv[0]);
        return -1;
    }

    return 0;    
}

int rtl8291_cli_register_page_get(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 regData;
    uint32 i, length, offset;

    offset = 0;
    length = 0;
    if(argc < 2)
    {
        printf("%s a0/a2l/a2p0/a2p1/wb/rb/dig/txlut/apdlut/rsvlut1/rsvlut2/digext/digext2/ind/info \n", argv[0]);
        printf("EX: %s info\n", argv[0]);
        return -1;
    }
    else
    {
        if(0 == strcmp(argv[1], "a0"))
        {
            offset = 0;
            length = 0x100;
        }
        else if(0 == strcmp(argv[1], "a2l"))
        {
            offset = 0x100;
            length = 0x80;
        }
        else if(0 == strcmp(argv[1], "a2p0"))
        {
            offset = 0x180;
            length = 0x80;
        }
        else if(0 == strcmp(argv[1], "a2p1"))
        {
                offset = 0x200;
                length = 0x80;
        }
        else if(0 == strcmp(argv[1], "wb"))
        {
            offset = 0x280;
            length = 0x80;
        }
        else if(0 == strcmp(argv[1], "rb"))
        {
            offset = 0x300;
            length = 0x80;
        }
        else if(0 == strcmp(argv[1], "dig"))
        {
            offset = 0x380;
            length = 0x80;
        }
        else if(0 == strcmp(argv[1], "txlut"))
        {
            offset = 0x400;
            length = 0x80;
        }
        else if(0 == strcmp(argv[1], "apdlut"))
        {
            offset = 0x480;
            length = 0x80;
        }
        else if(0 == strcmp(argv[1], "rsvlut1"))
        {
            offset = 0x500;
            length = 0x80;
        }
        else if(0 == strcmp(argv[1], "rsvlut2"))
        {
            offset = 0x580;
            length = 0x80;
        }
        else if(0 == strcmp(argv[1], "digext"))
        {
            offset = 0x700;
            length = 0x80;
        }
        else if(0 == strcmp(argv[1], "digext2"))
        {
            offset = 0x780;
            length = 0x80;
        }
        else if(0 == strcmp(argv[1], "ind"))
        {
            offset = 0x600;
            length = 0x80;
        }        
        else if(0 == strcmp(argv[1], "info"))
        {
            offset = 0x680;
            length = 0x80;
        }
        else
        {
            printf("%s a0/a2l/a2p0/a2p1/wb/rb/dig/txlut/apdlut/rsvlut1/rsvlut2/digext/digext2/ind/info \n", argv[0]);
            printf("EX: %s info\n", argv[0]);
            return -1;
        }
    }

    for(i=0;i<length;i++)
    {
        ret = rtk_ldd_parameter2_get(devId, offset+i, &regData);
        if(ret)
        {
            printf(" Dump registers failed (%d)\n", ret);
            return -1;
        } 
            
        if(i%16 == 0)
        {
            printf("0x%04x ", offset+i);
        }
                
        printf("0x%02x ", regData);
        if(i%8 == 7)
            printf("   ");
        if(i%16 == 15)
            printf("\n");                 
    }
        
    return 0;   
}


int rtl8291_cli_efuse_set(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 entry, value;

    if(argc >= 3)
    {
        
        entry = _vlaue_translate(argv[1]); 
        if (entry>511)
        {
           printf("%s <entry: 0~511> <value: 0~0xFF>\n", argv[0]);
           return -1;
        }
        value = _vlaue_translate(argv[2]); 
        if (value>0xFF)
        {
           printf("%s <entry: 0~511> <value: 0~0xFF>\n", argv[0]);
           return -1;
        }

        ret = rtk_ldd_efuse2_set(devId, entry, value);
        printf("\n");
        if(ret)
        {
            printf("rtk_ldd_efuse2_set failed (%d)\n", ret);
            return -1;
        }
        printf("entry = 0x%x, value = 0x%x\n", entry, value);
    }
    else
    {
        printf("%s <entry: 0~511> \n", argv[0]);
        return -1;
    }

    return 0;    
}

int rtl8291_cli_efuse_get(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 entry, value;

    if(argc >= 2)
    {
        
        entry = _vlaue_translate(argv[1]); 
        if (entry>511)
        {
           printf("%s <entry: 0~511> \n", argv[0]);
           return -1;
        }
        ret = rtk_ldd_efuse2_get(devId, entry, &value);
        printf("\n");
        if(ret)
        {
            printf("rtk_ldd_efuse2_get failed (%d)\n", ret);
            return -1;
        }
        printf("entry = 0x%x, value = 0x%x\n", entry, value);
    }
    else
    {
        printf("%s <entry: 0~511> \n", argv[0]);
        return -1;
    }

    return 0;    
}

int rtl8291_cli_update_flash(uint32 devId)
{
    int ret;
    uint32 version, addr;

    addr = RTL8291_PARAMETER_VERSION_ADDR;    
    _8291_cal_flash_data_get(devId, addr, 4, &version);
    printf("RTL8291 LDD flash version = 0x%8x\n", version);    
    if (RTL8291_PARAMETER_VERSION_VALUE!=version)
    {
        version = RTL8291_PARAMETER_VERSION_VALUE;
        _8291_cal_flash_data_set(devId, addr, 4, version);
        _8291_cal_flash_data_get(devId, addr, 4, &version);
        printf("Update version to 0x%8x\n", version);        
        addr = RTL8291_FAULT_CFG1_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, 0xFF);
        addr = RTL8291_FAULT_CFG2_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, 0xF1); 
        addr = RTL8291_FAULT_CFG3_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, 0x3F);         
    }
    
    return 0;
}

int rtl8291_cli_rxpower2_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 rssi_code1, rssi_code2, rssi_code3, rssi_code4;
    int data_a, data_b, data_c;    
    rtk_ldd_rxpwr_t lddRxPwr;
    uint32 devId = 0x5;
    int cal_mode;
    uint32 chip_mode;
    float irssi_na = 0;
    uint32 loopcnt=0;

    double temp_f, temp_f2, i_rssi_nA;
    uint32 rx_power;
    uint32 i, temp32;   
    int rx_a, rx_b, rx_c;
    int shft_a, shft_b, shft_c;    
    uint64 sum_i;
    uint32 i_rssi; 

    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8291_MODE != chip_mode)
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;
    } 
    
    if(argc >= 4)
    { 
        //RSSIFL
        rssi_code1   = _vlaue_translate(argv[1]);
        //RSSI_K
        rssi_code2   = _vlaue_translate(argv[2]);
        //RSSI_R6400
        rssi_code3   = _vlaue_translate(argv[3]);
        //RSSIFL_SEL        
        data_a   = _vlaue_translate(argv[4]);
        //data_b   = _vlaue_translate(argv[5]);
        //data_c   = _vlaue_translate(argv[6]);    
        //cal_mode = _vlaue_translate(argv[6]);
        cal_mode = 0; //For 32bit mode
        memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
        lddRxPwr.rssi_code1 = rssi_code1;
        lddRxPwr.rssi_code2 = rssi_code2;        
        lddRxPwr.rssi_code3 = rssi_code3;         
        lddRxPwr.data_a = data_a;
        //lddRxPwr.data_b = data_b;
        //lddRxPwr.data_c = data_c;
        lddRxPwr.op_mode = cal_mode;

        //printf("rtk_ldd_rx_power2_get: %d %d %d %d %d\n", lddRxPwr.rssi_code1, lddRxPwr.rssi_code2, lddRxPwr.data_a, lddRxPwr.data_b, lddRxPwr.data_c); 
#if 1
        ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
        if(ret)
        {
            printf("Get RX power fail!!! (%d)\n", ret);
            return -1;
         }
#endif        
        //printf("rtk_ldd_rx_power2_get: I_rssi = %d I_k = %d I_cal = %d\n", lddRxPwr.rx_a, lddRxPwr.rx_b, lddRxPwr.rx_c); 
        irssi_na = (float)(lddRxPwr.i_rssi)*31.25;
        printf("rtk_ldd_rx_power2_get: I_rssi final = %f nA (%d uA/32) \n", irssi_na, lddRxPwr.i_rssi);  

        return 0;
    }
    else if ((argv[1]!=NULL)&&(strcmp(argv[1], "hw") == 0)&&(argc >= 3))
    {
        loopcnt   = _vlaue_translate(argv[2]);
        if (100 < loopcnt)
        {
           printf("Loop Count should less than 100. \n");
           return -1;
        }
        
        //sum_i = 0;
        printf("idx   I-RSSI  RX_POWER\n"); 
        for (i=0;i<loopcnt;i++)
        {
            memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
            //The unit of RTL8291_RX_REAL_MODE output is uA/32, for HW implement
            lddRxPwr.op_mode = RTL8291_RX_HARDWARE_MODE;
            //lddRxPwr.rx_lsb = 4000000;
            //The value is from Wade's excel.
            //lddRxPwr.rx_code_200u = 1326;
            //lddRxPwr.rx_r6400 = 6545;
            ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
            if(ret)
            {
                printf("Get RTL8291 RX power fail!!! (%d)\n", ret);            
                return -1;
            }
            //sum_i += lddRxPwr.i_rssi;         
            printf("%4d  %8d  %8d\n", i, lddRxPwr.i_rssi, lddRxPwr.rx_power);           
        }
        
        return 0;
    }
    else if ((argv[1]!=NULL)&&(argc >= 2))
    {
        loopcnt   = _vlaue_translate(argv[1]);
        if (100 < loopcnt)
        {
           printf("Loop Count should less than 100. \n");
           return -1;
        }
        _8291_cal_flash_data_get(devId, RTL8291_RX_A_ADDR+RTL8291_PARAMETER_BASE_ADDR, 2, &temp32);
        rx_a = (int16)temp32;     
        _8291_cal_flash_data_get(devId, RTL8291_RX_B_ADDR+RTL8291_PARAMETER_BASE_ADDR, 2, &temp32);
        rx_b = (int16)temp32;         
        _8291_cal_flash_data_get(devId, RTL8291_RX_C_ADDR+RTL8291_PARAMETER_BASE_ADDR, 2, &temp32);
        rx_c = (int16)temp32;         
        _8291_cal_flash_data_get(devId, RTL8291_SHIFT_RX_A_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, &shft_a);        
        _8291_cal_flash_data_get(devId, RTL8291_SHIFT_RX_B_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, &shft_b);  
        _8291_cal_flash_data_get(devId, RTL8291_SHIFT_RX_C_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, &shft_c);  
        
        printf("RTL8291 RX_A = %d, SHIFT_RX_A = %d\n", rx_a, shft_a);          
        printf("RTL8291 RX_B = %d, SHIFT_RX_B = %d\n", rx_b, shft_b);    
        printf("RTL8291 RX_C = %d, SHIFT_RX_C = %d\n", rx_c, shft_c);
        
        //printf("LSB = 4000000, code_250u = 1273, R6400 = 6400\n"); 
        
        sum_i = 0;
        printf("idx   RSSIFL    RSSIFL_SEL   I-RSSI  \n"); 
        for (i=0;i<loopcnt;i++)
        {
            memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
            //The unit of RTL8291_RX_REAL_MODE output is uA/32, for HW implement
            lddRxPwr.op_mode = RTL8291_RX_REAL_MODE;
            lddRxPwr.rx_lsb = 4000000;
            //The value is from Wade's excel.
            lddRxPwr.rx_code_200u = 1326;
            lddRxPwr.rx_r6400 = 6545;
            ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
            if(ret)
            {
                printf("Get RTL8291 RX power fail!!! (%d)\n", ret);            
                return -1;
            }
            sum_i += lddRxPwr.i_rssi;         
            printf("%4d  %8d  %8d  %8d     %12d\n", i, lddRxPwr.rssi_code1, lddRxPwr.data_a, lddRxPwr.i_rssi, sum_i);           
        }
        
        printf("LSB = %d, code_200u(RSSI_K) = %d, R_6400 = %d\n", lddRxPwr.rx_lsb, lddRxPwr.rx_code_200u, lddRxPwr.rx_r6400); 
        
        i_rssi = (sum_i+loopcnt/2)/loopcnt;
        i_rssi_nA = (double)i_rssi*31.25;
        
        printf("Get RTL8291 RX power RSSI Current  = %f nA (i_rssi = %d)\n", i_rssi_nA, i_rssi);
        
        if(0==i_rssi || 0==rx_b )                 
        {
           printf("Invalid RSSI current or DDMI coefficients!!!\n");
           temp_f = 0;
        }
        else
        {
           temp_f = ((double)rx_a/(1<<shft_a))*((double)i_rssi*i_rssi) + \
                    ((double)rx_b/(1<<shft_b))*((double)i_rssi) + \
                    ((double)rx_c/(1<<shft_c));                   
        }
        //20230809: Change unit from 0.1uW to  0.01uW
        rx_power = (0.0<temp_f) ? (uint32)(temp_f) : 0; /* 0.01uW */              
        temp_f2  = __log10((double)(rx_power)*1/100000)*10;                          
        printf("RX RTL8291 power = %d 0.01uW (%f dBm)\n", rx_power, temp_f2); 
        
        return 0;
    }
    else
    {
        //printf("%s <UINT:code_rssi> <UINT:code_k> <UINT:code_r6400> <INT:M -2~5> <INT:mode:0/1/2> <uint: nA is 0, 0.01uA is 1, 0.1uA is 2> <cal_mode: 0 for 32bit, 1 for 64bit >\n", argv[0]);   
        printf("%s <UINT:code_rssi> <UINT:code_k> <UINT:code_r6400> <INT:rssifl_sel> \n", argv[0]);   
        return -1;
    }

    return 0;    

}


/*
 * Get RSSI Current (nA)
 */
uint32 _rtl8291_cal_get_rssi_current(uint32 devId)
{
    int ret;
    uint32 i, loopcnt;
    double i_rssi_nA;
    uint32 ctrl_data;
    rtk_ldd_rxpwr_t lddRxPwr;
    uint64 sum_i=0;
    int32 i_rssi;

    loopcnt = 50;

    //rtk_ldd_parameter2_get(devId, 0x387, &ctrl_data);
    //rtk_ldd_parameter2_set(devId, 0x387, 0x10);    
    printf("idx  RSSIFL  RSSIFL_SEL  PGA_SEL   I-RSSI(HW)  RX-POWER  I-RSSI(SW)  B1   B0   Sum-I \n"); 
    //osal_time_mdelay(500);
    for (i=0;i<loopcnt;i++)
    {
        memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
        //The unit of RTL8291_RX_REAL_MODE output is uA/32, for HW implement
        lddRxPwr.op_mode = RTL8291_RX_HARDWARE_MODE;
        ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
        if(ret)
        {
            printf("Get RTL8291 RX power fail!!! (%d)\n", ret);
            //rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);            
            return -1;
        }
        if ((lddRxPwr.i_rssi&0x8000)!=0)
        {
            //i_rssi = 0xFFFF0000|(lddRxPwr.i_rssi);
            i_rssi = 0;
        }
        else
        {
            i_rssi =lddRxPwr.i_rssi;
        }
        printf("%4d %8d  %8d  %8d  %8d  %8d  %8d  0x%02X  0x%02X  %8d \n", i, lddRxPwr.rssi_code1, lddRxPwr.rssi_code2,lddRxPwr.rssi_code3, i_rssi, lddRxPwr.rx_power, lddRxPwr.data_c, lddRxPwr.rx_a, lddRxPwr.rx_b, sum_i);           
        
        //sum_i += lddRxPwr.i_rssi;
        //sum_i += lddRxPwr.data_c;
        sum_i += i_rssi;           
    }
        
    i_rssi = (uint32)((sum_i+loopcnt/2)/loopcnt);
    i_rssi_nA = (double)i_rssi*31.25;

    //rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);    
    printf("LSB = %d, code_200u(RSSI_K) = %d, R_6400 = %d\n", lddRxPwr.rx_lsb, lddRxPwr.rx_code_200u, lddRxPwr.rx_r6400); 
    printf("Get RTL8291 RX power I_RSSI = %f nA, (%d uA/32)\n", i_rssi_nA, i_rssi);

    return i_rssi;

}

int rtl8291_cli_cal_rx_ddmi(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int wfd = -1;    
    int32 addr;
    int32 rx_a, rx_b, rx_c;
    int32 shft_a, shft_b, shft_c;    
    uint8 value, temp8, i;
    uint8 *init_data, *ptr_data;
    uint32 temp32;
    uint32 rx_pwr[3];
    uint32 rx_rssi[3]; 
    double a,b,c;      
    double ori_A,ori_B, ori_C;      
    double tmp_f; 
    double in_dBm;
    float  out_pwr, tmp_pwr;
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
#ifdef RTL8291_CAL_TIME    
    gettimeofday(&tv_tmp1,&tz); 
#endif   
    
    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "0"))
        {   
            printf("reset cal data.\n");

            fp = fopen(EUROPA_CAL_FILE_LOCATION,"rb");
            if (NULL ==fp)
            {
                printf("%s does not exist. Create an empty file.\n", EUROPA_CAL_FILE_LOCATION);       
                fp = fopen(EUROPA_CAL_FILE_LOCATION,"wb");
                if (NULL ==fp)
                {
                    printf("Create %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION); 
                    return -1;    
                }
                //fclose(fp);
                //return -1;
            }

            init_data = (uint8 *)malloc(EUROPA_CAL_FILE_SIZE*sizeof(uint8));
            memset(init_data, 0x0, sizeof(char)*EUROPA_CAL_FILE_SIZE);    
            ret=fseek(fp, 0, SEEK_SET); 
            if (ret!=0)
            {
                free(init_data);
                fclose(fp);
                return ret;
            }
            ret=fread(init_data, sizeof(char), EUROPA_CAL_FILE_SIZE, fp);
            //if (ret!=0)
            //{
            //    free(init_data);
            //    fclose(fp);
            //    return ret;
            //}
            fclose(fp);

            //Clear RX DDMI parameter
            ptr_data = init_data + EUROPA_CAL_RXPWR1_ADDR;
            memset(ptr_data, 0x0, sizeof(char)*4*6); 

            fp = fopen(EUROPA_CAL_FILE_LOCATION, "wb");          
            if (NULL ==fp)
            {
                printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
                free(init_data);            
                return -1;             
            }  
            ret=fseek(fp, 0, SEEK_SET);
            if (ret!=0)
            {
                free(init_data);
                fclose(fp);
                return ret;
            }
            fwrite(init_data, sizeof(char), EUROPA_CAL_FILE_SIZE,fp);           
            free(init_data);
      
            fflush(fp); 
            wfd = fileno(fp);
            if(wfd != -1)
            {
                fsync(wfd);
            }
     
            fclose(fp);                           
        }
        else if(0 == strcmp(argv[1], "calc"))
        {
            printf("calculate cal data.\n");
            _europa_cal_data_get(EUROPA_CAL_RXPWR1_ADDR, &rx_pwr[0]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI1_ADDR, &rx_rssi[0]);             
            _europa_cal_data_get(EUROPA_CAL_RXPWR2_ADDR, &rx_pwr[1]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI2_ADDR, &rx_rssi[1]); 
            _europa_cal_data_get(EUROPA_CAL_RXPWR3_ADDR, &rx_pwr[2]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI3_ADDR, &rx_rssi[2]);                
            
            if ((rx_rssi[0]!=0)&&(rx_rssi[1]!=0)&&(rx_rssi[2]!=0))
            {        
                printf("Data1: Power=%6d (0.01uW)  Irssi=%6d (uA/32)\n",
                       rx_pwr[0], rx_rssi[0]);             
                printf("Data2: Power=%6d (0.01uW)  Irssi=%6d (uA/32)\n",
                       rx_pwr[1], rx_rssi[1]); 
                printf("Data3: Power=%6d (0.01uW)  Irssi=%6d (uA/32)\n",
                       rx_pwr[2], rx_rssi[2]); 
                
                printf("---- regression ...\n");
                _8291_cal_regression(rx_rssi[0], rx_pwr[0],
                             rx_rssi[1], rx_pwr[1],
                             rx_rssi[2], rx_pwr[2], &a, &b, &c);
                printf("    a = %lf\n", a);
                printf("    b = %lf\n", b);
                printf("    c = %lf\n", c);
                 
                printf("---- shifting...\n");
#if 0                
                _8291_cal_shift(a, &shft_a, &a);
                _8291_cal_shift(b, &shft_b, &b);
                _8291_cal_shift(c, &shft_c, &c);
#else
                _europa_cal_data_get(EUROPA_CAL_RXSH1_ADDR, &shft_a);
                _europa_cal_data_get(EUROPA_CAL_RXSH2_ADDR, &shft_b);             
                _europa_cal_data_get(EUROPA_CAL_RXSH3_ADDR, &shft_c);
                if (shft_a==0||shft_a>=40)
                {
                    shft_a = 30;
                }
                if (shft_b==0||shft_b>=40)
                {
                    shft_b = 15;
                }
                if (shft_c==0||shft_c>=40)
                {
                    shft_c = 6;
                }
                _8291_cal_shift2(a, shft_a, &a);
                _8291_cal_shift2(b, shft_b, &b);
                _8291_cal_shift2(c, shft_c, &c);

#endif
                
                printf("    a = %f\n", a);
                printf("    b = %f\n", b);
                printf("    c = %f\n", c);
                    
                printf("---- casting...\n");
                rx_a = (int16) a;
                rx_b = (int16) b;
                rx_c = (int16) c;                
                printf(" rx_a = 0x%08x     shift a = %d\n", rx_a, shft_a);
                printf(" rx_b = 0x%08x     shift b = %d\n", rx_b, shft_b);
                printf(" rx_c = 0x%08x     shift c = %d\n", rx_c, shft_c); 
            
                // save immediately
                addr = RTL8291_RX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 2, (uint32)rx_a);
                addr = RTL8291_RX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 2, (uint32)rx_b);
                addr = RTL8291_RX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 2, (uint32)rx_c);

                addr = RTL8291_SHIFT_RX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 1, (uint32)shft_a);
                addr = RTL8291_SHIFT_RX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 1, (uint32)shft_b);
                addr = RTL8291_SHIFT_RX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 1, (uint32)shft_c);                
            }
            else
            {
                printf("Data1: Power=%6d (0.01uW)  Irssi=%6d (uA/32)\n",
                       rx_pwr[0], rx_rssi[0]);             
                printf("Data2: Power=%6d (0.01uW)  Irssi=%6d (uA/32)\n",
                       rx_pwr[1], rx_rssi[1]);           
                    
                printf("---- regression ...\n");
                _8291_cal_lin_regression(rx_rssi[0], rx_pwr[0], rx_rssi[1], rx_pwr[1],
                                       &b, &c);
                printf("  b = %f\n", b);
                printf("  c = %f\n", c);
                        
                printf("---- shifting...\n");
                _8291_cal_shift(b, &shft_b, &b);
                _8291_cal_shift(c, &shft_c, &c);
                printf("    b = %f\n", b);
                printf("    c = %f\n", c);
                    
                printf("---- casting...\n");
                rx_a = shft_a = 0;      
                rx_b = (int16) b;
                rx_c = (int16) c;
                printf(" rx_b = 0x%08x     shift b = %d\n", rx_b, shft_b);
                printf(" rx_c = 0x%08x     shift c = %d\n", rx_c, shft_c);        

                // save immediately
                addr = RTL8291_RX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 2, (uint32)rx_a);
                addr = RTL8291_RX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 2, (uint32)rx_b);
                addr = RTL8291_RX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 2, (uint32)rx_c);

                addr = RTL8291_SHIFT_RX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 1, (uint32)shft_a);
                addr = RTL8291_SHIFT_RX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 1, (uint32)shft_b);
                addr = RTL8291_SHIFT_RX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
                _8291_cal_flash_data_set(devId, addr, 1, (uint32)shft_c);
                
            }
        }
        else if(0 == strcmp(argv[1], "shift"))
        {
            printf("Add cal shift a/b/c default value.\n");
            if (argc >= 5)
            {
                shft_a = _vlaue_translate(argv[2]);
                shft_b = _vlaue_translate(argv[3]);
                shft_c = _vlaue_translate(argv[4]);
                _europa_cal_data_add(EUROPA_CAL_RXSH1_ADDR, shft_a);
                _europa_cal_data_add(EUROPA_CAL_RXSH2_ADDR, shft_b);             
                _europa_cal_data_add(EUROPA_CAL_RXSH3_ADDR, shft_c);
                return 0;
            } 
            else
            {
                printf("Input error!\n");             
                printf("%s %s <shift a> <shift b> <shift c>\n", argv[0], argv[1]);
                printf("case shift is used to chang rx shift a/b/c default value, default is 30/15/6.\n");            
                return -1;
            }      
        }        
        else if(0 == strcmp(argv[1], "1"))
        {       
            if ((argc >= 4)&&((0 == strcmp(argv[2], "dbm"))||(0 == strcmp(argv[2], "dBm"))))
            {
                printf("Set RXPWR1 data.\n"); 
                //rx_pwr[0] = _vlaue_translate(argv[2]);
                in_dBm = atof(argv[3]);
                //20240619: For HW design, change to 0.1uW
                //out_pwr = _8291_dBm2001uw(in_dBm);
                out_pwr = _8291_dBm201uw(in_dBm);
                printf("in_dBm = %f (dBm)  out_pwr =%f (0.1uW)\n", in_dBm, out_pwr); 
                rx_pwr[0] = (uint32)out_pwr; 
                _europa_cal_data_add(EUROPA_CAL_RXPWR1_ADDR, rx_pwr[0]);                   
                rx_rssi[0] = _rtl8291_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI1_ADDR, rx_rssi[0]);                   
                    
                printf("Data1: Power=%6d (0.1uW)  Irssi=%6d (uA/32)\n",
                    rx_pwr[0], rx_rssi[0]);                               
            }        
            else if (argc == 3)
            {
                printf("Set RXPWR1 data.\n"); 
                rx_pwr[0] = _vlaue_translate(argv[2]);
                //in_dBm = atof(argv[2]);
                //out_pwr = __dBm2001uw(in_dBm);
                //rx_pwr[0] = (uint32)out_pwr; 
                _europa_cal_data_add(EUROPA_CAL_RXPWR1_ADDR, rx_pwr[0]);                   
                rx_rssi[0] = _rtl8291_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI1_ADDR, rx_rssi[0]);                   
                    
                printf("Data1: Power=%6d (0.1uW)  Irssi=%6d (uA/32)\n",
                        rx_pwr[0], rx_rssi[0]);                               
            }
            else
            {
                printf("%s %s <rx_pwr1: Unit is 0.1uW> \n", argv[0], argv[1]);
                printf("%s %s dbm <tx_pwr1: Unit is dbm> \n", argv[0], argv[1]);
                return -1;
            }
        }
        else if(0 == strcmp(argv[1], "2"))
        {       
            if ((argc >= 4)&&((0 == strcmp(argv[2], "dbm"))||(0 == strcmp(argv[2], "dBm"))))
            {
                printf("Set RXPWR2 data.\n"); 
                //rx_pwr[1] = _vlaue_translate(argv[2]);
                in_dBm = atof(argv[3]);
                //20240619: For HW design, change to 0.1uW
                //out_pwr = _8291_dBm2001uw(in_dBm);
                out_pwr = _8291_dBm201uw(in_dBm);
                printf("in_dBm = %f (dBm)  out_pwr =%f (0.1uW)\n", in_dBm, out_pwr); 
                rx_pwr[1] = (uint32)out_pwr; 
                _europa_cal_data_add(EUROPA_CAL_RXPWR2_ADDR, rx_pwr[1]);                   
                rx_rssi[1] = _rtl8291_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI2_ADDR, rx_rssi[1]);                    
                printf("Data2: Power=%6d (0.1uW) Irssi=%6d (uA/32)\n",
                     rx_pwr[1], rx_rssi[1]);                          
            }
            else if (argc == 3)
            {
                printf("Set RXPWR2 data.\n"); 
                rx_pwr[1] = _vlaue_translate(argv[2]);
                //in_dBm = atof(argv[2]);
                //out_pwr = __dBm2001uw(in_dBm);
                //rx_pwr[1] = (uint32)out_pwr; 
                _europa_cal_data_add(EUROPA_CAL_RXPWR2_ADDR, rx_pwr[1]);                   
                rx_rssi[1] = _rtl8291_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI2_ADDR, rx_rssi[1]);                  
                printf("Data2: Power=%6d (0.1uW) Irssi=%6d (uA/32)\n",
                       rx_pwr[1], rx_rssi[1]);                        
            }
            else
            {
                printf("%s %s <rx_pwr2: Unit is 0.1uW> \n", argv[0], argv[1]);
                printf("%s %s dbm <tx_pwr2: Unit is dbm> \n", argv[0], argv[1]);
                return -1;                
            }             
        } 
        else if(0 == strcmp(argv[1], "3"))
        {       
            if ((argc >= 4)&&((0 == strcmp(argv[2], "dbm"))||(0 == strcmp(argv[2], "dBm"))))
            {
                printf("Set RXPWR3 data.\n"); 
                //rx_pwr[2] = _vlaue_translate(argv[2]); 
                in_dBm = atof(argv[3]);
                //20240619: For HW design, change to 0.1uW
                //out_pwr = _8291_dBm2001uw(in_dBm);
                out_pwr = _8291_dBm201uw(in_dBm);                
                printf("in_dBm = %f (dBm)  out_pwr =%f (0.1uW)\n", in_dBm, out_pwr); 
                rx_pwr[2] = (uint32)out_pwr; 
                _europa_cal_data_add(EUROPA_CAL_RXPWR3_ADDR, rx_pwr[2]);                   
                rx_rssi[2] = _rtl8291_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI3_ADDR, rx_rssi[2]);                  
                printf("Data3: Power=%6d (0.1uW) Irssi=%6d (uA/32)\n",
                       rx_pwr[2], rx_rssi[2]);                        
            }        
            else if (argc == 3)
            {
                printf("Set RXPWR3 data.\n"); 
                rx_pwr[2] = _vlaue_translate(argv[2]); 
                //in_dBm = atof(argv[2]);
                //out_pwr = __dBm2001uw(in_dBm);
                //rx_pwr[2] = (uint32)out_pwr; 
                _europa_cal_data_add(EUROPA_CAL_RXPWR3_ADDR, rx_pwr[2]);                   
                rx_rssi[2] = _rtl8291_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI3_ADDR, rx_rssi[2]);                  
                printf("Data3: Power=%6d (0.1uW) Irssi=%6d (uA/32)\n",
                       rx_pwr[2], rx_rssi[2]);                        
            }
            else
            {
                printf("%s %s <rx_pwr3: Unit is 0.1uW> \n", argv[0], argv[1]);
                printf("%s %s dbm <tx_pwr3: Unit is dbm> \n", argv[0], argv[1]);
                return -1;                    
            }             
        }          
        else if(0 == strcmp(argv[1], "4"))
        {       
            if (argc > 3)
            {
                printf("Set RXPWR1 data. (Offline)\n");                
                in_dBm = atof(argv[2]);
                out_pwr = _8291_dBm201uw(in_dBm);
                //memcpy(&rx_pwr[0], &out_pwr, sizeof(uint32));
                //memcpy(&tmp_pwr, &rx_pwr[0], sizeof(uint32));
                rx_pwr[0] = (uint32)out_pwr; 
                _europa_cal_data_add(EUROPA_CAL_RXPWR1_ADDR, rx_pwr[0]);
                rx_rssi[0] = _vlaue_translate(argv[3]);                 
                _europa_cal_data_add(EUROPA_CAL_RXRSSI1_ADDR, rx_rssi[0]);                                  
                printf("Data1: Power=%6d (0.1uW) Irssi=%6d (uA/32)\n", rx_pwr[0], rx_rssi[0]);                               
            }
            else
            {
                    printf("%s %s <rx_pwr1> <rx_current> \n", argv[0], argv[1]);
            }
        }
        else if(0 == strcmp(argv[1], "5"))
        {       
            if (argc > 3)
            {
                printf("Set RXPWR2 data. (Offline)\n"); 
                in_dBm = atof(argv[2]);
                out_pwr = _8291_dBm201uw(in_dBm);
                rx_pwr[1] = (uint32)out_pwr;
                _europa_cal_data_add(EUROPA_CAL_RXPWR2_ADDR, rx_pwr[1]);                   
                rx_rssi[1] = _vlaue_translate(argv[3]);                     
                _europa_cal_data_add(EUROPA_CAL_RXRSSI2_ADDR, rx_rssi[1]);                  
                printf("Data2: Power=%f (0.1uW)  Irssi=%6d (uA/32\n", (float)rx_pwr[1], rx_rssi[1]);                
            }
            else
            {
                printf("%s %s <rx_pwr1> <rx_current> \n", argv[0], argv[1]);
            }             
        } 
        else if(0 == strcmp(argv[1], "6"))
        {       
            if (argc > 3)
            {
                printf("Set RXPWR3 data. (Offline)\n"); 
                in_dBm = atof(argv[2]);
                out_pwr = _8291_dBm201uw(in_dBm);
                rx_pwr[2] = (uint32)out_pwr;
                _europa_cal_data_add(EUROPA_CAL_RXPWR3_ADDR, rx_pwr[2]);                   
                rx_rssi[2] = _vlaue_translate(argv[3]);                     
                _europa_cal_data_add(EUROPA_CAL_RXRSSI3_ADDR, rx_rssi[2]);                  
                printf("Data3: Power=%6d (0.1uW)  Irssi=%6d (uA/32\n",
                       rx_pwr[2], rx_rssi[2]);                        
            }
            else
            {
                printf("%s %s <rx_pwr1> <rx_current> \n", argv[0], argv[1]);
            }             
        }        
        else if(0 == strcmp(argv[1], "7"))
        {
            printf("calculate cal data.\n");
            _europa_cal_data_get(EUROPA_CAL_RXPWR1_ADDR, &rx_pwr[0]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI1_ADDR, &rx_rssi[0]);             
            _europa_cal_data_get(EUROPA_CAL_RXPWR2_ADDR, &rx_pwr[1]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI2_ADDR, &rx_rssi[1]); 
            _europa_cal_data_get(EUROPA_CAL_RXPWR3_ADDR, &rx_pwr[2]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI3_ADDR, &rx_rssi[2]);                
            
            printf("---- regression ...\n");
            _8291_cal_regression(rx_rssi[0], rx_pwr[0],
                         rx_rssi[1], rx_pwr[1],
                         rx_rssi[2], rx_pwr[2], &a, &b, &c);
            printf("    a = %f\n", a);
            printf("    b = %f\n", b);
            printf("    c = %f\n", c);
                
            ori_A = a;
            ori_B = b;
            ori_C = c;
            
            printf("---- shifting...\n");
            a = a*(1<<RX_A_SHIFT);
            b = b*(1<<RX_B_SHIFT);
            c = c*(1<<RX_C_SHIFT);
            printf("    a = %f\n", a);
            printf("    b = %f\n", b);
            printf("    c = %f\n", c);
                
            printf("---- casting...\n");
            rx_a = (int32) a;
            rx_b = (int32) b;
            rx_c = (int32) c;
            printf("    a = 0x%08x\n", rx_a);
            printf("    b = 0x%08x\n", rx_b);
            printf("    c = 0x%08x\n", rx_c);
                     
            printf("---- reversing...\n");
            gettimeofday(&tv_tmp1,&tz);             
            _8291_cal_reverse(ori_A, ori_B, ori_C);
            gettimeofday(&tv_tmp2,&tz);    
            Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
            printf("[TIME]: europa_cli_cal_rx_ddmi reversing = %f ms\n", Duration);                   
        }   
        else if(0 == strcmp(argv[1], "8"))
        {
            printf("calculate cal data.\n");
            _europa_cal_data_get(EUROPA_CAL_RXPWR1_ADDR, &rx_pwr[0]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI1_ADDR, &rx_rssi[0]);             
            _europa_cal_data_get(EUROPA_CAL_RXPWR2_ADDR, &rx_pwr[1]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI2_ADDR, &rx_rssi[1]); 
            _europa_cal_data_get(EUROPA_CAL_RXPWR3_ADDR, &rx_pwr[2]);
            _europa_cal_data_get(EUROPA_CAL_RXRSSI3_ADDR, &rx_rssi[2]);                
            
            printf("---- regression(uA/32, 0.1uW) ... \n");
            _8291_cal_regression(rx_rssi[0], rx_pwr[0],
                         rx_rssi[1], rx_pwr[1],
                         rx_rssi[2], rx_pwr[2], &a, &b, &c);
            printf("    a = %f\n", a);
            printf("    b = %f\n", b);
            printf("    c = %f\n", c);
                
            ori_A = a;
            ori_B = b;
            ori_C = c;
            
            printf("---- shifting...\n");
            _8291_cal_shift(a, &shft_a, &a);
            _8291_cal_shift(b, &shft_b, &b);
            _8291_cal_shift(c, &shft_c, &c);
            printf("    a = %f\n", a);
            printf("    b = %f\n", b);
            printf("    c = %f\n", c);
                
            printf("---- casting...\n");
            rx_a = (int32) a;
            rx_b = (int32) b;
            rx_c = (int32) c;
            printf("    a = 0x%08x     shift a = %d\n", rx_a, shft_a);
            printf("    b = 0x%08x     shift b = %d\n", rx_b, shft_b);
            printf("    c = 0x%08x     shift c = %d\n", rx_c, shft_c);
                     
            //printf("---- reversing...\n");
            //gettimeofday(&tv_tmp1,&tz);             
            //_8291_cal_reverse(ori_A, ori_B, ori_C);
            //gettimeofday(&tv_tmp2,&tz);    
            //Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
            //printf("[TIME]: europa_cli_cal_rx_ddmi reversing = %f ms\n", Duration);                   
        }        
    }
    else
    {
        printf("%s <case> \n", argv[0]);
        printf("case can be 0, 1, 2, 3 and calc, input power unit is 0.01uW or dBm\n");
        printf("case shift is used to chang rx shift a/b/c default value, default is 30/15/6.\n");
        printf("europacli cal rxddmi <case:1, 2, 3> <Power: unit is 0.01uW>\n");
        printf("europacli cal rxddmi <case:1, 2, 3> <dbm or dBm> <Power: unit is dBm>\n");        
        return -1;
    }
#ifdef RTL8291_CAL_TIME    
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: rtl8291_cli_cal_rx_ddmi Duration = %f ms\n", Duration);
#endif
    
    return 0; 

}

int rtl8291_cli_rxpower_get(uint32 devId, uint32 loopcnt)
{
    double temp_f, temp_f2, i_rssi_nA, i_rssi_sw_nA;
    uint32 rx_power=0, rx_power_hw=0;
    int ret;
    uint32 i, temp32;
    uint32 ctrl_data;    
    int rx_a, rx_b, rx_c;
    int shft_a, shft_b, shft_c;    
    rtk_ldd_rxpwr_t lddRxPwr;
    int64 sum_i, sum_rxpwr=0, sum_i_sw;
    int32 i_rssi, i_rssi_sw;    

    
    if (100 < loopcnt)
    {
       printf("Loop Count should less than 100. \n");
       return -1;
    }
    _8291_cal_flash_data_get(devId, RTL8291_RX_A_ADDR+RTL8291_PARAMETER_BASE_ADDR, 2, &temp32);
    rx_a = (int16)temp32;    
    _8291_cal_flash_data_get(devId, RTL8291_RX_B_ADDR+RTL8291_PARAMETER_BASE_ADDR, 2, &temp32);
    rx_b = (int16)temp32;        
    _8291_cal_flash_data_get(devId, RTL8291_RX_C_ADDR+RTL8291_PARAMETER_BASE_ADDR, 2, &temp32);
    rx_c = (int16)temp32;        
    _8291_cal_flash_data_get(devId, RTL8291_SHIFT_RX_A_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, &shft_a);       
    _8291_cal_flash_data_get(devId, RTL8291_SHIFT_RX_B_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, &shft_b);  
    _8291_cal_flash_data_get(devId, RTL8291_SHIFT_RX_C_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, &shft_c);  

    printf("RTL8291 RX_A = %d, SHIFT_RX_A = %d\n", rx_a, shft_a);         
    printf("RTL8291 RX_B = %d, SHIFT_RX_B = %d\n", rx_b, shft_b);   
    printf("RTL8291 RX_C = %d, SHIFT_RX_C = %d\n", rx_c, shft_c);

    //rtk_ldd_parameter2_get(devId, 0x387, &ctrl_data);
    //rtk_ldd_parameter2_set(devId, 0x387, 0x10);

    //printf("LSB = 4000000, code_250u = 1273, R6400 = 6400\n"); 

    sum_i = 0;
    sum_i_sw = 0;
    printf("idx  RSSIFL  RSSIFL_SEL  PGA_SEL   I-RSSI   RX-POWER   Data_A    Data_B   Data_C  B1   B0   Sum-I  Sum-RXPWR\n"); 
    for (i=0;i<loopcnt;i++)
    {
        memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
        //The unit of RTL8291_RX_REAL_MODE output is uA/32, for HW implement
        lddRxPwr.op_mode = RTL8291_RX_HARDWARE_MODE;
        //lddRxPwr.rx_lsb = 4000000;
        //lddRxPwr.rx_code_200u = 1273;
        //lddRxPwr.rx_r6400 = 6400;
        ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
        if(ret)
        {
            printf("Get RTL8291 RX power fail!!! (%d)\n", ret);
            //rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);            
            return -1;
        }
        if ((lddRxPwr.i_rssi&0x8000)!=0)
        {
            //i_rssi = 0xFFFF0000|(lddRxPwr.i_rssi);
            i_rssi = 0;
        }
        else
        {
            i_rssi =lddRxPwr.i_rssi;
        }       
        sum_i += i_rssi;
        sum_i_sw += lddRxPwr.data_c;
        sum_rxpwr += lddRxPwr.rx_power;
        printf("%4d %8d  %8d  %8d  %8d  %8d  %8d  %8d  %8d  0x%02X  0x%02X  %8d %12d \n", i, lddRxPwr.rssi_code1, lddRxPwr.rssi_code2,lddRxPwr.rssi_code3, i_rssi, lddRxPwr.rx_power, lddRxPwr.data_a, lddRxPwr.data_b, lddRxPwr.data_c, lddRxPwr.rx_a, lddRxPwr.rx_b, sum_i, sum_rxpwr);           
    }

    //rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);
    
    printf("LSB = %d, code_200u(RSSI_K) = %d, R_6400 = %d\n", lddRxPwr.rx_lsb, lddRxPwr.rx_code_200u, lddRxPwr.rx_r6400); 

    i_rssi = (sum_i+loopcnt/2)/loopcnt;
    i_rssi_nA = (double)i_rssi*31.25;
    i_rssi_sw = (sum_i_sw+loopcnt/2)/loopcnt;
    i_rssi_sw_nA = (double)i_rssi_sw*31.25;   
    rx_power_hw = (sum_rxpwr+loopcnt/2)/loopcnt;

    printf("Get RTL8291 RX power RSSI Current(HW)  = %f nA (i_rssi = %d) RX Power(HW) = %d 0.1uW\n", i_rssi_nA, i_rssi, rx_power_hw);
    printf("Get RTL8291 RX power RSSI Current  = %f nA (i_rssi = %d)\n", i_rssi_sw_nA, i_rssi_sw);

    if(0==i_rssi || 0==rx_b )                
    {
       printf("Invalid RSSI current or DDMI coefficients!!!\n");
       temp_f = 0;
    }
    else
    {
       temp_f = ((double)rx_a/(1<<shft_a))*((double)i_rssi*i_rssi) + \
                ((double)rx_b/(1<<shft_b))*((double)i_rssi) + \
                ((double)rx_c/(1<<shft_c));                   
    }
    //20240620: unit is 0.1uW
    rx_power = (0.0<temp_f) ? (uint32)(temp_f) : 0; /* 0.1uW */              
    temp_f2  = __log10((double)(rx_power)*1/10000)*10;                          
    printf("RX RTL8291 power = %d 0.1uW (%f dBm)\n", rx_power, temp_f2);
    temp_f2  = __log10((double)(rx_power_hw)*1/10000)*10;      
    printf("RX RTL8291 power(HW) = %d 0.1uW (%f dBm)\n", rx_power_hw, temp_f2);     
    return 0;    
}


uint32 _rtl8291_cal_get_mpd_current(uint32 devId)
{
    int ret;
    uint32 i_mpd;
    uint32 loopcnt, i;
    uint64 sum;
    rtk_ldd_txpwr_t lddTxPwr;

    loopcnt = 10;

    sum = 0;
    for (i=0;i<loopcnt;i++)
    {
        memset(&lddTxPwr, 0, sizeof(rtk_ldd_txpwr_t));    
        lddTxPwr.op_mode = RTL8291_TX_HARDWARE_MODE;
        //lddTxPwr.tx_lsb = 4000000;
        //lddTxPwr.tx_code_250u = 2615;
        //lddTxPwr.tx_r_mpd = 500;        
        ret = rtk_ldd_tx_power2_get(devId, &lddTxPwr);
        if(ret)
        {
            printf("Get RTL8291MPD current fail!!! (%d)\n", ret);
            return -1;
        }
        sum += lddTxPwr.i_mpd;
    }
    i_mpd = (sum+loopcnt/2)/loopcnt;

    return i_mpd; /* unit is uA/32 */
}


int rtl8291_cli_cal_tx_ddmi(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    int wfd = -1;
    int32 addr;
    int32 tx_a, tx_b;
    int32 shft_a, shft_b;        
    uint8 value, temp8, i;
    uint8 *init_data, *ptr_data;
    uint32 temp32;
    uint32 tx_pwr[2];
    uint32 tx_mpd[2]; 
    double    a,b;    
    uint32 tx_iavg;
    double in_dBm;
    float  out_pwr, tmp_pwr;    
#ifdef RTL8291_CAL_TIME
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "0"))
        {
            printf("reset cal data.\n");
#if 0            
            fp = fopen(EUROPA_CAL_FILE_LOCATION, "wb");          
            if (NULL ==fp)
            {
                printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
                return -1;             
            }        
            init_data = (uint8 *)malloc(EUROPA_CAL_FILE_SIZE);
            memset(init_data, 0x0, sizeof(char)*EUROPA_CAL_FILE_SIZE);    
            fseek(fp, 0, SEEK_SET); 
            fwrite(init_data, sizeof(char), EUROPA_CAL_FILE_SIZE,fp);       
            fclose(fp);
            free(init_data);
#endif
            fp = fopen(EUROPA_CAL_FILE_LOCATION,"rb");
            if (NULL ==fp)
            {
                printf("%s does not exist. Create an empty file.\n", EUROPA_CAL_FILE_LOCATION);       
                fp = fopen(EUROPA_CAL_FILE_LOCATION,"wb");
                if (NULL ==fp)
                {
                    printf("Create %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION); 
                    return -1;  
                }
                fclose(fp);
                return -1;
            }  
            
            init_data = (uint8 *)malloc(EUROPA_CAL_FILE_SIZE*sizeof(uint8));
            memset(init_data, 0x0, sizeof(char)*EUROPA_CAL_FILE_SIZE);    
            ret=fseek(fp, 0, SEEK_SET);
            if (ret!=0)
            {
                free(init_data);
                fclose(fp);
                return ret;
            }
            ret=fread(init_data, sizeof(char), EUROPA_CAL_FILE_SIZE, fp);
            if (ret!=0)
            {
                free(init_data);
                fclose(fp);
                return ret;
            }
            fclose(fp);

            //Clear TX DDMI parameter
            memset(init_data, 0x0, sizeof(char)*EUROPA_CAL_RXPWR1_ADDR); 

            fp = fopen(EUROPA_CAL_FILE_LOCATION, "wb");          
            if (NULL ==fp)
            {
                printf("Open %s error!!!!!!!\n", EUROPA_CAL_FILE_LOCATION);
                free(init_data);            
                return -1;             
            }  
            ret=fseek(fp, 0, SEEK_SET);
            if (ret!=0)
            {
                free(init_data);
                fclose(fp);
                return ret;
            }
            fwrite(init_data, sizeof(char), EUROPA_CAL_FILE_SIZE,fp);           
            free(init_data);
            
            fflush(fp); 
            wfd = fileno(fp);
            if(wfd != -1)
            {
                fsync(wfd);
            }            
                    
            fclose(fp);            
#if 0            
            //tx_pwr[0] = tx_pwr[1] = 0;
            //tx_mpd[0] = tx_mpd[1] = 0;
            setenv("TXPWR1","0",1);
            setenv("TXPWR2","0",1);
            setenv("TXMPD1","0",1);
            setenv("TXMPD2","0",1);    
#endif            
        }
        else if(0 == strcmp(argv[1], "calc"))
        {
            printf("calculate cal data.\n");
            _europa_cal_data_get(EUROPA_CAL_TXPWR1_ADDR, &tx_pwr[0]);
            _europa_cal_data_get(EUROPA_CAL_TXMPD1_ADDR, &tx_mpd[0]);            
            _europa_cal_data_get(EUROPA_CAL_TXPWR2_ADDR, &tx_pwr[1]);
            _europa_cal_data_get(EUROPA_CAL_TXMPD2_ADDR, &tx_mpd[1]);  
            _europa_cal_data_get(EUROPA_CAL_TX_IAVG_ADDR, &tx_iavg); 
      
            printf("Data1: Power=%6d 0.1uW Impd=%6d (uA/32\n",
                tx_pwr[0], tx_mpd[0]);
            printf("Data2: Power=%6d 0.1W Impd=%6d (uA/32)\n",
                tx_pwr[1], tx_mpd[1]); 
        
            printf("Iavg = 0x%x\n", tx_iavg);             
        
            printf("---- regression ...\n");
               _8291_cal_lin_regression(tx_mpd[0], tx_pwr[0], tx_mpd[1], tx_pwr[1],
                       &a, &b);
            printf("  a = %f\n", a);
            printf("  b = %f\n", b);
        
            printf("---- shifting...\n");
            _8291_cal_shift(a, &shft_a, &a);
            _8291_cal_shift(b, &shft_b, &b);;
            printf("    a = %f\n", a);
            printf("    b = %f\n", b);

            printf("---- casting...\n");
            tx_a = (int16) a;
            tx_b = (int16) b;
            printf(" tx_a = 0x%08x       shift a = %d\n", tx_a, shft_a);
            printf(" tx_b = 0x%08x       shift b = %d\n", tx_b, shft_b);
      
            // save immediately
            addr = RTL8291_TX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
            _8291_cal_flash_data_set(devId, addr, 2, tx_a);
            addr = RTL8291_TX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
            _8291_cal_flash_data_set(devId, addr, 2, tx_b);

            addr = RTL8291_SHIFT_TX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
            _8291_cal_flash_data_set(devId, addr, 1, (uint32)shft_a);
            addr = RTL8291_SHIFT_TX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
            _8291_cal_flash_data_set(devId, addr, 1, (uint32)shft_b);

#if 0        
                        printf("TXPWR1=%sn \n",getenv("TXPWR1"));
                        printf("TXPWR2=%sn \n",getenv("TXPWR2"));
                        printf("TXMPD1=%sn \n",getenv("TXMPD1"));
                        printf("TXMPD2=%sn \n",getenv("TXMPD2"));
#endif
        }
        else if(0 == strcmp(argv[1], "shift"))
        {
            printf("Add cal shift a/b/c default value.\n");
            if (argc >= 4)
            {
                shft_a = _vlaue_translate(argv[2]);
                shft_b = _vlaue_translate(argv[3]);
                _europa_cal_data_add(EUROPA_CAL_TXSH1_ADDR, shft_a);
                _europa_cal_data_add(EUROPA_CAL_TXSH2_ADDR, shft_b);             
                return 0;
            } 
            else
            {
                printf("Input error!\n");             
                printf("%s %s <shift a> <shift b>\n", argv[0], argv[1]);
                printf("case shift is used to chang rx shift a/b default value, default is .\n");            
                return -1;
            }      
        }          
        else if(0 == strcmp(argv[1], "1"))
        {      
            if ((argc >= 4)&&((0 == strcmp(argv[2], "dbm"))||(0 == strcmp(argv[2], "dBm"))))
            {
                printf("Set TXPWR1 data.\n"); 
                in_dBm = atof(argv[3]);
                out_pwr = _8291_dBm201uw(in_dBm);
                printf("in_dBm = %f (dBm)  out_pwr =%f (0.1uW)\n",
                   in_dBm, out_pwr);
                tx_pwr[0] = (uint32)out_pwr; 
                //tx_pwr[0] = _vlaue_translate(argv[2]);        
                _europa_cal_data_add(EUROPA_CAL_TXPWR1_ADDR, tx_pwr[0]);                       
                tx_mpd[0] = _rtl8291_cal_get_mpd_current(devId);
                _europa_cal_data_add(EUROPA_CAL_TXMPD1_ADDR, tx_mpd[0]);           
                printf("Data1: Power=%6d (0.1uW)  Impd=%6d (uA/32)\n",
                   tx_pwr[0], tx_mpd[0]);

                rtk_ldd_apcIavg2_get(devId, &tx_iavg);
                _europa_cal_data_add(EUROPA_CAL_TX_IAVG_ADDR, tx_iavg); 
                printf("Iavg power = 0x%x\n", tx_iavg);     
#if 0        
                setenv("TXPWR1",argv[2],1);
                //setenv("TXMPD1","AA",1);    
                ret = system("export TXMPD1=AA");
                
                printf("ret = %d \n",ret);
                printf("TXPWR1=%s \n",getenv("TXPWR1"));
                printf("TXPWR2=%s \n",getenv("TXPWR2"));
                printf("TXMPD1=%s \n",getenv("TXMPD1"));
                printf("TXMPD2=%s \n",getenv("TXMPD2"));
#endif                                
               }
            else if (argc == 3)
            {
                printf("Set TXPWR1 data.\n"); 
                //in_dBm = atof(argv[2]);
                //out_pwr = _8291_dBm2001uw(in_dBm);
                //tx_pwr[0] = (uint32)out_pwr; 
                tx_pwr[0] = _vlaue_translate(argv[2]);        
                _europa_cal_data_add(EUROPA_CAL_TXPWR1_ADDR, tx_pwr[0]);                       
                tx_mpd[0] = _rtl8291_cal_get_mpd_current(devId);
                _europa_cal_data_add(EUROPA_CAL_TXMPD1_ADDR, tx_mpd[0]);           
                printf("Data1: Power=%6d (0.1uW)  Impd=%6d (uA/32)\n",
                   tx_pwr[0], tx_mpd[0]);

                rtk_ldd_apcIavg2_get(devId, &tx_iavg);
                _europa_cal_data_add(EUROPA_CAL_TX_IAVG_ADDR, tx_iavg); 
                printf("Iavg power = 0x%x\n", tx_iavg);                                  
            }            
            else
            {
                printf("%s %s <tx_pwr1: Unit is 0.1uW> \n", argv[0], argv[1]);
                printf("%s %s dbm <tx_pwr1: Unit is dbm> \n", argv[0], argv[1]);
                return -1;
            }
        }
        else if(0 == strcmp(argv[1], "2"))
        {      
            if ((argc >= 4)&&((0 == strcmp(argv[2], "dbm"))||(0 == strcmp(argv[2], "dBm"))))
            {
                printf("Set TXPWR2 data.\n"); 
                in_dBm = atof(argv[3]);
                out_pwr = _8291_dBm201uw(in_dBm);
                tx_pwr[1] = (uint32)out_pwr; 
                //tx_pwr[1] = _vlaue_translate(argv[2]);
                printf("in_dBm = %f (dBm)  out_pwr =%f (0.1uW)\n", in_dBm, out_pwr);                
                _europa_cal_data_add(EUROPA_CAL_TXPWR2_ADDR, tx_pwr[1]);                       
                tx_mpd[1] = _rtl8291_cal_get_mpd_current(devId);
                _europa_cal_data_add(EUROPA_CAL_TXMPD2_ADDR, tx_mpd[1]);           
                printf("Data2: Power=%6d (0.1uW)  Impd=%6d (uA/32)\n",
                   tx_pwr[1], tx_mpd[1]);
        
#if 0           
                setenv("TXPWR2",argv[2],1);
                setenv("TXMPD2","BB",1);

                printf("TXPWR1=%s \n",getenv("TXPWR1"));
                printf("TXPWR2=%s \n",getenv("TXPWR2"));
                printf("TXMPD1=%s \n",getenv("TXMPD1"));
                printf("TXMPD2=%s \n",getenv("TXMPD2"));
#endif
                
#if 0            
               tx_pwr[1] = *value_ptr;
               tx_mpd[1] = _ldd_get_mpd_current();
        
               printf("Data1: Power=%6dnW Impd=%6dnA\n",
                   tx_pwr[0], tx_mpd[0]);
               printf("Data2: Power=%6dnW Impd=%6dnA\n",
                   tx_pwr[1], tx_mpd[1]); 
#endif               
            }
            else if (argc == 3)
            {
                printf("Set TXPWR2 data.\n"); 
                //in_dBm = atof(argv[2]);
                //out_pwr = _8291_dBm2001uw(in_dBm);
                //tx_pwr[1] = (uint32)out_pwr; 
                tx_pwr[1] = _vlaue_translate(argv[2]);        
                _europa_cal_data_add(EUROPA_CAL_TXPWR2_ADDR, tx_pwr[1]);                       
                tx_mpd[1] = _rtl8291_cal_get_mpd_current(devId);
                _europa_cal_data_add(EUROPA_CAL_TXMPD2_ADDR, tx_mpd[1]);           
                printf("Data2: Power=%6d (0.1uW)  Impd=%6d (uA/32)\n",
                   tx_pwr[1], tx_mpd[1]);               
            }            
            else
            {
                printf("%s %s <tx_pwr2: Unit is 0.1uW> \n", argv[0], argv[1]);
                printf("%s %s dbm <tx_pwr2: Unit is dbm> \n", argv[0], argv[1]);
                return -1;
            }            
        }  
        else if(0 == strcmp(argv[1], "3"))
        {       
            if (argc >= 3)
            {
                printf("Set TXPWR1 data(Offline).\n"); 
                tx_pwr[0] = _vlaue_translate(argv[2]);          
                _europa_cal_data_add(EUROPA_CAL_TXPWR1_ADDR, tx_pwr[0]);                       
                tx_mpd[0] = _vlaue_translate(argv[3]);
                _europa_cal_data_add(EUROPA_CAL_TXMPD1_ADDR, tx_mpd[0]);           
                printf("Data1: Power=%6d (0.01uW)  Impd=%6d (uA/32)\n",
                           tx_pwr[0], tx_mpd[0]);
        
                rtk_ldd_apcIavg2_get(devId, &tx_iavg);
                _europa_cal_data_add(EUROPA_CAL_TX_IAVG_ADDR, tx_iavg); 
                printf("Iavg power = 0x%x\n", tx_iavg);         
            }
            else
            {
                printf("%s %s <tx_pwr1: Unit is 0.01uW> \n", argv[0], argv[1]);
            }
        }
        else if(0 == strcmp(argv[1], "4"))
        {       
            if (argc >= 3)
            {
                printf("Set TXPWR2 data.\n"); 
                tx_pwr[1] = _vlaue_translate(argv[2]);          
                _europa_cal_data_add(EUROPA_CAL_TXPWR2_ADDR, tx_pwr[1]);                       
                tx_mpd[1] = _vlaue_translate(argv[3]);
                _europa_cal_data_add(EUROPA_CAL_TXMPD2_ADDR, tx_mpd[1]);           
                printf("Data2: Power=%6d (0.1uW)  Impd=%6d (uA/32)\n",
                           tx_pwr[1], tx_mpd[1]);
                             
            }
            else
            {
                printf("%s %s <tx_pwr2: Unit is 0.1uW> \n", argv[0], argv[1]);
            }             
        } 

    }
    else
    {
        printf("%s <case> \n", argv[0]);
        printf("case can be 0, 1, 2, and calc, input power unit is 0.01uW or dBm\n");
        printf("europacli cal txddmi <case:1, 2> <Power: unit is 0.01uW>\n");
        printf("europacli cal txddmi <case:1, 2> <dbm or dBm> <Power: unit is dBm\n");        
        return -1;
    }

#ifdef RTL8291_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: europa_cli_cal_tx_ddmi Duration = %f ms\n", Duration);
#endif

    return 0;    
}

int rtl8291_cli_txpower_get(uint32 devId, uint32 loopcnt, uint32 r_mpd, uint32 code_250u, uint32 i_cal)
{

    int ret;
    int tx_a, tx_b, tx_c;
    int shft_a, shft_b, shft_c;        
    uint32 i_mpd, temp32;
    int32 i;
    uint64 sum_mpd=0, sum_txpwr=0;
    int32 tx_power, tx_power_hw;
    uint32 parm;
    double temp_f, i_mpd_nA, temp_f2;
    uint32 ctrl_data;
    rtk_ldd_txpwr_t lddTxPwr;

    //osal_time_mdelay(500);    
    _8291_cal_flash_data_get(devId, RTL8291_TX_A_ADDR+RTL8291_PARAMETER_BASE_ADDR, 2, &temp32);
    tx_a = (int16)temp32;
    _8291_cal_flash_data_get(devId, RTL8291_TX_B_ADDR+RTL8291_PARAMETER_BASE_ADDR, 2, &temp32);
    tx_b = (int16)temp32;    
    _8291_cal_flash_data_get(devId, RTL8291_TX_C_ADDR+RTL8291_PARAMETER_BASE_ADDR, 2, &temp32);
    tx_c = (int16)temp32;
    _8291_cal_flash_data_get(devId, RTL8291_SHIFT_TX_A_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, &shft_a);
    _8291_cal_flash_data_get(devId, RTL8291_SHIFT_TX_B_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, &shft_b);
    _8291_cal_flash_data_get(devId, RTL8291_SHIFT_TX_C_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, &shft_c);
    printf("RTL8291 TX_A = %d, SHIFT_TX_A = %d\n", tx_a, shft_a);         
    printf("RTL8291 TX_B = %d, SHIFT_TX_B = %d\n", tx_b, shft_b);   
    printf("RTL8291 TX_C = %d, SHIFT_TX_C = %d\n", tx_c, shft_c);

    printf("Start to get RTL8291 TX Power ... \n");

    //rtk_ldd_parameter2_get(devId, 0x387, &ctrl_data);
    //rtk_ldd_parameter2_set(devId, 0x387, 0x10);

    printf("idx     I_MPD   TX_Power\n");     
    sum_mpd = 0;
    for (i=0;i<loopcnt;i++)
    {
        //printf("Loop %d: \n", i);
        memset(&lddTxPwr, 0, sizeof(rtk_ldd_txpwr_t));  
        lddTxPwr.op_mode = RTL8291_TX_HARDWARE_MODE;
        //lddTxPwr.tx_lsb = 4000000;
        //lddTxPwr.tx_code_250u = 2615;
        //lddTxPwr.tx_r_mpd = 500;
        //lddTxPwr.tx_code_250u = code_250u;
        //lddTxPwr.tx_r_mpd = r_mpd;
        //lddTxPwr.i_cal = i_cal;
        ret = rtk_ldd_tx_power2_get(devId, &lddTxPwr);
        if(ret)
        {
            printf("Get RTL8291 TX power fail!!! (%d)\n", ret);
            //rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);                
            return -1;
        }
        sum_mpd += lddTxPwr.i_mpd;
        sum_txpwr += lddTxPwr.tx_power;
        printf("%4d  %8d  %8d\n", i, lddTxPwr.i_mpd, lddTxPwr.tx_power);     
    }
        
    //printf("LSB = %d, code_250u(MPD_K) = %d, R_MPD = %d, I_cal = %d\n", lddTxPwr.tx_lsb, lddTxPwr.tx_code_250u, lddTxPwr.tx_r_mpd, lddTxPwr.i_cal); 
    
    i_mpd = (sum_mpd+loopcnt/2)/loopcnt;
    i_mpd_nA = (double)i_mpd*31.25;
    tx_power_hw = (sum_txpwr+loopcnt/2)/loopcnt;    
     
    //rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);

    if(0==i_mpd || 0==tx_a)
    {
        printf("Invalid MPD current or DDMI coefficients!!! i_mpd = %d, tx_a = %d\n", i_mpd, tx_a);
        temp_f = 0;
    }
    else if (0 == tx_b)
    {
        /* tx_b=0 means tx ddmi is calibrated at single point */
        temp_f = (double)i_mpd*(double)tx_a/(1<<shft_a);
    }
    else
    {
        temp_f = (double)i_mpd*(double)tx_a/(1<<shft_a) + \
        (double)(tx_b)/(1<<shft_b);
    }
         
     //20230620: unit is 0.1uW        
    tx_power = (0.0<temp_f) ? (uint32)(temp_f) : 0; /* 0.1 uW */
            
    temp_f2  = __log10((double)(tx_power)*1/10000)*10;                                   
    printf("Get RTL8291 TX power I_MPD = %f nA (%d uA/32)\n", i_mpd_nA, i_mpd);
    printf("TX Power = %d 0.1uW (%f dBm)\n", tx_power, temp_f2);
    temp_f2  = __log10((double)(tx_power_hw)*1/10000)*10;     
    printf("TX Power (HW) = %d 0.1uW (%f dBm)\n", tx_power_hw, temp_f2);
    
    return 0;    
}

int rtl8291_cli_rxparam_set(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr,length;
    int32 para;

    if(argc < 3)
    { 
        printf("%s <a/b/c/sha/shb/shc> <value>\n", argv[0]);
        return -1;    
    }

    if(0 == strcmp(argv[1], "a")) 
    {
        addr = RTL8291_RX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 2;
        printf("RX parameter a = %d\n",  para);        
    }
    else if(0 == strcmp(argv[1], "b")) 
    {
        addr = RTL8291_RX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 2;
        printf("RX parameter b = %d\n",  para);                
    }
    else if(0 == strcmp(argv[1], "c")) 
    {
        addr = RTL8291_RX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 2;
        printf("RX parameter c = %d\n",  para);        
    }
    else if(0 == strcmp(argv[1], "sha")) 
    {
        addr = RTL8291_SHIFT_RX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 1;
        printf("RX parameter shift a = %d\n",  para);        
    }
    else if(0 == strcmp(argv[1], "shb")) 
    {
        addr = RTL8291_SHIFT_RX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 1;
        printf("RX parameter shift b = %d\n",  para);                
    }
    else if(0 == strcmp(argv[1], "shc")) 
    {
        addr = RTL8291_SHIFT_RX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 1;
        printf("RX parameter shift c = %d\n",  para);        
    }
    else
    {
        printf("no such mode %s\n", argv[1]);
        return -1;
    }

    _8291_cal_flash_data_set(devId, addr, length, para);

    return 0;
}  

int rtl8291_cli_rxparam_get(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, para1;

    addr = RTL8291_RX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 2, &para1);
    printf("RX parameter a: flash = %d\n",  para1);
    addr = RTL8291_RX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 2, &para1);
    printf("RX parameter b: flash = %d\n", para1);
    addr = RTL8291_RX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 2, &para1);
    printf("RX parameter c: flash = %d\n", para1);

    addr = RTL8291_SHIFT_RX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 1, &para1);
    printf("RX parameter shift a: flash = %d\n",  para1);
    addr = RTL8291_SHIFT_RX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 1, &para1);
    printf("RX parameter shift b: flash = %d\n", para1);
    addr = RTL8291_SHIFT_RX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 1, &para1);
    printf("RX parameter shift c: flash = %d\n", para1);

    return 0;
}    /* end of cparser_cmd_debug_europa_get_rx_param */

int rtl8291_cli_txparam_set(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr;
    int32 para;

    if(argc < 3)
    { 
        printf("%s <a/b/c/sha/shb/shc> <value>\n", argv[0]);
        return -1;    
    }

    if(0 == strcmp(argv[1], "a")) 
    {
        addr = RTL8291_TX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter a = %d\n",  para);
        _8291_cal_flash_data_set(devId, addr, 2, para);
    }
    else if(0 == strcmp(argv[1], "b")) 
    {
        addr = RTL8291_TX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter b = %d\n",  para);
        _8291_cal_flash_data_set(devId, addr, 2, para);
    }
    else if(0 == strcmp(argv[1], "c")) 
    {
        addr = RTL8291_TX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter c = %d\n",  para);
        _8291_cal_flash_data_set(devId, addr, 2, para);
    }
    else if(0 == strcmp(argv[1], "sha")) 
    {
        addr = RTL8291_SHIFT_TX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter shift a = %d\n",  para);
        _8291_cal_flash_data_set(devId, addr, 1, para);
    }
    else if(0 == strcmp(argv[1], "shb")) 
    {
        addr = RTL8291_SHIFT_TX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter shift b = %d\n",  para); 
        _8291_cal_flash_data_set(devId, addr, 1, para);
    }
    else if(0 == strcmp(argv[1], "shc")) 
    {
        addr = RTL8291_SHIFT_TX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter shift c = %d\n",  para); 
        _8291_cal_flash_data_set(devId, addr, 1, para);
    }
    else
    {
        printf("no such mode %s\n", argv[1]);
        return -1;
    }

    return 0;

}  

int rtl8291_cli_txparam_get(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, para1;

    addr = RTL8291_TX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 2, &para1);
    printf("TX parameter a: flash = %d\n",  para1);
    addr = RTL8291_TX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 2, &para1);
    printf("TX parameter b: flash = %d\n", para1);
    addr = RTL8291_TX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 2, &para1);
    printf("TX parameter c: flash = %d\n", para1);

    addr = RTL8291_SHIFT_TX_A_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 1, &para1);
    printf("TX parameter shift a: flash = %d\n",  para1);
    addr = RTL8291_SHIFT_TX_B_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 1, &para1);
    printf("TX parameter shift b: flash = %d\n", para1);
    addr = RTL8291_SHIFT_TX_C_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 1, &para1);
    printf("TX parameter shift c: flash = %d\n", para1);

    return 0;
} 


int rtl8291_cli_cal_power(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;      
    int ret;
    double measured_pwr, target_pwr;
    double measured_uW, target_uW;
    double current_iavg, target_iavg, w71_iavg, w71_target;
    uint32 data_w71_original, data_w71_target, data_temp;

    if(argc < 2)
    {
        addr = RTL8291_APC_IAVG_SET_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);  
        printf("Get RTL8291 iavg power = 0x%x\n", value);   
        rtk_ldd_apcIavg2_get(devId, &value); 
        printf("Current RTL8291 iavg power = 0x%x\n", value);          
        //printf("%s auto <measured pwr> <target pwr> \n", argv[0]);   
        //printf("Or %s <value>\n", argv[0]);         
        //return -1;
    }
    else if (argc == 2)
    {
        if ((ret=strcmp(argv[1], "auto")) != 0 )
        { 
            value = _vlaue_translate(argv[1]);   
            addr = RTL8291_APC_IAVG_SET_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8291 iavg power = 0x%x\n", value);     
            rtk_ldd_apcIavg2_set(devId, value);        
        }
        else
        { 
            printf("%s auto <measured pwr> <target pwr> \n", argv[0]);   
        }
    }
#if 1
    else if(argc >= 4)
    {
        if ((ret=strcmp(argv[1], "auto")) == 0 )
        {

            measured_pwr = atof(argv[2]);     
            target_pwr = atof(argv[3]); 
            
            //printf("measured pwr = %lf dBm,  target pwr = %lf dBm  \n", measured_pwr, target_pwr);           
            //europa_i2c_read(EUROPA_I2C_PORT, 0x54, 58, ); 
            //RTL8291_WB_REG71_REG_APCDIG_IAVG_DAC_7_0 = RTL8290B_W58
            rtk_ldd_parameter2_get(devId, 0x2F1, &data_w71_original);    
            //printf("W4B= 0x%x \n", data_w4b_original);        
            w71_iavg = (double)data_w71_original/32;
            current_iavg = 25*pow(2,w71_iavg);
            //printf("current_iavg = %lf \n", current_iavg);        
            measured_uW = _8291_dBm2uw(measured_pwr);
            target_uW = _8291_dBm2uw(target_pwr);    
            //printf("measured pwr = %lf uW,    target pwr = %lf uW  \n", measured_uW, target_uW); 
            target_iavg = current_iavg*(target_uW/measured_uW);
            //printf("target_iavg = %lf \n", target_iavg);  
            w71_target = 32*log10(target_iavg/25)/log10(2);
            
            data_w71_target = (uint32)w71_target;
            
            printf("taget W71= 0x%x\n", data_w71_target); 
            
            if ((data_w71_target < 0x20)||(data_w71_target > 0xC0))
            {    
                printf("Overflow! Taget value should be 0x20~0xC0.\n");     
            }
            else
            {
                if (data_w71_target<(data_w71_target-0x10))
                {
                   //To avoid DCL lost lock, use while loop to do step-by-step.
                   data_temp = data_w71_target; 
                   while((data_temp-0x10)>data_w71_target)
                   {
                       data_temp-=0x10;
                       //europa_i2c_write(EUROPA_I2C_PORT, 0x54, 58, data_temp);
                       rtk_ldd_parameter2_set(devId, 0x2F1, data_temp);   

                       //printf("data_temp = 0x%x\n", data_temp);                            
                       osal_time_mdelay(20);    
                   }           
                }
                //europa_i2c_write(EUROPA_I2C_PORT, 0x54, 58, data_w58_target); 
                rtk_ldd_parameter2_set(devId, 0x2F1, data_w71_target);  
                addr = RTL8291_APC_IAVG_SET_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
                _8291_cal_flash_data_set(devId, addr, 1, data_w71_target);
                printf("Set iavg power = 0x%x\n", data_w71_target);                    
            }                
        }
        else
        {
            printf("%s auto <measured pwr> <target pwr> \n", argv[0]);   

        }

    }
#endif    
    else
    {   
        printf("Or %s <value>\n", argv[0]); 
    }

    return 0; 
}

int rtl8291_cli_cal_er(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret=0;
    uint32 addr, value; 
    uint32 er_idx=0, trim_idx=0, i, er_fin, er_trim_fin;
    float measured_er, target_er;
    float er_meas, er_targ, oma_meas, oma_targ, oma_k;
    float er_factor, lut_factor, er_factor_k;
    float er_trim_factor, er_trim_factor_k;
    float P1_k, P0_k, ER_k, ER_k_dB;
    float P1_k2, P0_k2, ER_k2, ER_k2_dB, oma_k2;

    if((argv[1]!=NULL)&&(ret=strcmp(argv[1], "auto")) == 0 )
    {
        if (argc < 4)
        {
           printf("%s auto <measured ER (dB)> <target ER (dB)> \n", argv[0]);  
           return ret; 
        }       
        measured_er = atof(argv[2]);     
        target_er = atof(argv[3]);

        //Step 0 
        rtk_ldd_apcEr2_get(devId, &value); 
        er_idx = value;
        printf("Current ER = 0x%x \n", value);   
    
        //Step 1~2
        //printf("Step0: Current ER = %d measured_er = %f (dB) target_er = %f (dB)\n", value, measured_er, target_er);
        er_targ = pow(10,target_er/10);        
        er_meas = pow(10,measured_er/10);
        oma_targ = 2*(er_targ-1)/(er_targ+1);
        oma_meas = 2*(er_meas-1)/(er_meas+1);
        //printf("Step1: measured_er = %f OMA_measure = %f target_er = %f OMA_target = %f\n", er_meas, oma_meas, er_targ, oma_targ);

        //Step 3~4
        er_factor = oma_targ/oma_meas;
        //printf("Step2: er_idx = 0x%x er_factor = %f \n", er_idx, er_factor);
        for (i=0;i<64;i++)
        {
            er_fin = i;
            lut_factor = rtl8291_er_factor[i][er_idx];
            er_factor_k = lut_factor;
            //printf("idx = %d, er_factor_k = %f \n", i, er_factor_k);            
            if (lut_factor>er_factor)
            {
                break;
            }
        }
        //printf("Result: er_fin = %d, er_factor_k = %f \n", er_fin, er_factor_k);    
#if 0
        //Step 5~7
        oma_k = oma_meas*er_factor_k;
        P1_k = (2 + oma_k)/2;
        P0_k = 2 - P1_k;
        ER_k = P1_k/P0_k;
        ER_k_dB = 10*log10(ER_k);    
        er_trim_factor = oma_targ/oma_k;
        //printf("Step3: oma_k = %f er_trim_factor = %f\n", oma_k, er_trim_factor);
        //printf("Step4(Optional): P1_k = %f P0_k = %f ER_k = %f ER_k_dB = %f\n", P1_k, P0_k, ER_k, ER_k_dB);
        //printf("Step5: trim_idx = 0x%x er_trim_factor = %f \n", trim_idx, er_trim_factor);
        for (i=0;i<32;i++)
        {
            er_trim_fin = i;
            lut_factor = rtl8291_er_trim_factor[trim_idx][i];
            //printf("idx = %d, er_trim_factor_k = %f \n", i, lut_factor);            
            if (lut_factor>er_trim_factor)
            {
                break;
            }
        }
        //printf("Result: er_trim_fin = %d, er_trim_factor_k = %f \n", er_trim_fin, lut_factor);

        oma_k2 = oma_k * lut_factor;
        P1_k2 = (2 + oma_k2)/2;
        P0_k2 = 2 - P1_k2;
        ER_k2 = P1_k2/P0_k2;
        ER_k2_dB = 10*log10(ER_k2);
        //printf("Step5(Optional): OMA_k2 = %f P1_k2 = %f P0_k2 = %f ER_k2 = %f ER_k2_dB = %f\n", oma_k2, P1_k2, P0_k2, ER_k2, ER_k2_dB);
#endif
        addr = RTL8291_APC_ER_SET_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, er_fin); 
        rtk_ldd_apcEr2_set(devId, er_fin);
        printf("Set RTL8291 er = 0x%x    \n", er_fin);  

#if 0
        addr = RTL8291_APC_ER_TRIM_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, er_trim_fin);    
        //printf("Set RTL8290C trim = 0x%x\n", er_trim_fin);      
        rtk_ldd_apcErTrim2_set(devId, er_trim_fin);
        printf("Set RTL8290C er = 0x%x  trim = 0x%x\n", er_fin, er_trim_fin);  
#endif
    }
    else if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value>0x3F)
        {
            printf("Input should be 0~0x3F\n");       
            return -1;
        }
        addr = RTL8291_APC_ER_SET_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, value);
        printf("Set RTL8291 er = 0x%x\n", value);   
        rtk_ldd_apcEr2_set(devId, value);         
    }
    else
    { 
        addr = RTL8291_APC_ER_SET_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8291 er = 0x%x\n", value);
    }

    return 0; 
}

int rtl8291_cli_cal_erc_comp(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2;
    int  cal_value, step;      
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif   
#if 0
    if((argc < 1)||(argc >=3))
    { 

        printf("%s <value>\n", argv[0]);
        printf("<value> should be 0~3. 0 for disable.\n");                
        return -1; 
    }
    else
    { 
        if(argc == 1)
        {
            addr = RTL8291_ERC_COMP_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
            _8291_cal_flash_data_get(devId, addr, 1, &value); 
            printf("ERC comp enable = 0x%x\n", value);
        }
        else 
        { 
            step = _vlaue_translate(argv[1]);
            
            if ((step<0)||(step>3))
            {
                printf("%s <value>\n", argv[0]);
                printf("<value> should be 0~3. 0 for disable.\n");
                return -1;
            }
            

            if (step ==0)
            {        
                value = 0;     
                addr = RTL8291_ERC_COMP_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
                _8291_cal_flash_data_set(devId, addr, 1, value);
    
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 67, &value);        
                value = value|0x80;      
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, value);    
                
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 124, &value2);    
                value = value2&0xF7;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);    
                value = value2|0x08;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);
                
                printf("Set ERC comp disable\n");  
            }        
            else
            {
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 67, &value);        
                value = value|0x80;      
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, value);    
                
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 124, &value2);    
                value = value2&0xF7;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);    
                value = value2|0x08;
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 124, value);
                osal_time_mdelay(10);
            
                addr = RTL8291_ERC_COMP_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
                _8291_cal_flash_data_set(devId, addr, 1, step);
    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 77, 0xD0);         
                osal_time_mdelay(10);
                europa_i2c_read(EUROPA_I2C_PORT, 0x55, 29, &value);                
    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 77, 0xC0);                 
                osal_time_mdelay(10);
                europa_i2c_read(EUROPA_I2C_PORT, 0x55, 29, &value2); 
                
                //printf("value = 0x%x (%d) value2 = 0x%x (%d) \n", value, value, value2, value2);     
                
                printf("Do ERC ...\n");       
                if (value2 > 0x7F)
                {
                    printf("Value2 = 0x%x overflow!\n", value2); 
                }
                    
                cal_value = (int)(value2&0x7F);        
                
                if ((value&0x01)==0)
                {
                    cal_value = 0 - cal_value;
                } 
                
                //printf("Before: cal_value = %d \n", cal_value); 
                
                cal_value = cal_value - 15*step;
    
                //printf("after: cal_value = %d \n", cal_value); 
    
                if (cal_value<=-127)
                {
                    cal_value = -127;
                }
                
                value = abs(cal_value);
                if (cal_value > 0)
                {
                    value = 0x80 | value;
                }
    
                //printf("after: W69 = 0x%x \n", value); 
    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, 0x82);    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 69, value);    
                europa_i2c_write(EUROPA_I2C_PORT, 0x54, 67, 0xA2);    
    
                europa_i2c_read(EUROPA_I2C_PORT, 0x54, 69, &value); 
                //printf("Read W69 = 0x%x \n", value);             
                
                printf("Set ERC comp enable\n");             
            } 
        }     
    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_erc_comp Duration = %f ms\n", Duration);
#endif
#endif    
    return 0; 
}

#if 0
int rtl8291_cli_cal_trim(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value; 
    
    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value>0x1F)
        {
            printf("Input should be 0~0x1F\n");       
            return -1;
        }
        addr = RTL8291_APC_ER_TRIM_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, value);
        printf("Set RTL8291 trim = 0x%x\n", value);      
        rtk_ldd_apcErTrim2_set(devId, value);                           
    }
    else
    { 
        addr = RTL8291_APC_ER_TRIM_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8291 trim = 0x%x\n", value);
        rtk_ldd_apcErTrim2_get(devId, &value); 
        printf("Current trim = 0x%x\n", value);        
    }
   
    return 0; 
}
#endif

int rtl8291_cli_cal_los(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;
    uint32 data;
   
    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        addr = RTL8291_RXLOS_REF_DAC_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, value);
        printf("Set RTL8291 los = 0x%x\n", value);     
        rtk_ldd_rxlosRefDac2_set(devId, (value&0x7F));        
    }
    else
    { 
        addr = RTL8291_RXLOS_REF_DAC_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8291 los = 0x%x\n", value);
        rtk_ldd_rxlosRefDac2_get(devId, &data);
        printf("Current los = 0x%x\n", data);
    }
    
    return 0; 
}

int rtl8291_cli_cal_hyst(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;
       
    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]); 
        if (value >0x7)
        {
            printf("HYST value should be 0~0x7\n");
            return -1; 
        }    
        addr = RTL8291_RXLOS_HYST_SEL_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, value);
        printf("Set hyst = 0x%x\n", value);   
        rtk_ldd_rxlosHystSel2_set(devId, value);                 
    }
    else
    { 
        addr = RTL8291_RXLOS_HYST_SEL_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8291 hyst = 0x%x\n", value); 
        rtk_ldd_rxlosHystSel2_get(devId, &value); 
        printf("Current hyst = 0x%x\n", value); 

    }
  
    return 0; 
}

#if 1
int rtl8291_cli_cal_cmpd(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 addr, value, value2;      
    uint32 bias, mod, cmpd;
    uint32 chipId, rev, subType;
    uint32 ib, im; 
      
    if(argc >= 2)
    { 
#if 0
        if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
        {
            return ret;
        }
    
        if(0 == strcmp(argv[1], "check"))
        {  
            //_rtl8291_ibim_low(devId);
    
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                system("diag reg set 0x40a30 0x000"); 
                system("diag register set 0x40540 0x30");                
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                system("diag reg set 0x022A30 0x000"); 
                system("diag register set 0x22540 0x30");  
            }
            else
            {
                printf("Unknow Chip ID, do nothing...\n");
            }

            osal_time_mdelay(100);

            bias = 0;
            mod = 0;
            cmpd = 0;
            ret = rtk_ldd_parameter2_get(devId, 0x2CD, &value);
            if(ret)
            {
                printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
                return -1;
            }
            cmpd = value&0x7;
            
            while((bias>=mod)&&(cmpd !=0))
            {
                ret = rtk_ldd_tx_bias2_get(devId, &bias);
                if(ret)
                {
                    printf("Get TX Bias Fail!!! (%d)\n", ret);
                    return -1;
                }
                        

                ret = rtk_ldd_tx_mod2_get(devId, &mod);
                if(ret)
                {
                    printf("Get TX Mod Fail!!! (%d)\n", ret);
                    return -1;
                }
                
                //printf("cmpd = 0x%x, ib = 0x%x, im = 0x%x\n", cmpd, bias, mod);

                if (bias>=mod)
                {
                    cmpd = cmpd-1;
                    ret = rtk_ldd_parameter2_get(devId, 0x2CD, &value);
                    if(ret)
                    {
                        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
                        return -1;
                    }   
                    value = (value&0xF8)|cmpd;
                    ret = rtk_ldd_parameter2_set(devId, 0x2CD, value);
                    if(ret)
                    {
                        printf("rtk_ldd_parameter2_set failed (%d)\n", ret);
                        return -1;
                    }
                    osal_time_mdelay(150);                        
                }
            }

            addr = RTL8291_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _8291_cal_flash_data_set(devId, addr, 1, cmpd);            
            printf("cmpd = 0x%x, ib = 0x%x, im = 0x%x\n", cmpd, bias, mod);

            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                //EPON PRBS OFF
                system("diag register set 0x40a30 0"); 
                //GPON PRBS OFF
                system("diag register set 0x40098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x40540 0");
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                //EPON PRBS OFF           
                system("diag register set 0x22A30 0");
                //GPON PRBS OFF
                system("diag register set 0x22098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");    
                /*force BEN=0*/                
                system("diag register set 0x22540 0");
            }
            else
            {
                printf("Unknow Chip ID, do nothing...\n");
            } 

            //_rtl8291_ibim_recover(devId);  
            
        }
        else
#endif            
        {    
            value = _vlaue_translate(argv[1]); 
            if (value > 0x7)
            {
                printf("Value = 0x%x > 0x7\n", value);
                return -1; 
            }
            
            addr = RTL8291_APC_CMPD_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            rtk_ldd_parameter2_get(devId, 0x2F2, &value2);    
            value2 = (value2&0xE3)|(value&0x7)<<2;
            rtk_ldd_parameter2_set(devId, 0x2F2, value2);             
            printf("Set RTL8291 cmpd = 0x%x (0x%x)\n", value, value2);
        }
    }
    else
    { 
        addr = RTL8291_APC_CMPD_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8291 cmpd = 0x%x\n", value);
        rtk_ldd_parameter2_get(devId, 0x2F2, &value2);     
        printf("Current cmpd = 0x%x (Total = 0x%x)\n", (value2&0x1C)>>2, value2);    
    }
    
    return 0; 
}
#endif

int rtl8291_cli_cal_lpf(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2;
    
    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value > 0x3)
        {
            printf("Value = 0x%x > 0x3\n", value);
            return -1; 
        }
    
        addr = RTL8291_APC_LPF_BW_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, value);
        rtk_ldd_parameter2_get(devId, 0x2EC, &value2);  
        value2 = (value2&0xFC)|value;
        rtk_ldd_parameter2_set(devId, 0x2EC, value2);            
        printf("Set RTL8291 LPF = 0x%x (0x%x)\n", value, value2);                         
    }
    else
    { 
        addr = RTL8291_APC_LPF_BW_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8291 LPF = 0x%x\n", value);
        rtk_ldd_parameter2_get(devId, 0x2EC, &value2);  
        value = (value2&0x03);          
        printf("Current LPF = 0x%x (Total = 0x%x)\n", value, value2);    
    }
    
    return 0; 
}

int rtl8291_cli_cal_cross(
    uint32 devId, 
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2, value3;      
    uint32 data1, data2;
    rtk_enable_t enable;

    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "show"))
        {
            addr = RTL8291_TX_CROSS_EN_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
            _8291_cal_flash_data_get(devId, addr, 1, &value); 
            printf("RTL8291 tx cross enable = 0x%x\n", value);
            addr = RTL8291_TX_CROSS_SIGN_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
            _8291_cal_flash_data_get(devId, addr, 1, &value); 
            printf("RTL8291 tx cross sign = 0x%x\n", value);
            addr = RTL8291_TX_CROSS_STR_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
            _8291_cal_flash_data_get(devId, addr, 1, &value); 
            printf("RTL8291 tx cross str = 0x%x\n", value);
            rtk_ldd_txCross2_get(devId, &enable, &data1, &data2);              
            printf("RTL8291 Current tx cross enable = 0x%x, sign = 0x%x, str = 0x%x\n", enable, data1, data2);            
        }
        else if(0 == strcmp(argv[1], "disable"))
        {        
            value = 0;     
            addr = RTL8291_TX_CROSS_EN_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8291 TX cross disable\n");
            rtk_ldd_txCross2_get(devId, &value, &value2, &value3);    
            rtk_ldd_txCross2_set(devId, 0, value2, value3);    
        }
        else if(0 == strcmp(argv[1], "enable"))
        {        
            value = 1;     
            addr = RTL8291_TX_CROSS_EN_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8291 TX cross enable\n");
            rtk_ldd_txCross2_get(devId, &value, &value2, &value3);    
            rtk_ldd_txCross2_set(devId, 1, value2, value3);    
        }        
        else if(0 == strcmp(argv[1], "up"))
        {
            if(argc >= 3) 
            {
                value2 = _vlaue_translate(argv[2]);
                if (value2>0x3)
                {
                    printf("%s <up> <value:0~3>\n", argv[0]);
                    return -1; 
                }
                value = 0;
                addr = RTL8291_TX_CROSS_SIGN_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
                _8291_cal_flash_data_set(devId, addr, 1, value); 
                printf("RTL8291 tx cross sign = 0x%x\n", value);
                value = 1;          
                addr = RTL8291_TX_CROSS_EN_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
                _8291_cal_flash_data_set(devId, addr, 1, value); 
                printf("RTL8291 tx cross enable = 0x%x\n", value);
                          
                addr = RTL8291_TX_CROSS_STR_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
                _8291_cal_flash_data_set(devId, addr, 1, value2); 
                printf("RTL8291 tx cross str = 0x%x\n", value2);  
                rtk_ldd_txCross2_set(devId, 1, 0, value2);            
            }
            else
            {
                printf("%s <up> <value:0~3>\n", argv[0]);
                return -1; 
            } 
            
        }
        else if(0 == strcmp(argv[1], "down"))
        {        
            if(argc >= 3) 
            {
                value2 = _vlaue_translate(argv[2]);
                if (value2>0x3)
                {
                    printf("%s <down> <value:0~3>\n", argv[0]);
                    return -1; 
                }            
                value = 1;
                addr = RTL8291_TX_CROSS_SIGN_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
                _8291_cal_flash_data_set(devId, addr, 1, value); 
                printf("RTL8291 tx cross sign = 0x%x\n", value);
                value = 1;          
                addr = RTL8291_TX_CROSS_EN_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
                _8291_cal_flash_data_set(devId, addr, 1, value); 
                printf("RTL8291 tx cross enable = 0x%x\n", value);
           
                addr = RTL8291_TX_CROSS_STR_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
                _8291_cal_flash_data_set(devId, addr, 1, value2); 
                printf("RTL8291 tx cross str = 0x%x\n", value2);  
                rtk_ldd_txCross2_set(devId, 1, 1, value2);                    
            }
            else
            {
                printf("%s <down> <value:0~3>\n", argv[0]);
                return -1; 
            }

        }
        else
        {
            printf("%s <up/down> <value:0~3>\n", argv[0]);
            printf("%s <show/disable>\n", argv[0]);
            return -1;  
        }  
       
    }
    else
    { 

        printf("%s <up/down> <value>\n", argv[0]);
        printf("%s <show/enable/disable>\n", argv[0]);
        return -1; 
    }

    return 0; 
}



int rtl8291_cli_cal_refresh(
    uint32 devId,  
    int argc,
    char *argv[],
    FILE *fp)
{ 

    rtk_ldd_config2_refresh(devId); 

    return 0;
}    

void _rtl8291_gen_laserlut(uint32 devId, uint32 bias, uint32 mod, int32 temperature, double sb0, double sb1, double sm0, double sm1)
{
    int ret;
    uint32  ib0, im0;
    double  ftmp;
    int i;
    
    uint32 lut[RTL8291_LASER_LUT_SZ] = { 0 };
    int t, t0;
    uint8 txlut[RTL8291_TXLUT_SZ] = { 0 };

    ib0 = bias;
    im0 = mod;
    t0  = temperature;
        
    printf("Ib=%d Im=%d T0=%d SB0=%f SB1=%f SM0=%f SM1=%f\n",
            ib0, im0, t0, sb0, sb1, sm0, sm1);
        
    for (t=-40; t<=120; t++)
    {
       if (t == t0)
       {
           lut[(t+40)*2]   = RTL8291_IBCONV(ib0);
           lut[(t+40)*2+1] = RTL8291_IMCONV(im0);
       }
       else if (t < t0)
       {
           ftmp = ib0 - (t0 - t)*sb0;
           if (3.0 > ftmp)
              ftmp=3.0;
           lut[(t+40)*2] = RTL8291_IBCONV((int)ftmp);
        
           ftmp = im0 - (t0 - t)*sm0;
           if (12.0 > ftmp)
               ftmp=12.0;
           lut[(t+40)*2+1] = RTL8291_IMCONV((int)ftmp);
       }
       else if (t > t0)
       {
           ftmp = ib0 + (t - t0)*sb1;
           if (ftmp>50.0)
           ftmp=50.0;
           lut[(t+40)*2] = RTL8291_IBCONV((int)ftmp);
        
           ftmp = im0 + (t - t0)*sm1;
           if (ftmp>80.0)
              ftmp=80.0;
           lut[(t+40)*2+1] = RTL8291_IMCONV((int)ftmp);
       }
       if ((t%5)==0)
       {
           printf("T = %4d IB: 0x%4x IM: 0x%4x \n", t, lut[(t+40)*2], lut[(t+40)*2+1]);       
       }
    }

    for (i=0; i<=31; i++)
    {
       t= i*10;
       //APCDIG_BIAS_INI<10:4>
       txlut[i*2] = (uint8)((lut[t]&0x7F0)>>4);       
       //APCDIG_BIAS_INI<3:0>
       txlut[i*2+1] = (uint8)(lut[t]&0xF);
       //APCDIG_MOD_INI<10:4>
       txlut[i*2+RTL8291_TXLUT_OFFSET] = (uint8)((lut[t+1]&0x7F0)>>4);       
       //APCDIG_MOD_INI<3:0>
       txlut[i*2+RTL8291_TXLUT_OFFSET+1] = (uint8)(lut[t+1]&0xF); 
       printf("Selected: i = %2d IB: 0x%4x IM: 0x%4x \n", i, lut[t], lut[t+1]);       
    }
    
    printf("Write to flash.... \n");         
    for (i=0;i<RTL8291_TXLUT_SZ;i++)
    {
        if(i%16 == 0)
        {
            printf("0x%04x ", RTL8291_TXLUT_ADDR_BASE +i);
        }

        printf("0x%02x ", txlut[i]);
        if(i%8 == 7)
            printf("   ");
        if(i%16 == 15)
            printf("\n");                    
    }

        
    _8291_cli_flash_array_set(devId, RTL8291_TXLUT_ADDR_BASE, RTL8291_TXLUT_SZ, txlut);
        
    for (t=-40; t<=120; t++)
    {
       printf("%4x %4x", lut[(t+40)*2], lut[(t+40)*2+1]);
       printf(" ");
        
       if ((t+1)%8 == 0)
       {
          printf("\n");
       }
    }
    printf("\n");

    return;    
}

int rtl8291_cli_laserlut_set(uint32 devId)
{
    int ret;
    uint32    bias, mod;
    double    sb0, sb1, sm0, sm1;
    int32     temperature;
    
    uint32 tempK, temp32;
        
    ret = rtk_ldd_tx_bias2_get(devId, &bias);
    if(ret)
    {
        printf("Get Bias Fail!!! (%d)\n", ret);
        return -1;
    }
    bias = (bias )/1000;  /* uA to mA */
    
    ret = rtk_ldd_tx_mod2_get(devId, &mod);
    if(ret)
    {
        printf("Get Mod Fail!!! (%d)\n", ret);
            return -1;
    }
    mod = (mod)/1000;  /* uA to mA */
    
    tempK = 0;
    ret = rtk_ldd_temperature2_get(devId, &temp32);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret); 
        return -1;
    }
    tempK = (temp32&0x7FC00)>>10;
    temperature = (int32)(tempK-273); /* degK to degC */
    
    printf("tempK = %d K, temperature = %d C\n", tempK, temperature);  
    
    sb0 = 0.06;
    sb1 = 0.12;
    sm0 = 0.03;
    sm1 = 0.10;
    
    _rtl8291_gen_laserlut(devId, bias, mod, temperature, sb0, sb1, sm0, sm1);

    return 0; 
}

int rtl8291_cli_laserinfo_get(uint32 devId)
{
    int ret;
    uint32 bias;
    uint32 mod;
    int8 offset;
    double ib, im;
    uint32 temp_data, addr;
    //float tempK, tempC;
    uint8 buf[2];
    double tmp = 0;

    bias = 0;    
    ret = rtk_ldd_tx_bias2_get(devId, &bias);
    if(ret)
    {
        printf("rtk_ldd_tx_bias2_get Fail!!! (%d)\n", ret);
        return -1;
    }
    ib = ((double)bias)/1000;  /* uA to mA */
    
    mod = 0;
    ret = rtk_ldd_tx_mod2_get(devId, &mod);
    if(ret)
    {
        printf("rtk_ldd_tx_mod2_get Fail!!! (%d)\n", ret);
        return -1;
    }
    im = ((double)mod)/1000;  /* uA to mA */


    ret = rtk_ldd_temperature2_get(devId, &temp_data);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return -1;
    }
    //temp32 = temp_data - RTL8291_TEMP_ZERO;

    //Diagshell
    //tmp = (-1)*((~(pSrcData->buf[0]))+1)+((double)(pSrcData->buf[1])*1/256);


    buf[0] = (temp_data&0xFF00)>>8;
    buf[1] = (temp_data&0xFF);
    if (128 >= buf[0]) //MSB: buf[0]; LSB: buf[1]
    {
        tmp = (-1)*((~(buf[0]))+1)+((double)(buf[1])*1/256);
    }
    else
    {
        tmp = (char)buf[0]+((double)(buf[1])*1/256);
    }
    //printf("RTL8291 tmp = %f\n", tmp); 

    addr = RTL8291_TEMP_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
    _8291_cal_flash_data_get(devId, addr, 1, &temp_data); 

    offset = temp_data&0xFF;

    printf("offset = 0x%x\n", offset); 

    tmp = tmp - offset;

    printf("Temperature = %f C\n", tmp); 

    printf("Bias=%2.2f Modulation=%2.2f\n", ib, im);

    return 0;
}


int rtl8291_cli_temperature_get(uint32 devId, int argc, char *argv[])
{
    int ret;
    uint32 temp_data, addr;
    int8 offset;
    //float tempK, tempC;
    int temp32;
    //signed short temp16;
    uint8 buf[2];
    double tmp = 0;
    uint32 reg_data1=0, reg_data2=0;

    if(argc >= 2)
    { 
        temp_data = _vlaue_translate(argv[1]);    

        printf("External Input: temp_data = 0x%x\n", temp_data); 

        buf[0] = (temp_data&0xFF00)>>8;
        buf[1] = (temp_data&0xFF);            
        if (128 >= buf[0]) //MSB: buf[0]; LSB: buf[1]
        {
            printf("DBG(<128): buf[0] = 0x%x, buf[1] = 0x%x\n", buf[0], buf[1]);       
            tmp = (-1)*((~(buf[0]))+1)+((double)(buf[1])*1/256);
            printf("DBG(Calc): (-1)*((~(buf[0]))+1) = %d, ((double)(buf[1])*1/256) = %f\n", (-1)*((~(buf[0]))+1), ((double)(buf[1])*1/256));			
        }
        else
        {
            printf("DBG(>128): buf[0] = 0x%x, buf[1] = 0x%x\n", buf[0], buf[1]);               
            tmp = (signed char)buf[0]+((double)(buf[1])*1/256);
            printf("DBG(Calc): (char)buf[0] = %d, ((double)(buf[1])*1/256) = %f\n", (signed char)buf[0], ((double)(buf[1])*1/256));
            temp32 = 0xFFFFFF00|buf[0];
            printf("DBG(Calc): (int)buf[0] = %d, temp32 = %d \n", (int)buf[0], temp32);		
        }

        printf("Temperature = %f C\n", tmp); 

        return 0;
    }


    ret = rtk_ldd_temperature2_get(devId, &temp_data);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return -1;
    }

    //rtk_ldd_parameter2_get(devId, 0x160, &reg_data1); 
    //rtk_ldd_parameter2_get(devId, 0x161, &reg_data2); 

    //printf("DBG: temp_data = 0x%x, 0x160 = 0x%x, 0x161 = 0x%x\n", temp_data, reg_data1, reg_data2); 

    buf[0] = (temp_data&0xFF00)>>8;
    buf[1] = (temp_data&0xFF);
    //printf("DBG: buf[0] = 0x%x, buf[1] = 0x%x\n", buf[0], buf[1]);    
    if (128 >= buf[0]) //MSB: buf[0]; LSB: buf[1]
    {
        tmp = (-1)*((~(buf[0]))+1)+((double)(buf[1])*1/256);
    }
    else
    {
        tmp = (signed char)buf[0]+((double)(buf[1])*1/256);
    }
    //printf("RTL8291 tmp = %f\n", tmp); 

    addr = RTL8291_TEMP_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
    _8291_cal_flash_data_get(devId, addr, 1, &temp_data); 

    offset = temp_data&0xFF;

    printf("offset = 0x%x\n", offset); 

    tmp = tmp - offset;

    printf("Temperature = %f C\n", tmp); 
    
    return 0;
}

int rtl8291_cli_vdd_get(uint32 devId)
{
    int ret;
    uint32 vdd = 0;
    uint8 buf[2];
    double tmp = 0;    

    ret = rtk_ldd_vdd2_get(devId, &vdd);
    if(ret)
    {
        printf("rtk_ldd_vdd2_get Fail!!! (%d)\n", ret);
        return -1;
    }

    buf[0] = (vdd&0xFF00)>>8;
    buf[1] = (vdd&0xFF);

    //Diagshell
    tmp = (double)((buf[0] << 8) | buf[1])*1/10000;
    
    printf("VDD =  %f V (0x%x)\n", tmp, vdd);
    
    return 0;    
}

int rtl8291_cli_txbias_set(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    double curr;
    uint32 Ibias;

    if(argc >= 2)
    { 
        curr = atof(argv[1]);    

        if (120.0 < curr)
        {
            printf("Invalid bias current %f\n", curr);
            return -1;
        }
    
        Ibias = (uint32)(curr * 1000); /* convert mA to uA unit */
    
        printf("Input current = %f mA, Ibias = %d uA\n", curr, Ibias);

        ret = rtk_ldd_tx_bias2_set(devId, Ibias);
        if(ret)
        {
            printf("Set RTL8291 TX Bias Fail!!! (%d)\n", ret);
            return -1;
        }
    
        return 0;

    }
    else
    {
        printf("%s <tx bias> \n", argv[0]);
        printf("The unit is mA \n");        
        return -1;
    }
    
    return 0;    
}


int rtl8291_cli_txbias_get(uint32 devId)
{
    int ret;
    uint32 regData1, regData2, biasRaw;
    uint32 bias;
    
    ret = rtk_ldd_parameter2_get(devId, 0x2F3, &regData1);
    if(ret)
    {
        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
        return -1;
    }
    ret = rtk_ldd_parameter2_get(devId, 0x2F5, &regData2);
    if(ret)
    {
        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
        return -1;
    }
    biasRaw = ((regData1&0x7F)<<4)|((regData2&0xF0)>>4);
    bias = ((biasRaw&0x7FF)*(120*1000))>>11;
    printf("Config: TX Bias =  %duA (0x%x)\n", bias, bias);

    bias = 0;    
    ret = rtk_ldd_tx_bias2_get(devId, &bias);
    if(ret)
    {
        printf("rtk_ldd_tx_bias2_get Fail!!! (%d)\n", ret);
        return -1;
    }
    printf("Current: TX Bias =  %duA (0x%x)\n", bias, bias);
    
    return 0;    
}

int rtl8291_cli_txmod_set(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    double curr;
    uint32 Imod;
    uint32 mod_sys;

    if(argc >= 2)
    { 
        curr = atof(argv[1]);    

        if (120.0 < curr)
        {
            printf("Invalid mod current %f\n", curr);
            return -1;
        }
    
        Imod = (uint32)(curr * 1000); /* convert mA to uA unit */

        printf("Input current = %f mA, Imod = %d uA\n", curr, Imod);
        
        ret = rtk_ldd_tx_mod2_set(devId, Imod);
        if(ret)
        {
            printf("Set TX Mod Fail!!! (%d)\n", ret);
            return -1;
        }

        return 0;
    }
    else
    {
        printf("%s <tx mod> \n", argv[0]);
        printf("The unit is mA \n");        
        return -1;
    }

    return 0;
}


int rtl8291_cli_txmod_get(uint32 devId)
{
    int ret;
    uint32 mod;
    uint32 regData1, regData2, modRaw;
    
    ret = rtk_ldd_parameter2_get(devId, 0x2F4, &regData1);
    if(ret)
    {
        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
        return -1;
    }
    ret = rtk_ldd_parameter2_get(devId, 0x2F5, &regData2);
    if(ret)
    {
        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
        return -1;
    }
    modRaw = ((regData1&0x7F)<<4)|(regData2&0xF);
    mod = ((modRaw&0x7FF)*(120*1000))>>11;
    printf("Config: TX MOD =  %duA (0x%x)\n", mod, mod);

    mod = 0;
    ret = rtk_ldd_tx_mod2_get(devId, &mod);
    if(ret)
    {
        printf("rtk_ldd_tx_mod2_get Fail!!! (%d)\n", ret);
        return -1;
    }
    printf("Current: TX MOD =  %duA (0x%x)\n", mod, mod);
    
    return 0;    
}

int rtl8291_cli_loopmode_set(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 loop_mode, value, tx_sd_threshold;

    //000: DOL, 001: APC_B, 010: APC_M, 011:APC and ERC timing division
    //100: DOL, 101: SCL_B, 110: ERC only, 111: APC and ERC simultaneously
    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "dol"))
        {
            loop_mode = LDD_LOOP2_DOL;
            printf("Change to DOL mode\n");

        }
        else if(0 == strcmp(argv[1], "dcl"))
        {
            loop_mode = LDD_LOOP2_DCL;
            printf("Change to DCL mode\n");

        }
        else if(0 == strcmp(argv[1], "apc_b"))
        {
            loop_mode = LDD_LOOP2_APC_B;
            printf("Change to APC Bias mode\n");

        }
        else if(0 == strcmp(argv[1], "apc_m"))
        {
            loop_mode = LDD_LOOP2_APC_M;
            printf("Change to APC Mod mode\n");
        }
        else if(0 == strcmp(argv[1], "scl-b"))
        {
            loop_mode = LDD_LOOP2_SCL_B;
            printf("Change to SCL Bias mode\n");
        }
        else if(0 == strcmp(argv[1], "erc"))
        {
            loop_mode = LDD_LOOP2_ERC;
            printf("Change to ERC only mode\n");
        }
        else if(0 == strcmp(argv[1], "apc_erc_s"))
        {
            loop_mode = LDD_LOOP2_APC_ERC_S;
            printf("Change to  APC and ERC simultaneously mode\n");
        }
        else
        {
            printf("%s <dol/dcl/apc_b/apc_m/scl-b/erc/apc_erc_s> \n", argv[0]);        
            return -1;
        }
            
        rtk_ldd_loopMode2_set(devId, loop_mode);

        _8291_cal_flash_data_set(devId, RTL8291_APC_LOOP_MODE_OFFSET_ADDR+ RTL8291_PARAMETER_BASE_ADDR, 1, loop_mode);           

        return 0;

    }
    else
    {
        printf("%s <dol/dcl/apc_b/apc_m/scl-b/erc/apc_erc_s> \n", argv[0]);        
        return -1;
    }

    return 0;    

}

int rtl8291_cli_loopmode_get(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 mode, value;

    rtk_ldd_parameter2_get(devId, 0x2F6, &mode);

    mode = (mode&0x70)>>4;

    //000: DOL, 001: APC_B, 010: APC_M, 011:APC and ERC timing division
    //100: DOL, 101: SCL_B, 110: ERC only, 111: APC and ERC simultaneously
    if (mode == 0)
    {
       printf("Loop mode(0x%x) is DOL mode\n", mode);

    }
    else if(mode == 0x3)
    {
        printf("Loop mode(0x%x) is DCL (APC and ERC timing division) mode\n", mode);
    }
    else if(mode == 0x2)
    {
        printf("Loop mode(0x%x) is APC_M mode\n", mode);
    }
    else if(mode == 0x1)
    {
        printf("Loop mode(0x%x) is APC_B mode\n", mode);
    }
    else if(mode == 0x5)
    {
        printf("Loop mode(0x%x) is SCL_B mode\n", mode);
    }
    else if(mode == 0x6)
    {
        printf("Loop mode(0x%x) is ERC only mode\n", mode);
    }
    else if(mode == 0x7)
    {
        printf("Loop mode(0x%x) is APC and ERC simultaneously mode\n", mode);
    }
    else
    {
        printf("Loop mode = 0x%x \n", mode);        
    }

    _8291_cal_flash_data_get(devId, RTL8291_APC_LOOP_MODE_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR, 1, &value);           

    printf("Loop mode in flash = 0x%x \n", value);    


    return 0;    

}


// * debug europa set apdlut ( factor | offset ) <UINT:rfb1> <UINT:rfb2> <UINT:vbr> <UINT:temp> <UINT:value> <UINT:c1> <UINT:c2>
//cparser_result_t
//cparser_cmd_debug_europa_set_apdlut_factor_offset_rfb1_rfb2_vbr_temp_value_c1_c2(
//    cparser_context_t *context,
//    uint32_t  *rfb1_ptr,
//    uint32_t  *rfb2_ptr,
//    uint32_t  *vbr_ptr,
//    uint32_t  *temp_ptr,
//    uint32_t  *value_ptr,
//    uint32_t  *c1_ptr,
//    uint32_t  *c2_ptr)
int rtl8291_cli_apdlut_set(
    uint32 devId,  
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret = 0;
    uint32 lut[161];
    uint32 factor, offset;
    uint32 vbr;
    uint32 dac, dac2;
    uint32 temp, c1, c2;
    int32 pos, pos2, i, j, num;
    uint32 slope;
    uint8 *init_data;
    uint32 rfb1, rfb2;
    uint64 vapd;
    uint32 v, vstd=0;
    uint32 dac_idx;
    uint32 apdlut[32];
    uint32 lut_idx;
    uint8 apdlut_flash[64];
    uint32 Vref, Vmax, Vmin, Voffset;
    float dark_curr;
    int32 temperature; 
    
    if((argc < 9)&&(argc !=2))
    {
        printf("%s factor/offset <UINT:rfb1, unit is Kohm> <UINT:rfb2, unit is Kohm> <UINT:vbr, unit is mV> <UINT:temp> <UINT:value for offset or factor> <UINT:c1> <UINT:c2> \n", argv[0]);
        printf("%s enable/disable\n", argv[0]); 
        return -1;
    }

    if (argc ==2)
    {
        if(0 == strcmp(argv[1], "enable"))
        {
            //rtk_ldd_calibration_state_set(ENABLED);
            rtk_ldd_parameter2_get(devId, 0x396, &temp);
            temp = (temp&0xF6)|(1<<3);
            rtk_ldd_parameter2_set(devId, 0x396, temp);
            _8291_cal_flash_data_set(devId, RTL8291_APD_LUT_EN_OFFSET_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, 1);     
            printf("Set APDLUT to Enable\n"); 
            return 0;        
        }
        else if(0 == strcmp(argv[1], "disable"))
        {
            //rtk_ldd_calibration_state_set(DISABLED);
            rtk_ldd_parameter2_get(devId, 0x396, &temp);
            temp = (temp&0xF6)|(1);
            rtk_ldd_parameter2_set(devId, 0x396, temp);
            _8291_cal_flash_data_set(devId, RTL8291_APD_LUT_EN_OFFSET_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, 0);             
            printf("Set APDLUT to Disable\n"); 
            return 0;        
        }
        else
        {
            printf("Input Error!!!\n");
            printf("%s enable/disable\n", argv[0]); 
            return -1;

        }
    }

    rfb1  = _vlaue_translate(argv[2]);
    rfb2  = _vlaue_translate(argv[3]);
    vbr   = _vlaue_translate(argv[4]);        
    temperature  = _vlaue_translate(argv[5]);
    if ((temperature<-40)||(temperature>120))
    {
        printf("Input temp Error!!! temp should be -40 ~ 120C.\n");
        return -1;
    }
    c1    = _vlaue_translate(argv[7]);
    c2    = _vlaue_translate(argv[8]);        

    if(vbr>0xFFFFFFFF)
    {
        printf("VBR input error!!!\n");
        return -1;
    }        

    if(0 == strcmp(argv[1], "factor"))
    {
        factor = _vlaue_translate(argv[6]);
        if(factor>0xFFFFFFFF)
        {
            printf("factor input error!!!\n");
            return -1;
        }        
        vapd = (uint64)vbr*factor/1000;
    }
    else if(0 == strcmp(argv[1], "offset"))
    {
        offset = _vlaue_translate(argv[6]);
        vapd = vbr-offset; /* offset is in mV also */
    }
    else
    {
        printf("no such mode %s\n", argv[1]);
        return -1;
    }

    Vref = 1300;
    dark_curr = 11.71875;
    dac_idx = 2;         
    Vmax = Vref + Vref*rfb1/rfb2 + rfb1*(25*dac_idx + 150)+(uint32)(dark_curr*rfb1);
    Vmin = Vref + Vref*rfb1/rfb2 + rfb1*(25*dac_idx)+(uint32)(dark_curr*rfb1);
    
    printf("Input Rfb1 = %d K, Rfb2 = %d K, Vref = %d mV, Dark Current = %f mA\n", rfb1, rfb2, Vref, dark_curr);
    printf("Vmax = %d mV, Vmin = %d mV, DAC Offset = %duA \n", Vmax, Vmin, 25*dac_idx);
    
    //dac = (vapd -27600000)/156250;
    //Vout  = 1200 + 1200 * (Rfb1/Rfb2) + Rfb1 * 12 + (Rfb1 * DAC_CODE * 78125) / 100000
    //dac = (vapd - 1200 - 1200*rfb1/rfb2 - rfb1*12)*100000/78125/rfb1;

    //Index 2 for DAC_OFFSET 50uA (0 for 0, 1 for 25uA, 2 for 50uA, ... 6 for 150uA)
    dac_idx = 2;
    ret = rtl8291_cal_booster_Vout2Dac(vapd, rfb1, rfb2, &dac, &dac_idx);
    if(ret)
    {
        printf("rtl8291_cal_booster_Dac2Vout!!! (%d)\n", ret);
        return -1;
    }

    if (dac>511)
    {
        printf("DAC over 511!!!\n");
        return -1;
    }

    if (dac_idx!=0)
    {
        printf("DAC_OFFSET Index = %d, The DAC offset current is %d uA!!!\n", dac_idx, dac_idx*25);
    }

    //RL6733 APD LUT from 233 K to 393 K
    pos = temperature + 40;
    if ((pos<0)||(pos>160))
    {
        printf("pos Error!!! pos should be 0 ~ 160.\n");
        return -1;
    }
    lut[pos] = dac;
    rtl8291_cal_booster_Dac2Vout(dac, dac_idx, rfb1, rfb2, &vstd);

    //For -40~temp
    if (pos>0)
    {
    pos2 = pos -1;
    slope= c1;
    while(pos2>=0)
    {
        //dac2 = dac - (pos-pos2)*slope/1000;
        //rtl8291_cal_booster_Dac2Vout(dac, dac_idx, rfb1, rfb2, &v);
        //printf("pos = %d,  pre V= %d, V = %d\n", pos2, v, v-(pos-pos2)*slope);  
        v = vstd - (pos-pos2)*slope;
        rtl8291_cal_booster_Vout2Dac(v, rfb1, rfb2, &dac2, &dac_idx);
        //printf("pos = %d, pos2 = %d, Vstd = %d, V = %d\n", pos, pos2, vstd, v); 
        if (dac2>0)
        {
           lut[pos2] = dac2;
        }
        else
        {
           lut[pos2] = 0;
        }
        pos2--;
    }
    }

    if (pos<=160)
    {    
    //For temp~120
    pos2 = pos + 1;
    slope= c2;
    while(pos2<=160)
    {
        //dac2 = dac + (pos2-pos)*slope/1000;
        //v = _europa_cal_booster_Dac2Vout(dac, rfb1, rfb2);
        //rtl8291_cal_booster_Dac2Vout(dac, dac_idx, rfb1, rfb2, &v);
        v = vstd + (pos2-pos)*slope;
        //dac2 = _europa_cal_booster_Vout2Dac(v, rfb1, rfb2);
        rtl8291_cal_booster_Vout2Dac(v, rfb1, rfb2, &dac2, &dac_idx);
        if (dac2>511)
        {
           lut[pos2] = 511;
        }
        else
        {
           lut[pos2] = dac2;
        }
        pos2++;
    }
    }
    printf("Temperature  &  DAC value\n ");

    for (i=0;i<11;i++)
    {
        if (i<10)
        {
            num =16;
        }
        else
        {
            num = 1;
        }

        printf("Temperature: ");
        for(j=0;j<num;j++)
        {
            printf("%5d ", j+i*16-40);
        }
        printf("\n ");
        printf("DAC:           ");
        for(j=0;j<num;j++)
        {
            printf("0x%03x ", lut[j+i*16]);
        }
        printf("\n\n ");
    }

    //6*6 + 25*5 = 161. 6+25 = 31
    lut_idx=0;
    memset(apdlut, 0, sizeof(uint32)*RTL8291_APD_LUT_SZ);
    for(i=0;i<RTL8291_APD_LUT_SZ;i++)
    {
        if(i<=2)
        {
            lut_idx=i*6;
        }
        else if (i<=29)
        {
            lut_idx=i*5+2;        
        }
        else
        {
            lut_idx=lut_idx+6;        
        }
        if (lut_idx<=160)
        {
            apdlut[i] = lut[lut_idx];
            apdlut_flash[i*2] = (uint8)(lut[lut_idx]&0x1FE)>>1;
            apdlut_flash[i*2+1] = (uint8)(lut[lut_idx]&0x1);            
            printf("apdlut[%02d] = lut[%03d] = 0x%x, apdlut_flash[%02d] = 0x%x, apdlut_flash[%02d] = 0x%x\n", i, lut_idx, lut[lut_idx], i*2, apdlut_flash[i*2], i*2+1, apdlut_flash[i*2+1]);            
        }
        else
        {
            printf("lut_idx Error!!! lut_idx = %d \n", lut_idx); 
        }
    }

    //_europa_cal_flash_array_set(RTL8290B_APD_LUT_ADDR, 151, lut);
    
    printf("Write to flash array.... \n");         
    for (i=0;i<64;i++)
    {
        if(i%16 == 0)
        {
            printf("0x%04x ", RTL8291_APDLUT_ADDR_BASE +i);
        }

        printf("0x%02x ", apdlut_flash[i]);
        if(i%8 == 7)
            printf("   ");
        if(i%16 == 15)
            printf("\n");                    
    }
       
    _8291_cli_flash_array_set(devId, RTL8291_APDLUT_ADDR_BASE, 64, apdlut_flash);
        
    return 0;
}


int rtl8291_cli_apdlut_get(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int start_addr, file_len, length;
    unsigned int input;
    unsigned int i;
    uint32 value;
    //unsigned char temp[EUROPA_PARAMETER_SIZE];

    //rtk_ldd_calibration_state_set(ENABLED);
    rtk_ldd_parameter2_get(devId, 0x396, &value);
    printf("RTL8291 APD_LUT_ENABLE = %s (0x%x)\n", (value&0x8)?"ENABLE":"DISABLE", value);
    printf("RTL8291 APD_LUT_SOURCE = %s (0x%x)\n", (value&0x1)?"REG":"LUT", value);
    _8291_cal_flash_data_get(devId, RTL8291_APD_LUT_EN_OFFSET_ADDR+RTL8291_PARAMETER_BASE_ADDR, 1, &value);    
    printf("RTL8291 APD_LUT flash config = 0x%x\n", value);

    start_addr = RTL8291_APDLUT_ADDR_BASE;
    
    printf("rtl8291_cli_apdlut_get: start address = %d \n", start_addr);     
    
     _8291_cli_flashBlock_get( devId, start_addr, 0x80);
        
    printf("RTL8291 APD lookup table get success!!!\n");  

    return 0;    
}

int rtl8291_cli_dac_set(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 dac_offset=0, dac_code=0;

    if(argc >= 3)
    { 
        dac_offset = _vlaue_translate(argv[1]);
        dac_code = _vlaue_translate(argv[2]); 

        ret = rtk_ldd_dac_set(devId, dac_offset, dac_code);
        if(ret)
        {
            printf("Set RTL8291 DAC Fail!!! (%d)\n", ret);
            return -1;
        }

        printf("Set DAC Offset = 0x%x, DAC Code =  0x%x\n", dac_offset, dac_code);
    
        return 0;
    }
    else
    {
        printf("%s <dac_offset: 0~0xF> <dac_code: 0~0x1FF> \n", argv[0]);      
        return -1;
    }
    
    return 0;    
}


int rtl8291_cli_dac_get(uint32 devId)
{
    int ret;
    uint32 dac_offset=0, dac_code=0;
    
    ret = rtk_ldd_dac_get(devId, &dac_offset, &dac_code);
    if(ret)
    {
        printf("Get RTL8291 DAC Fail!!! (%d)\n", ret);
        return -1;
    }

    printf("Get DAC Offset = 0x%x, DAC Code =  0x%x\n", dac_offset, dac_code);
    
    return 0;    
}

/*
 * europacli set booster <UINT:Vout> <UINT:Rfb1> <UINT:Rfb2>
 */
int rtl8291_cli_booster_set(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret=0;
    uint32 Vout;
    uint32 Rfb1;
    uint32 Rfb2;
    uint32 dac_code, dac_offset, offset_idx;

    if((argc < 4)&&(argc != 2))
    { 
        printf("%s <UINT:Vout> <UINT:Rfb1> <UINT:Rfb2> \n", argv[0]);        
        return -1;
    }

    printf("Set booster:\n"); 
    
    Vout = _vlaue_translate(argv[1]);     /* mV */   
    if (20000>Vout || 80000<Vout)
    {
        printf("Not support voltage %d(mV)\n", Vout);
        return -1;
    }
    
    //200k/28k 
    if (argc == 2)
    {
        Rfb1 = 200;
        Rfb2 = 28;        
    }
    else
    {
        Rfb1 = _vlaue_translate(argv[2]);    
        Rfb2 = _vlaue_translate(argv[3]);
    }
    //dac_code = _europa_cal_booster_Vout2Dac(Vout, Rfb1, Rfb2);
    offset_idx = 2;
    ret = rtl8291_cal_booster_Vout2Dac(Vout, Rfb1, Rfb2, &dac_code, &offset_idx);

    if (0x1ff<dac_code)
    {
        printf("Booster DAC = 0x%d out-of-range\n", dac_code);
        return 0;
    }

    //offset_idx = 2 mapping to 50uA, dac_offset = 0x4
    dac_offset = _8291_dac_idx2offset(offset_idx);
    ret = rtk_ldd_dac_set(devId, dac_offset, dac_code);
    if(ret)
    {
        printf("Set RTL8291 DAC Fail!!! (%d)\n", ret);
        return -1;
    }

    printf("RTL8291 Booster DAC set 0x%02x (Vout=%d mV, Rfb1 = %d, Rfb2 = %d)\n", dac_code, Vout, Rfb1, Rfb2);
    
    return 0;
} 


/*
 * debug europa get booster <UINT:Rfb1> <UINT:Rfb2>
 */
int rtl8291_cli_booster_get(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret=0, offset_idx;
    uint32 dac_code, dac_offset;
    uint32 Vout;
    uint32 Rfb1;
    uint32 Rfb2;
    
    int32 value1, value2;

    if((argc < 3)&&(argc != 1))
    { 
        printf("%s <UINT:Rfb1> <UINT:Rfb2> \n", argv[0]);        
        return -1;
    }

    if(argc ==1)
    {
        Rfb1 = 200;
        Rfb2 = 10;
    }
    else
    {
        value1 = _vlaue_translate(argv[1]);   
        value2 = _vlaue_translate(argv[2]);
        if ((value1<0)||(value2<=0))
        {
            printf("%s <UINT:Rfb1> <UINT:Rfb2> should > 0 \n", argv[0]);
            return -1;
        }
        Rfb1 = (uint32)value1;   
        Rfb2 = (uint32)value2; 
    }

    //europa_i2c_read(EUROPA_I2C_PORT, 0x54, 0x64, &dac_reg); 
    ret = rtk_ldd_dac_get(devId, &dac_offset, &dac_code);
    if(ret)
    {
        printf("Set RTL8291 DAC Fail!!! (%d)\n", ret);
        return -1;
    }    
    //Vout = _europa_cal_booster_Dac2Vout(dac_reg, Rfb1, Rfb2);

    //offset_idx should be mapping from dac_offset, but now fix to 2 (50uA)
    
    offset_idx = _8291_dac_offset2idx(dac_offset);
    ret = rtl8291_cal_booster_Dac2Vout(dac_code, offset_idx, Rfb1, Rfb2, &Vout);
    if(ret)
    {
        printf("rtl8291_cal_booster_Dac2Vout!!! (%d)\n", ret);
        return -1;
    }

    printf("RTL8291 Booster DAC is 0x%02x DAC offset is 0x%x (Vout=%d mV, Rfb1 = %d , Rfb2 = %d)\n", dac_code, dac_offset, Vout, Rfb1, Rfb2);

    return 0;
}    /* end of europa_cli_booster_get */


int rtl8291_cli_cal_toffset(
    uint32 devId, 
    int argc,
    char *argv[],
    FILE *fp)
{
    int32 ret, temp;
    int32 offset;   
    uint32 addr;
    int8 tmp_offset;

    if(argc >= 3)
    { 
        /* optional parameter means assign offset directly */
        printf("Ignore temperature input.\n");
        offset = _vlaue_translate(argv[2]); 
        printf("Temperature offset=%d.\n", offset);
        if (abs(offset)>=128)
        {
            printf("offset should be -127 to 127. \n");      
            return -1;
        }
        addr = RTL8291_TEMP_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, offset&0xFF);         
    }
    else if (argc == 2)
    {
        uint32 tempK,temp32;
        int32 tempC;

        temp = _vlaue_translate(argv[1]);         
        ret = rtk_ldd_temperature2_get(devId, &temp32);
        if(ret)
        {
            printf("Get Temperature Fail!!! (%d)\n", ret);
            return -1;
        }
        tempK = (temp32&0x7FC00)>>10;
        tempC = (int32)(tempK-273); /* degK to degC */        
        printf("Temperature=%dK (%dC)\n", tempK, tempC);
        
        offset = tempC - temp;
        
        addr = RTL8291_TEMP_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, offset&0xFF);          

    }
    else
    {
        addr = RTL8291_TEMP_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;      
        _8291_cal_flash_data_get(devId, addr, 1, &offset); 
        tmp_offset = offset&0xFF;

        printf("Temperature offset = %d now\n", (int)tmp_offset);      
        return 0;
    }

    return 0;    
}

int rtl8291_cli_cal_tscale(
    uint32 devId, 
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2, value3;      
    uint32 apd_step, bias_step, mod_step;
    const char* StepStr[4] = {
    "1.25K",
    "2.5K",
    "5K",
    "NULL"};

    if(argc >= 3)
    { 
        if(0 == strcmp(argv[1], "apdlut"))
        {
            if (0 == strcmp(argv[2], "5"))
            {
                apd_step = 2;
            }
            else if (0 == strcmp(argv[2], "2.5"))
            {
                apd_step = 1;
            }
            else if (0 == strcmp(argv[2], "1.25"))
            {
                apd_step = 0;
            }
            else
            {
                printf("Input Error!! Value should be 5, 2.5, 1.25.\n");
                return -1;             
            }
            addr = RTL8291_TEMP_INTR_SCALE_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
            _8291_cal_flash_data_get(devId, addr, 1, &value2);
            value2 = (value2&0xCF)|((apd_step&0x3)<<4);
            _8291_cal_flash_data_set(devId, addr, 1, value2);

            rtk_ldd_parameter2_get(devId, 0x397, &value);
            value = (value&0xCF)|((apd_step&0x3)<<4);
            rtk_ldd_parameter2_set(devId, 0x397, value);
            
            printf("Set APD LUT Step size = %s \n", StepStr[apd_step]);

        }
        else if(0 == strcmp(argv[1], "txlut"))
        {
            if (0 == strcmp(argv[2], "5"))
            {
                bias_step = 2;
                mod_step = 2;
            }
            else if (0 == strcmp(argv[2], "2.5"))
            {
                bias_step = 1;
                mod_step = 1;
            }
            else if (0 == strcmp(argv[2], "1.25"))
            {
                bias_step = 0;
                mod_step = 0;
            }
            else
            {
                printf("Input Error!! Value should be 5, 2.5, 1.25.\n");
                return -1;             
            }
            addr = RTL8291_TEMP_INTR_SCALE_ADDR + RTL8291_PARAMETER_BASE_ADDR; 
            _8291_cal_flash_data_get(devId, addr, 1, &value2);
            value2 = (value2&0xF0)|((bias_step&0x3)<<2)|(mod_step&0x3);
            _8291_cal_flash_data_set(devId, addr, 1, value2);

            rtk_ldd_parameter2_get(devId, 0x397, &value3);
            value3 = (value3&0xF0)|((bias_step&0x3)<<2)|(mod_step&0x3);
            rtk_ldd_parameter2_set(devId, 0x397, value3);
            
            printf("Set TX BIAS LUT Step size = %s \n", StepStr[bias_step]);
            printf("Set TX MOD LUT Step size = %s \n", StepStr[mod_step]);
        }
        else
        {
            printf("%s <apdlut/txlut> <5/2.5/1.25>\n", argv[0]);
            return -1; 
        } 
            
    }
    else
    { 
        printf("%s <apdlut/txlut> <5/2.5/1.25>\n", argv[0]);
        rtk_ldd_parameter2_get(devId, 0x397, &value);
        printf("Current Status (0x%x):\n", value);
        apd_step = (value&0x30)>>4;
        bias_step = (value&0xC)>>2;
        mod_step = value&0x3;
        printf("APD LUT Step size = %s \n", StepStr[apd_step]);
        printf("TX BIAS LUT Step size = %s \n", StepStr[bias_step]);
        printf("TX MOD LUT Step size = %s \n", StepStr[bias_step]);

        addr = RTL8291_TEMP_INTR_SCALE_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);             
        printf("Calibration Status (0x%x):\n", value);
        apd_step = (value&0x30)>>4;
        bias_step = (value&0xC)>>2;
        mod_step = value&0x3;
        printf("APD LUT Step size = %s \n", StepStr[apd_step]);
        printf("TX BIAS LUT Step size = %s \n", StepStr[bias_step]);
        printf("TX MOD LUT Step size = %s \n", StepStr[bias_step]);    

        return -1;
    }

    return 0;    
}

int rtl8291_cli_cal_rxpol(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;
    uint32 data;
    char   temp1;
    int   temp2, temp4;
    uint32 temp3;
 
    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value>0x1)
        {
            printf("Input should be 0 or 1\n");
#if 0            
            temp1 = value&0xFF;
            temp2 = (int)temp1;
            temp3 = value&0xFFF;
            temp4 = 0xFFFFF000|temp3;
            
            printf("temp1 = %d, temp2 = %d\n", temp1, temp2);    
            printf("temp3 = %d, temp4 = %d\n", temp3, temp4);
#endif            
            return -1;
        }
        addr = RTL8291_RXLOS_POLARITY_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_set(devId, addr, 1, value);
        printf("Set RTL8291 RX Polarity = 0x%x\n", value, (value>0)?"RXLOS":"RXSD");
        rtk_ldd_parameter2_get(devId, 0x398, &data);
        data = (data&0xFE)|(value&0x1);
        rtk_ldd_parameter2_set(devId, 0x398, data);
    }
    else
    { 
        addr = RTL8291_RXLOS_POLARITY_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8291 RX Polarity = 0x%x (%s)\n", value, (value>0)?"RXLOS":"RXSD");
    }

    return 0; 
}


int rtl8291_cli_cal_rxlos(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 i,data1, data2, data3, los_flag, flag2;
    uint32 w2e_data, w2d_data, w2d_data_ori;
    uint32 w2b_data_ori, w2b_data;
    uint32 addr, value, value2;
    uint32 w83_data;
    uint32 data;
    uint32 ctrl_data;

    if(argc < 2)
    { 
        addr = RTL8291_RXLOS_REF_DAC_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);  
        addr = RTL8291_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value2);          
        printf("los = 0x%x, range = 0x%x\n", value, value2);
        rtk_ldd_rxlosRefDac2_get(devId, &data);
        rtk_ldd_parameter2_get(devId, 0x2AD, &w2d_data);    
        printf("current los = 0x%x, range = 0x%x\n", data, ((w2d_data&0x8)>>3));    
    }
    else if (argc >= 2)
    {
        if ((ret=strcmp(argv[1], "auto")) == 0 )
        {
            printf("Please plug the proper LOS optical power to ONU...\n");
            //printf("Wait 3 sec...\n");
            //DDMI Control... ???
            //rtk_ldd_parameter2_get(devId, 0x387, &ctrl_data);
            //rtk_ldd_parameter2_set(devId, 0x387, 0x10);
            
            //osal_time_mdelay(3000);
            rtk_ldd_parameter2_get(devId, 0x2AE, &w2e_data); 
            //# W2B<4>    REG_RXLOS_ASSERT_SET_L    0    Assert set in assert manual mode. 0 is deassert and 1 is assert.
            //# W2B<3>    REG_RXLOS_ASSERT_TESTMD_L    Assert manual mode set. 0 is DIG_TOP control and 1 is REG set.
            //# W2D<3>    REG_RXLOS_RANGEX2_L 0        DAC reference X 2
            rtk_ldd_parameter2_get(devId, 0x2AB, &w2b_data_ori);    
            rtk_ldd_parameter2_get(devId, 0x2AD, &w2d_data_ori);
            
            data1 = 0x10;
            w2e_data = (w2e_data&0x01)|(data1<<1);
            rtk_ldd_parameter2_set(devId, 0x2AE, w2e_data);

            //set W2B_1 [ expr ($W2B_orig & 0xE7 | 0x18)  ]
            w2b_data = (w2b_data_ori&0xE7)|0x18;
            rtk_ldd_parameter2_set(devId, 0x2AB, w2b_data);         
            
            los_flag = 0;
            //printf("Start: data1 = 0x%x, los_flag = 0x%x\n", data1, los_flag);
            while ((data1<0x7F)&&(los_flag==0))
            {
                flag2 = 0; 
                rtk_ldd_parameter2_get(devId, 0x2AE, &w2e_data);             
                rtk_ldd_parameter2_get(devId, 0x2AD, &w2d_data);                
                for (i=0; i<100; i++)
                {
                    rtk_ldd_parameter2_get(devId, 0x385, &data2);
                    rtk_ldd_parameter2_get(devId, 0x398, &data3);
                    //$LOS_SD_PLOARITY_CTL ^ $RX_LOS_STATUS
                    flag2 = ((data2&0x4)>>2)^(data3&0x1);
                    if (flag2!=0)
                    {
                        break;
                    }                     
                }
                //printf("W38 = 0x%x, W39 = 0x%x, range = 0x%x, Status2 = 0x%x\n", w38_data, w39_data, ((w38_data&0x8)>>3), data2);                        
                if (flag2 != 0)
                {
                    data1++;
                    w2e_data = (w2e_data&0x01)|(data1<<1);
                    rtk_ldd_parameter2_set(devId, 0x2AE, w2e_data);
                }
                else
                {
                    los_flag = 1;
                    //printf("LOS Flag = 1, W38 = 0x%x, W39 = 0x%x\n", w38_data, w39_data);
                }  

                if (data1>=0x7F)
                {
                    w2d_data = 0;
                    rtk_ldd_parameter2_get(devId, 0x2AD, &w2d_data);
                    //RANGE_SELL
                    if ((w2d_data&0x08)==0)
                    {
                        w2d_data = w2d_data|0x8;
                        rtk_ldd_parameter2_set(devId, 0x2AD, w2d_data);
    
                        data1=0;
                        w2e_data = (w2e_data&0x01)|(data1<<1);
                        rtk_ldd_parameter2_set(devId, 0x2AE, w2e_data);                        
                    }
                    else
                    {                
                        data1++;
                        rtk_ldd_parameter2_set(devId, 0x2AB, w2b_data_ori);
                        rtk_ldd_parameter2_set(devId, 0x2AD, w2d_data_ori);
                        //rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);                        
                        printf("RX LOS Fail, data = 0x%x, W2D = 0x%x\n", data1, w2d_data);
                        return 0;
                    }
                }   
            }

            addr = RTL8291_RXLOS_REF_DAC_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;     
            _8291_cal_flash_data_set(devId, addr, 1, data1);

            addr = RTL8291_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;       
            _8291_cal_flash_data_set(devId, addr, 1, ((w2d_data&0x8)>>3));
            
            printf("RX LOS Finish. range = 0x%x, data1 = 0x%x\n",((w2d_data&0x8)>>3), data1);

            rtk_ldd_parameter2_set(devId, 0x2AB, w2b_data_ori);
            rtk_ldd_parameter2_set(devId, 0x2AD, w2d_data_ori);
            //rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);

        }
        else if ((ret=strcmp(argv[1], "range")) == 0 )
        {
            if(argc == 2)
            { 
                addr = RTL8291_RXLOS_REF_DAC_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;     
                _8291_cal_flash_data_get(devId, addr, 1, &value);  
                addr = RTL8291_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
                _8291_cal_flash_data_get(devId, addr, 1, &value2);           
                printf("los = 0x%x, range = 0x%x\n", value, value2);
                rtk_ldd_rxlosRefDac2_get(devId, &data);
                rtk_ldd_parameter2_get(devId, 0x2AD, &w2d_data);   
                printf("current los = 0x%x, range = 0x%x\n", data, ((w2d_data&0x8)>>3));    
                return 0; 
            }

            value = _vlaue_translate(argv[2]); 
            if(value>1)
            {
                printf("%s range <value: 1 or 0> \n", argv[0]);       
                return -1;
            }
            addr = RTL8291_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;     
            _8291_cal_flash_data_set(devId, addr, 1, value);
            w2d_data = 0;
            rtk_ldd_parameter2_get(devId, 0x2AD, &w2d_data);             
            w2d_data = (w2d_data&0xF7)|((value&0x1)<<3);
            rtk_ldd_parameter2_set(devId, 0x2AD, w2d_data);    
        }        
        else
        {  
            value = _vlaue_translate(argv[1]);   
            if(value>0x7F)
            {
                printf("%s <value: less than 0x7F> \n", argv[0]);       
                return -1;
            }            
            addr = RTL8291_RXLOS_REF_DAC_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            printf("Set los = 0x%x\n", value);     
            rtk_ldd_rxlosRefDac2_set(devId, (value&0x7F));        
        }
    }

    return 0;    
}

#if 1
/* Convert DAC value to Vout voltage
 *
 *  Rfb1: resistor (K ohms)
 *  Rfb2: resistor (K ohms)
 */
int rtl8291_cal_booster_Dac2Vout(uint32 dac_code, uint offset_idx, uint32 Rfb1, uint32 Rfb2, uint32 *Vout)
{
    float Vtemp;
    uint32 Vref = 1300; /* 1.3V */

    if ((dac_code>511)||(offset_idx>6))
        return -1;

    //Vout = Vfb * (1 + Rfb1 / Rfb2) + Rfb1 * Idac = Vfb * (1 + Rfb1 / Rfb2) + Rfb1 * (11.71875uA + DAC_OFFSET + LSB * DAC_CODE)
    Vtemp = Vref + (float)Vref*Rfb1/Rfb2 + (float)Rfb1*(11.71875 + 25*offset_idx + (float)dac_code*150/511);

    //printf("Dac2Vout: dac_code = %d  offset_idx = %d, Vout = %f(%d) mV\n", dac_code, offset_idx, Vtemp, Vout);

    if (Vtemp>0)
    {
        *Vout = (uint)Vtemp;
        printf("Dac2Vout: dac_code = %d  offset_idx = %d, Vout = %f(%d) mV\n", dac_code, offset_idx, Vtemp, *Vout);
        
    }
    else
       {
        printf("Vout = %f mV!!! Set to 0.\n", Vtemp);
        *Vout = 0;
    }

   return 0;
}


/* Convert Vout voltage to DAC value
 *
 *  Vout: desired output voltage (mV)
 *  Rfb1: resistor (K ohms)
 *  Rfb2: resistor (K ohms)
 */
int rtl8291_cal_booster_Vout2Dac(uint32 Vout, uint32 Rfb1, uint32 Rfb2, uint32 *dac, uint32 *dac_idx)
{
    uint32 dac_code=0, ibx50u1=0, ibx50u2=0, ibx25u1=0, ibx25u2=0;
    uint32 Vref = 1300; /* 1.3V */
    float Vdefault, Vtemp, lsb;
    float Voffset,Vthreshold1;
    //float Vthreshold2, Vthreshold3, Vthreshold4, Vthreshold5;
    uint32 offset_code[7] = {0, 0x1, 0x4, 0x5, 0xc, 0xd, 0xf};
    uint32 offset_idx=0;

    //Vout = Vfb * (1 + Rfb1 / Rfb2) + Rfb1 * Idac = Vfb * (1 + Rfb1 / Rfb2) + Rfb1 * (11.71875 + DAC_OFFSET + LSB * DAC_CODE)
    //Vfb: 1.3V
    //Rfb1: 200k ohms (on board)
    //Rfb2: 28k ohms (on chip)
    //DAC_OFFSET fixed current =11.71875uA
    //W01<7>: REG_IBX50U2, W01<6>: REG_IBX50U1, W01<5>: REG_IBX25U2, W01<4>: REG_IBX25U1
    //Total 4bits, the combination are 0uA, 25uA, 50uA, 75uA, 100uA, 125uA, 150uA
    //LSB: 9 bits DAC, assume full scale is 150uA, so the accuracy 150/(2^9-1) = 0.293524uA
    //EX: open 50uA DAC offset. The Vout is calculated as following:
    //Vout = 1.3*(1+200k/28k) + 200k*[ (11.71875u+50u) + (150u/(2^9-1)*DAC_CODE] = 10.58571 + 11.71875 + 0.058708 * DAC_CODE = 22.03446+ 0.058708 *DAC_CODE
    //DAC_CODE =  (Vout - Vfb * (1 + Rfb1 / Rfb2))/Rfb1*LSB - 11.71875/LSB
    //         =  (Vout -Vfb )/(Rfb1*LSB) - Vfb / (Rfb2*LSB) - 11.71875/LSB - DAC_OFFSET/LSB
    lsb = 150/511;
    Vdefault = (float)Vref*(1+Rfb1/Rfb2) + (float)Rfb1*11.71875;
    Voffset = (float)Rfb1*25;
    //idx=0, DAC_OFFSET = 0, DAC_CODE = 511 (full code for 150uA)
    Vthreshold1 = Vdefault + (float)Rfb1*150;
    //idx=1, DAC_OFFSET = 25uA, DAC_CODE = 511 (full code for 150uA)
    //Vthreshold2 = Vdefault + (float)Rfb1*150 + Voffset;
    //idx=2, DAC_OFFSET = 50uA, DAC_CODE = 511 (full code for 150uA)
    //Vthreshold3 = Vdefault + (float)Rfb1*150 + Voffset*2;
    //idx=3, DAC_OFFSET = 25uA+50uA, DAC_CODE = 511 (full code for 150uA)
    //Vthreshold4 = Vdefault + (float)Rfb1*150 + Voffset;
    //idx=4, DAC_OFFSET = 50uA+50uA, DAC_CODE = 511 (full code for 150uA)
    //Vthreshold5 = Vdefault + (float)Rfb1*150 + Voffset*2;
    
    //printf("Input Vout = %d, Rfb1 = %d, Rfb2 = %d, dac_idx = %d\n", Vout, Rfb1, Rfb2, *dac_idx);


    /* avoid overflow if Vout is lower than Vdefault */
    if ((*dac_idx!=0)&&(*dac_idx>6))
    {
        printf("DAC OFFSET Overflow!!!! \n");
        return -1;
    }    

    if (*dac_idx==0)
    {
        if (Vout<Vdefault)
        {
            dac_code = 0;
            //ibx50u1 = ibx50u2 = ibx25u1 = ibx25u2 = 0;
            offset_idx=0;
            printf("Vout under Vdefault = %f!!!! \n", Vdefault);
            return -1;
        }
        else if (Vout <= Vthreshold1)
        {
            Vtemp = (float)Vout;
            offset_idx=0;
        }
        else if (Vout < (Vthreshold1+Voffset))
        {
            offset_idx=1;
            Vtemp = (float)Vout - Voffset*offset_idx;
        }
        else if (Vout < (Vthreshold1+Voffset*2))
        {
            offset_idx=2;
            Vtemp = (float)Vout - Voffset*offset_idx;
        }
        else if (Vout < (Vthreshold1+Voffset*3))
        {
            offset_idx=3;
            Vtemp = (float)Vout - Voffset*offset_idx;
        }
        else if (Vout < (Vthreshold1+Voffset*4))
        {
            offset_idx=4;
            Vtemp = (float)Vout - Voffset*offset_idx;
        }
        else if (Vout < (Vthreshold1+Voffset*5))
        {
            offset_idx=5;
            Vtemp = (float)Vout - Voffset*offset_idx;
        }
        else if (Vout < (Vthreshold1+Voffset*6))
        {
            offset_idx=6;
        }
        else
        {
            printf("Vout overflow!!!! \n");
            return -1;
        }
    }
    else
    {
        offset_idx=*dac_idx;
        Vdefault = Vdefault + Voffset*offset_idx;
        if (Vout<Vdefault)
        {
            dac_code = 0;
            //ibx50u1 = ibx50u2 = ibx25u1 = ibx25u2 = 0;
            offset_idx=0;
            printf("Vout under Vdefault = %f!!!! \n", Vdefault);
            return -1;
        }    
    }
    
    Vtemp = (float)Vout - Voffset*offset_idx;

    dac_code = (Vtemp-(float)Vref)*511/(Rfb1*150) - (float)Vref*511/(Rfb2*150) - 11.71875*511/150;

    *dac = dac_code;
    *dac_idx = offset_idx;

    //printf("Vout2Dac: Vout = %d mV,  offset_idx = %d, dac_code = %d\n", Vout, offset_idx, dac_code);

#if 0
   {
    //dac_code = (Vout*128)/(Rfb1*100) - (Vref*128)/(Rfb1*100) - (Vref*128)/(Rfb2*100) - 12*128/100;
        dac_code = (Vout-Vref)*128/(Rfb1*100) - (Vref*511)/(Rfb2*150) - 11.71875/LSB*511/150;
   }
#endif

   return 0;
}

int rtl8291_cli_cal_txes(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2; 
 
    if(argc >= 3)
    {
        if(0 == strcmp(argv[1], "vgmode"))
        {
            value = _vlaue_translate(argv[2]);     
            if (value>0x1)
            {
                printf("Input should be 0 or 1\n");         
                return -1;
            }
            addr = RTL8291_TX_ES_EMP_VG_MODE_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8291 TX_ES_EMP_VG_MODE = 0x%x\n", value);    
            rtk_ldd_txEsConfig_set(devId, LDD_TXES_VG_MODE, value);
            return 0;
        }
        else if(0 == strcmp(argv[1], "vgstr"))
        {
            value = _vlaue_translate(argv[2]);     
            if (value>0x7)
            {
                printf("Input should be 0~0x7\n");         
                return -1;
            }
            addr = RTL8291_TX_ES_EMP_VG_STR_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8291 TX_ES_EMP_VG_STR = 0x%x\n", value);    
            rtk_ldd_txEsConfig_set(devId, LDD_TXES_VG_STR, value);
            return 0;
        }    
        else if(0 == strcmp(argv[1], "level"))
        {
            value = _vlaue_translate(argv[2]);     
            if (value>0x7)
            {
                printf("Input should be 0~0x7\n");         
                return -1;
            }
            addr = RTL8291_TX_ES_EMP_LEVEL_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8291 TX_ES_EMP_LEVEL = 0x%x\n", value);    
            rtk_ldd_txEsConfig_set(devId, LDD_TXES_EMP_LEVEL, value);
            return 0;
        }
        else if(0 == strcmp(argv[1], "vgamp"))
        {
            value = _vlaue_translate(argv[2]);     
            if (value>0xF)
            {
                printf("Input should be 0~0xF\n");         
                return -1;
            }
            addr = RTL8291_TX_ES_EMP_VG_AMP_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8291 TX_ES_EMP_VG_AMP = 0x%x\n", value);    
            rtk_ldd_txEsConfig_set(devId, LDD_TXES_VG_AMP, value);
            return 0;
        }
        else if(0 == strcmp(argv[1], "rise"))
        {
            value = _vlaue_translate(argv[2]);     
            if (value>0xF)
            {
                printf("Input should be 0~0x7\n");         
                return -1;
            }
            addr = RTL8291_TX_ES_DELAY_RISE_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8291 TX_ES_DELAY_RISE = 0x%x\n", value);    
            rtk_ldd_txEsConfig_set(devId, LDD_TXES_DELY_RISING, value);
            return 0;
        }
        else if(0 == strcmp(argv[1], "fall"))
        {
            value = _vlaue_translate(argv[2]);     
            if (value>0xF)
            {
                printf("Input should be 0~0xF\n");         
                return -1;
            }
            addr = RTL8291_TX_ES_DELAY_FALL_OFFSET_ADDR+ RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8291 TX_ES_DELAY_FALL = 0x%x\n", value);    
            rtk_ldd_txEsConfig_set(devId, LDD_TXES_DELY_FALLING, value);
            return 0;
        }
        else if(0 == strcmp(argv[1], "enable"))
        {
            value = _vlaue_translate(argv[2]);     
            if (value>0xF)
            {
                printf("Input should be 0~0xF\n");         
                return -1;
            }
            addr = RTL8291_TX_ES_ADJ_ENABLE_OFFSET_ADDR+ RTL8291_PARAMETER_BASE_ADDR;    
            _8291_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8291 RTL8291_TX_ES_ADJ_ENABLE = 0x%x\n", value);    
            rtk_ldd_txEsConfig_set(devId, LDD_TXES_ADJ_ENABLE, value);
            return 0;
        }
        else if(0 == strcmp(argv[1], "test"))
        {
            value = _vlaue_translate(argv[2]);     
            //printf("RTL8291 API Test... %d \n", value);    
            rtk_ldd_txEsConfig_set(devId, LDD_TXES_TEST_MODE, value);
            return 0;
        }        
        else
        {
            printf("%s <enable/vgstr/vgmode/vgstr/level/vgamp/rise/fall> <value>\n", argv[0]);
            return 0;
        } 
    }
    else
    { 
        printf("%s <enable/vgstr/vgmode/vgstr/level/vgamp/rise/fall> <dac_code: 0~0x1FF> \n", argv[0]);      

    
        addr = RTL8291_TX_ES_ADJ_ENABLE_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;      
        _8291_cal_flash_data_get(devId, addr, 1, &value);
        rtk_ldd_txEsConfig_get(devId, LDD_TXES_ADJ_ENABLE, &value2);
        printf("RTL8291 TX_ES_ADJ_ENABLE = 0x%x, current = 0x%x\n", value, value2);
    
        addr = RTL8291_TX_ES_EMP_VG_MODE_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);
        rtk_ldd_txEsConfig_get(devId, LDD_TXES_VG_MODE, &value2);
        printf("RTL8291 TX_ES_EMP_VG_MODE = 0x%x, current = 0x%x\n", value, value2);

        addr = RTL8291_TX_ES_EMP_VG_STR_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);
        rtk_ldd_txEsConfig_get(devId, LDD_TXES_VG_STR, &value2);
        printf("RTL8291 TX_ES_EMP_VG_STR = 0x%x, current = 0x%x\n", value, value2);

        addr = RTL8291_TX_ES_EMP_LEVEL_OFFSET_ADDR+ RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);
        rtk_ldd_txEsConfig_get(devId, LDD_TXES_EMP_LEVEL, &value2);
        printf("RTL8291 TX_ES_EMP_LEVEL = 0x%x, current = 0x%x\n", value, value2);

        addr = RTL8291_TX_ES_EMP_VG_AMP_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);
        rtk_ldd_txEsConfig_get(devId, LDD_TXES_VG_AMP, &value2);
        printf("RTL8291 TX_ES_EMP_VG_AMP = 0x%x, current = 0x%x\n", value, value2);    

        addr = RTL8291_TX_ES_DELAY_RISE_OFFSET_ADDR+ RTL8291_PARAMETER_BASE_ADDR;    
        _8291_cal_flash_data_get(devId, addr, 1, &value);
        rtk_ldd_txEsConfig_get(devId, LDD_TXES_DELY_RISING, &value2);
        printf("RTL8291 TX_ES_DELAY_RISE = 0x%x, current = 0x%x\n", value, value2);
        
        addr = RTL8291_TX_ES_DELAY_FALL_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;      
        _8291_cal_flash_data_get(devId, addr, 1, &value);
        rtk_ldd_txEsConfig_get(devId, LDD_TXES_DELY_FALLING, &value2);
        printf("RTL8291 TX_ES_DELAY_FALL = 0x%x, current = 0x%x\n", value, value2);      
    }

    return 0; 
}



int rtl8291_cli_cal_vbr(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 rfb1, rfb2, vstart, vend;
    uint32 i_rssi;
    uint32 loopcnt, i;
    uint64 sum_i, sum_v, sum_w;

    uint32 init_dac, max_dac;
    uint32 orig_dac, dac;

    uint32 start_v, end_v, Vbr;
    uint32 tmph, tmpl;
    //W01<7>: REG_IBX50U2, W01<6>: REG_IBX50U1, W01<5>: REG_IBX25U2, W01<4>: REG_IBX25U1
    //Total 4bits, the combination are 
    //0uA(idx0), 
    //25uA(idx1, REG_IBX25U1), 
    //50uA(idx2, REG_IBX50U1), 
    //75uA(idx3, REG_IBX25U1+REG_IBX50U1), 
    //100uA(idx4, REG_IBX50U1+REG_IBX50U2), 
    //125uA(idx5, REG_IBX50U1+REG_IBX50U2+REG_IBX25U1), 
    //150uA(idx5, REG_IBX50U1+REG_IBX50U2++REG_IBX25U1+REG_IBX25U2)
    uint32 offset_code[7] = {0, 0x1, 0x4, 0x5, 0xc, 0xd, 0xf};
    uint32 init_offset=0, max_offset=0;
    rtk_ldd_rxpwr_t lddRxPwr;
    float i_rssi_nA, dark_curr, v_factor, v_float;
    uint32 orig_offset, offset_idx;
    uint32 temp_data, Vref, Vmax, Vmin;
    uint8 buf[2];
    double tmp = 0;
    uint32 v_offset=0, v_work=0, value=0, addr=0;
    int32  value2;

    if(argc >= 7)
    { 
        /* === get parameter */    
        rfb1   = _vlaue_translate(argv[1]);
        rfb2   = _vlaue_translate(argv[2]);
        vstart = _vlaue_translate(argv[3]);
        vend   = _vlaue_translate(argv[4]);

        if (argv[5]!=NULL)
        {
            if (0 == strcmp(argv[5], "offset"))
            {
                if (argv[6]==NULL)
                    return -1;
                v_offset= _vlaue_translate(argv[6]);
            }
            else if (0 == strcmp(argv[5], "factor"))
            {
                if (argv[6]==NULL)
                    return -1;
                v_factor= atof(argv[6]);
            }
            else
            {
                printf("%s <UINT:Rfb1> <UINT:Rfb2> <UINT:Vstart> <UINT:Vend> <offset/factor> <value> \n", argv[0]);  
                printf("Rfb1 & Rfb2 unit is K Ohm Vstart & Vend unit is V \n");  
                return -1;
            }
        }
        else
        {
            printf("%s <UINT:Rfb1> <UINT:Rfb2> <UINT:Vstart> <UINT:Vend> <offset/factor> <value> \n", argv[0]);  
            printf("Rfb1 & Rfb2 unit is K Ohm Vstart & Vend unit is V \n");  
            return -1;
        }
        
        start_v = vstart * 1000;  /* V to mV */
        end_v = vend * 1000;

        // get REG_DAC_HB/REG_DAC_LB, restore the value after Vbr test
        rtk_ldd_dac_get(devId, &orig_offset, &orig_dac); 
#if 0        
        rtk_ldd_parameter2_get(devId, RTL8291_REG_DAC_HB_ADDR, &tmph);
        rtk_ldd_parameter2_get(devId, RTL8291_REG_DAC_LB_ADDR, &tmpl);
        orig_dac = ((tmph&0xFF)<<1)|(tmpl&0x1);
        
        rtk_ldd_parameter2_get(devId, RTL8291_WB_REG01_ADDR, &tmph);
        orig_offset = (tmph&0xF0)>>4;
#endif

        /* === detect Vbr */
        //Default DAC Offset is 50uA by SD suggestion.
        value2= _8291_dac_offset2idx(orig_offset);
        if ((value2==-1)||(value2==0xFF))
        {
            return -1;
        }
        init_offset = value2;        
        value2 = _8291_dac_offset2idx(orig_offset);
        if ((value2==-1)||(value2==0xFF))
        {
            return -1;
        }
        max_offset = value2;
        //init_offset = 2;
        //max_offset = 2;
        ret = rtl8291_cal_booster_Vout2Dac(start_v, rfb1, rfb2, &init_dac, &init_offset);
        ret = rtl8291_cal_booster_Vout2Dac(end_v, rfb1, rfb2, &max_dac, &max_offset);

        if (max_offset<init_offset)
        {
            printf("max_offset = %d, init_offset = %d, init_offset>max_offset!!!\n", max_offset, init_offset);
            return -1;
        }
        if ((max_offset!=0)||(init_offset!=0))
        {
            printf("Warnning: max_offset = %d, init_offset = %d. DAC Offset is used!!!\n", max_offset, init_offset);
        }

        Vref = 1300;
        dark_curr = 11.71875;
        offset_idx = init_offset;        
        Vmax = Vref + Vref*rfb1/rfb2 + rfb1*(dark_curr + 25*offset_idx + 150);
        Vmin = Vref + Vref*rfb1/rfb2 + rfb1*(dark_curr + 25*offset_idx );

        printf("Input Rfb1 = %d K, Rfb2 = %d K, Vref = %d, dark current = %f\n", rfb1, rfb2, Vref, dark_curr);
        printf(" DAC Offset = %duA, Vmax = %d mV, Vmin = %d mV\n", 25*offset_idx, Vmax, Vmin);
    
        loopcnt = 1;
        dac = init_dac;
        offset_idx = init_offset;
        printf("Detecting Vbr from %dmV to %dmV...\n", start_v, end_v);
        
        i_rssi = 0;        
        while((i_rssi<RTL8291_RSSI_CURR_BREAKDOWN_TH)&&((dac<=max_dac)&&(offset_idx<=max_offset)))
        {
            tmph = (dac&0x1FE)>>1;
            tmpl = (dac&0x1);
            //rtk_ldd_parameter2_set(devId, RTL8291_REG_DAC_HB_ADDR, tmph);
            //rtk_ldd_parameter2_set(devId, RTL8291_REG_DAC_LB_ADDR, tmpl);
            rtk_ldd_parameter2_set(devId, 0x3A3, tmph);
            rtk_ldd_parameter2_set(devId, 0x3A4, tmpl);

            //rtk_ldd_parameter2_get(devId, RTL8291_WB_REG01_ADDR, &tmph);
            rtk_ldd_parameter2_get(devId, 0x281, &tmph); 
            tmph = ((offset_code[offset_idx]&0xF)<<4)|(tmph&0xF);
            //rtk_ldd_parameter2_set(devId, RTL8291_WB_REG01_ADDR, tmph);
            rtk_ldd_parameter2_set(devId, 0x281, tmph);

            sum_i = 0;
            i_rssi = 0;
            for (i=0;i<loopcnt;i++)
            {
                memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
                //The unit of RTL8291_RX_REAL_MODE output is uA/32, for HW implement
                lddRxPwr.op_mode = RTL8291_RX_HARDWARE_MODE;
                //lddRxPwr.rx_lsb = 4000000;
                //lddRxPwr.rx_code_200u = 1273;
                //lddRxPwr.rx_r6400 = 6400;
                ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
                if(ret)
                {
                    printf("Get RTL8291 RX power fail!!! (%d)\n", ret);          
                    return -1;
                }
                //i_rssi= lddRxPwr.i_rssi; 
                //sum_i += lddRxPwr.i_rssi;
                i_rssi= lddRxPwr.data_c; 
                sum_i += lddRxPwr.data_c;
            }       
            //diag_util_printf("Break Down Test: APD value = 0x%x, cnt1 = %d, cnt2    = %d\n", dac, cnt1, cnt2);
            i_rssi = sum_i/loopcnt;
            i_rssi_nA = (float)i_rssi*31.25;
            printf("     DAC=0x%02x, I_RSSI=%f nA (%d uA/32) DAC offset_idx=%d, max_dac = %d, max_offset = %d\n", dac, i_rssi_nA, i_rssi, offset_idx, max_dac, max_offset);
            if (i_rssi > RTL8291_RSSI_CURR_BREAKDOWN_TH)
            {
                break;
            }
            else
            {
                dac++;
            }

            if ((dac==0x511)&&(offset_idx<max_offset)&&(max_offset!=0))
            {
                //125uA (125*511/150=425)
                dac = 425; 
                offset_idx++;
            }            
        }
        
        printf("restore orig DAC=0x%x\n", orig_dac);
        rtk_ldd_dac_set(devId, orig_offset, orig_dac);         
#if 0        
        tmph = (orig_dac&0x1FE)>>1;
        tmpl = (orig_dac&0x1);
        rtk_ldd_parameter2_set(devId, RTL8291_REG_DAC_HB_ADDR, tmph);
        rtk_ldd_parameter2_set(devId, RTL8291_REG_DAC_LB_ADDR, tmpl);

        rtk_ldd_parameter2_get(devId, RTL8291_WB_REG01_ADDR, &tmph);        
        tmph = ((orig_offset&0xF)<<4)|(tmph&0xF);
        rtk_ldd_parameter2_set(devId, RTL8291_WB_REG01_ADDR, tmph);
#endif
        printf("\nTest Result: DAC=0x%x, I_RSSI=%f nA (%d uA/32)\n\n", dac, i_rssi_nA, i_rssi);
        
        /* === calculate Vout value */
        if (RTL8291_RSSI_CURR_BREAKDOWN_TH > i_rssi)
        {
             printf("!!! FAILE TO DETECT BREAKDOWN VOLTAGE !!!\n");
        
             Vbr = 0;
        }
        else
        {
            ret = rtl8291_cal_booster_Dac2Vout(dac, offset_idx, rfb1, rfb2, &Vbr);
            if(ret)
            {
                printf("rtl8291_cal_booster_Dac2Vout!!! (%d)\n", ret);
                return -1;
            }
        }

        /* === get temperature */
        ret = rtk_ldd_temperature2_get(devId, &temp_data);
        if(ret)
        {
            printf("Get Temperature Fail!!! (%d)\n", ret);
            return -1;
        }

        buf[0] = (temp_data&0xFF00)>>8;
        buf[1] = (temp_data&0xFF);
        if (128 >= buf[0]) //MSB: buf[0]; LSB: buf[1]
        {
            tmp = (-1)*((~(buf[0]))+1)+((double)(buf[1])*1/256);
        }
        else
        {
            tmp = (char)buf[0]+((double)(buf[1])*1/256);
        }
           
        printf("\nVbr=%d (mV) @ T=%f C (DAC=0x%02x, DAC offset_idx=%d, max_dac = %d, max_offset = %d)\n", Vbr, tmp, dac, offset_idx, max_dac, max_offset);

        if (0 == strcmp(argv[5], "offset"))
        {
            v_work = Vbr-v_offset;
            ret = rtl8291_cal_booster_Vout2Dac(v_work, rfb1, rfb2, &dac, &offset_idx);
            printf("\nV_work =%d (mV) DAC=0x%02x, DAC offset_idx=%d\n", v_work, dac, offset_idx);            
        }
        else if (0 == strcmp(argv[5], "factor"))
        {
            v_float = (float)Vbr*v_factor;
            v_work = (uint32)v_float;    
            ret = rtl8291_cal_booster_Vout2Dac(v_work, rfb1, rfb2, &dac, &offset_idx);
            printf("\nV_work =%d (mV) DAC=0x%02x, DAC offset_idx=%d\n", v_work, dac, offset_idx);
        }
        
        rtk_ldd_dac_set(devId, offset_code[offset_idx], dac);

        addr = RTL8291_APD_DAC_CODE_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        value = (dac&0xFF00)>>8;    
        _8291_cal_flash_data_set(devId, addr, 1, value);
        value = (dac&0xFF);    
        _8291_cal_flash_data_set(devId, (addr+1), 1, value);
        
    }
    else
    {
        printf("%s <UINT:Rfb1> <UINT:Rfb2> <UINT:Vstart> <UINT:Vend> <offset/factor> <value> \n", argv[0]);  
        printf("Rfb1 & Rfb2 unit is K Ohm Vstart & Vend unit is V \n");  
        return -1;
    }

    return 0;    

}
#endif


int rtl8291_cli_efuse_dump(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 entry_start=0, entry_end=0, value=0, pre_value=0;
    uint32 loopcnt1=0, loopcnt2=0, idx=0, error_idx=0xFFF;
    uint8 tmp1[512], tmp2[512];
    uint32 err_cnt=0, log_flag=0, total_err_cnt=0;
    uint32 cnt1=0, cnt2=0;
    uint32 bitshift=8, bit_idx, gpioId, check_idx, delay_time, delay_time2;
    uint32 writeBit, mode, enable, data, data2, data3;

    if((argv[1]!=NULL)&&(0 == strcmp(argv[1], "all")))
    {
        entry_start = 0; 
        entry_end = 511;     
        for (idx=entry_start;idx<=entry_end;idx++)
        {
            ret = rtk_ldd_efuse2_get(devId, idx, &value);
            if(ret)
            {
                printf("rtk_ldd_efuse2_get failed (%d)\n", ret);
                return -1;
            }
                               
            if(idx%16 == 0)
            {
                printf("0x%04x ", idx);
            }
            printf("0x%02x ", value);
            if(idx%8 == 7)
                printf("   ");
            if(idx%16 == 15)
                printf("\n");            
        }

        printf("EFUSE Dump Finished!\n");                  
    }
    else if((argv[1]!=NULL)&&(0 == strcmp(argv[1], "test")))
    {
        
        entry_start = 0; 
        entry_end = 511;
        bitshift = 8;
        //GPIO3<1> = 32*3+1 = 97
        //Change to RXLOS pin GPIO 13
        gpioId = 13;
        delay_time = 1000;
        if (argv[2]!=NULL)
        {
            if (0 == strcmp(argv[2], "enable"))
            {
                log_flag=1;     
                printf("Enable log \n");
            }
            else
            {
                log_flag=0;
                printf("Disable log \n");                
            }
        }

        if ((argv[3]!=NULL)&&(argv[4]!=NULL))
        {
            entry_start = _vlaue_translate(argv[3]); 
            entry_end = _vlaue_translate(argv[4]);
            if (entry_start>511)
            {
               printf("entry_start: 0~511 \n");
               return -1;
            }
            if ((entry_end>511)||(entry_end<entry_start))
            {
               printf("entry_end: 0~511 and entry_end should larger than entry_start. \n", argv[0]);
               return -1;
            }            
        }
        if (argv[5]!=NULL)
        {
            gpioId = _vlaue_translate(argv[5]);             
        }

        if (argv[6]!=NULL)
        {
            delay_time = _vlaue_translate(argv[6]);
            if (delay_time>5000)
            {
                printf("Input delay_time should be less than 5000ms.\n");
                return -1;    
            }            
        }
        else
        {
            delay_time = 200;
        }

        if (argv[7]!=NULL)
        {
            delay_time2 = _vlaue_translate(argv[7]);
            if (delay_time2>5000)
            {
                printf("Input delay_time2 should be less than 5000ms.\n");
                return -1;    
            }            
        }
        else
        {
            delay_time2 = 200;
        }
        
        printf("Input: start entry = %d, end entry = %d, log = %s (0x%x)\n", entry_start, entry_end, (log_flag==0)?"DISABLE":"ENABLE", log_flag);

        printf("Input: GPIO= %d, delay_time = %d, delay_time2 = %d\n", gpioId, delay_time, delay_time2);

        if (ret = rtk_gpio_mode_set(gpioId, GPIO_OUTPUT) != 0)
        {
            return ret;
        }
        //Relay board OFF
        if (ret = rtk_gpio_databit_set(gpioId, 0) != 0)
        {
            return ret;
        } 
        if (ret = rtk_gpio_state_set(gpioId ,ENABLED) != 0)
        {
            return ret;
        }
        osal_time_mdelay(delay_time);

        //Relay board ON
        if (ret = rtk_gpio_databit_set(gpioId, 1) != 0)
        {
            return ret;
        } 
        osal_time_mdelay(delay_time2);
        //Disable PWM
        rtk_ldd_parameter2_set(devId, 0x3F3, 0x4);

        //Check GPIO Status
        if (ret = rtk_gpio_mode_get(gpioId,&mode) != 0)
        {
            return ret;
        }
        if (ret = rtk_gpio_state_get(gpioId,&enable) != 0)
        {
            return ret;
        }
        if (ret = rtk_gpio_databit_get(gpioId,&data) != 0)
        {
            return ret;
        }
        rtk_ldd_parameter2_get(devId, 0x3F3, &data2);       
        printf("Befor test: GPIO[%d] mode = %s, state = %s, data = %d, 0x3F3 = 0x%x\n", gpioId, mode?"GPIO_OUTPUT":"GPIO_INPUT", enable?"ENABLED":"DISABLED", data, data2);

        printf("Start EFUSE Test...\n");         
        for (idx=entry_start;idx<=entry_end;idx++)
        {
            for (bit_idx=0;bit_idx<bitshift;bit_idx++)
            {

                ret = rtk_ldd_efuse2_get(devId, idx, &value);
                if(ret)
                {
                    printf("rtk_ldd_efuse2_get failed (%d)\n", ret);
                    return -1;
                }
                writeBit = value&(~(1<<bit_idx));
                if (log_flag==1)
                {
                    printf("Idx-BitIdx[%04d-%d], value = 0x%x, writeBit = 0x%x \n", idx, bit_idx, value, writeBit);
                }    
                ret = rtk_ldd_efuse2_set(devId, idx, writeBit);
                if(ret)
                {
                    printf("rtk_ldd_efuse2_set failed (%d)\n", ret);
                    return -1;
                }

                //Relay board OFF
                if (ret = rtk_gpio_databit_set(gpioId, 0) != 0)
                {
                    return ret;
                }
                
                if (ret = rtk_gpio_databit_get(gpioId,&data) != 0)
                {
                    return ret;
                }         
                osal_time_mdelay(delay_time);
                //Relay board ON
                if (ret = rtk_gpio_databit_set(gpioId, 1) != 0)
                {
                    return ret;
                }
                if (ret = rtk_gpio_databit_get(gpioId,&data2) != 0)
                {
                    return ret;
                }
                osal_time_mdelay(delay_time2);                                
                //Disable PWM
                rtk_ldd_parameter2_set(devId, 0x3F3, 0x4);
                
                rtk_ldd_parameter2_get(devId, 0x3F3, &data3);                  
                printf("Set GPIO[%d] from %d delay %dms to %d, 0x3F3 = 0x%x \n", gpioId, data, delay_time, data2, data3);

                for (check_idx=entry_start;check_idx<=entry_end;check_idx++)
                {
                    ret = rtk_ldd_efuse2_get(devId, check_idx, &value);
                    if(ret)
                    {
                        printf("rtk_ldd_efuse2_get failed (%d)\n", ret);
                        return -1;
                    }
                    if (check_idx<idx)
                    {
                        if (value!=0)
                        {
                            printf("Bit Check Fail!!! Index = %d, Value = 0x%x, Wish =0x%x\n", check_idx, value, 0);
                            return -1;
                        }
                        else
                        {
                            if (log_flag==1)
                            {
                                printf("Bit Check PASS!!! Index = %d, Value = 0x%x\n", check_idx, value);
                            }
                        }
                    }
                    else if (check_idx>idx)
                    {
                        if (value!=0xFF)
                        {
                            printf("Bit Check Fail!!! Index = %d, Value = 0x%x, Wish =0x%x\n", check_idx, value, 0xFF);
                            return -1;
                        }
                        else
                        {
                            if (log_flag==1)
                            {
                                printf("Bit Check PASS!!! Index = %d, Value = 0x%x\n", check_idx, value);
                            }
                        }
                    }
                    else
                    {
                        if (value!=writeBit)
                        {
                            printf("Bit Check Fail!!! Index = %d, Value = 0x%x, Wish =0x%x\n", check_idx, value, writeBit);
                            return -1;
                        }
                        else
                        {
                            if (log_flag==1)
                            {
                                printf("Bit Check PASS!!! Index = %d, Value = 0x%x\n", check_idx, value);
                            }
                        }
                    }                        
                }
                
            }           
        }
        
        printf("EFUSE Test Finished!\n"); 
        return 0;
    }
    else if(argc >= 5)
    {
        
        entry_start = _vlaue_translate(argv[1]); 
        if (entry_start>511)
        {
           printf("entry_start: 0~511 \n", argv[0]);
           return -1;
        }
        entry_end = _vlaue_translate(argv[2]); 
        if ((entry_end>511)||(entry_end<entry_start))
        {
           printf("entry_end: 0~511 and entry_end should larger than entry_start. \n", argv[0]);
           return -1;
        }
        loopcnt1 = _vlaue_translate(argv[3]);
        if (loopcnt1>0xffffffff)
        {
            printf("loopcnt1 overflow!!! \n");
            return -1;
        }    
        loopcnt2 = _vlaue_translate(argv[4]);
        if (loopcnt2>0xffffffff)
        {
            printf("loopcnt2 overflow!!! \n");
            return -1;
        }

        if (argv[5]!=NULL)
        {
            if (0 == strcmp(argv[5], "enable"))
            {
                log_flag=1;
                
                printf("Enable log \n");
            }
            else
            {
                log_flag=0;
                printf("Disable log \n");                
            }
        }
        printf("Input: loop1 = %d, loop2 = %d log = %s (0x%x)\n",loopcnt1, loopcnt2, (log_flag==0)?"DISABLE":"ENABLE", log_flag);

        memset(tmp1, 0, 512);
        memset(tmp2, 0, 512);

        printf("Start 1st, Use the result to be backup...\n");         
        for (idx=entry_start;idx<=entry_end;idx++)
        {
            ret = rtk_ldd_efuse2_get(devId, idx, &value);
            if(ret)
            {
                printf("rtk_ldd_efuse2_get failed (%d)\n", ret);
                return -1;
            }
            tmp1[idx] = (uint8)value;

            if (log_flag==1)
            {
                if((idx-entry_start)%16 == 0)
                {
                    printf("0x%04x ", idx);
                }
        
                printf("0x%02x ", tmp1[idx]);
                if((idx-entry_start)%8 == 7)
                    printf("   ");
                if((idx-entry_start)%16 == 15)
                    printf("\n"); 
            }
        }        
    
        printf("Start loop...\n");
        total_err_cnt =0;
        for(cnt1=0;cnt1<=loopcnt1;cnt1++)
        {
            err_cnt=0;        
            for (idx=entry_start;idx<=entry_end;idx++)
            {
                ret = rtk_ldd_efuse2_get(devId, idx, &value);
                if(ret)
                {
                    printf("rtk_ldd_efuse2_get failed (%d)\n", ret);
                    return -1;
                }
                tmp2[idx] = (uint8)value;
    
                if (tmp2[idx]!=tmp1[idx])
                {
                    printf("loop = %d entry = %d orig = 0x%x now = 0x%x\n", cnt1, idx, tmp1[idx], tmp2[idx]); 
                    error_idx = idx;
                    //To be check base, use original value
                    pre_value = tmp1[idx];
                    //err_cnt++;
                    break;
                }              
            }
            if (error_idx<512)
            {
                break;
            }
            
            if (log_flag==1)
            {
                printf("loop = %d err_cnt = %d\n", cnt1, err_cnt); 
            }
#if 0            
            if (err_cnt!=0)
            {
                total_err_cnt++;
                if (log_flag==1)
                {                
                    for (idx=entry_start;idx<=entry_end;idx++)
                    {
                           
                        if((idx-entry_start)%16 == 0)
                        {
                            printf("0x%04x ", idx);
                        }
                
                        printf("0x%02x ", tmp2[idx]);
                        if((idx-entry_start)%8 == 7)
                            printf("   ");
                        if((idx-entry_start)%16 == 15)
                            printf("\n");        
                    }
                }
                printf("Change compare base to this...\n");
                memcpy(tmp1, tmp2, 512);
                memset(tmp2, 0, 512);        
            }
#endif
        }

        if (error_idx<512)
        {
            printf("loop2 error_idx = %d \n", error_idx); 
            err_cnt=0; 
            for(cnt2=0;cnt2<=loopcnt2;cnt2++)
            {
                ret = rtk_ldd_efuse2_get(devId, error_idx, &value);
                if(ret)
                {
                    printf("rtk_ldd_efuse2_get failed (%d)\n", ret);
                    return -1;
                }
                if (value!=pre_value)
                {
                    err_cnt++;
                    printf("loop2 = %d err_cnt = %d ori = 0x%x, now = 0x%x\n", cnt2, err_cnt, pre_value, value);
                }            
                if (log_flag==1)
                {                                
                    if(cnt2%16 == 0)
                    {
                        printf("0x%04x ", cnt2);
                    }
                    printf("0x%02x ", value);
                    if(cnt2%8 == 7)
                        printf("   ");
                    if(cnt2%16 == 15)
                        printf("\n");        
                }   
            }
        
            printf("loop2 err_cnt = %d \n", err_cnt); 
        }
#if 0
        if (total_err_cnt!=0)
        {
            total_err_cnt=0;
            for(cnt2=0;cnt2<=loopcnt2;cnt2++)
            {
                err_cnt=0;        
                for (idx=entry_start;idx<=entry_end;idx++)
                {
                    ret = rtk_ldd_efuse2_get(devId, idx, &value);
                    if(ret)
                    {
                        printf("rtk_ldd_efuse2_get failed (%d)\n", ret);
                        return -1;
                    }
                    tmp2[idx] = (uint8)value;
                
                    if (tmp2[idx]!=tmp1[idx])
                    {
                        err_cnt++;
                    }            
                }    
                printf("loop2 = %d err_cnt = %d \n", cnt2, err_cnt);         
                if (err_cnt!=0)
                {
                    total_err_cnt++;
                    if (log_flag==1)
                    {                
                        for (idx=entry_start;idx<=entry_end;idx++)
                        {
                                    
                            if((idx-entry_start)%16 == 0)
                            {
                                printf("0x%04x ", idx);
                            }
                            
                            printf("0x%02x ", tmp2[idx]);
                            if((idx-entry_start)%8 == 7)
                                printf("   ");
                            if((idx-entry_start)%16 == 15)
                                printf("\n");        
                        }
                    }
                    printf("Change compare base to this...\n");
                    memcpy(tmp1, tmp2, 512);
                    memset(tmp2, 0, 512);        
                }    
            }
            printf("loop2 total_err_cnt = %d \n", total_err_cnt); 
        }
#endif        
    }
    else
    {
        printf("%s <entry_start> <entry_end> <loopcnt1> <loopcnt2> <enable/disable>\n", argv[0]);
        return -1;
    }

    return 0;    
}

int rtl8291_cli_sram_dump(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 value=0, value2=0;
    uint32 loopcnt1=0, loopcnt2=0, idx=0, error_idx=0xFFF;
    uint32 tmp1[2];
    uint32 err_cnt=0, log_flag=0, total_err_cnt=0;
    uint32 cnt1=0, cnt2=0;

    if(argc >= 3)
    {        
        loopcnt1 = _vlaue_translate(argv[1]);
        if (loopcnt1>0xffffffff)
        {
            printf("loopcnt1 overflow!!! \n");
            return -1;
        }    
        loopcnt2 = _vlaue_translate(argv[2]);
        if (loopcnt2>0xffffffff)
        {
            printf("loopcnt2 overflow!!! \n");
            return -1;
        }    
        if (argv[3]!=NULL)
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                log_flag=1;
                
                printf("Enable log \n");
            }
            else
            {
                log_flag=0;
                printf("Disable log \n");                
            }
        }
        printf("Input: loop1 = %d, loop2 = %d log = %s (0x%x)\n",loopcnt1, loopcnt2, (log_flag==0)?"DISABLE":"ENABLE", log_flag);

        memset(tmp1, 0, sizeof(uint32)*2);   
#if 0
        ret = rtk_ldd_parameter2_get(devId, 0, &value);
        if(ret)
        {
            printf("rtk_ldd_efuse2_get failed (%d)\n", ret);
            return -1;
        }
        tmp1[0] = value;

        ret = rtk_ldd_parameter2_get(devId, 0x6b5, &value2);
        if(ret)
        {
            printf("rtk_ldd_efuse2_get failed (%d)\n", ret);
            return -1;
        }
        tmp1[1] = value2;
        printf("Init: Reg 0 = 0x%x, Reg 0x6B5 = 0x%x\n", tmp1[0], tmp1[1]);         
#else
        tmp1[0] = tmp1[1] = 0xFF;
        printf("Init: Reg 0 = 0x%x, Reg 0x6B5 = 0x%x\n", tmp1[0], tmp1[1]);
#endif


        printf("Start loop1...\n");
        total_err_cnt =0;
        for(cnt1=0;cnt1<=loopcnt1;cnt1++)
        {      
            ret = rtk_ldd_parameter2_get(devId, 0, &value);
            if(ret)
            {
                printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
                return -1;
            }
            if (tmp1[0]!=value)
            {
                error_idx = 0;
                printf("loop = %d Reg 0 = 0x%x now = 0x%x\n", cnt1, tmp1[0], value); 
                break;
            }              

            ret = rtk_ldd_parameter2_get(devId, 0x6b5, &value2);
            if(ret)
            {
                printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
                return -1;
            }
            if (tmp1[1]!=value2)
            {
                error_idx = 1;           
                printf("loop = %d Reg 0x6B5 = 0x%x now = 0x%x\n", cnt1, tmp1[1], value2); 
                break;
            }     
            
            
            if (log_flag==1)
            {
                printf("loop = %d Reg 0 = 0x%x, Reg 0x6B5 = 0x%x\n", cnt1, value, value2); 
            }
        }

        if (error_idx!=0xFFF)
        {
            printf("loop1 = %d Test FAIL!!! \n", loopcnt1); 
        
            if (error_idx==0)
            {
                printf("loop2 error_idx = %d, Test Reg 0 again \n", error_idx);
                err_cnt = 0;
                for(cnt2=0;cnt2<=loopcnt2;cnt2++)
                {
                    ret = rtk_ldd_parameter2_get(devId, 0, &value);
                    if(ret)
                    {
                        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
                        return -1;
                    }
                    if (tmp1[0]!=value)
                    {
                        err_cnt++;
                        if (log_flag==1)
                        {
                            printf("loop = %d Reg 0 = 0x%x now = 0x%x\n", cnt1, tmp1[0], value); 
                        }
                    }    
                }    
            }
            
            if (error_idx==1)
            {
                printf("loop2 error_idx = %d, Test Reg 0x6B5 again \n", error_idx);
                err_cnt = 0;
                for(cnt2=0;cnt2<=loopcnt2;cnt2++)
                {
                    ret = rtk_ldd_parameter2_get(devId, 0x6B5, &value2);
                    if(ret)
                    {
                        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
                        return -1;
                    }
                    if (tmp1[1]!=value2)
                    {
                        err_cnt++;
                        if (log_flag==1)
                        {
                            printf("loop = %d Reg 0x6B5 = 0x%x now = 0x%x\n", cnt1, tmp1[1], value2); 
                        }
                    }    
                }            
            }
    
            printf("loop2 err_cnt = %d \n", err_cnt); 
        } 
        else
        {
            printf("loop1 = %d Test PASS \n", loopcnt1); 
        }
    }
    else
    {
        printf("%s <loopcnt1> <loopcnt2> <enable/disable>\n", argv[0]);
        return -1;
    }

    return 0;    
}


#if 0
int rtl8291_cli_cal_vbr(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 rfb1, rfb2, vstart, vend;
    uint32 i_rssi;
    uint32 loopcnt, i;
    uint64 sum_i, sum_v, sum_w;

    uint32 init_dac, max_dac;
    uint32 orig_dac, dac;

    uint32 start_v, end_v, Vbr;
    uint32 tmph, tmpl;
    //W01<7>: REG_IBX50U2, W01<6>: REG_IBX50U1, W01<5>: REG_IBX25U2, W01<4>: REG_IBX25U1
    //Total 4bits, the combination are 
    //0uA(idx0), 
    //25uA(idx1, REG_IBX25U1), 
    //50uA(idx2, REG_IBX50U1), 
    //75uA(idx3, REG_IBX25U1+REG_IBX50U1), 
    //100uA(idx4, REG_IBX50U1+REG_IBX50U2), 
    //125uA(idx5, REG_IBX50U1+REG_IBX50U2+REG_IBX25U1), 
    //150uA(idx5, REG_IBX50U1+REG_IBX50U2++REG_IBX25U1+REG_IBX25U2)
    uint32 offset_code[7] = {0, 0x1, 0x4, 0x5, 0xc, 0xd, 0xf};
    uint32 init_offset=0, max_offset=0;
    rtk_ldd_rxpwr_t lddRxPwr;
    float i_rssi_nA, dark_curr, v_factor, v_float;
    uint32 orig_offset, offset_idx;
    uint32 temp_data, Vref, Vmax, Vmin;
    uint8 buf[2];
    double tmp = 0;
    uint32 v_offset=0, v_work=0, value=0, addr=0;

    if(argc >= 7)
    { 
        /* === get parameter */    
        rfb1   = _vlaue_translate(argv[1]);
        rfb2   = _vlaue_translate(argv[2]);
        vstart = _vlaue_translate(argv[3]);
        vend   = _vlaue_translate(argv[4]);

        if (argv[5]!=NULL)
        {
            if (0 == strcmp(argv[5], "offset"))
            {
                if (argv[6]==NULL)
                    return -1;
                v_offset= _vlaue_translate(argv[6]);
            }
            else if (0 == strcmp(argv[5], "factor"))
            {
                if (argv[6]==NULL)
                    return -1;
                v_factor= atof(argv[6]);
            }
            else
            {
                printf("%s <UINT:Rfb1> <UINT:Rfb2> <UINT:Vstart> <UINT:Vend> <offset/factor> <value> \n", argv[0]);  
                printf("Rfb1 & Rfb2 unit is K Ohm Vstart & Vend unit is V \n");  
                return -1;
            }
        }
        else
        {
            printf("%s <UINT:Rfb1> <UINT:Rfb2> <UINT:Vstart> <UINT:Vend> <offset/factor> <value> \n", argv[0]);  
            printf("Rfb1 & Rfb2 unit is K Ohm Vstart & Vend unit is V \n");  
            return -1;
        }
        
        start_v = vstart * 1000;  /* V to mV */
        end_v = vend * 1000;

        // get REG_DAC_HB/REG_DAC_LB, restore the value after Vbr test
        rtk_ldd_parameter2_get(devId, RTL8291_REG_DAC_HB_ADDR, &tmph);
        rtk_ldd_parameter2_get(devId, RTL8291_REG_DAC_LB_ADDR, &tmpl);
        orig_dac = ((tmph&0xFF)<<1)|(tmpl&0x1);
        
        rtk_ldd_parameter2_get(devId, RTL8291_WB_REG01_ADDR, &tmph);
        orig_offset = (tmph&0xF0)>>4;
        
        /* === detect Vbr */
        //Default DAC Offset is 50uA by SD suggestion.
        init_offset = 2;
        max_offset = 2;
        ret = rtl8291_cal_booster_Vout2Dac(start_v, rfb1, rfb2, &init_dac, &init_offset);
        ret = rtl8291_cal_booster_Vout2Dac(end_v, rfb1, rfb2, &max_dac, &max_offset);

        if (max_offset<init_offset)
        {
            printf("max_offset = %d, init_offset = %d, init_offset>max_offset!!!\n", max_offset, init_offset);
            return -1;
        }
        if ((max_offset!=0)||(init_offset!=0))
        {
            printf("Warnning: max_offset = %d, init_offset = %d. DAC Offset is used!!!\n", max_offset, init_offset);
        }

        Vref = 1300;
        dark_curr = 11.71875;
        offset_idx = init_offset;        
        Vmax = Vref + Vref*rfb1/rfb2 + rfb1*(dark_curr + 25*offset_idx + 150);
        Vmin = Vref + Vref*rfb1/rfb2 + rfb1*(dark_curr + 25*offset_idx );

        printf("Input Rfb1 = %d K, Rfb2 = %d K, Vref = %d, dark current = %f\n", rfb1, rfb2, Vref, dark_curr);
        printf(" DAC Offset = %duA, Vmax = %d mV, Vmin = %d mV\n", 25*offset_idx, Vmax, Vmin);
    
        loopcnt = 1;
        dac = init_dac;
        offset_idx = init_offset;
        printf("Detecting Vbr from %dmV to %dmV...\n", start_v, end_v);
        
        
        while((i_rssi<RTL8291_RSSI_CURR_BREAKDOWN_TH)&&((dac<=max_dac)&&(offset_idx<=max_offset)))
        {
            tmph = (dac&0x1FE)>>1;
            tmpl = (dac&0x1);
            rtk_ldd_parameter2_set(devId, RTL8291_REG_DAC_HB_ADDR, tmph);
            rtk_ldd_parameter2_set(devId, RTL8291_REG_DAC_LB_ADDR, tmpl);

            rtk_ldd_parameter2_get(devId, RTL8291_WB_REG01_ADDR, &tmph);        
            tmph = ((offset_code[offset_idx]&0xF)<<4)|(tmph&0xF);
            rtk_ldd_parameter2_set(devId, RTL8291_WB_REG01_ADDR, tmph);

            sum_i = 0;
            i_rssi = 0;
            for (i=0;i<loopcnt;i++)
            {
                memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
                //The unit of RTL8291_RX_REAL_MODE output is uA/32, for HW implement
                lddRxPwr.op_mode = RTL8291_RX_HARDWARE_MODE;
                //lddRxPwr.rx_lsb = 4000000;
                //lddRxPwr.rx_code_200u = 1273;
                //lddRxPwr.rx_r6400 = 6400;
                ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
                if(ret)
                {
                    printf("Get RTL8291 RX power fail!!! (%d)\n", ret);          
                    return -1;
                }
                i_rssi= lddRxPwr.i_rssi; 
                sum_i += lddRxPwr.i_rssi;
 
            }       
            //diag_util_printf("Break Down Test: APD value = 0x%x, cnt1 = %d, cnt2    = %d\n", dac, cnt1, cnt2);
            i_rssi = sum_i/loopcnt;
            i_rssi_nA = (float)i_rssi*31.25;
            printf("     DAC=0x%02x, I_RSSI=%f nA (%d uA/32) DAC offset_idx=%d, max_dac = %d, max_offset = %d\n", dac, i_rssi_nA, i_rssi, offset_idx, max_dac, max_offset);
            if (i_rssi > RTL8291_RSSI_CURR_BREAKDOWN_TH)
            {
                break;
            }
            else
            {
                dac++;
            }

            if ((dac==0x511)&&(offset_idx<max_offset)&&(max_offset!=0))
            {
                //125uA (125*511/150=425)
                dac = 425; 
                offset_idx++;
            }            
        }
        
        printf("restore orig DAC=0x%x\n", orig_dac);
        tmph = (orig_dac&0x1FE)>>1;
        tmpl = (orig_dac&0x1);
        rtk_ldd_parameter2_set(devId, RTL8291_REG_DAC_HB_ADDR, tmph);
        rtk_ldd_parameter2_set(devId, RTL8291_REG_DAC_LB_ADDR, tmpl);

        rtk_ldd_parameter2_get(devId, RTL8291_WB_REG01_ADDR, &tmph);        
        tmph = ((orig_offset&0xF)<<4)|(tmph&0xF);
        rtk_ldd_parameter2_set(devId, RTL8291_WB_REG01_ADDR, tmph);

        printf("\nTest Result: DAC=0x%x, I_RSSI=%f nA (%d uA/32)\n\n", dac, i_rssi_nA, i_rssi);
        
        /* === calculate Vout value */
        if (RTL8291_RSSI_CURR_BREAKDOWN_TH > i_rssi)
        {
             printf("!!! FAILE TO DETECT BREAKDOWN VOLTAGE !!!\n");
        
             Vbr = 0;
        }
        else
        {
            ret = rtl8291_cal_booster_Dac2Vout(dac, offset_idx, rfb1, rfb2, &Vbr);
            if(ret)
            {
                printf("rtl8291_cal_booster_Dac2Vout!!! (%d)\n", ret);
                return -1;
            }
        }

        /* === get temperature */
        ret = rtk_ldd_temperature2_get(devId, &temp_data);
        if(ret)
        {
            printf("Get Temperature Fail!!! (%d)\n", ret);
            return -1;
        }

        buf[0] = (temp_data&0xFF00)>>8;
        buf[1] = (temp_data&0xFF);
        if (128 >= buf[0]) //MSB: buf[0]; LSB: buf[1]
        {
            tmp = (-1)*((~(buf[0]))+1)+((double)(buf[1])*1/256);
        }
        else
        {
            tmp = (char)buf[0]+((double)(buf[1])*1/256);
        }
           
        printf("\nVbr=%d (mV) @ T=%f C (DAC=0x%02x, DAC offset_idx=%d, max_dac = %d, max_offset = %d)\n", Vbr, tmp, dac, offset_idx, max_dac, max_offset);

        if (0 == strcmp(argv[5], "offset"))
        {
            v_work = Vbr-v_offset;
            ret = rtl8291_cal_booster_Vout2Dac(v_work, rfb1, rfb2, &dac, &offset_idx);
            printf("\nV_work =%d (mV) DAC=0x%02x, DAC offset_idx=%d\n", v_work, dac, offset_idx);            
        }
        else if (0 == strcmp(argv[5], "factor"))
        {
            v_float = (float)Vbr*v_factor;
            v_work = (uint32)v_float;    
            ret = rtl8291_cal_booster_Vout2Dac(v_work, rfb1, rfb2, &dac, &offset_idx);
            printf("\nV_work =%d (mV) DAC=0x%02x, DAC offset_idx=%d\n", v_work, dac, offset_idx);
        }
        rtk_ldd_dac_set(devId, offset_idx, dac);

        addr = RTL8291_APD_DAC_CODE_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;
        value = (dac&0xFF00)>>8;    
        _8291_cal_flash_data_set(devId, addr, 1, value);
        value = (dac&0xFF);    
        _8291_cal_flash_data_set(devId, (addr+1), 1, value);

        
    }
    else
    {
        printf("%s <UINT:Rfb1> <UINT:Rfb2> <UINT:Vstart> <UINT:Vend> <offset/factor> <value> \n", argv[0]);  
        printf("Rfb1 & Rfb2 unit is K Ohm Vstart & Vend unit is V \n");  
        return -1;
    }

    return 0;    

}



int rtl8291_cli_cal_prbs(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 chipId, rev, subType;
    uint32 addr, value, value2, value3, ib, im;      
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif   

    if((argc < 2)||(argc > 3))
    { 
        //system("diag gpon get ");
        printf("%s <7/15/23/31/off>\n", argv[0]);
        return -1; 
    }

    if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return ret;
    }

    if(argc >= 2)
    { 
        if((ret = _rtl8291_gpon_init()) != 0)
        {
            return ret;
        }
    
        if(0 == strcmp(argv[1], "off"))
        { 
#if 0    
            system("diag gpon set force-prbs off");   
#else
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                //EPON PRBS OFF
                system("diag register set 0x40a30 0"); 
                //GPON PRBS OFF
                system("diag register set 0x40098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x40540 0");                     
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                //EPON PRBS OFF           
                system("diag register set 0x22A30 0");
                //GPON PRBS OFF
                system("diag register set 0x22098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");    
                /*force BEN=0*/                
                system("diag register set 0x22540 0"); 
            }
#if defined(CONFIG_LUNA_G3_SERIES)                
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("diag rt_ponmisc set force-prbs off");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }
#endif
        
            _rtl8291_ibim_recover(devId);

        }
        else
        {

            _rtl8291_ibim_low(devId);
    
            if(0 == strcmp(argv[1], "7"))
            {
#if 0            
                system("diag gpon set force-prbs prbs7");
#else
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag register set 0x40094 0x407c"); 
                    system("diag register set 0x40098 1"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag register set 0x22094 0x407c");
                    system("diag register set 0x22098 0x1");    
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs07");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }
#endif 
           }
            else if(0 == strcmp(argv[1], "15"))
            {        
#if 0            
                system("diag gpon set force-prbs prbs15");

#else
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag register set 0x40094 0x408d"); 
                    system("diag register set 0x40098 1"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag register set 0x22094 0x408d");
                    system("diag register set 0x22098 0x1"); 
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs15");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }
#endif

            }
            else if(0 == strcmp(argv[1], "23"))
            {         
#if 0            
                system("diag gpon set force-prbs prbs23");   
#else
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag register set 0x40094 0x409e"); 
                    system("diag register set 0x40098 1"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag register set 0x22094 0x409e");
                    system("diag register set 0x22098 0x1"); 
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs23");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }
#endif
            }
            else if(0 == strcmp(argv[1], "31"))
            {        
#if 0            
                system("diag gpon set force-prbs prbs31");   
#else
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag register set 0x40094 0x40af"); 
                    system("diag register set 0x40098 1"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag register set 0x22094 0x40af");
                    system("diag register set 0x22098 0x1");
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs31");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }
#endif            
            }
            else
            {
                printf("%s <7/15/23/31/off>\n", argv[0]);
                return -1; 
            }
        }

    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_prbs Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

int rtl8291_cli_cal_epon_prbs(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int32   ret;
    uint32 chipId, rev, subType;
    uint32 addr, value, ib, im;      
#ifdef RTL8290B_CAL_TIME  
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    if((argc < 2)||(argc > 3))
    { 
        //system("diag gpon get ");
        printf("%s <7/7r/9/11/15/off/benon/benoff>\n", argv[0]);
        return -1; 
    }

    if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return ret;
    }

    //system("diag debug pon mode set epon"); 
    if((ret = _rtl8291_epon_init()) != 0)
    {
        return ret;
    }

    //Set EPON LPF to 40MHz
    value = 0;   
    addr = RTL8291_EPON_APC_LPF_BW_OFFSET_ADDR + RTL8291_PARAMETER_BASE_ADDR;
    _8291_cal_flash_data_get(devId, addr, 1, &value);
    if (value != 0)
    {
        value = 0;      
        _8291_cal_flash_data_set(devId, addr, 1, value);
    }
    rtk_ldd_parameter2_get(devId, 0x2c3, &value);    
    value = (value&0x1F);//Set EPON LPF to 40MHz
    rtk_ldd_parameter2_set(devId, 0x2c3, value);             
    printf("Set WB_REG43 = 0x%x (0x%x)\n", value);       

    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "off"))
        {
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                //EPON PRBS OFF
                system("diag register set 0x40a30 0"); 
                //GPON PRBS OFF
                system("diag register set 0x40098 0");
                //BEN OFF
                system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x40540 0");                     
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                //EPON PRBS OFF           
                system("diag register set 0x022A30 0");
                //GPON PRBS OFF
                system("diag register set 0x22098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");                
                /*force BEN=0*/                
                system("diag register set 0x22540 0");                 
            }
#if defined(CONFIG_LUNA_G3_SERIES)                
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("diag rt_ponmisc set force-prbs off");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }

            _rtl8291_ibim_recover(devId);

        }
        else if(0 == strcmp(argv[1], "benon"))
        {  
            _rtl8291_ibim_low(devId);

            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                system("diag register set 0x40540 0x30");
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                system("diag register set 0x22540 0x30");
            }
#if defined(CONFIG_LUNA_G3_SERIES)    
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("rt_ponmisc set force-laser-on state enable");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }    
        }
        else if(0 == strcmp(argv[1], "benoff"))
        {  
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                //EPON PRBS OFF
                system("diag register set 0x40a30 0"); 
                //GPON PRBS OFF
                system("diag register set 0x40098 0");
                //BEN OFF
                system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x40540 0"); 
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {
                //EPON PRBS OFF           
                system("diag register set 0x022A30 0");
                //GPON PRBS OFF
                system("diag register set 0x22098 0"); 
                //BEN OFF
                system("diag register set 0x360a0 0");
                /*force BEN=0*/                
                system("diag register set 0x22540 0");
            }
#if defined(CONFIG_LUNA_G3_SERIES)    
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                system("rt_ponmisc set force-laser-on state disable");
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }    
            
            _rtl8291_ibim_recover(devId); 
            
        }    
        else
        {
            _rtl8291_ibim_low(devId);
    
            if(0 == strcmp(argv[1], "7"))
            {
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag reg set 0x40a2C 0x0000");
                    system("diag reg set 0x40a30 0x803");                    
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag reg set 0x022A2C 0x0000");                 
                    system("diag reg set 0x022A30 0x803");                     
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs07");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }            
            }
            else if(0 == strcmp(argv[1], "7r"))
            {
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag reg set 0x40a2C 0x2000");                  
                    system("diag reg set 0x40a30 0x803");
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag reg set 0x022A2C 0x2000");                 
                    system("diag reg set 0x022A30 0x803");    
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs07");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }            
            }
            else if(0 == strcmp(argv[1], "9"))
            {         
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag reg set 0x40a2C 0x0100");                  
                    system("diag reg set 0x40a30 0x803");
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag reg set 0x022A2C 0x0100");
                    system("diag reg set 0x022A30 0x803");    
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs09");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }            
            }
            else if(0 == strcmp(argv[1], "11"))
            {        
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag reg set 0x40a2C 0x0200");                  
                    system("diag reg set 0x40a30 0x803"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag reg set 0x022A2C 0x0200");                  
                    system("diag reg set 0x022A30 0x803");
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs11");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }                
            }
            else if(0 == strcmp(argv[1], "15"))
            {        
                if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
                {
                    system("diag reg set 0x40a2C 0x0300");  
                    system("diag reg set 0x40a30 0x803"); 
                }
                else if (RTL9602C_CHIP_ID == chipId)
                {
                    system("diag reg set 0x022A2C 0x0300");  
                    system("diag reg set 0x022A30 0x803");
                }
#if defined(CONFIG_LUNA_G3_SERIES)    
                //For Cortina ARM-based 9607DQ series
                else if (RTL8277C_CHIP_ID == chipId)
                {
                    system("diag rt_ponmisc set force-prbs prbs15");
                }
#endif
                else
                {
                    printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
                }
            }
            else
            {
                printf("%s <7/9/11/15/off/benon/benoff>\n", argv[0]);
                return -1; 
            }

            /*force BEN=1*/
            if ((RTL9607C_CHIP_ID == chipId)||(RTL9603CVD_CHIP_ID == chipId))
            {
                system("diag reg set 0x40540 0x30");                    
            }
            else if (RTL9602C_CHIP_ID == chipId)
            {   
                system("diag reg set 0x022540 0x30");                     
            }
#if defined(CONFIG_LUNA_G3_SERIES)    
            //For Cortina ARM-based 9607DQ series
            else if (RTL8277C_CHIP_ID == chipId)
            {
                //do nothing
            }
#endif
            else
            {
                printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
            }            
        }

    }
#ifdef RTL8290B_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  europa_cli_cal_epon_prbs Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}
#endif

