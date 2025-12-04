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
#include <common/util/rt_util.h>

//#include "epon_oam_config.h"
//#include "epon_oam_err.h"
//#include "epon_oam_msgq.h"
#include "europa_cli.h"
#include "europa_8290c.h"

#include <osal/time.h>
#include <math.h>

#if CONFIG_GPON_VERSION < 2
#include <rtk/gpon.h>
#else
#include <module/gpon/gpon.h>
#endif

//For 8290C
#define RTL8290C_LASER_LUT_SZ 324
#define RTL8290C_TXLUT_SZ 128
#define RTL8290C_TXLUT_OFFSET 64
#define RTL8290C_IBCONV(b) ((b)*2047/100)
#define RTL8290C_IMCONV(m) ((m)*2047/100)

#define RTL8290C_TEMP_ZERO        0x4449A
//19bits, 9 bits integer, 10 bits fraction.
#define RTL8290C_TEMP_MASK        0x7FFFF 

//#define RTL8290C_CAL_TIME
#undef RTL8290C_CAL_TIME

static int32 rtl8290b_i2c_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData)
{
    int32   ret;

#if defined(CONFIG_COMMON_RT_API)
        ret = rt_i2c_read(i2cPort, devID, regAddr, pData);
#else
        ret = rtk_i2c_read(i2cPort, devID, regAddr, pData);
#endif

    return ret;
}

#if 0
int _europa_ldd_parser(void)
{
    uint8 tmp;
    uint32 value1, value2, tmp2;
    int ret, mode;    
    uint32 devId = 5;

    mode = 0;
    value1 = 0;
    value2 = 0; 
    
    rtl8290b_i2c_read(EUROPA_I2C_PORT, 0x55, 0x90, &value1);
    rtl8290b_i2c_read(EUROPA_I2C_PORT, 0x55, 0x91, &value2); 
    value1 = (value1<<8)|(value2&0xFF);     
    if (value1 != 0x8290)
    {     
        if (rtk_ldd_parameter2_get(devId, 0x680, &tmp2)==0)
        {
            value2 = (uint32)tmp2;
        }
        if (rtk_ldd_parameter2_get(devId, 0x681, &tmp2)==0)
        {
            value2 = (value2<<8)|(uint32)tmp2;    
        }    
    }

    if (value1 == 0x8290)
    {
        mode = EUROPA_LDD_8290B_MODE;        
    }
    else if (value2 == 0x8290)
    {
        mode = EUROPA_LDD_8290C_MODE;        
    }
    else
    {
        mode = EUROPA_LDD_UNKNOWN_MODE;   
    }
    //printf("\rvalue1 = 0x%x, value2 = 0x%x, mode = %d\n", value1, value2, mode);    
    
    return mode;
}
#endif

double __dBm2uw(double dBm_pwr)
{
    double uW_pwr;

    uW_pwr = 1000*pow(10,dBm_pwr/10);
    
    return uW_pwr;
}

float __dBm2001uw(double dBm_pwr)
{
    double uW_pwr;
    float out_pwr;

    uW_pwr = 100000*pow(10,dBm_pwr/10);
    out_pwr = (float)uW_pwr;  
    return out_pwr;
}

void _8290c_short_data_set(uint8 *ptr_data, uint32 length, uint32 value)
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

void _8290c_short_data_get(uint8 *ptr_data, uint32 length, uint32 *value)
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
uint32 _8290c_cal_regression(
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
uint32 _8290c_cal_lin_regression(
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

int _8290c_cal_shift(double input, int *pShft, double *output)
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

uint32 _8290c_cal_reverse(
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

int _rtl8290c_flash_init(uint32 devId)
{
    FILE *fp;
    int wfd = -1;
    int length, start_addr, file_len;
    unsigned int input;
    unsigned int i;
    int ret=0;           
    if (devId == 0x5)
        fp = fopen(RTL8290C_DEVICE_0_FILE, "wb");  
    else if (devId == 0x6)
        fp = fopen(RTL8290C_DEVICE_1_FILE, "wb");  
    else if (devId == 0x7)
        fp = fopen(RTL8290C_DEVICE_2_FILE, "wb");
    else if (devId == 0x8)
        fp = fopen(RTL8290C_DEVICE_3_FILE, "wb");  
    else if (devId == 0x9)
        fp = fopen(RTL8290C_DEVICE_4_FILE, "wb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
        return -1;    
    }

    if (NULL ==fp)
    {
        printf("Create file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 
    
    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp);
        return ret;
    }
    fwrite(rtl8290c_a0_reg, sizeof(unsigned char), sizeof(rtl8290c_a0_reg),fp);
    fwrite(rtl8290c_a2l_reg, sizeof(unsigned char), sizeof(rtl8290c_a2l_reg),fp);
    ret=fseek(fp, 0x280, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp);
        return ret;
    }
    fwrite(rtl8290c_wb_reg, sizeof(unsigned char), sizeof(rtl8290c_wb_reg),fp);    
    ret=fseek(fp, 0x380, SEEK_SET); 
    if (ret!=0)
    {
        fclose(fp);
        return ret;
    }
    fwrite(rtl8290c_digital_reg, sizeof(unsigned char), sizeof(rtl8290c_digital_reg),fp);       
    fwrite(rtl8290c_txlut_reg, sizeof(unsigned char), sizeof(rtl8290c_txlut_reg),fp);  
    fwrite(rtl8290c_apdlut_reg, sizeof(unsigned char), sizeof(rtl8290c_apdlut_reg),fp);  
    fwrite(rtl8290c_rsvlut1_reg, sizeof(unsigned char), sizeof(rtl8290c_rsvlut1_reg),fp);
    fwrite(rtl8290c_rsvlut2_reg, sizeof(unsigned char), sizeof(rtl8290c_rsvlut2_reg),fp);    
    ret=fseek(fp, 0x700, SEEK_SET);     
    if (ret!=0)
    {
        fclose(fp);
        return ret;
    }
    fwrite(rtl8290c_digitalext_reg, sizeof(unsigned char), sizeof(rtl8290c_digitalext_reg),fp);   
    fwrite(rtl8290c_user_parameter, sizeof(unsigned char), sizeof(rtl8290c_user_parameter),fp);      
    fflush(fp); 
    wfd = fileno(fp);
    if(wfd != -1)
    {
        fsync(wfd);
    }
    
    fclose(fp);
    printf("Create File Success!!!\n"); 

    _8290c_cal_flash_data_set(devId, RTL8290C_PARAMETER_VERSION_ADDR, 4, RTL8290C_PARAMETER_VERSION_VALUE);

    return 0;    
}

int _rtl8290c_flash_delete(uint32 devId)
{
           
    if (devId == 0x5)
        unlink(RTL8290C_DEVICE_0_FILE);   
    else if (devId == 0x6)
        unlink(RTL8290C_DEVICE_1_FILE);   
    else if (devId == 0x7)
        unlink(RTL8290C_DEVICE_2_FILE);   
    else if (devId == 0x8)
        unlink(RTL8290C_DEVICE_3_FILE);   
    else if (devId == 0x9)
        unlink(RTL8290C_DEVICE_4_FILE);   
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
        return -1;    
    }

    printf("Delete devId %d flash File Success!!!\n", devId); 

    return 0;    
}

int _rtl8290c_load_data(
    unsigned char *ptr,
    uint32 devId,
    int *file_len)
{
    int i, output;
    FILE *fp;
    int ret=0;

    if (devId == 0x5)
        fp = fopen(RTL8290C_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8290C_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8290C_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8290C_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8290C_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
        return -1;    
    }

    if (NULL ==fp)
    {
        printf("Open devId 0x%x file  error!!!!!!!\n", devId); 
        return -1;           
    } 
    
    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp);
        return ret;
    }
    memset(ptr, 0x0, sizeof(char)*RTL8290C_PARAMETER_SIZE);  
    for(i = 0; i < RTL8290C_PARAMETER_SIZE; i++)
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


int _rtl8290c_save_data(
    unsigned char *ptr,
    uint32 devId,
    int *file_len)
{
    int i, output;
    int wfd = -1;    
    FILE *fp;
    int ret=0;

    if (devId == 0x5)
        fp = fopen(RTL8290C_DEVICE_0_FILE, "wb");  
    else if (devId == 0x6)
        fp = fopen(RTL8290C_DEVICE_1_FILE, "wb");  
    else if (devId == 0x7)
        fp = fopen(RTL8290C_DEVICE_2_FILE, "wb");  
    else if (devId == 0x8)
        fp = fopen(RTL8290C_DEVICE_3_FILE, "wb");  
    else if (devId == 0x9)
        fp = fopen(RTL8290C_DEVICE_4_FILE, "wb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
        return -1;    
    }

    if (NULL ==fp)
    {
        printf("Open devId 0x%x file  error!!!!!!!\n", devId); 
        return -1;           
    } 

    ret=fseek(fp, 0, SEEK_SET); 
    if (ret!=0)
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

/* 
   set ASCII string type parameters in A0
   eg. A0/20~35 : Vendor Name
       A0/40~55 : Vendor Part Name
       A0/56~59 : Vendor Rev */
int _8290c_cli_string_set(uint32 devId, int start_addr, uint32 size, char *str)
{
    int ret;
    uint32 temp;
    FILE *fp;

    if (devId == 0x5)
        fp = fopen(RTL8290C_DEVICE_0_FILE, "rb+");  
    else if (devId == 0x6)
        fp = fopen(RTL8290C_DEVICE_1_FILE, "rb+");  
    else if (devId == 0x7)
        fp = fopen(RTL8290C_DEVICE_2_FILE, "rb+");  
    else if (devId == 0x8)
        fp = fopen(RTL8290C_DEVICE_3_FILE, "rb+");  
    else if (devId == 0x9)
        fp = fopen(RTL8290C_DEVICE_4_FILE, "rb+");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
        return -1;    
    }

    if (NULL ==fp)
    {
        printf("Open 8290C DEVICE FILE error!!!!!!!\n");
        return -1;
    }

    //printf("%s( %d, %d, \"%s\" )\n", __FUNCTION__, start_addr, size, str);
 
    ret = fseek(fp, start_addr, SEEK_SET);
    if (ret!=0)
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
int _8290c_cli_string_get(uint32 devId, int start_addr, uint32 size, char *str)
{
    uint32 temp;
    uint32 europa_id;
    uint32 version;
    FILE *fp;
    int ret=0;

    if (devId == 0x5)
        fp = fopen(RTL8290C_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8290C_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8290C_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8290C_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8290C_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
        return -1;    
    }
    
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 
 
    ret=fseek(fp, start_addr, SEEK_SET);
    if (ret!=0)
    {
        fclose(fp);
        return ret;
    }

    ret=fread((void*)str, sizeof(char), size, fp);
    fclose(fp);
        
    return 0;    
}

int _8290c_cli_flashBlock_get(uint32 devId, int start_addr, int length)
{
    int output,i;
    FILE *fp;
    int ret=0;
    
    if (devId == 0x5)
        fp = fopen(RTL8290C_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8290C_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8290C_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8290C_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8290C_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
        return -1;    
    }
    
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n"); 
        return -1;           
    } 
                     
    /* set the file pointer to start address */
    ret=fseek(fp, start_addr, SEEK_SET); 
    if (ret!=0)
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

int _8290c_cli_flashBlock_set( uint32 devId, int start_addr, unsigned int size)
{
    int wfd = -1;
    int file_len, length;
    unsigned int input;
    unsigned int i;
    unsigned char temp[RTL8290C_PARAMETER_SIZE];
    int ret=0;
    
    _rtl8290c_load_data(temp,devId,&file_len);  
           
    printf("The maximum file size is %d. If you want to end the data inupt less than maximum , please key in abcd.  \n", size);              
    printf("Please insert data:\n");       
       
    length = 0;
    for(i = start_addr; i < (start_addr + size); i++)
    {
        ret=scanf("%x",&input);
        if (ret<0)
        {
            return ret;
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
    
    _rtl8290c_save_data(temp,devId,&file_len);    

    return 0;    
}

void _8290c_cli_flash_array_set(uint32 devId, uint32 address, uint32 length, uint8 *data)
{
    FILE *fp;
    int wfd = -1;
    uint8 *init_data, *ptr_data;
    uint32 i;
    uint8 output;
    int ret=0;
    
    if (devId == 0x5)
        fp = fopen(RTL8290C_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8290C_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8290C_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8290C_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8290C_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
        return -1;    
    }
    
    if (NULL ==fp)
    {
        printf("Open file error!!!!!!!\n"); 
        return -1;			   
    }
  
    printf("devId = 0x%x.\n", devId); 
    
    init_data = (uint8 *)malloc(RTL8290C_PARAMETER_SIZE*sizeof(uint8));

    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        free(init_data);
        fclose(fp);
        return;
    }
    memset(init_data, 0x0, sizeof(uint8)*RTL8290C_PARAMETER_SIZE);
    ret=fread(init_data, 1, RTL8290C_PARAMETER_SIZE, fp);
    //if (ret!=0)
    //{
    //    free(init_data);
    //    fclose(fp);
    //    return;
    //}
    fclose(fp);

    ptr_data = init_data;
    ptr_data = ptr_data + address;

    memcpy(ptr_data, data, length);

    if (devId == 0x5)
        fp = fopen(RTL8290C_DEVICE_0_FILE, "wb");  
    else if (devId == 0x6)
        fp = fopen(RTL8290C_DEVICE_1_FILE, "wb");  
    else if (devId == 0x7)
        fp = fopen(RTL8290C_DEVICE_2_FILE, "wb");  
    else if (devId == 0x8)
        fp = fopen(RTL8290C_DEVICE_3_FILE, "wb");  
    else if (devId == 0x9)
        fp = fopen(RTL8290C_DEVICE_4_FILE, "wb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
        return -1;    
    }

    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        free(init_data);        
        return;
    }
    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        free(init_data);
        fclose(fp);
        return;
    }

    fwrite(init_data, sizeof(char), RTL8290C_PARAMETER_SIZE, fp);

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

void _8290c_cal_flash_data_get(uint32 devId, uint32 address, uint32 length, uint32 *value)
{
    FILE *fp;
    uint8 *init_data, *ptr_data;
    int ret=0;
    
    if (devId == 0x5)
        fp = fopen(RTL8290C_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8290C_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8290C_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8290C_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8290C_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
        return -1;    
    }

    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return;
    }

    init_data = (uint8 *)malloc(RTL8290C_PARAMETER_SIZE*sizeof(uint8));

    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        free(init_data);
        fclose(fp);
        return;
    }
    memset(init_data, 0x0, sizeof(uint8)*RTL8290C_PARAMETER_SIZE);
    ret=fread(init_data, 1, RTL8290C_PARAMETER_SIZE, fp);
    //if (ret!=0)
    //{
    //    free(init_data);
    //    fclose(fp);
    //    return;
    //}
    ptr_data = init_data;

    ptr_data = ptr_data + address;
    _8290c_short_data_get(ptr_data, length, value);

    free(init_data);
    fclose(fp);

    return;
}

void _8290c_cal_flash_data_set(uint32 devId, uint32 address, uint32 length, uint32 value)
{
    FILE *fp;
    int wfd = -1;
    uint8 *init_data, *ptr_data;
    uint32 i;
    uint8 output;
    int ret=0;

    if (devId == 0x5)
        fp = fopen(RTL8290C_DEVICE_0_FILE, "rb");  
    else if (devId == 0x6)
        fp = fopen(RTL8290C_DEVICE_1_FILE, "rb");  
    else if (devId == 0x7)
        fp = fopen(RTL8290C_DEVICE_2_FILE, "rb");  
    else if (devId == 0x8)
        fp = fopen(RTL8290C_DEVICE_3_FILE, "rb");  
    else if (devId == 0x9)
        fp = fopen(RTL8290C_DEVICE_4_FILE, "rb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);     
        return -1;    
    }

    if (NULL ==fp)
    {
        printf("Open 8290C DEVICE FILE error!!!!!!!\n");
        return;
    }

    init_data = (uint8 *)malloc(RTL8290C_PARAMETER_SIZE*sizeof(uint8));

    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        free(init_data);
        fclose(fp);
        return;
    }
    memset(init_data, 0x0, sizeof(uint8)*RTL8290C_PARAMETER_SIZE);
    ret=fread(init_data, 1, RTL8290C_PARAMETER_SIZE, fp);
    //if (ret!=0)
    //{
    //    free(init_data);
    //    fclose(fp);
    //   return;
    //}
    fclose(fp);

    ptr_data = init_data;
    ptr_data = ptr_data + address;
    _8290c_short_data_set(ptr_data, length, value);

    if (devId == 0x5)
        fp = fopen(RTL8290C_DEVICE_0_FILE, "wb");  
    else if (devId == 0x6)
        fp = fopen(RTL8290C_DEVICE_1_FILE, "wb");  
    else if (devId == 0x7)
        fp = fopen(RTL8290C_DEVICE_2_FILE, "wb");  
    else if (devId == 0x8)
        fp = fopen(RTL8290C_DEVICE_3_FILE, "wb");  
    else if (devId == 0x9)
        fp = fopen(RTL8290C_DEVICE_4_FILE, "wb");  
    else
    {
        printf("devId = 0x%x, no such devId in RTL8290C.\n", devId);
        free(init_data);
        return;    
    }

    if (NULL ==fp)
    {
        printf("Open 8290C DEVICE FILE error!!!!!!!\n");
        free(init_data);        
        return;
    }
    ret=fseek(fp, 0, SEEK_SET);
    if (ret!=0)
    {
        free(init_data);
        fclose(fp);
        return;
    }
    fwrite(init_data,sizeof(char), RTL8290C_PARAMETER_SIZE,fp);
        
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


void _rtl8290c_gen_laserlut(uint32 devId, uint32 bias, uint32 mod, int32 temperature, double sb0, double sb1, double sm0, double sm1)
{
    int ret;
    uint32  ib0, im0;
    double  ftmp;
    int i;
    
    uint32 lut[RTL8290C_LASER_LUT_SZ] = { 0 };
    int t, t0;
    uint8 txlut[RTL8290C_TXLUT_SZ] = { 0 };

    ib0 = bias;
    im0 = mod;
    t0  = temperature;
        
    printf("Ib=%d Im=%d T0=%d SB0=%f SB1=%f SM0=%f SM1=%f\n",
            ib0, im0, t0, sb0, sb1, sm0, sm1);
        
    for (t=-40; t<=120; t++)
    {
       if (t == t0)
       {
           lut[(t+40)*2]   = RTL8290C_IBCONV(ib0);
           lut[(t+40)*2+1] = RTL8290C_IMCONV(im0);
       }
       else if (t < t0)
       {
           ftmp = ib0 - (t0 - t)*sb0;
           if (3.0 > ftmp)
              ftmp=3.0;
           lut[(t+40)*2] = RTL8290C_IBCONV((int)ftmp);
        
           ftmp = im0 - (t0 - t)*sm0;
           if (12.0 > ftmp)
               ftmp=12.0;
           lut[(t+40)*2+1] = RTL8290C_IMCONV((int)ftmp);
       }
       else if (t > t0)
       {
           ftmp = ib0 + (t - t0)*sb1;
           if (ftmp>50.0)
           ftmp=50.0;
           lut[(t+40)*2] = RTL8290C_IBCONV((int)ftmp);
        
           ftmp = im0 + (t - t0)*sm1;
           if (ftmp>80.0)
              ftmp=80.0;
           lut[(t+40)*2+1] = RTL8290C_IMCONV((int)ftmp);
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
       txlut[i*2+RTL8290C_TXLUT_OFFSET] = (uint8)((lut[t+1]&0x7F0)>>4);       
       //APCDIG_MOD_INI<3:0>
       txlut[i*2+RTL8290C_TXLUT_OFFSET+1] = (uint8)(lut[t+1]&0xF); 
       printf("Selected: i = %2d IB: 0x%4x IM: 0x%4x \n", i, lut[t], lut[t+1]);       
    }
    
    printf("Write to flash.... \n");         
    for (i=0;i<RTL8290C_TXLUT_SZ;i++)
    {
        if(i%16 == 0)
        {
            printf("0x%04x ", RTL8290C_TXLUT_ADDR_BASE +i);
        }

        printf("0x%02x ", txlut[i]);
        if(i%8 == 7)
            printf("   ");
        if(i%16 == 15)
            printf("\n");                    
    }

        
    _8290c_cli_flash_array_set(devId, RTL8290C_TXLUT_ADDR_BASE, RTL8290C_TXLUT_SZ, txlut);
        
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

int _rtl8290c_gpon_init(void)
{
    uint32 chipId, rev, subType;  
    int ret;

    if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return ret;
    }

    if (RTL9607C_CHIP_ID == chipId)
    {
        if (rev > CHIP_REV_ID_B)
        {
            rtk_ponmac_mode_t mode;    
                
            if((ret = rtk_ponmac_mode_get(&mode)) != 0)
            {
                return ret;
            }

            //printf("PON Mode = %d", mode);

            if (mode == 0) //GPON mode
            {
                system("diag gpon init");
            }
            else if (mode == 1) //EPON mode
            {
                //20220329: 9607CVB init patch
                system("diag reg set 0x270 0x1f");
                system("diag reg set 0x40038 0x900");
                system("diag reg set 0x4003c 0x20");
                system("diag reg set 0x4072c 0x1800");
                system("diag reg set 0x405e8 0x2e2");
                system("diag reg set 0x405dc 0x2000");
                system("diag reg set 0x40708 0x2e30");
                system("diag reg set 0x405e0 0x23c2");
                system("diag reg set 0x405e4 0x6f42");
                system("diag reg set 0x405b8 0x2607");
                system("diag reg set 0x405bc 0x8090");
                system("diag reg set 0x40588 0xc41d");
                system("diag reg set 0x405b4 0x9150");
                system("diag reg set 0x40594 0xa4");
                system("diag reg set 0x40598 0x8000");
                system("diag reg set 0x405a4 0x187C");
                system("diag reg set 0x40714 0xc5b0");
                system("diag reg set 0x405F8 0x3689");
                system("diag reg set 0x405FC 0xf4a");
                system("diag reg set 0x40710 0xc081");
                system("diag reg set 0x270 0x08"); 
            }
            else
            {
                printf("PON Mode Error( mode = %d)!!!", mode);                
            }            
        }
        else
        {
            system("diag reg set 0x40030 0xf30");
            system("diag reg set 0x40508 0x0");
            system("diag reg set 0x270 0x1f");
            system("diag reg set 0x40038 0x900");
            system("diag reg set 0x4003c 0x20");
            system("diag reg set 0x405c4 0x2a89");
            system("diag reg set 0x405e0 0x7BC0");
            system("diag reg set 0x405e4 0xEf40");
            system("diag reg set 0x405d4 0xE5AC");
            system("diag reg set 0x405a4 0x1800");
            system("diag reg set 0x40598 0x8002");
            system("diag reg set 0x40588 0x231D");
            system("diag reg set 0x405A0 0x1245");
            system("diag reg set 0x405b0 0x11");
            system("diag reg set 0x405b4 0x9150");
            system("diag reg set 0x405b8 0x2107");
            system("diag reg set 0x40580 0x80");
             system("diag reg set 0x270 0x8");
            system("diag reg set 0x40508 0x3000");
        }
    }
    else if (RTL9602C_CHIP_ID == chipId)
    {
        system("diag reg set 0x1D0 0x1f");
        system("diag reg set 0x225d8 0x29");
        system("diag reg set 0x225ac 0xB000");
        system("diag reg set 0x225b0 0x4848");
        system("diag reg set 0x22738 0x80C5");
        system("diag reg set 0x22504 0x30");
        system("diag reg set 0x22038 0x900");
        system("diag reg set 0x2203c 0x0");
        system("diag reg set 0x1D0 0x8");
        system("diag reg set 0x22508 0x3000");
    }
    else if (RTL9603CVD_CHIP_ID == chipId)
    {
#if 1 //20210707 IB too large
        system("diag reg set 0x200 0x1f");
        system("diag reg set 0x40038 0x900");
        system("diag reg set 0x4058c 0x1929");
        system("diag reg set 0x405A4 0x2a89");
        system("diag reg set 0x405c4 0x3032");
        system("diag reg set 0x405d0 0x8dad");
        system("diag reg set 0x405d4 0x13A4");
        system("diag reg set 0x405e8 0x822d");
        system("diag reg set 0x405ec 0x23A4");
        system("diag reg set 0x200 0x8");
#endif
        system("diag gpon init");
    }
#if defined(CONFIG_LUNA_G3_SERIES)    
    //For Cortina ARM-based 9607DQ series
    else if (RTL8277C_CHIP_ID == chipId)
    {
        system("killall -9 monitord omci_app eponoamd");
        system("diag rt_ponmisc set force-prbs off");
        system("diag rt_ponmisc set mode 255 speed 0");        
        system("diag rt_ponmisc set mode 0 speed 0");
        system("diag rt_gpon init");
        system("diag rt_gpon activate init-state o1");
    }
#endif    
    else
    {
        printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
    }

    return 0;
}


int _rtl8290c_epon_init(void)
{
    uint32 chipId, rev, subType;  
    int ret;

    if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
    {
        return ret;
    }

    if (RTL9607C_CHIP_ID == chipId)
    {
        if (rev > CHIP_REV_ID_B)
        {
            //20220329: 9607CVB init patch
            system("diag reg set 0x270 0x1f");    
            system("diag reg set 0x40c00 0x1140");
            system("diag reg set 0x40808 0x71e0");
            system("diag reg set 0x40038 0x900");
            system("diag reg set 0x407ac 0x1800");
            system("diag reg set 0x405e8 0x2e2");
            system("diag reg set 0x405dc 0x2000");
            system("diag reg set 0x40788 0x2e30");
            system("diag reg set 0x405e0 0x6BC0");
            system("diag reg set 0x405b8 0x2607");
            system("diag reg set 0x405bc 0x8090");
            system("diag reg set 0x40588 0xc81d");
            system("diag reg set 0x405b4 0x9150");
            system("diag reg set 0x40594 0xa4");
            system("diag reg set 0x40598 0x8000");
            system("diag reg set 0x405a4 0x187C");
            system("diag reg set 0x405ac 0x94e0");
            system("diag reg set 0x40794 0xca70");
            system("diag reg set 0x405F8 0x3689");
            system("diag reg set 0x405FC 0xf4a");
            system("diag reg set 0x270 0xC");
            
            //system("diag epon init");             
        }
        else
        {
            system("diag reg set 0x270 0x1f");    
            system("diag reg set 0x40c00 0x1140");
            system("diag reg set 0x40808 0x71e0");
            system("diag reg set 0x40038 0x900");
            system("diag reg set 0x405c4 0x2a89");
            system("diag reg set 0x405e0 0x7BC0");
            system("diag reg set 0x405e4 0xEf40");
            system("diag reg set 0x405d4 0xFDF0");
            system("diag reg set 0x40588 0x231D");
            system("diag reg set 0x405a4 0x1800");
            system("diag reg set 0x40598 0x8002");
            system("diag reg set 0x405A0 0x1245");
            system("diag reg set 0x405b0 0x11");
            system("diag reg set 0x405b4 0x9150");
            system("diag reg set 0x405b8 0x2107");
            system("diag reg set 0x40580 0x80");
            system("diag reg set 0x270 0xC");
        }
    }
    else if (RTL9602C_CHIP_ID == chipId)
    {
        system("diag reg set 0x1D0 0x1f");
        system("diag reg set 0x22c00 0x1140");
        system("diag reg set 0x22808 0x71e0");
        system("diag reg set 0x225d8 0x29");
        system("diag reg set 0x225ac 0xB000");
        system("diag reg set 0x225b0 0x4848");
        system("diag reg set 0x227b8 0x80C5");
        system("diag reg set 0x22038 0x900");
        system("diag reg set 0x1D0 0xC");        
    }
    else if (RTL9603CVD_CHIP_ID == chipId)    
    {
        //system("diag reg set 0x200 0x1f");
        system("diag reg set 0x40038 0x900");
        system("diag reg set 0x40c00 0x1140");
        system("diag reg set 0x4058c 0x1929");
        system("diag reg set 0x405A4 0x2a89");
        system("diag reg set 0x405c4 0x3032");
        system("diag reg set 0x405d0 0x8dad");
        system("diag reg set 0x405d4 0x13a4");
        system("diag reg set 0x405e8 0x822d");
        system("diag reg set 0x405ec 0x23a4");
        system("diag reg set 0x200 0xC");

        system("diag epon init");        
    } 
#if defined(CONFIG_LUNA_G3_SERIES)
    //For Cortina ARM-based 9607DQ series
    else if (RTL8277C_CHIP_ID == chipId)
    {
        system("killall -9 monitord omci_app eponoamd");    
        system("diag rt_ponmisc set force-prbs off");
        system("diag rt_ponmisc set mode 255 speed 0");        
        system("diag rt_ponmisc set mode 1 speed 0");
        system("diag rt_epon init");        
    }
#endif    
    else
    {
        printf("Unknow Chip ID (0x%x), do nothing...\n", chipId);
    }

    return 0;
}

void _rtl8290c_ibim_low(uint32 devId)
{
    int ret;
    rtk_ldd_loop_mode2_t loop_mode;
    uint32 data, wb5b_data;

    ret = rtk_ldd_parameter2_get(devId, 0x2DB, &wb5b_data);
    if(ret)
    {
        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
        return;
    }
    data = wb5b_data&(~(0x30));
    ret = rtk_ldd_parameter2_set(devId, 0x2DB, data);
    if(ret)
    {
        printf("rtk_ldd_parameter2_set failed (%d)\n", ret);
        return;
    }  

    loop_mode = LDD_LOOP2_DOL;        
    rtk_ldd_loopMode2_set(devId, loop_mode);

    rtk_ldd_tx_bias2_set(devId, 196);
    rtk_ldd_tx_mod2_set(devId, 196);
    
    loop_mode = LDD_LOOP2_DCL;        
    rtk_ldd_loopMode2_set(devId, loop_mode);

    return;
}

void _rtl8290c_ibim_recover(uint32 devId)
{
    int ret;
    uint32 addr, value;
    uint32 ib, im, ibmin_en;
    rtk_ldd_loop_mode2_t loop_mode;
    uint32 data, wb5b_data;

    loop_mode = LDD_LOOP2_DOL;        
    rtk_ldd_loopMode2_set(devId, loop_mode);

    addr = RTL8290C_APC_IBIAS_INIT_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
    _8290c_cal_flash_data_get(devId, addr, 1, &ib); 
    printf("\nIBIAS = 0x%x\n", ib);
    addr = RTL8290C_APC_IMOD_INIT_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
    _8290c_cal_flash_data_get(devId, addr, 1, &im); 
    printf("IMOD = 0x%x\n", im);
    value = ((im&0xF)<<4)|(ib&0xF);

    value = (ib*100000)>>11;
    rtk_ldd_tx_bias2_set(devId, value);
    value = (im*100000)>>11;    
    rtk_ldd_tx_mod2_set(devId, value);

    addr = RTL8290C_APC_BIAS_MIN_EN_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
    _8290c_cal_flash_data_get(devId, addr, 1, &ibmin_en);  

    loop_mode = LDD_LOOP2_DCL;        
    rtk_ldd_loopMode2_set(devId, loop_mode);
    
    if (ibmin_en == 0x1)
    {
        ret = rtk_ldd_parameter2_get(devId, 0x2DB, &wb5b_data);
        if(ret)
        {
            printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
            return;
        }
        data = wb5b_data|(0x30);
        ret = rtk_ldd_parameter2_set(devId, 0x2DB, data);
        if(ret)
        {
            printf("rtk_ldd_parameter2_set failed (%d)\n", ret);
            return;
        }    
    }    
    
    return;
}


int rtl8290c_cli_reset(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)

{
    int ret=0;

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
    return ret;
}    




int rtl8290c_cli_temperature_get(uint32 devId)
{
    int ret;
    uint32 temp_data, addr;
    int8 offset;
    float tempK, tempC;
    int temp32;
    signed short temp16;
    uint8 buf[2];
    double tmp = 0;

    ret = rtk_ldd_temperature2_get(devId, &temp_data);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return -1;
    }
    temp32 = temp_data - RTL8290C_TEMP_ZERO;
    
#if 0
    tempK = (float)temp_data/1024;
    tempC = tempK-273.15;

    //DDMI transformation
    temp16 = temp32/4;

    printf("Temperature=%f K (%f C) raw_data = 0x%x\n", tempK, tempC, temp_data);    

    printf("DDMI: temp32=%d temp16 = %d (0x%x)\n", temp32, temp16, temp16); 
#endif

    buf[0] = temp32>>10;
    buf[1] = (temp32&0x3FF)>>2;
    if (128 >= buf[0]) //MSB: buf[0]; LSB: buf[1]
    {
        tmp = (-1)*((~(buf[0]))+1)+((double)(buf[1])*1/256);
    }
    else
    {
        tmp = (char)buf[0]+((double)(buf[1])*1/256);
    }

    addr = RTL8290C_TEMP_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
    _8290c_cal_flash_data_get(devId, addr, 1, &temp_data); 
    offset = temp_data&0xFF;

    tmp = tmp - offset;

    printf("Temperature = %f C\n", tmp); 
 
    return 0;
}

int rtl8290c_cli_vdd_get(uint32 devId)
{
    int ret;
    uint32 vdd;
    
    vdd = 0;
    
    ret = rtk_ldd_vdd2_get(devId, &vdd);
    if(ret)
    {
        printf("rtk_ldd_vdd2_get Fail!!! (%d)\n", ret);
        return -1;
    }
    printf("VDD =  %duV (0x%x)\n", vdd, vdd);
    
    return 0;    
}

int rtl8290c_cli_txbias_set(
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

        if (100.0 < curr)
        {
            printf("Invalid bias current %f\n", curr);
            return -1;
        }
    
        Ibias = (uint32)(curr * 1000); /* convert mA to uA unit */
    
        printf("Input current = %f mA, Ibias = %d uA\n", curr, Ibias);

        ret = rtk_ldd_tx_bias2_set(devId, Ibias);
        if(ret)
        {
            printf("Set RTL8290C TX Bias Fail!!! (%d)\n", ret);
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


int rtl8290c_cli_txbias_get(uint32 devId)
{
    int ret;
    uint32 regData1, regData2, biasRaw;
    uint32 bias;
    
    ret = rtk_ldd_parameter2_get(devId, 0x2C7, &regData1);
    if(ret)
    {
        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
        return -1;
    }
    ret = rtk_ldd_parameter2_get(devId, 0x2C9, &regData2);
    if(ret)
    {
        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
        return -1;
    }
    biasRaw = ((regData1&0x7F)<<4)|((regData2&0xF0)>>4);
    bias = ((biasRaw&0x7FF)*(100*1000))>>11;
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

int rtl8290c_cli_txmod_set(
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

        if (100.0 < curr)
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


int rtl8290c_cli_txmod_get(uint32 devId)
{
    int ret;
    uint32 mod;
    uint32 regData1, regData2, modRaw;
    
    ret = rtk_ldd_parameter2_get(devId, 0x2C8, &regData1);
    if(ret)
    {
        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
        return -1;
    }
    ret = rtk_ldd_parameter2_get(devId, 0x2C9, &regData2);
    if(ret)
    {
        printf("rtk_ldd_parameter2_get failed (%d)\n", ret);
        return -1;
    }
    modRaw = ((regData1&0x7F)<<4)|(regData2&0xF);
    mod = ((modRaw&0x7FF)*(100*1000))>>11;
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

int rtl8290c_cli_loopmode_set(
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

        _8290c_cal_flash_data_set(devId, RTL8290C_APC_LOOP_MODE_OFFSET_ADDR+ RTL8290C_PARAMETER_BASE_ADDR, 1, loop_mode);           

        return 0;

    }
    else
    {
        printf("%s <dol/dcl/apc_b/apc_m/scl-b/erc/apc_erc_s> \n", argv[0]);        
        return -1;
    }

    return 0;    

}

int rtl8290c_cli_loopmode_get(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 mode, value;

    rtk_ldd_parameter2_get(devId, 0x2D2, &mode);

    mode = (mode&0xE0)>>5;

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

    _8290c_cal_flash_data_get(devId, RTL8290C_APC_LOOP_MODE_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR, 1, &value);           

    printf("Loop mode in flash = 0x%x \n", value);    


    return 0;    

}

int rtl8290c_cli_register_set(
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
                printf("%s %s %s <address: 0~0x7F> \n", argv[0], argv[1], argv[2]);
                return -1;
            }
        
            addr = _vlaue_translate(argv[2]);
            data = _vlaue_translate(argv[3]); 

            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/dig/digex <address: 0~0x7F> <data> \n", argv[0]);
                return -1;
            }
            if (data >0xFF)
            {
                printf("data should be 0~0xFF\n");
                
            }

            fin_addr = RTL8290C_WB_ADDR_BASE + addr;
            regData = (uint32)(data&0xFF);
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

            fin_addr = RTL8290C_DIG_ADDR_BASE + addr;
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

            fin_addr = RTL8290C_DIGEX_ADDR_BASE + addr;
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


int rtl8290c_cli_register_get(
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
            if((addr<0)||(addr>0x7F))
            {
                printf("addr = 0x%x, out of range \n", addr);
                printf("%s wb/rb <address: 0~0x7F> \n", argv[0]);
                return -1;
            }
            fin_addr = RTL8290C_WB_ADDR_BASE + addr;
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
            fin_addr = RTL8290C_RB_ADDR_BASE + addr;
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
            fin_addr = RTL8290C_DIG_ADDR_BASE + addr;
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
            fin_addr = RTL8290C_DIGEX_ADDR_BASE + addr;
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

int rtl8290c_cli_efuse_set(
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
        if (entry>39)
        {
           printf("%s <entry: 0~39> <value: 0~0xFF>\n", argv[0]);
           return -1;
        }
        value = _vlaue_translate(argv[2]); 
        if (value>0xFF)
        {
           printf("%s <entry: 0~39> <value: 0~0xFF>\n", argv[0]);
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
        printf("%s <entry: 0~39> \n", argv[0]);
        return -1;
    }

    return 0;    
}

int rtl8290c_cli_efuse_get(
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
        if (entry>39)
        {
           printf("%s <entry: 0~39> \n", argv[0]);
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
        printf("%s <entry: 0~39> \n", argv[0]);
        return -1;
    }

    return 0;    
}


int rtl8290c_cli_laserlut_set(uint32 devId)
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
    
    _rtl8290c_gen_laserlut(devId, bias, mod, temperature, sb0, sb1, sm0, sm1);

    return 0; 
}

int rtl8290c_cli_laserinfo_get(uint32 devId)
{
    int ret;
    uint32 bias;
    uint32 mod;
    int8 offset;
    double ib, im;
    uint32 temp_data, addr;
    float tempK, tempC;

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
      
    tempK = tempC = 0;
    ret = rtk_ldd_temperature2_get(devId, &temp_data);
    if(ret)
    {
        printf("Get Temperature Fail!!! (%d)\n", ret);
        return -1;
    }
    tempK = (float)temp_data/1024;
    tempC = tempK-273.15;            
    printf("Temperature=%f K (%f C) \n", tempK, tempC);     

    addr = RTL8290C_TEMP_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
    _8290c_cal_flash_data_get(devId, addr, 1, &temp_data); 
    offset = temp_data&0xFF;
    
    printf("Temperature offset = %d now\n", (int)offset);  

    printf("Bias=%2.2f Modulation=%2.2f\n", ib, im);

    return 0;
}

int rtl8290c_cli_update_flash(uint32 devId)
{
    int ret;
    uint32 version, addr;

    addr = RTL8290C_PARAMETER_VERSION_ADDR;    
    _8290c_cal_flash_data_get(devId, addr, 4, &version);
    printf("RTL8290C LDD flash version = 0x%8x\n", version);	
    if (RTL8290C_PARAMETER_VERSION_VALUE!=version)
    {
        version = RTL8290C_PARAMETER_VERSION_VALUE;
        _8290c_cal_flash_data_set(devId, addr, 4, version);
        _8290c_cal_flash_data_get(devId, addr, 4, &version);
        printf("Update version to 0x%8x\n", version);		
		addr = RTL8290C_TXSD_MODE_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;	
		_8290c_cal_flash_data_set(devId, addr, 1, 0); 
		addr = RTL8290C_APC_LASER_ON_DELAY_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;	
		_8290c_cal_flash_data_set(devId, addr, 1, 0); 
    }
	
    return 0;
}

int rtl8290c_cli_rxpower2_get(
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
    
    chip_mode = _europa_ldd_parser();

    if(EUROPA_LDD_8290C_MODE != chip_mode)
    {
        printf("LDD Chip Not Supported!!!\n");
        return -1;
    } 
    
    if(argc >= 4)
    { 
        rssi_code1   = _vlaue_translate(argv[1]);
        rssi_code2   = _vlaue_translate(argv[2]);
        rssi_code3   = _vlaue_translate(argv[3]);        
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

        
    }
    else
    {
        //printf("%s <UINT:code_rssi> <UINT:code_k> <UINT:code_r6400> <INT:M -2~5> <INT:mode:0/1/2> <uint: nA is 0, 0.01uA is 1, 0.1uA is 2> <cal_mode: 0 for 32bit, 1 for 64bit >\n", argv[0]);   
        printf("%s <UINT:code_rssi> <UINT:code_k> <UINT:code_r6400> <INT:rssifl_sel> \n", argv[0]);   
        return -1;
    }

    return 0;    

}
#if 0
#if 0
static int europa_cli_rxtrans_get(
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
    int loopcnt,i;
    
    if(argc >= 6)
    { 
        rssi_code1   = _vlaue_translate(argv[1]);
        data_a   = _vlaue_translate(argv[2]);
        data_b   = _vlaue_translate(argv[3]);
        data_c   = _vlaue_translate(argv[4]);    
        loopcnt   = _vlaue_translate(argv[5]);    

        for (i=0;i<=loopcnt;i++)
        {
        memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
        lddRxPwr.rssi_code1 = rssi_code1 + i*100;     
        lddRxPwr.data_a = data_a;
        lddRxPwr.data_b = data_b;
        lddRxPwr.data_c = data_c;
        lddRxPwr.rx_c = 0xFF;

        //printf("rtk_ldd_rx_power2_get: %d %d %d %d %d\n", lddRxPwr.rssi_code1, lddRxPwr.rssi_code2, lddRxPwr.data_a, lddRxPwr.data_b, lddRxPwr.data_c); 
#if 1
        ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
        if(ret)
        {
            printf("Get RX power fail!!! (%d)\n", ret);
            return -1;
         }
#endif        
        printf("europa_cli_rxtrans_get: IRSSI = %8d  RX power = %8d nW  (%5d 0.1uW)\n", lddRxPwr.rssi_code1, lddRxPwr.rx_a, lddRxPwr.rx_b); 
        }
        
    }
    else
    {
        printf("%s <UINT:I RSSI> <INT: RX_A> <INT: RX_B> <INT: RX_C> <INT: loopcnt>\n", argv[0]);   
        return -1;
    }

    return 0;    

}
#else
static int europa_cli_rxtrans_get(
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 rssi_code1, rssi_code2, rssi_code3, rssi_code4;
    int data_a, data_b, data_c; 
    int shft_a, shft_b, shft_c; 
    rtk_ldd_rxpwr_t lddRxPwr;
    uint32 devId = 0x5;
    int cal_mode;
    int loopcnt,i;
    int rxpwr_old, rxpwr_old2, rxpwr_new, rxpwr_new2;
        
    if(argc >= 6)
    { 
        rssi_code1   = _vlaue_translate(argv[1]);
        data_a   = _vlaue_translate(argv[2]);
        data_b   = _vlaue_translate(argv[3]);
        data_c   = _vlaue_translate(argv[4]);    
        loopcnt   = _vlaue_translate(argv[5]); 
        memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));        
        lddRxPwr.rx_a = data_a;
        lddRxPwr.rx_b = data_b;
        lddRxPwr.rx_c = data_c;
        lddRxPwr.data_a = 30;
        lddRxPwr.data_b = 13;
        lddRxPwr.data_c = 12;

        for (i=0;i<=loopcnt;i++)
        {
            //memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
            lddRxPwr.rssi_code1 = rssi_code1 + i*100;     
            lddRxPwr.op_mode= 0xFF;

           //printf("rtk_ldd_rx_power2_get: %d %d %d %d %d\n", lddRxPwr.rssi_code1, lddRxPwr.rssi_code2, lddRxPwr.data_a, lddRxPwr.data_b, lddRxPwr.data_c); 
#if 1
            ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
            if(ret)
            {
                printf("Get RX power fail!!! (%d)\n", ret);
                return -1;
            }
#endif    
            rxpwr_old = (int)(lddRxPwr.rssi_code2);
            rxpwr_old2 = (int)(lddRxPwr.rssi_code3);
            //printf("europa_cli_rxtrans_get: IRSSI = %8d  RX power = %8d nW  (%5d 0.1uW)\n", lddRxPwr.rssi_code1, rxpwr_old, rxpwr_old2); 

            //memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
            lddRxPwr.rssi_code1 = rssi_code1 + i*100;      
            lddRxPwr.op_mode = 0xFE;
        
            //printf("rtk_ldd_rx_power2_get: %d %d %d %d %d\n", lddRxPwr.rssi_code1, lddRxPwr.rssi_code2, lddRxPwr.data_a, lddRxPwr.data_b, lddRxPwr.data_c); 
#if 1
            ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
            if(ret)
            {
                printf("Get RX power fail!!! (%d)\n", ret);
                return -1;
            }
#endif
            rxpwr_new = (int)(lddRxPwr.rssi_code2);
            rxpwr_new2 = (int)(lddRxPwr.rssi_code3);

            printf("IRSSI = %8d  OLD: RX power = %8d nW (%5d 0.1uW) NEW: RX power = %8d nW (%5d 0.1uW)\n", lddRxPwr.rssi_code1, rxpwr_old, rxpwr_old2, rxpwr_new, rxpwr_new2); 

        }
        
    }
    else
    {
        printf("%s <UINT:I RSSI> <INT: RX_A> <INT: RX_B> <INT: RX_C> <INT: loopcnt>\n", argv[0]);   
        return -1;
    }

    return 0;    

}


#endif
static int europa_cli_param_shift_get(
    int argc,
    char *argv[],
    FILE *fp)
{   
    int ret;
    double float_a, float_b,float_c;
    int    shift_a, shift_b, shift_c;
    int    fin_a, fin_b, fin_c;
    uint32 i_rssi, shft_rssi;
    uint32 devId = 0;
    int    i,  loopcnt;    
    rtk_ldd_rxpwr_t lddRxPwr;    
#ifdef RTL8290B_CAL_TIME    
    struct    timeval    tv_tmp1, tv_tmp2;
    struct    timezone   tz;
    double Duration; 
    
    gettimeofday(&tv_tmp1,&tz); 
#endif

    if(argc >= 6)
    {
            float_a = atof(argv[1]);     
            float_b = atof(argv[2]);              
            float_c = atof(argv[3]); 
            i_rssi  = _vlaue_translate(argv[4]);   
            loopcnt  = _vlaue_translate(argv[5]);             
            
            printf("a = %lf, b = %lf, c = %lf\n", float_a, float_b, float_c);  
            
            shift_a = 0;
            _europa_cal_shift(float_a, &shift_a, &float_a);
            //fin_a = (int)(float_a*(1<<shift_a));
            fin_a = (int)(float_a); 
            
            shift_b = 0;
           _europa_cal_shift(float_b, &shift_b, &float_b); 
            //fin_b = (int)(float_b*(1<<shift_b));
            fin_b = (int)(float_b);
            
            shift_c = 0;           
           _europa_cal_shift(float_c, &shift_c, &float_c);  
            //fin_c = (int)(float_c*(1<<shift_c));
            fin_c = (int)(float_c);

            printf("a = %lf, shift_a = %2d, fin_a = %6d (0x%X) \n", float_a, shift_a, fin_a, fin_a);             
            printf("b = %lf, shift_b = %2d, fin_b = %6d (0x%X) \n", float_b, shift_b, fin_b, fin_b); 
            printf("c = %lf, shift_c = %2d, fin_c = %6d (0x%X) \n", float_c, shift_c, fin_c, fin_c); 

#if 0
            _europa_cal_shift2(i_rssi, &shft_rssi);
            printf("i_rssi = %6d (0x%X) shft_rssi = %6d (0x%X)\n", i_rssi, i_rssi, shft_rssi, shft_rssi); 
#endif

            memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));          
            lddRxPwr.rx_a = fin_a;
            lddRxPwr.rx_b = fin_b;
            lddRxPwr.rx_c = fin_c;
            lddRxPwr.data_a = shift_a;
            lddRxPwr.data_b = shift_b;
            lddRxPwr.data_c = shift_c;
    
            for (i=0;i<=loopcnt;i++)
            {
                lddRxPwr.i_rssi= i_rssi + i*100;      
                lddRxPwr.op_mode = RTL8290C_RX_CURR2PWR_MODE;
#if 1
                ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
                if(ret)
                {
                    printf("Get RX power fail!!! (%d)\n", ret);
                    return -1;
                }
#endif        
                printf("Result: IRSSI = %8d  RX power = %8d nW    (%5d 0.1uW)\n", lddRxPwr.i_rssi, lddRxPwr.rssi_code2, lddRxPwr.rx_power); 
    
            }
    }
    else
    {
        printf("This command is used to calculate RX A/B/C from floating point to integer.\n", argv[0]); 
        printf("%s <float a> <float b> <float c> <UINT i_rssi> <UINT loopcnt> \n", argv[0]);   
    }
#ifdef RTL8290B_CAL_TIME    
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: europa_cli_cal_power Duration = %f ms\n", Duration);
#endif
    return 0; 
}
#endif

/*
 * Get RSSI Current (nA)
 */
uint32 _rtl8290c_cal_get_rssi_current(uint32 devId)
{
    int ret;
    uint32 i, loopcnt;
    double i_rssi_nA;
    uint32 ctrl_data;
    rtk_ldd_rxpwr_t lddRxPwr;
    uint64 sum_i=0;
    uint32 i_rssi;

    loopcnt = 50;

    rtk_ldd_parameter2_get(devId, 0x387, &ctrl_data);
    rtk_ldd_parameter2_set(devId, 0x387, 0x10);    

    //osal_time_mdelay(500);
    for (i=0;i<loopcnt;i++)
    {
        memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
        //The unit of RTL8290C_RX_REAL_MODE output is uA/32, for HW implement
        lddRxPwr.op_mode = RTL8290C_RX_REAL_MODE;
        ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
        if(ret)
        {
            printf("Get RTL8290C RX power fail!!! (%d)\n", ret);
            rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);            
            return -1;
        }
        //printf("%4d  %8d  %8d  %8d   %8d  %8d  %8d   %8lld\n", i, lddRxPwr.rssi_code1, lddRxPwr.rssi_code2, lddRxPwr.data_a, lddRxPwr.data_b, lddRxPwr.data_c, lddRxPwr.i_rssi, sum_i);    
        sum_i += lddRxPwr.i_rssi;
    }
        
    i_rssi = (uint32)((sum_i+loopcnt/2)/loopcnt);
    i_rssi_nA = (double)i_rssi*31.25;

    rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);    
    //printf("LSB = %d, code_200u(RSSI_K) = %d, R_6400 = %d\n", lddRxPwr.rx_lsb, lddRxPwr.rx_code_200u, lddRxPwr.rx_r6400); 
    printf("Get RTL8290C RX power I_RSSI = %f nA, (%d uA/32)\n", i_rssi_nA, i_rssi);

    return i_rssi;

}

int rtl8290c_cli_cal_rx_ddmi(
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
#ifdef RTL8290C_CAL_TIME    
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
                _8290c_cal_regression(rx_rssi[0], rx_pwr[0],
                             rx_rssi[1], rx_pwr[1],
                             rx_rssi[2], rx_pwr[2], &a, &b, &c);
                printf("    a = %f\n", a);
                printf("    b = %f\n", b);
                printf("    c = %f\n", c);
                 
                printf("---- shifting...\n");
                _8290c_cal_shift(a, &shft_a, &a);
                _8290c_cal_shift(b, &shft_b, &b);
                _8290c_cal_shift(c, &shft_c, &c);
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
                addr = RTL8290C_RX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 2, (uint32)rx_a);
                addr = RTL8290C_RX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 2, (uint32)rx_b);
                addr = RTL8290C_RX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 2, (uint32)rx_c);

                addr = RTL8290C_SHIFT_RX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 1, (uint32)shft_a);
                addr = RTL8290C_SHIFT_RX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 1, (uint32)shft_b);
                addr = RTL8290C_SHIFT_RX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 1, (uint32)shft_c);                
            }
            else
            {
                printf("Data1: Power=%6d (0.01uW)  Irssi=%6d (uA/32)\n",
                       rx_pwr[0], rx_rssi[0]);             
                printf("Data2: Power=%6d (0.01uW)  Irssi=%6d (uA/32)\n",
                       rx_pwr[1], rx_rssi[1]);           
                    
                printf("---- regression ...\n");
                _8290c_cal_lin_regression(rx_rssi[0], rx_pwr[0], rx_rssi[1], rx_pwr[1],
                                       &b, &c);
                printf("  b = %f\n", b);
                printf("  c = %f\n", c);
                        
                printf("---- shifting...\n");
                _8290c_cal_shift(b, &shft_b, &b);
                _8290c_cal_shift(c, &shft_c, &c);
                printf("    b = %f\n", b);
                printf("    c = %f\n", c);
                    
                printf("---- casting...\n");
                rx_a = shft_a = 0;      
                rx_b = (int16) b;
                rx_c = (int16) c;
                printf(" rx_b = 0x%08x     shift b = %d\n", rx_b, shft_b);
                printf(" rx_c = 0x%08x     shift c = %d\n", rx_c, shft_c);        

                // save immediately
                addr = RTL8290C_RX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 2, (uint32)rx_a);
                addr = RTL8290C_RX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 2, (uint32)rx_b);
                addr = RTL8290C_RX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 2, (uint32)rx_c);

                addr = RTL8290C_SHIFT_RX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 1, (uint32)shft_a);
                addr = RTL8290C_SHIFT_RX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 1, (uint32)shft_b);
                addr = RTL8290C_SHIFT_RX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
                _8290c_cal_flash_data_set(devId, addr, 1, (uint32)shft_c);
                
            }
        }
        else if(0 == strcmp(argv[1], "1"))
        {
            if ((argc >= 4)&&((0 == strcmp(argv[2], "dbm"))||(0 == strcmp(argv[2], "dBm"))))
            {
                printf("Set RXPWR1 data.\n"); 
                //rx_pwr[0] = _vlaue_translate(argv[2]);
                in_dBm = atof(argv[3]);
                out_pwr = __dBm2001uw(in_dBm);
                rx_pwr[0] = (uint32)out_pwr; 
                _europa_cal_data_add(EUROPA_CAL_RXPWR1_ADDR, rx_pwr[0]);                   
                rx_rssi[0] = _rtl8290c_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI1_ADDR, rx_rssi[0]);                   
                    
                printf("Data1: Power=%6d (0.01uW)  Irssi=%6d (uA/32)\n",
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
                rx_rssi[0] = _rtl8290c_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI1_ADDR, rx_rssi[0]);                   
                    
                printf("Data1: Power=%6d (0.01uW)  Irssi=%6d (uA/32)\n",
                        rx_pwr[0], rx_rssi[0]);                               
            }
            else
            {
                    printf("%s %s <rx_pwr1: Unit is dBm> \n", argv[0], argv[1]);
            }
        }
        else if(0 == strcmp(argv[1], "2"))
        {
            if ((argc >= 4)&&((0 == strcmp(argv[2], "dbm"))||(0 == strcmp(argv[2], "dBm"))))
            {
                printf("Set RXPWR2 data.\n"); 
                //rx_pwr[1] = _vlaue_translate(argv[2]);
                in_dBm = atof(argv[3]);
                out_pwr = __dBm2001uw(in_dBm);
                rx_pwr[1] = (uint32)out_pwr; 
                _europa_cal_data_add(EUROPA_CAL_RXPWR2_ADDR, rx_pwr[1]);                   
                rx_rssi[1] = _rtl8290c_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI2_ADDR, rx_rssi[1]);                    
                printf("Data2: Power=%6d (0.01uW) Irssi=%6d (uA/32)\n",
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
                rx_rssi[1] = _rtl8290c_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI2_ADDR, rx_rssi[1]);                  
                printf("Data2: Power=%6d (0.01uW) Irssi=%6d (uA/32)\n",
                       rx_pwr[1], rx_rssi[1]);                        
            }
            else
            {
                printf("%s %s <rx_pwr2: Unit is dBm> \n", argv[0], argv[1]);
            }             
        } 
        else if(0 == strcmp(argv[1], "3"))
        {
            if ((argc >= 4)&&((0 == strcmp(argv[2], "dbm"))||(0 == strcmp(argv[2], "dBm"))))
            {
                printf("Set RXPWR3 data.\n"); 
                //rx_pwr[2] = _vlaue_translate(argv[2]); 
                in_dBm = atof(argv[3]);
                out_pwr = __dBm2001uw(in_dBm);
                rx_pwr[2] = (uint32)out_pwr; 
                _europa_cal_data_add(EUROPA_CAL_RXPWR3_ADDR, rx_pwr[2]);                   
                rx_rssi[2] = _rtl8290c_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI3_ADDR, rx_rssi[2]);                  
                printf("Data3: Power=%6d (0.01uW) Irssi=%6d (uA/32)\n",
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
                rx_rssi[2] = _rtl8290c_cal_get_rssi_current(devId);
                _europa_cal_data_add(EUROPA_CAL_RXRSSI3_ADDR, rx_rssi[2]);                  
                printf("Data3: Power=%6d (0.01uW) Irssi=%6d (uA/32)\n",
                       rx_pwr[2], rx_rssi[2]);                        
            }
            else
            {
                printf("%s %s <rx_pwr3: Unit is dBm> \n", argv[0], argv[1]);
            }             
        }          
        else if(0 == strcmp(argv[1], "4"))
        {       
            if (argc > 3)
            {
                printf("Set RXPWR1 data. (Offline)\n"); 
#if 0                
                rx_pwr[0] = _vlaue_translate(argv[2]); 
                _europa_cal_data_add(EUROPA_CAL_RXPWR1_ADDR, rx_pwr[0]);
                rx_rssi[0] = _vlaue_translate(argv[3]);                 
                _europa_cal_data_add(EUROPA_CAL_RXRSSI1_ADDR, rx_rssi[0]);                                  
                printf("Data1: Power=%6d (0.01uW)  Irssi=%6d (uA/32\n",    rx_pwr[0], rx_rssi[0]);              
#endif
                
                in_dBm = atof(argv[2]);
                out_pwr = __dBm2001uw(in_dBm);
                //memcpy(&rx_pwr[0], &out_pwr, sizeof(uint32));
                //memcpy(&tmp_pwr, &rx_pwr[0], sizeof(uint32));
                rx_pwr[0] = (uint32)out_pwr; 
                _europa_cal_data_add(EUROPA_CAL_RXPWR1_ADDR, rx_pwr[0]);
                rx_rssi[0] = _vlaue_translate(argv[3]);                 
                _europa_cal_data_add(EUROPA_CAL_RXRSSI1_ADDR, rx_rssi[0]);                                  
                printf("Data1: Power=%6d (0.01uW) Irssi=%6d (uA/32)\n", rx_pwr[0], rx_rssi[0]);                               
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
#if 0                
                rx_pwr[1] = _vlaue_translate(argv[2]); 
                _europa_cal_data_add(EUROPA_CAL_RXPWR2_ADDR, rx_pwr[1]);                   
                //rx_rssi[1] = _europa_cal_get_rssi_current();
                rx_rssi[1] = _vlaue_translate(argv[3]);                     
                _europa_cal_data_add(EUROPA_CAL_RXRSSI2_ADDR, rx_rssi[1]);                  
                printf("Data2: Power=%6d (0.01uW)  Irssi=%6d (uA/32\n", rx_pwr[1], rx_rssi[1]);
#endif
                in_dBm = atof(argv[2]);
                out_pwr = __dBm2001uw(in_dBm);
                rx_pwr[1] = (uint32)out_pwr;
                _europa_cal_data_add(EUROPA_CAL_RXPWR2_ADDR, rx_pwr[1]);                   
                rx_rssi[1] = _vlaue_translate(argv[3]);                     
                _europa_cal_data_add(EUROPA_CAL_RXRSSI2_ADDR, rx_rssi[1]);                  
                printf("Data2: Power=%f (0.01uW)  Irssi=%6d (uA/32\n", (float)rx_pwr[1], rx_rssi[1]);                
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
                rx_pwr[2] = _vlaue_translate(argv[2]); 
                _europa_cal_data_add(EUROPA_CAL_RXPWR3_ADDR, rx_pwr[2]);                   
                //rx_rssi[2] = _europa_cal_get_rssi_current();
                rx_rssi[2] = _vlaue_translate(argv[3]);                     
                _europa_cal_data_add(EUROPA_CAL_RXRSSI3_ADDR, rx_rssi[2]);                  
                printf("Data3: Power=%6d (0.01uW)  Irssi=%6d (uA/32\n",
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
            _8290c_cal_regression(rx_rssi[0], rx_pwr[0],
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
            _8290c_cal_reverse(ori_A, ori_B, ori_C);
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
            _8290c_cal_regression(rx_rssi[0], rx_pwr[0],
                         rx_rssi[1], rx_pwr[1],
                         rx_rssi[2], rx_pwr[2], &a, &b, &c);
            printf("    a = %f\n", a);
            printf("    b = %f\n", b);
            printf("    c = %f\n", c);
                
            ori_A = a;
            ori_B = b;
            ori_C = c;
            
            printf("---- shifting...\n");
            _8290c_cal_shift(a, &shft_a, &a);
            _8290c_cal_shift(b, &shft_b, &b);
            _8290c_cal_shift(c, &shft_c, &c);
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
            //_8290c_cal_reverse(ori_A, ori_B, ori_C);
            //gettimeofday(&tv_tmp2,&tz);    
            //Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
            //printf("[TIME]: europa_cli_cal_rx_ddmi reversing = %f ms\n", Duration);                   
        }        
    }
    else
    {
        printf("%s <case> \n", argv[0]);
        printf("case can be 0, 1, 2, 3 and calc, input power unit is 0.01uW or dBm\n");
        printf("europacli cal rxddmi <case:1, 2, 3> <Power: unit is 0.01uW>\n");
        printf("europacli cal rxddmi <case:1, 2, 3> <dbm or dBm> <Power: unit is dBm\n");        
        return -1;
    }
#ifdef RTL8290C_CAL_TIME    
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: rtl8290c_cli_cal_rx_ddmi Duration = %f ms\n", Duration);
#endif
    
    return 0; 

}

int rtl8290c_cli_rxpower_get(uint32 devId, uint32 loopcnt)
{
    double temp_f, temp_f2, i_rssi_nA;
    uint32 rx_power;
    int ret;
    uint32 i, temp32;
    uint32 ctrl_data;    
    int rx_a, rx_b, rx_c;
    int shft_a, shft_b, shft_c;    
    rtk_ldd_rxpwr_t lddRxPwr;
    uint64 sum_i;
    uint32 i_rssi;    

    
    if (100 < loopcnt)
    {
       printf("Loop Count should less than 100. \n");
       return -1;
    }
    _8290c_cal_flash_data_get(devId, RTL8290C_RX_A_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 2, &temp32);
    rx_a = (int16)temp32;    
    _8290c_cal_flash_data_get(devId, RTL8290C_RX_B_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 2, &temp32);
    rx_b = (int16)temp32;        
    _8290c_cal_flash_data_get(devId, RTL8290C_RX_C_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 2, &temp32);
    rx_c = (int16)temp32;        
    _8290c_cal_flash_data_get(devId, RTL8290C_SHIFT_RX_A_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 1, &shft_a);       
    _8290c_cal_flash_data_get(devId, RTL8290C_SHIFT_RX_B_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 1, &shft_b);  
    _8290c_cal_flash_data_get(devId, RTL8290C_SHIFT_RX_C_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 1, &shft_c); 

    printf("RTL8290C RX_A = %d, SHIFT_RX_A = %d\n", rx_a, shft_a);         
    printf("RTL8290C RX_B = %d, SHIFT_RX_B = %d\n", rx_b, shft_b);   
    printf("RTL8290C RX_C = %d, SHIFT_RX_C = %d\n", rx_c, shft_c);

    rtk_ldd_parameter2_get(devId, 0x387, &ctrl_data);
    rtk_ldd_parameter2_set(devId, 0x387, 0x10);

    //printf("LSB = 4000000, code_250u = 1273, R6400 = 6400\n"); 

    sum_i = 0;
    printf("idx   RSSI_FL  RSSIFL_SEL  data_a   data_b   data_c   I-RSSI   Sum\n"); 
    for (i=0;i<loopcnt;i++)
    {
        memset(&lddRxPwr, 0, sizeof(rtk_ldd_rxpwr_t));
        //The unit of RTL8290C_RX_REAL_MODE output is uA/32, for HW implement
        lddRxPwr.op_mode = RTL8290C_RX_REAL_MODE;
        //lddRxPwr.rx_lsb = 4000000;
        //lddRxPwr.rx_code_200u = 1273;
        //lddRxPwr.rx_r6400 = 6400;
        ret = rtk_ldd_rx_power2_get(devId, &lddRxPwr);
        if(ret)
        {
            printf("Get RTL8290C RX power fail!!! (%d)\n", ret);
            rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);            
            return -1;
        }
        sum_i += lddRxPwr.i_rssi;        
        //printf("%4d  %8d  %8d  %8d\n", i, lddRxPwr.data_a, lddRxPwr.data_b, lddRxPwr.i_rssi); 
        printf("%4d  %8d  %8d  %8d   %8d  %8d  %8d  %12lld\n", i, lddRxPwr.rssi_code1, lddRxPwr.rssi_code2, lddRxPwr.data_a, lddRxPwr.data_b, lddRxPwr.data_c, lddRxPwr.i_rssi, sum_i);           
    }

    rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);

    printf("LSB = %d, code_200u(RSSI_K) = %d, R_6400 = %d\n", lddRxPwr.rx_lsb, lddRxPwr.rx_code_200u, lddRxPwr.rx_r6400); 

    i_rssi = (sum_i+loopcnt/2)/loopcnt;
    i_rssi_nA = (double)i_rssi*31.25;

    printf("Get RTL8290C RX power RSSI Current  = %f nA (i_rssi = %d)\n", i_rssi_nA, i_rssi);

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
    printf("RX RTL8290C power = %d 0.01uW (%f dBm)\n", rx_power, temp_f2); 
    
    return 0;    
}


uint32 _rtl8290c_cal_get_mpd_current(uint32 devId)
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
        lddTxPwr.op_mode = RTL8290C_TX_REAL_MODE;
        //lddTxPwr.tx_lsb = 4000000;
        //lddTxPwr.tx_code_250u = 2615;
        //lddTxPwr.tx_r_mpd = 500;        
        ret = rtk_ldd_tx_power2_get(devId, &lddTxPwr);
        if(ret)
        {
            printf("Get RTL8290CMPD current fail!!! (%d)\n", ret);
            return -1;
        }
        sum += lddTxPwr.i_mpd;
    }
    i_mpd = (sum+loopcnt/2)/loopcnt;

    return i_mpd; /* unit is uA/32 */
}


int rtl8290c_cli_cal_tx_ddmi(
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
#ifdef RTL8290C_CAL_TIME
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
            //if (ret!=0)
            //{
            //    free(init_data);
            //    fclose(fp);
            //    return ret;
            //}
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
      
            printf("Data1: Power=%6d 0.01uW Impd=%6d (uA/32\n",
                tx_pwr[0], tx_mpd[0]);
            printf("Data2: Power=%6d 0.01W Impd=%6d (uA/32)\n",
                tx_pwr[1], tx_mpd[1]); 
        
            printf("Iavg = 0x%x\n", tx_iavg);             
        
            printf("---- regression ...\n");
               _8290c_cal_lin_regression(tx_mpd[0], tx_pwr[0], tx_mpd[1], tx_pwr[1],
                       &a, &b);
            printf("  a = %f\n", a);
            printf("  b = %f\n", b);
        
            printf("---- shifting...\n");
            _8290c_cal_shift(a, &shft_a, &a);
            _8290c_cal_shift(b, &shft_b, &b);;
            printf("    a = %f\n", a);
            printf("    b = %f\n", b);

            printf("---- casting...\n");
            tx_a = (int16) a;
            tx_b = (int16) b;
            printf(" tx_a = 0x%08x       shift a = %d\n", tx_a, shft_a);
            printf(" tx_b = 0x%08x       shift b = %d\n", tx_b, shft_b);
      
            // save immediately
            addr = RTL8290C_TX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
            _8290c_cal_flash_data_set(devId, addr, 2, tx_a);
            addr = RTL8290C_TX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
            _8290c_cal_flash_data_set(devId, addr, 2, tx_b);

            addr = RTL8290C_SHIFT_TX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
            _8290c_cal_flash_data_set(devId, addr, 1, (uint32)shft_a);
            addr = RTL8290C_SHIFT_TX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
            _8290c_cal_flash_data_set(devId, addr, 1, (uint32)shft_b);

#if 0        
                        printf("TXPWR1=%sn \n",getenv("TXPWR1"));
                        printf("TXPWR2=%sn \n",getenv("TXPWR2"));
                        printf("TXMPD1=%sn \n",getenv("TXMPD1"));
                        printf("TXMPD2=%sn \n",getenv("TXMPD2"));
#endif
        }
        else if(0 == strcmp(argv[1], "1"))
        {      
            if ((argc >= 4)&&((0 == strcmp(argv[2], "dbm"))||(0 == strcmp(argv[2], "dBm"))))
            {
                printf("Set TXPWR1 data.\n"); 
                in_dBm = atof(argv[3]);
                out_pwr = __dBm2001uw(in_dBm);
                tx_pwr[0] = (uint32)out_pwr; 
                //tx_pwr[0] = _vlaue_translate(argv[2]);        
                _europa_cal_data_add(EUROPA_CAL_TXPWR1_ADDR, tx_pwr[0]);                       
                tx_mpd[0] = _rtl8290c_cal_get_mpd_current(devId);
                _europa_cal_data_add(EUROPA_CAL_TXMPD1_ADDR, tx_mpd[0]);           
                printf("Data1: Power=%6d (0.01uW)  Impd=%6d (uA/32)\n",
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
                //out_pwr = __dBm2001uw(in_dBm);
                //tx_pwr[0] = (uint32)out_pwr; 
                tx_pwr[0] = _vlaue_translate(argv[2]);        
                _europa_cal_data_add(EUROPA_CAL_TXPWR1_ADDR, tx_pwr[0]);                       
                tx_mpd[0] = _rtl8290c_cal_get_mpd_current(devId);
                _europa_cal_data_add(EUROPA_CAL_TXMPD1_ADDR, tx_mpd[0]);           
                printf("Data1: Power=%6d (0.01uW)  Impd=%6d (uA/32)\n",
                   tx_pwr[0], tx_mpd[0]);

                rtk_ldd_apcIavg2_get(devId, &tx_iavg);
                _europa_cal_data_add(EUROPA_CAL_TX_IAVG_ADDR, tx_iavg); 
                printf("Iavg power = 0x%x\n", tx_iavg);                                  
            }            
            else
            {
                printf("%s %s <tx_pwr1: Unit is dBm> \n", argv[0], argv[1]);
            }
        }
        else if(0 == strcmp(argv[1], "2"))
        {
            if ((argc >= 4)&&((0 == strcmp(argv[2], "dbm"))||(0 == strcmp(argv[2], "dBm"))))
            {
                printf("Set TXPWR2 data.\n"); 
                in_dBm = atof(argv[3]);
                out_pwr = __dBm2001uw(in_dBm);
                tx_pwr[1] = (uint32)out_pwr; 
                //tx_pwr[1] = _vlaue_translate(argv[2]);        
                _europa_cal_data_add(EUROPA_CAL_TXPWR2_ADDR, tx_pwr[1]);                       
                tx_mpd[1] = _rtl8290c_cal_get_mpd_current(devId);
                _europa_cal_data_add(EUROPA_CAL_TXMPD2_ADDR, tx_mpd[1]);           
                printf("Data2: Power=%6d (0.01uW)  Impd=%6d (uA/32)\n",
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
                //out_pwr = __dBm2001uw(in_dBm);
                //tx_pwr[1] = (uint32)out_pwr; 
                tx_pwr[1] = _vlaue_translate(argv[2]);        
                _europa_cal_data_add(EUROPA_CAL_TXPWR2_ADDR, tx_pwr[1]);                       
                tx_mpd[1] = _rtl8290c_cal_get_mpd_current(devId);
                _europa_cal_data_add(EUROPA_CAL_TXMPD2_ADDR, tx_mpd[1]);           
                printf("Data2: Power=%6d (0.01uW)  Impd=%6d (uA/32)\n",
                   tx_pwr[1], tx_mpd[1]);               
            }            
            else
            {
                printf("%s %s <tx_pwr2: Unit is dBm> \n", argv[0], argv[1]);
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
                printf("Data2: Power=%6d (0.01uW)  Impd=%6d (uA/32)\n",
                           tx_pwr[1], tx_mpd[1]);
                             
            }
            else
            {
                printf("%s %s <tx_pwr2: Unit is 0.01uW> \n", argv[0], argv[1]);
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

#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: rtl8290c_cli_cal_tx_ddmi Duration = %f ms\n", Duration);
#endif

    return 0;    
}

int rtl8290c_cli_txpower_get(uint32 devId, uint32 loopcnt, uint32 r_mpd, uint32 code_250u, uint32 i_cal)
{

    int ret;
    int tx_a, tx_b, tx_c;
    int shft_a, shft_b, shft_c;        
    uint32 i_mpd, temp32;
    int32 i;
    uint64 sum_mpd;
    int32 tx_power;
    uint32 parm;
    double temp_f, i_mpd_nA, temp_f2;
    uint32 ctrl_data;
    rtk_ldd_txpwr_t lddTxPwr;

    //osal_time_mdelay(500);    
    _8290c_cal_flash_data_get(devId, RTL8290C_TX_A_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 2, &temp32);
    tx_a = (int16)temp32;
    _8290c_cal_flash_data_get(devId, RTL8290C_TX_B_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 2, &temp32);
    tx_b = (int16)temp32;    
    _8290c_cal_flash_data_get(devId, RTL8290C_TX_C_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 2, &temp32);
    tx_c = (int16)temp32;
    _8290c_cal_flash_data_get(devId, RTL8290C_SHIFT_TX_A_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 1, &shft_a);
    _8290c_cal_flash_data_get(devId, RTL8290C_SHIFT_TX_B_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 1, &shft_b);
    _8290c_cal_flash_data_get(devId, RTL8290C_SHIFT_TX_C_ADDR+RTL8290C_PARAMETER_BASE_ADDR, 1, &shft_c);
    printf("RTL8290C TX_A = %d, SHIFT_TX_A = %d\n", tx_a, shft_a);         
    printf("RTL8290C TX_B = %d, SHIFT_TX_B = %d\n", tx_b, shft_b);   
    printf("RTL8290C TX_C = %d, SHIFT_TX_C = %d\n", tx_c, shft_c);

    printf("Start to get RTL8290C TX Power ... \n");

    rtk_ldd_parameter2_get(devId, 0x387, &ctrl_data);
    rtk_ldd_parameter2_set(devId, 0x387, 0x10);

    printf("idx   TXAPC  TXAPC_RSEL     Ical     I_250u   I_mpd   Impd(final)\n");     
    sum_mpd = 0;
    for (i=0;i<loopcnt;i++)
    {
        //printf("Loop %d: \n", i);
        memset(&lddTxPwr, 0, sizeof(rtk_ldd_txpwr_t));  
        lddTxPwr.op_mode = RTL8290C_TX_REAL_MODE;
        //lddTxPwr.tx_lsb = 4000000;
        //lddTxPwr.tx_code_250u = 2615;
        //lddTxPwr.tx_r_mpd = 500;
        lddTxPwr.tx_code_250u = code_250u;
        lddTxPwr.tx_r_mpd = r_mpd;
        lddTxPwr.i_cal = i_cal;
        ret = rtk_ldd_tx_power2_get(devId, &lddTxPwr);
        if(ret)
        {
            printf("Get RTL8290C TX power fail!!! (%d)\n", ret);
            rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);                
            return -1;
        }
        sum_mpd += lddTxPwr.i_mpd;
        printf("%4d  %8d  %8d  %8d  %8d  %8d  %8d\n", i, lddTxPwr.rssi_code1, lddTxPwr.rssi_code2, lddTxPwr.data_a, lddTxPwr.data_b, lddTxPwr.data_c, lddTxPwr.i_mpd);     
    }
    
    printf("LSB = %d, code_250u(MPD_K) = %d, R_MPD = %d, I_cal = %d\n", lddTxPwr.tx_lsb, lddTxPwr.tx_code_250u, lddTxPwr.tx_r_mpd, lddTxPwr.i_cal); 
    
    i_mpd = (sum_mpd+loopcnt/2)/loopcnt;
    i_mpd_nA = (double)i_mpd*31.25;
     
    rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);

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
    
     //20230809: Change unit from 0.1uW to  0.01uW        
    tx_power = (0.0<temp_f) ? (uint32)(temp_f) : 0; /* 0.01 uW */
             
    temp_f2  = __log10((double)(tx_power)*1/100000)*10;                                   
    printf("Get RTL8290C TX power I_MPD = %f nA (%d uA/32)\n", i_mpd_nA, i_mpd);
    printf("TX Power = %d 0.01uW (%f dBm)\n", tx_power, temp_f2);
     
    return 0;    
}

int rtl8290c_cli_rxparam_set(
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
        addr = RTL8290C_RX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 2;
        printf("RX parameter a = %d\n",  para);        
    }
    else if(0 == strcmp(argv[1], "b")) 
    {
        addr = RTL8290C_RX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 2;
        printf("RX parameter b = %d\n",  para);                
    }
    else if(0 == strcmp(argv[1], "c")) 
    {
        addr = RTL8290C_RX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 2;
        printf("RX parameter c = %d\n",  para);        
    }
    else if(0 == strcmp(argv[1], "sha")) 
    {
        addr = RTL8290C_SHIFT_RX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 1;
        printf("RX parameter shift a = %d\n",  para);        
    }
    else if(0 == strcmp(argv[1], "shb")) 
    {
        addr = RTL8290C_SHIFT_RX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 1;
        printf("RX parameter shift b = %d\n",  para);                
    }
    else if(0 == strcmp(argv[1], "shc")) 
    {
        addr = RTL8290C_SHIFT_RX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        length = 1;
        printf("RX parameter shift c = %d\n",  para);        
    }
    else
    {
        printf("no such mode %s\n", argv[1]);
        return -1;
    }

    _8290c_cal_flash_data_set(devId, addr, length, para);

    return 0;
}  

int rtl8290c_cli_rxparam_get(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, para1;

    addr = RTL8290C_RX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 2, &para1);
    printf("RX parameter a: flash = %d\n",  para1);
    addr = RTL8290C_RX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 2, &para1);
    printf("RX parameter b: flash = %d\n", para1);
    addr = RTL8290C_RX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 2, &para1);
    printf("RX parameter c: flash = %d\n", para1);

    addr = RTL8290C_SHIFT_RX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 1, &para1);
    printf("RX parameter shift a: flash = %d\n",  para1);
    addr = RTL8290C_SHIFT_RX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 1, &para1);
    printf("RX parameter shift b: flash = %d\n", para1);
    addr = RTL8290C_SHIFT_RX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 1, &para1);
    printf("RX parameter shift c: flash = %d\n", para1);

    return 0;
}    /* end of cparser_cmd_debug_europa_get_rx_param */

int rtl8290c_cli_txparam_set(
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
        addr = RTL8290C_TX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter a = %d\n",  para);
        _8290c_cal_flash_data_set(devId, addr, 2, para);
    }
    else if(0 == strcmp(argv[1], "b")) 
    {
        addr = RTL8290C_TX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter b = %d\n",  para);
        _8290c_cal_flash_data_set(devId, addr, 2, para);
    }
    else if(0 == strcmp(argv[1], "c")) 
    {
        addr = RTL8290C_TX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter c = %d\n",  para);
        _8290c_cal_flash_data_set(devId, addr, 2, para);
    }
    else if(0 == strcmp(argv[1], "sha")) 
    {
        addr = RTL8290C_SHIFT_TX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter shift a = %d\n",  para);
        _8290c_cal_flash_data_set(devId, addr, 1, para);
    }
    else if(0 == strcmp(argv[1], "shb")) 
    {
        addr = RTL8290C_SHIFT_TX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter shift b = %d\n",  para); 
        _8290c_cal_flash_data_set(devId, addr, 1, para);
    }
    else if(0 == strcmp(argv[1], "shc")) 
    {
        addr = RTL8290C_SHIFT_TX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
        para = _vlaue_translate(argv[2]);
        printf("TX parameter shift c = %d\n",  para); 
        _8290c_cal_flash_data_set(devId, addr, 1, para);
    }
    else
    {
        printf("no such mode %s\n", argv[1]);
        return -1;
    }

    return 0;

}  

int rtl8290c_cli_txparam_get(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, para1;

    addr = RTL8290C_TX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 2, &para1);
    printf("TX parameter a: flash = %d\n",  para1);
    addr = RTL8290C_TX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 2, &para1);
    printf("TX parameter b: flash = %d\n", para1);
    addr = RTL8290C_TX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 2, &para1);
    printf("TX parameter c: flash = %d\n", para1);

    addr = RTL8290C_SHIFT_TX_A_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 1, &para1);
    printf("TX parameter shift a: flash = %d\n",  para1);
    addr = RTL8290C_SHIFT_TX_B_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 1, &para1);
    printf("TX parameter shift b: flash = %d\n", para1);
    addr = RTL8290C_SHIFT_TX_C_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 1, &para1);
    printf("TX parameter shift c: flash = %d\n", para1);

    return 0;
} 

int rtl8290c_cli_cal_power(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;      
    int ret;
    double measured_pwr, target_pwr;
    double measured_uW, target_uW;
    double current_iavg, target_iavg, w4b_iavg, w4b_target;
    uint32 data_w4b_original, data_w4b_target, data_temp;
#ifdef RTL8290C_CAL_TIME
        struct      timeval     tv_tmp1, tv_tmp2;
        struct      timezone     tz;
        double Duration; 
        
        gettimeofday(&tv_tmp1,&tz); 
#endif

    if(argc < 2)
    {
        addr = RTL8290C_APC_IAVG_SET_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value);  
        printf("Get RTL8290C iavg power = 0x%x\n", value);   
        rtk_ldd_apcIavg2_get(devId, &value); 
        printf("Current RTL8290C iavg power = 0x%x\n", value);          
        //printf("%s auto <measured pwr> <target pwr> \n", argv[0]);   
        //printf("Or %s <value>\n", argv[0]);         
        //return -1;
    }
    else if (argc == 2)
    {
        if ((ret=strcmp(argv[1], "auto")) != 0 )
        { 
            value = _vlaue_translate(argv[1]);   
            addr = RTL8290C_APC_IAVG_SET_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
            _8290c_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8290C iavg power = 0x%x\n", value);     
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
            //RTL8290C_WB_REG4B_ADDR = RTL8290B_W58
            rtk_ldd_parameter2_get(devId, 0x2cb, &data_w4b_original);    
            //printf("W4B= 0x%x \n", data_w4b_original);        
            w4b_iavg = (double)data_w4b_original/32;
            current_iavg = 25*pow(2,w4b_iavg);
            //printf("current_iavg = %lf \n", current_iavg);        
            measured_uW = __dBm2uw(measured_pwr);
            target_uW = __dBm2uw(target_pwr);    
            //printf("measured pwr = %lf uW,    target pwr = %lf uW  \n", measured_uW, target_uW); 
            target_iavg = current_iavg*(target_uW/measured_uW);
            //printf("target_iavg = %lf \n", target_iavg);  
            w4b_target = 32*log10(target_iavg/25)/log10(2);
            
            data_w4b_target = (uint32)w4b_target;
            
            printf("taget W58 = 0x%x\n", data_w4b_target); 
            
            if ((data_w4b_target < 0x20)||(data_w4b_target > 0xC0))
            {    
                printf("Overflow! Taget value should be 0x20~0xC0.\n");     
            }
            else
            {
                if (data_w4b_target<(data_w4b_target-0x10))
                {
                   //To avoid DCL lost lock, use while loop to do step-by-step.
                   data_temp = data_w4b_target; 
                   while((data_temp-0x10)>data_w4b_target)
                   {
                       data_temp-=0x10;
                       //europa_i2c_write(EUROPA_I2C_PORT, 0x54, 58, data_temp);
                       rtk_ldd_parameter2_set(devId, 0x2cb, data_temp);   

                       //printf("data_temp = 0x%x\n", data_temp);                            
                       osal_time_mdelay(20);    
                   }           
                }
                //europa_i2c_write(EUROPA_I2C_PORT, 0x54, 58, data_w58_target); 
                rtk_ldd_parameter2_set(devId, 0x2cb, data_w4b_target);  
                addr = RTL8290C_APC_IAVG_SET_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
                _8290c_cal_flash_data_set(devId, addr, 1, data_w4b_target);
                printf("Set iavg power = 0x%x\n", data_w4b_target);                    
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

#ifdef RTL8290C_CAL_TIME
        gettimeofday(&tv_tmp2,&tz);    
        Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
        printf("[TIME]: rtl8290c_cli_cal_power Duration = %f ms\n", Duration);
#endif

    return 0; 
}

int rtl8290c_cli_cal_er(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret=0;
    uint32 addr, value, value2; 
    uint32 er_idx=0, trim_idx=0, i, er_fin, er_trim_fin;
    float measured_er, target_er;
    float er_meas, er_targ, oma_meas, oma_targ, oma_k;
    float er_factor, lut_factor, er_factor_k;
    float er_trim_factor, er_trim_factor_k;
    float P1_k, P0_k, ER_k, ER_k_dB;
    float P1_k2, P0_k2, ER_k2, ER_k2_dB, oma_k2;
#ifdef RTL8290C_CAL_TIME
    struct      timeval     tv_tmp1, tv_tmp2;
    struct      timezone     tz;
    double Duration; 
            
    gettimeofday(&tv_tmp1,&tz); 
#endif

    //printf("argc = %d argv[0] = %s argv[1] = %s \n", argc, argv[0], argv[1]);  
#if 1
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
        rtk_ldd_apcErTrim2_get(devId, &value2);
        er_idx = value;
        trim_idx = value2;
        printf("Current ER = 0x%x TRIM = 0x%x\n", value, value2);   
    
        //Step 1~2
        //printf("Step0: Input:ER = %d measured_er = %f (dB) target_er = %f (dB)\n", value, measured_er, target_er);
        er_targ = pow(10,target_er/10);        
        er_meas = pow(10,measured_er/10);
        oma_targ = 2*(er_targ-1)/(er_targ+1);
        oma_meas = 2*(er_meas-1)/(er_meas+1);
        //printf("Step1: measured_er = %f OMA_measure = %f target_er = %f OMA_target = %f\n", er_meas, oma_meas, er_targ, oma_targ);

        //Step 3~4
        er_factor = oma_targ/oma_meas;
        //printf("Step2: er_idx = 0x%x er_factor = %f \n", er_idx, er_factor);
        for (i=0;i<16;i++)
        {
            er_fin = i;
            lut_factor = rtl8290c_er_factor[er_idx][i];
            er_factor_k = lut_factor;
            //printf("idx = %d, er_factor_k = %f \n", i, er_factor_k);            
            if (lut_factor>er_factor)
            {
                break;
            }
        }
        //printf("Result: er_fin = %d, er_factor_k = %f \n", er_fin, er_factor_k);    

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
            lut_factor = rtl8290c_er_trim_factor[trim_idx][i];
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

        addr = RTL8290C_APC_ER_SET_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, er_fin);
        //printf("Set RTL8290C er = 0x%x\n", er_fin);   
        rtk_ldd_apcEr2_set(devId, er_fin);

        addr = RTL8290C_APC_ER_TRIM_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, er_trim_fin);    
        //printf("Set RTL8290C trim = 0x%x\n", er_trim_fin);      
        rtk_ldd_apcErTrim2_set(devId, er_trim_fin);
        printf("Set RTL8290C er = 0x%x  trim = 0x%x\n", er_fin, er_trim_fin);  

    }
    else if(argc >= 3)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value>0xF)
        {
            printf("Input ER should be 0~0xF\n");       
            return -1;
        }
        value2 = _vlaue_translate(argv[2]);   
        if (value2>0x1F)
        {
            printf("Input TRIM should be 0~0x1F\n");       
            return -1;
        }

        addr = RTL8290C_APC_ER_SET_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, value);  
        rtk_ldd_apcEr2_set(devId, value);

        addr = RTL8290C_APC_ER_TRIM_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, value2);   
        rtk_ldd_apcErTrim2_set(devId, value2);
        
        printf("Set RTL8290C ER = 0x%x TRIM = 0x%x\n", value, value2);   
    }
    else if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value>0xF)
        {
            printf("Input should be 0~0xF\n");       
            return -1;
        }
        addr = RTL8290C_APC_ER_SET_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, value);
        printf("Set RTL8290C er = 0x%x\n", value);   
        rtk_ldd_apcEr2_set(devId, value);         
    }    
    else
#endif
    { 
        addr = RTL8290C_APC_ER_SET_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value); 
        addr = RTL8290C_APC_ER_TRIM_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value2);  
        printf("RTL8290C default ER = 0x%x TRIM = 0x%x\n", value, value2);
        rtk_ldd_apcEr2_get(devId, &value); 
        rtk_ldd_apcErTrim2_get(devId, &value2); 
        printf("Current ER = 0x%x TRIM = 0x%x\n", value, value2);        
    }
    
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]: rtl8290c_cli_cal_er Duration = %f ms\n", Duration);
#endif

    return 0; 
}

int rtl8290c_cli_cal_erc_comp(
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
            addr = RTL8290C_ERC_COMP_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
            _8290c_cal_flash_data_get(devId, addr, 1, &value); 
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
                addr = RTL8290C_ERC_COMP_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
                _8290c_cal_flash_data_set(devId, addr, 1, value);
    
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
            
                addr = RTL8290C_ERC_COMP_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
                _8290c_cal_flash_data_set(devId, addr, 1, step);
    
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


int rtl8290c_cli_cal_trim(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value; 
 #ifdef RTL8290C_CAL_TIME
    struct      timeval     tv_tmp1, tv_tmp2;
    struct      timezone     tz;
    double Duration; 
        
    gettimeofday(&tv_tmp1,&tz); 
#endif

    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value>0x1F)
        {
            printf("Input should be 0~0x1F\n");       
            return -1;
        }
        addr = RTL8290C_APC_ER_TRIM_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, value);
        printf("Set RTL8290C trim = 0x%x\n", value);      
        rtk_ldd_apcErTrim2_set(devId, value);                           
    }
    else
    { 
        addr = RTL8290C_APC_ER_TRIM_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8290C trim = 0x%x\n", value);
        rtk_ldd_apcErTrim2_get(devId, &value); 
        printf("Current trim = 0x%x\n", value);        
    }
    
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_trim Duration = %f ms\n", Duration);
#endif

    return 0; 
}

int rtl8290c_cli_cal_los(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;
    uint32 data;
#ifdef RTL8290C_CAL_TIME
    struct        timeval     tv_tmp1, tv_tmp2;
    struct        timezone     tz;
    double Duration; 
            
    gettimeofday(&tv_tmp1,&tz); 
#endif

    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        addr = RTL8290C_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, value);
        printf("Set RTL8290C los = 0x%x\n", value);     
        rtk_ldd_rxlosRefDac2_set(devId, (value&0x7F));        
    }
    else
    { 
        addr = RTL8290C_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8290C los = 0x%x\n", value);
        rtk_ldd_rxlosRefDac2_get(devId, &data);
        printf("Current los = 0x%x\n", data);
    }
    
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_los Duration = %f ms\n", Duration);
#endif 

    return 0; 
}

int rtl8290c_cli_cal_hyst(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value;
#ifdef RTL8290C_CAL_TIME
    struct        timeval     tv_tmp1, tv_tmp2;
    struct        timezone     tz;
    double Duration; 
                
    gettimeofday(&tv_tmp1,&tz); 
#endif
       
    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        addr = RTL8290C_RXLOS_HYST_SEL_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, value);
        printf("Set hyst = 0x%x\n", value);   
        rtk_ldd_rxlosHystSel2_set(devId, value);                 
    }
    else
    { 
        addr = RTL8290C_RXLOS_HYST_SEL_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8290C hyst = 0x%x\n", value); 
        rtk_ldd_rxlosHystSel2_get(devId, &value); 
        printf("Current hyst = 0x%x\n", value); 

    }
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_hyst Duration = %f ms\n", Duration);
#endif 
  
    return 0; 
}

int rtl8290c_cli_cal_cmpd(
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
#ifdef RTL8290C_CAL_TIME
    struct        timeval     tv_tmp1, tv_tmp2;
    struct        timezone     tz;
    double Duration; 
                
    gettimeofday(&tv_tmp1,&tz); 
#endif

    if(argc >= 2)
    { 
#if 1
        if((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != 0)
        {
            return ret;
        }
    
        if(0 == strcmp(argv[1], "check"))
        {  
            _rtl8290c_ibim_low(devId);
    
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

            addr = RTL8290C_APC_CMPD_OFFSET_ADDR + RTL8290B_PARAM_ADDR_BASE;    
            _8290c_cal_flash_data_set(devId, addr, 1, cmpd);            
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

            _rtl8290c_ibim_recover(devId);  
            
        }
        else
        {    
            value = _vlaue_translate(argv[1]); 
            if (value > 0x7)
            {
                printf("Value = 0x%x > 0x7\n", value);
                return -1; 
            }
            
            addr = RTL8290C_APC_CMPD_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
            _8290c_cal_flash_data_set(devId, addr, 1, value);
            rtk_ldd_parameter2_get(devId, 0x2cd, &value2);    
            value2 = (value2&0xF8)|value;
            rtk_ldd_parameter2_set(devId, 0x2cd, value2);             
            printf("Set RTL8290C cmpd = 0x%x (0x%x)\n", value, value2);
        }
#endif
    }
    else
    { 
        addr = RTL8290C_APC_CMPD_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8290C cmpd = 0x%x\n", value);
        rtk_ldd_parameter2_get(devId, 0x2cd, &value2);     
        printf("Current cmpd = 0x%x (Total = 0x%x)\n", value2&0x7, value2);    
    }
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_cmpd Duration = %f ms\n", Duration);
#endif 
     
    return 0; 
}

int rtl8290c_cli_cal_lpf(
    uint32 devId,    
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2;
#ifdef RTL8290C_CAL_TIME
    struct        timeval     tv_tmp1, tv_tmp2;
    struct        timezone     tz;
    double Duration; 
                
    gettimeofday(&tv_tmp1,&tz); 
#endif

    if(argc >= 2)
    { 
        value = _vlaue_translate(argv[1]);   
        if (value > 0x3)
        {
            printf("Value = 0x%x > 0x3\n", value);
            return -1; 
        }
    
        addr = RTL8290C_APC_LPF_BW_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, value);
        rtk_ldd_parameter2_get(devId, 0x2c3, &value2);  
        value2 = (value2&0x1F)|(value<<5);
        rtk_ldd_parameter2_set(devId, 0x2c3, value2);            
        printf("Set RTL8290C LPF = 0x%x (0x%x)\n", value, value2);                         
    }
    else
    { 
        addr = RTL8290C_APC_LPF_BW_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8290C LPF = 0x%x\n", value);
        rtk_ldd_parameter2_get(devId, 0x2c3, &value2);  
        value = (value2&0xE0)>>5;          
        printf("Current LPF = 0x%x (Total = 0x%x)\n", value, value2);    
    }
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_lpf Duration = %f ms\n", Duration);
#endif 
    
    return 0; 
}

int rtl8290c_cli_cal_cross(
    uint32 devId, 
    int argc,
    char *argv[],
    FILE *fp)
{
    uint32 addr, value, value2, value3;      
    uint32 data1, data2;
    rtk_enable_t enable;
#ifdef RTL8290C_CAL_TIME
    struct        timeval     tv_tmp1, tv_tmp2;
    struct        timezone     tz;
    double Duration; 
                
    gettimeofday(&tv_tmp1,&tz); 
#endif

    if(argc >= 2)
    { 
        if(0 == strcmp(argv[1], "show"))
        {
            addr = RTL8290C_TX_CROSS_EN_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
            _8290c_cal_flash_data_get(devId, addr, 1, &value); 
            printf("RTL8290C tx cross enable = 0x%x\n", value);
            addr = RTL8290C_TX_CROSS_SIGN_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
            _8290c_cal_flash_data_get(devId, addr, 1, &value); 
            printf("RTL8290C tx cross sign = 0x%x\n", value);
            addr = RTL8290C_TX_CROSS_STR_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
            _8290c_cal_flash_data_get(devId, addr, 1, &value); 
            printf("RTL8290C tx cross str = 0x%x\n", value);
            rtk_ldd_txCross2_get(devId, &enable, &data1, &data2);              
            printf("RTL8290C Current tx cross enable = 0x%x, sign = 0x%x, str = 0x%x\n", enable, data1, data2);            
        }
        else if(0 == strcmp(argv[1], "disable"))
        {        
            value = 0;     
            addr = RTL8290C_TX_CROSS_EN_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
            _8290c_cal_flash_data_set(devId, addr, 1, value);
            printf("Set RTL8290C TX cross disable\n");
            rtk_ldd_txCross2_get(devId, &value, &value2, &value3);    
            rtk_ldd_txCross2_set(devId, 0, value2, value3);    
        }
        else if(0 == strcmp(argv[1], "up"))
        {
            if(argc >= 3) 
            {
                value = 0;
                addr = RTL8290C_TX_CROSS_SIGN_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
                _8290c_cal_flash_data_set(devId, addr, 1, value); 
                printf("RTL8290C tx cross sign = 0x%x\n", value);
                value = 1;          
                addr = RTL8290C_TX_CROSS_EN_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
                _8290c_cal_flash_data_set(devId, addr, 1, value); 
                printf("RTL8290C tx cross enable = 0x%x\n", value);
                value = _vlaue_translate(argv[2]);           
                addr = RTL8290C_TX_CROSS_STR_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
                _8290c_cal_flash_data_set(devId, addr, 1, value); 
                printf("RTL8290C tx cross str = 0x%x\n", value);  
                rtk_ldd_txCross2_set(devId, 1, 0, value);            
            }
            else
            {
                printf("%s <up> <value>\n", argv[0]);
                return -1; 
            } 
            
        }
        else if(0 == strcmp(argv[1], "down"))
        {        
            if(argc >= 3) 
            {
                value = 1;
                addr = RTL8290C_TX_CROSS_SIGN_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
                _8290c_cal_flash_data_set(devId, addr, 1, value); 
                printf("RTL8290C tx cross sign = 0x%x\n", value);
                value = 1;          
                addr = RTL8290C_TX_CROSS_EN_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
                _8290c_cal_flash_data_set(devId, addr, 1, value); 
                printf("RTL8290C tx cross enable = 0x%x\n", value);
                value = _vlaue_translate(argv[2]);           
                addr = RTL8290C_TX_CROSS_STR_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
                _8290c_cal_flash_data_set(devId, addr, 1, value); 
                printf("RTL8290C tx cross str = 0x%x\n", value);  
                rtk_ldd_txCross2_set(devId, 1, 1, value);                    
            }
            else
            {
                printf("%s <down> <value>\n", argv[0]);
                return -1; 
            }

        }
        else
        {
            printf("%s <up/down> <value>\n", argv[0]);
            printf("%s <show/enable>\n", argv[0]);
            return -1;  
        }  
       
    }
    else
    { 

        printf("%s <up/down> <value>\n", argv[0]);
        printf("%s <show/enable>\n", argv[0]);
        return -1; 
    }
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_cross Duration = %f ms\n", Duration);
#endif 

    return 0; 
}

int rtl8290c_cli_cal_toffset(
    uint32 devId, 
    int argc,
    char *argv[],
    FILE *fp)
{
    int32 ret, temp;
    int32 offset;   
    uint32 addr;
    int8 tmp_offset;
#ifdef RTL8290C_CAL_TIME
    struct        timeval     tv_tmp1, tv_tmp2;
    struct        timezone     tz;
    double Duration; 
                
    gettimeofday(&tv_tmp1,&tz); 
#endif

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
        addr = RTL8290C_TEMP_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, offset&0xFF);         
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
        
        addr = RTL8290C_TEMP_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, offset&0xFF);          

    }
    else
    {
        addr = RTL8290C_TEMP_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;      
        _8290c_cal_flash_data_get(devId, addr, 1, &offset); 
        tmp_offset = offset&0xFF;

        printf("Temperature offset = %d now\n", (int)tmp_offset);      
        return 0;
    }
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_toffset Duration = %f ms\n", Duration);
#endif 

    return 0;    
}

int rtl8290c_cli_cal_tscale(
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
#ifdef RTL8290C_CAL_TIME
    struct        timeval     tv_tmp1, tv_tmp2;
    struct        timezone     tz;
    double Duration; 
                
    gettimeofday(&tv_tmp1,&tz); 
#endif

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
            addr = RTL8290C_TEMP_INTR_SCALE_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
            _8290c_cal_flash_data_get(devId, addr, 1, &value2);
            value2 = (value2&0xCF)|((apd_step&0x3)<<4);
            _8290c_cal_flash_data_set(devId, addr, 1, value2);

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
            addr = RTL8290C_TEMP_INTR_SCALE_ADDR + RTL8290C_PARAMETER_BASE_ADDR; 
            _8290c_cal_flash_data_get(devId, addr, 1, &value2);
            value2 = (value2&0xF0)|((bias_step&0x3)<<2)|(mod_step&0x3);
            _8290c_cal_flash_data_set(devId, addr, 1, value2);

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

        addr = RTL8290C_TEMP_INTR_SCALE_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value);             
        printf("Calibration Status (0x%x):\n", value);
        apd_step = (value&0x30)>>4;
        bias_step = (value&0xC)>>2;
        mod_step = value&0x3;
        printf("APD LUT Step size = %s \n", StepStr[apd_step]);
        printf("TX BIAS LUT Step size = %s \n", StepStr[bias_step]);
        printf("TX MOD LUT Step size = %s \n", StepStr[bias_step]);    

        return -1; 
    }
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_tscale Duration = %f ms\n", Duration);
#endif 

    return 0; 
}



#if 0
int rtl8290c_cli_cal_tscale(
    uint32 devId, 
    int argc,
    char *argv[],
    FILE *fp)
{
    int32 ret, temp;
    uint32 scale;   
    uint32 addr, data;
    if (argc >= 2)
    {
        scale = _vlaue_translate(argv[1]); 

        if (0<scale && 8>scale)
        {
            rtk_ldd_parameter2_get(devId, 0x393, &data);
            data = (data&0xF8)|(scale&0x7);
            rtk_ldd_parameter2_set(devId, 0x393, data);
            printf("Temperature scale=%d.\n", scale);
            addr = RTL8290C_TEMP_INTR_SCALE_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
            _8290c_cal_flash_data_set(devId, addr, 1, scale);                 
        }
        else
        {
           printf("Invalid temperature scale=%d.\n", scale);
           return -1;
        }
    }
    else
    {
        rtk_ldd_parameter2_get(devId, 0x393, &data);
        scale = (data&0x7);
        printf("Read temperature scale=%d \n", scale);
        addr = RTL8290C_TEMP_INTR_SCALE_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &data);             
        printf("Calibration scale=%d \n", data); ;
    }

    return 0;    
}
#endif
    
int rtl8290c_cli_cal_rxpol(
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
#ifdef RTL8290C_CAL_TIME
    struct        timeval     tv_tmp1, tv_tmp2;
    struct        timezone     tz;
    double Duration; 
                
    gettimeofday(&tv_tmp1,&tz); 
#endif

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
        addr = RTL8290C_RX_OUT_POL_SWAP_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, value);
        printf("Set RTL8290C RXOUT Polarity = 0x%x\n", value);
        rtk_ldd_parameter2_get(devId, 0x2AC, &data);
        data = (data&0xBF)|((value&0x1)<<6);
        rtk_ldd_parameter2_set(devId, 0x2AC, data);
    }
    else
    { 
        addr = RTL8290C_RX_OUT_POL_SWAP_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8290C RXOUT Polarity = 0x%x \n", value);
        rtk_ldd_parameter2_get(devId, 0x2AC, &data);
        value = (data&0x40)>>6;
        printf("CurrentC RXOUT Polarity = 0x%x\n", value);		
    }
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_rxpol Duration = %f ms\n", Duration);
#endif 
    
    return 0; 
}


int rtl8290c_cli_cal_rxlospol(
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
#ifdef RTL8290C_CAL_TIME
    struct        timeval     tv_tmp1, tv_tmp2;
    struct        timezone     tz;
    double Duration; 
                
    gettimeofday(&tv_tmp1,&tz); 
#endif

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
        addr = RTL8290C_RXLOS_POLARITY_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_set(devId, addr, 1, value);
        printf("Set RTL8290C RX Polarity = 0x%x\n", value, (value>0)?"RXLOS":"RXSD");
        rtk_ldd_parameter2_get(devId, 0x398, &data);
        data = (data&0xFE)|(value&0x1);
        rtk_ldd_parameter2_set(devId, 0x398, data);
    }
    else
    { 
        addr = RTL8290C_RXLOS_POLARITY_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value);  
        printf("RTL8290C RXLOS Polarity = 0x%x (%s)\n", value, (value>0)?"RXLOS":"RXSD");
    }
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_rxpol Duration = %f ms\n", Duration);
#endif 
    
    return 0; 
}


int rtl8290c_cli_cal_rxlos(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 i,data1, data2, data3, los_flag, flag2;
    uint32 w31_data, w30_data_ori, w30_data;
    uint32 addr, value, value2;
    uint32 w83_data;
    uint32 data;
    uint32 ctrl_data;
#ifdef RTL8290C_CAL_TIME  
    struct      timeval     tv_tmp1, tv_tmp2;
    struct      timezone     tz;
    double Duration; 
        
    gettimeofday(&tv_tmp1,&tz); 
#endif   

    if(argc < 2)
    { 
        addr = RTL8290C_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value);  
        addr = RTL8290C_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
        _8290c_cal_flash_data_get(devId, addr, 1, &value2);          
        printf("los = 0x%x, range = 0x%x\n", value, value2);
        rtk_ldd_rxlosRefDac2_get(devId, &data);
        rtk_ldd_parameter2_get(devId, 0x2B0, &w30_data);    
        printf("current los = 0x%x, range = 0x%x\n", data, ((w30_data&0x2)>>1));    
    }
    else if (argc >= 2)
    {
        if ((ret=strcmp(argv[1], "auto")) == 0 )
        {
            printf("Please plug the proper LOS optical power to ONU...\n");
            //printf("Wait 3 sec...\n");    
            rtk_ldd_parameter2_get(devId, 0x387, &ctrl_data);
            rtk_ldd_parameter2_set(devId, 0x387, 0x10);

            //osal_time_mdelay(3000);
            rtk_ldd_parameter2_get(devId, 0x2B1, &w31_data); 
            //# W30[4] REG_ASSERT_TESTMODE : 0 is DIG_TOP control and 1 is REG set.
            //# W30[3] REG_ASSERT_SETL : 0 is deassert and 1 is assert.
            //# W30[1] REG_RANGE_SELL
            rtk_ldd_parameter2_get(devId, 0x2B0, &w30_data_ori);    
            
            data1 = 0x10;
            w31_data = (w31_data&0x80)|data1;
            rtk_ldd_parameter2_set(devId, 0x2B1, w31_data);

            //set W30_0 [ expr ($W30_orig & 0xE5) ]
            //set W30_1 [ expr ($W30_0 | 0x18 | $range_sel<<1) ]
            w30_data = (w30_data_ori&0xE5)|0x18;
            rtk_ldd_parameter2_set(devId, 0x2B0, w30_data);         
            
            los_flag = 0;
            //printf("Start: data1 = 0x%x, los_flag = 0x%x\n", data1, los_flag);
            while ((data1<0x7F)&&(los_flag==0))
            {
                flag2 = 0; 
                rtk_ldd_parameter2_get(devId, 0x2B1, &w31_data);             
                rtk_ldd_parameter2_get(devId, 0x2B0, &w30_data);                
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
                    w31_data = (w31_data&0x80)|data1;
                    rtk_ldd_parameter2_set(devId, 0x2B1, w31_data);
                }
                else
                {
                    los_flag = 1;
                    //printf("LOS Flag = 1, W38 = 0x%x, W39 = 0x%x\n", w38_data, w39_data);
                }  

                if (data1>=0x7F)
                {
                    w30_data = 0;
                    rtk_ldd_parameter2_get(devId, 0x2B0, &w30_data);
                    //RANGE_SELL
                    if ((w30_data&0x02)==0)
                    {
                        w30_data = w30_data|0x2;
                        rtk_ldd_parameter2_set(devId, 0x2B0, w30_data);
    
                        data1=0;
                        w31_data = (w31_data&0x80)|data1;
                        rtk_ldd_parameter2_set(devId, 0x2B1, w31_data);                        
                    }
                    else
                    {                
                        data1++;
                        rtk_ldd_parameter2_set(devId, 0x2B0, w30_data_ori);
                        rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);                        
                        printf("RX LOS Fail, data = 0x%x, W30 = 0x%x\n", data1, w30_data);
                        return 0;
                    }
                }   
            }

            addr = RTL8290C_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;     
            _8290c_cal_flash_data_set(devId, addr, 1, data1);

            addr = RTL8290C_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;       
            _8290c_cal_flash_data_set(devId, addr, 1, ((w30_data&0x2)>>1));
            
            printf("RX LOS Finish. range = 0x%x, data1 = 0x%x\n",((w30_data&0x2)>>1), data1);

            rtk_ldd_parameter2_set(devId, 0x2B0, w30_data_ori);
            rtk_ldd_parameter2_set(devId, 0x387, ctrl_data);

        }
        else if ((ret=strcmp(argv[1], "range")) == 0 )
        {
            if(argc == 2)
            { 
                addr = RTL8290C_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;     
                _8290c_cal_flash_data_get(devId, addr, 1, &value);  
                addr = RTL8290C_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
                _8290c_cal_flash_data_get(devId, addr, 1, &value2);           
                printf("los = 0x%x, range = 0x%x\n", value, value2);
                rtk_ldd_rxlosRefDac2_get(devId, &data);
                rtk_ldd_parameter2_get(devId, 0x2B0, &w30_data);   
                printf("current los = 0x%x, range = 0x%x\n", data, ((w30_data&0x2)>>1));    
                return 0; 
            }

            value = _vlaue_translate(argv[2]); 
            if(value>1)
            {
                printf("%s range <value: 1 or 0> \n", argv[0]);       
                return -1;
            }
            addr = RTL8290C_RXLOS_RANGE_SEL_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;     
            _8290c_cal_flash_data_set(devId, addr, 1, value);
            w30_data = 0;
            rtk_ldd_parameter2_get(devId, 0x2B0, &w30_data);             
            w30_data = (w30_data&0xFD)|((value&0x1)<<1);
            rtk_ldd_parameter2_set(devId, 0x2B0, w30_data);    
        }        
        else
        {  
            value = _vlaue_translate(argv[1]);   
            if(value>0x7F)
            {
                printf("%s <value: less than 0x7F> \n", argv[0]);       
                return -1;
            }            
            addr = RTL8290C_RXLOS_REF_DAC_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;    
            _8290c_cal_flash_data_set(devId, addr, 1, value);
            printf("Set los = 0x%x\n", value);     
            rtk_ldd_rxlosRefDac2_set(devId, (value&0x7F));        
        }
    }
    
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_rxlos Duration = %f ms\n", Duration);
#endif

    return 0;    
}

int rtl8290c_cli_cal_prbs(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int ret;
    uint32 chipId, rev, subType;
    uint32 addr, value, value2, value3, ib, im;      
#ifdef RTL8290C_CAL_TIME  
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
        if((ret = _rtl8290c_gpon_init()) != 0)
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
        
            _rtl8290c_ibim_recover(devId);

        }
        else
        {

            _rtl8290c_ibim_low(devId);
    
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
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_prbs Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

int rtl8290c_cli_cal_epon_prbs(
    uint32 devId,
    int argc,
    char *argv[],
    FILE *fp)
{
    int32   ret;
    uint32 chipId, rev, subType;
    uint32 addr, value, ib, im;      
#ifdef RTL8290C_CAL_TIME  
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
    if((ret = _rtl8290c_epon_init()) != 0)
    {
        return ret;
    }

    //Set EPON LPF to 40MHz
    value = 0;   
    addr = RTL8290C_EPON_APC_LPF_BW_OFFSET_ADDR + RTL8290C_PARAMETER_BASE_ADDR;
    _8290c_cal_flash_data_get(devId, addr, 1, &value);
    if (value != 0)
    {
        value = 0;      
        _8290c_cal_flash_data_set(devId, addr, 1, value);
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

            _rtl8290c_ibim_recover(devId);

        }
        else if(0 == strcmp(argv[1], "benon"))
        {  
            _rtl8290c_ibim_low(devId);

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
            
            _rtl8290c_ibim_recover(devId); 
            
        }    
        else
        {
            _rtl8290c_ibim_low(devId);
    
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
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_epon_prbs Duration = %f ms\n", Duration);
#endif
    
    return 0; 
}

int rtl8290c_cli_cal_refresh(
    uint32 devId,  
    int argc,
    char *argv[],
    FILE *fp)
{ 
#ifdef RTL8290C_CAL_TIME  
    struct      timeval     tv_tmp1, tv_tmp2;
    struct      timezone     tz;
    double Duration; 
        
    gettimeofday(&tv_tmp1,&tz); 
#endif

    rtk_ldd_config2_refresh(devId);
    
#ifdef RTL8290C_CAL_TIME
    gettimeofday(&tv_tmp2,&tz);    
    Duration = 1000 * (tv_tmp2.tv_sec - tv_tmp1.tv_sec) + ((tv_tmp2.tv_usec - tv_tmp1.tv_usec) / 1000.0);
    printf("[TIME]:  rtl8290c_cli_cal_refresh Duration = %f ms\n", Duration);
#endif

    return 0;
}    


