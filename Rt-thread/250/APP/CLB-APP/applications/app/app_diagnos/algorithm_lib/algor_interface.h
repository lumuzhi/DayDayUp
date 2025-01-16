
/**************************************
file name: algor_interface.h
function： 算法库所需的接口头文件
***************************************/
#ifndef _ALGOR_INTERFACE_H_
#define _ALGOR_INTERFACE_H_

#define PROJECT_LH_BOARD_PROTOCOL_SPI_SPI_APP_H_

/************************************************************
Function:         动态分配内存空间
Description:      
Input：            unsigned int size：分配内存空间的大小
Output:           None
Return:           返回一个指向已分配空间的 void 指针,如果内存不足,则返回 NULL。
Others:
************************************************************/
void* algor_malloc(unsigned int size);

/************************************************************
Function:         释放动态分配的内存空间
Description:      
Input：            void * p：需要释放的内存地址指针
Output:           None
Return:           None
Others:
************************************************************/
void algor_free(void * p);

/************************************************************
Function:         释放动态分配的内存空间
Description:      
Input：            void * p：需要释放的内存地址指针
Output:           None
Return:           None
Others:
************************************************************/
void algor_printf(const char* format, ...);


#endif /* _ALGOR_INTERFACE_H_ */


