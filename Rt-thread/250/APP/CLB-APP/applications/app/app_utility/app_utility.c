/**
 * @file
 * @details
 * @author
 * @date
 * @version
//        rt_kprintf("data_buff: \n");
//        for(rt_uint16_t i = 0; i < 32; i++)
//        {
//            rt_kprintf("0x%02X ", data_buff[i]);
//        }
//        rt_kprintf("\n");
**/

/* include */
#include <string.h>
#include "app_utility.h"

/* macro */

/* type declaration */

/* variable */
static const unsigned int fcstab[256u]=
{
    0x00000000u, 0x77073096u, 0xee0e612cu, 0x990951bau,
    0x076dc419u, 0x706af48fu, 0xe963a535u, 0x9e6495a3u,
    0x0edb8832u, 0x79dcb8a4u, 0xe0d5e91eu, 0x97d2d988u,
    0x09b64c2bu, 0x7eb17cbdu, 0xe7b82d07u, 0x90bf1d91u,
    0x1db71064u, 0x6ab020f2u, 0xf3b97148u, 0x84be41deu,
    0x1adad47du, 0x6ddde4ebu, 0xf4d4b551u, 0x83d385c7u,
    0x136c9856u, 0x646ba8c0u, 0xfd62f97au, 0x8a65c9ecu,
    0x14015c4fu, 0x63066cd9u, 0xfa0f3d63u, 0x8d080df5u,
    0x3b6e20c8u, 0x4c69105eu, 0xd56041e4u, 0xa2677172u,
    0x3c03e4d1u, 0x4b04d447u, 0xd20d85fdu, 0xa50ab56bu,
    0x35b5a8fau, 0x42b2986cu, 0xdbbbc9d6u, 0xacbcf940u,
    0x32d86ce3u, 0x45df5c75u, 0xdcd60dcfu, 0xabd13d59u,
    0x26d930acu, 0x51de003au, 0xc8d75180u, 0xbfd06116u,
    0x21b4f4b5u, 0x56b3c423u, 0xcfba9599u, 0xb8bda50fu,
    0x2802b89eu, 0x5f058808u, 0xc60cd9b2u, 0xb10be924u,
    0x2f6f7c87u, 0x58684c11u, 0xc1611dabu, 0xb6662d3du,
    0x76dc4190u, 0x01db7106u, 0x98d220bcu, 0xefd5102au,
    0x71b18589u, 0x06b6b51fu, 0x9fbfe4a5u, 0xe8b8d433u,
    0x7807c9a2u, 0x0f00f934u, 0x9609a88eu, 0xe10e9818u,
    0x7f6a0dbbu, 0x086d3d2du, 0x91646c97u, 0xe6635c01u,
    0x6b6b51f4u, 0x1c6c6162u, 0x856530d8u, 0xf262004eu,
    0x6c0695edu, 0x1b01a57bu, 0x8208f4c1u, 0xf50fc457u,
    0x65b0d9c6u, 0x12b7e950u, 0x8bbeb8eau, 0xfcb9887cu,
    0x62dd1ddfu, 0x15da2d49u, 0x8cd37cf3u, 0xfbd44c65u,
    0x4db26158u, 0x3ab551ceu, 0xa3bc0074u, 0xd4bb30e2u,
    0x4adfa541u, 0x3dd895d7u, 0xa4d1c46du, 0xd3d6f4fbu,
    0x4369e96au, 0x346ed9fcu, 0xad678846u, 0xda60b8d0u,
    0x44042d73u, 0x33031de5u, 0xaa0a4c5fu, 0xdd0d7cc9u,
    0x5005713cu, 0x270241aau, 0xbe0b1010u, 0xc90c2086u,
    0x5768b525u, 0x206f85b3u, 0xb966d409u, 0xce61e49fu,
    0x5edef90eu, 0x29d9c998u, 0xb0d09822u, 0xc7d7a8b4u,
    0x59b33d17u, 0x2eb40d81u, 0xb7bd5c3bu, 0xc0ba6cadu,
    0xedb88320u, 0x9abfb3b6u, 0x03b6e20cu, 0x74b1d29au,
    0xead54739u, 0x9dd277afu, 0x04db2615u, 0x73dc1683u,
    0xe3630b12u, 0x94643b84u, 0x0d6d6a3eu, 0x7a6a5aa8u,
    0xe40ecf0bu, 0x9309ff9du, 0x0a00ae27u, 0x7d079eb1u,
    0xf00f9344u, 0x8708a3d2u, 0x1e01f268u, 0x6906c2feu,
    0xf762575du, 0x806567cbu, 0x196c3671u, 0x6e6b06e7u,
    0xfed41b76u, 0x89d32be0u, 0x10da7a5au, 0x67dd4accu,
    0xf9b9df6fu, 0x8ebeeff9u, 0x17b7be43u, 0x60b08ed5u,
    0xd6d6a3e8u, 0xa1d1937eu, 0x38d8c2c4u, 0x4fdff252u,
    0xd1bb67f1u, 0xa6bc5767u, 0x3fb506ddu, 0x48b2364bu,
    0xd80d2bdau, 0xaf0a1b4cu, 0x36034af6u, 0x41047a60u,
    0xdf60efc3u, 0xa867df55u, 0x316e8eefu, 0x4669be79u,
    0xcb61b38cu, 0xbc66831au, 0x256fd2a0u, 0x5268e236u,
    0xcc0c7795u, 0xbb0b4703u, 0x220216b9u, 0x5505262fu,
    0xc5ba3bbeu, 0xb2bd0b28u, 0x2bb45a92u, 0x5cb36a04u,
    0xc2d7ffa7u, 0xb5d0cf31u, 0x2cd99e8bu, 0x5bdeae1du,
    0x9b64c2b0u, 0xec63f226u, 0x756aa39cu, 0x026d930au,
    0x9c0906a9u, 0xeb0e363fu, 0x72076785u, 0x05005713u,
    0x95bf4a82u, 0xe2b87a14u, 0x7bb12baeu, 0x0cb61b38u,
    0x92d28e9bu, 0xe5d5be0du, 0x7cdcefb7u, 0x0bdbdf21u,
    0x86d3d2d4u, 0xf1d4e242u, 0x68ddb3f8u, 0x1fda836eu,
    0x81be16cdu, 0xf6b9265bu, 0x6fb077e1u, 0x18b74777u,
    0x88085ae6u, 0xff0f6a70u, 0x66063bcau, 0x11010b5cu,
    0x8f659effu, 0xf862ae69u, 0x616bffd3u, 0x166ccf45u,
    0xa00ae278u, 0xd70dd2eeu, 0x4e048354u, 0x3903b3c2u,
    0xa7672661u, 0xd06016f7u, 0x4969474du, 0x3e6e77dbu,
    0xaed16a4au, 0xd9d65adcu, 0x40df0b66u, 0x37d83bf0u,
    0xa9bcae53u, 0xdebb9ec5u, 0x47b2cf7fu, 0x30b5ffe9u,
    0xbdbdf21cu, 0xcabac28au, 0x53b39330u, 0x24b4a3a6u,
    0xbad03605u, 0xcdd70693u, 0x54de5729u, 0x23d967bfu,
    0xb3667a2eu, 0xc4614ab8u, 0x5d681b02u, 0x2a6f2b94u,
    0xb40bbe37u, 0xc30c8ea1u, 0x5a05df1bu, 0x2d02ef8du
};
static const uint16_t  crc_table[] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
INT8U checksum(void *p_data, INT16U len, INT8U in_data)
{
    INT8U checksum = in_data;
    INT8U *ptr = p_data;

    for (INT16U i = 0; i < len; i++)
    {
        checksum ^= ptr[i];
    }

    return checksum;
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
unsigned int fcs32(const unsigned char *buf, unsigned int len, unsigned int fcs)
{
    unsigned int counter;
    for( counter = 0; counter < len; counter++)
        {
        fcs = (fcs >> 8) ^ fcstab[(fcs ^ buf[counter]) & 0xff];
    }
    return (fcs ^ 0xffffffff);
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
uint8_t CheckSum(uint8_t *buf, uint16_t len)
{
    uint8_t ret = 0;

    for(uint16_t i = 0; i < len; i++)
    {
        ret += buf[i];
    }
    return ret;
}
/**
 * @brief
 * @param
 * @return
 * @note
**/
uint16_t CheckSum16(uint8_t *buf, uint16_t len)
{
    uint16_t ret = 0;

    for(uint16_t i = 0; i < len; i++)
    {
        ret += buf[i];
    }
    return ret;
}
/**
 * @brief
 * @param
 * @return
 * @note
**/
uint8_t Check_xor_sum (void *p_data, uint32_t len, uint8_t in_data)
{
    uint8_t checksum = in_data;
    uint8_t *ptr = p_data;

    for(uint32_t i = 0; i < len; i++)
    {
        checksum ^= ptr[i];
    }

    return checksum;
}
/**
 * @brief
 * @param
 * @return
 * @note
**/
uint32_t CheckTotal(uint32_t *data, uint16_t size)
{
    uint32_t checksum = 0;

    for(uint16_t i = 0; i < size; i++)
    {
        checksum += data[i];
    }

    return checksum;
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
uint16_t Check16Sum(uint16_t *buf, uint16_t len)
{
    uint16_t ret = 0;

    for(uint16_t i = 0; i < len; i++)
    {
        ret += buf[i];
    }
    return ret;
}

/**
 * @brief
 * @param
 * @return
 * @note   IP地址检查
**/
bool ip_check(const char *ip)
{
    int dots = 0, setions = 0;
    char ip_temp[16+1] = {0};
    char *token = NULL;

    strncpy(ip_temp, ip, sizeof(ip_temp));
    token = strtok(ip_temp, ".");
    while (token != NULL)
    {
        if(strlen(token) > 3) {
            return false;
        }
        token = strtok(NULL, ".");
    }
    if(NULL == ip || *ip == '.'){
        return false;
    }
    while(*ip)
    {
        if(*ip == '.')
        {
            dots ++;
            if(setions >= 0 && setions <= 255)
            {
                 setions = 0;
                 ip++;
                 continue;
            }
            else {
                return false;
            }
        }
        else if (*ip >= '0' && *ip <= '9'){
            setions = setions * 10 + (*ip - '0');
        }
        else {
            return false;
        }
        ip++;
    }
    if (setions >= 0 && setions <= 255)
    {
        if(dots == 3) {
            return true;
        }
    }
    return false;
}

/**
 * @brief
 * @param
 * @return
 * @note   子网掩码检查
**/
bool mask_check(const char *mask)
{
   if(ip_check(mask) != false)
   {
       unsigned int b = 0, i, n[4];
       sscanf(mask, "%u.%u.%u.%u", &n[3], &n[2], &n[1], &n[0]);

       if(strcmp(mask,"0.0.0.0") == 0) {
           return false;
       }
       for(i = 0; i < 4; ++i) {
            b += n[i] << (i * 8);
       }
       b = ~b + 1;
       if((b & (b - 1)) == 0) {
           return true;
       }
   }
   return false;
}

/**
 * @brief
 * @param  使用inet_addr函数得到netAddr
 * @return
 * @note   私有IP地址检查
**/
bool isLocalAddr(uint32_t netAddr)
{
    unsigned char    data[4];
    bool    bRet = false;

    memcpy(data, (char *)&netAddr, 4);
    if(10 == data[0]){
        bRet = true;
    }
    else if(192 == data[0] && 168 == data[1]){
        bRet = true;
    }
    else if(172 == data[0])
    {
        if (data[1] >= 16 && data[1] <= 31){
            bRet = true;
        }
    }

    return bRet;
}

/**
 * @brief
 * @param
 * @return
 * @note   TRDP中用到
**/
uint16_t check_crc(uint8_t *data, uint32_t len)
{
    uint8_t *p = data;
    uint16_t crc = 0x0000;
    for(uint32_t i = 0; i < len; i++)
    {
        crc = (crc << 8) ^ crc_table[((crc >> 8) ^ *(uint8_t *)p++) & 0x00FF];
    }
    return crc;
}

/**
 * @brief
 * @param
 * @return
 * @note   插入排序
**/
void InsertSort(int* a, int n)
{
    for(int i = 0; i < n - 1; i++)
    {
        int end = i, temp = a[end + 1];

        while(end >= 0)
        {
            if(temp < a[end])
            {
                a[end + 1] = a[end];
                end--;
            }
            else {
                break;
            }
        }
        a[end + 1] = temp;
    }
}

/**
 * @brief
 * @param
 * @return
 * @note   CRC-8/MAXIM  x8+x5+x4+1
**/
uint8_t crc8_maxim(uint8_t *data, uint16_t len)
{
    uint8_t i;
    uint8_t crc = 0;         // Initial value
    while(len--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8C;        // 0x8C = reverse 0x31
            else
                crc >>= 1;
        }
    }
    return crc;
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void sys_delay(uint32_t cnts)
{
    for(; cnts != 0; cnts--) {
        ;
    }
}



