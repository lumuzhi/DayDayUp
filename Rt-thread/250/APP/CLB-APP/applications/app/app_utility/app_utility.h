/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_UTILITY_H__
#define __APP_UTILITY_H__

/* include */
#include "app_board.h"

/* macro */
#define INT8U                   uint8_t
#define INT16U                  uint16_t
#define INT32U                  uint32_t

#define MIN(a, b)               ((a) > (b) ? (b) : (a))
#define MIN3(a, b, c)           MIN(MIN(a, b), c)
#define MAX(a, b)               ((a) < (b) ? (b) : (a))
#define MAX3(a, b, c)           MAX(MAX((a), (b)), c)
#define MAX4(a, b, c, d)        MAX(MAX3((a), (b), (c)), d)
#define MAX5(a, b, c, d, e)     MAX(MAX4((a), (b), (c), (d)), e)
#define MAX6(a, b, c, d, e, f)  MAX(MAX5((a), (b), (c), (d), (e)), f)

#define DATA16(h, l)            (((h) << 8) | (l))
#define DATA16_H(d)             ((INT8U)((d) >> 8))
#define DATA16_L(d)             ((INT8U)(d))

#define DATA32(hh, hl, lh, ll)  (((hh) << 24) | ((hl) << 16) | ((lh) << 8) | (ll))
#define DATA32_HH(d)            ((INT8U)((d) >> 24))
#define DATA32_HL(d)            ((INT8U)((d) >> 16))
#define DATA32_LH(d)            ((INT8U)((d) >> 8))
#define DATA32_LL(d)            ((INT8U)(d))

#define STRUCT_OFFSET(S, e)     (INT32U)(&((S *)0)->e)

#define STR_LEN(s)              (sizeof(s) - 1)

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define hton16(n)       ((((n) & 0x00FF) << 8) | (((n) & 0xFF00) >> 8))
#define hton32(n)       ((((n) & 0x000000FF) << 24) | (((n) & 0x0000FF00) << 8) \
                       | (((n) & 0x00FF0000) >> 8) | (((n) & 0xFF000000) >> 24))
#define ntoh16(n)       hton16(n)
#define ntoh32(n)       hton32(n)
#else
#define hton16(n)       n
#define hton32(n)       n
#endif

/* function declaration */
INT8U checksum (void *p_data, INT16U len, INT8U in_data);
unsigned int fcs32(const unsigned char *buf, unsigned int len, unsigned int fcs);
uint8_t CheckSum(uint8_t *buf, uint16_t len);
uint16_t CheckSum16(uint8_t *buf, uint16_t len);
uint8_t Check_xor_sum (void *p_data, uint32_t len, uint8_t in_data);
uint32_t CheckTotal(uint32_t *data, uint16_t size);
uint16_t Check16Sum(uint16_t *buf, uint16_t len);
bool ip_check(const char *ip);
bool mask_check(const char *mask);
bool isLocalAddr(uint32_t netAddr);
uint16_t check_crc(uint8_t *data, uint32_t len);
void InsertSort(int* a, int n);
uint8_t crc8_maxim(uint8_t *data, uint16_t len);
void sys_delay(uint32_t cnts);

#endif /*__APP_UTILITY_H__*/

