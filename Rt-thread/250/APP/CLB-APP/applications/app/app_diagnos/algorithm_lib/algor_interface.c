
/**************************************
file name: algor_interface.c
算法库所需的接口实现文件
***************************************/
//#include "rtdevice.h"
//#include "app_diagnosis.h"
#include "algor_interface.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"


/************************************************************
Function:         动态分配内存空间
Description:      
Input：            size：分配内存空间的大小

Output:           None
Return:           返回一个指向已分配空间的 void 指针,如果内存不足,则返回 NULL。
Others:
************************************************************/
void* algor_malloc(unsigned int size)
{
   void* p = (void*)0;
	p = (void*)malloc(size);
   if ((void*)0 == p)
    {
    	printf("algor_interface.c --- algor_malloc fail!!! size: %d\n",size);
    }
   return p;
}

/************************************************************
Function:         释放动态分配的内存空间
Description:      
Input：            size：分配内存空间的大小

Output:           None
Return:           None
Others:
************************************************************/
void algor_free(void * p)
{
    free(p);
}

/************************************************************
Function:         打印输出
Description:      
Input：            void * p：需要释放的内存地址指针
Output:           None
Return:           None
Others:
************************************************************/
void algor_printf(const char* format, ...)
{
	va_list args;

	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}


