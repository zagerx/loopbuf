/*
 *      loopbuf.c     环形缓冲区
 
 *      缓冲区的创建    静态        unsigned char ucloopBuf[LBSIZE]
 *  *                              
 * Init_loopBuf
 
 */
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"




#include "zg_loopbuf.h"

#define LOOPBUF_SIZE 10
static unsigned char loopbuf[LOOPBUF_SIZE];
unsigned char *pstartaddr;
unsigned char *pendaddr;
unsigned char *pread;
unsigned char *pwrite;
unsigned short remainbyte;
unsigned short num;


static void loopbuf_test(void);

bool  Init_loopBuf(void)
{
    unsigned char *pbuf = NULL;
    pstartaddr = loopbuf;
    pendaddr = pstartaddr + LOOPBUF_SIZE -1;
    pread = pstartaddr;
    pwrite = pstartaddr;
    remainbyte = LOOPBUF_SIZE;
    num = 0;

    loopbuf_test();
}

bool add_toLoopBuf(unsigned char* pdata,unsigned short len)
{
    unsigned char len_Hbyte;
    unsigned char len_Lbyte;
    if(pwrite==NULL || pwrite<pstartaddr || pwrite>pendaddr || len+2>LOOPBUF_SIZE || len<=0)
        return false;
    len_Hbyte = (unsigned char)(len>>8);
    len_Lbyte = (unsigned char)(len&0xFF); 
    /*知道获取到足够的字节数*/    
    while(remainbyte < len+2)
    {
        unsigned char buf[100];
        get_loopBuf(buf);
    }

    if(pendaddr - pwrite < 1)
    {
        *pendaddr = len_Hbyte;
        *pstartaddr = len_Lbyte;
        pwrite = pstartaddr + 1;
        memcpy(pwrite,pdata,len);
        pwrite = pstartaddr + len - 1;
        num++;
        remainbyte -= (len+2);
    }else if(pendaddr - pwrite == 1)
    {
        *pwrite = len_Hbyte;
        *pendaddr = len_Lbyte;
        memcpy(pstartaddr,pdata,len);
        pwrite = pstartaddr + len;
        num++;
        remainbyte -= (len+2);
    }else{
        *pwrite = len_Hbyte;
        *(pwrite+1) = len_Lbyte;
        pwrite += 2;
        /*判断是否需要两次存放*/
        if(pendaddr-pwrite+1 > len)//一次存放 且有剩余情况
        {
            memcpy(pwrite,pdata,len);
            pwrite += len;
            num++;
            remainbyte -= (len+2);
        }else if(pendaddr-pwrite+1 == len)//一次存放 正好结束
        {
            memcpy(pwrite,pdata,len);
            pwrite = pstartaddr;
            num++;
            remainbyte -= (len+2);            
        }else//需要两次存放
        {
            unsigned short len1,len2;
            len1 = (pendaddr - pwrite + 1);
            len2 = len - len1;
            printf("len1 = %d len2 = %d\r\n",len1,len2);
            memcpy(pwrite,pdata,len1);
            memcpy(pstartaddr,pdata+len1,len2);
            pwrite = pstartaddr + len2;

            num++;
            remainbyte -= (len+2);
        }
    }
}

/*从缓冲区获取数据*/
bool get_loopBuf(unsigned char* pdata)
{
    unsigned char len_Hbyte;
    unsigned char len_Lbyte;
    unsigned short len;
    if(pread==NULL || pread<pstartaddr || pread>pendaddr || num<1)
        return false;

    if(pendaddr - pread < 1)
    {
        len_Hbyte = *pendaddr;         *pendaddr = 0;
        len_Lbyte = *pstartaddr;       *pstartaddr = 0;
        len = (unsigned short)len_Hbyte << 8 | len_Lbyte;

        memcpy(pdata,pstartaddr+1,len); memset(pstartaddr+1,0,len);
        pread = pstartaddr + (len - 1);
        num--;
        remainbyte += (len+2);
    }else if(pendaddr - pread < 1)
    {
        len_Hbyte = *pread;         *pread = 0;
        len_Lbyte = *pendaddr;      *pendaddr = 0;
        len = (unsigned short)len_Hbyte << 8 | len_Lbyte;

        memcpy(pdata,pstartaddr,len); memset(pstartaddr,0,len);
        pread = pstartaddr + len;
        num--;
        remainbyte += (len+2);
    }else{
        len_Hbyte = *pread;         *pread = 0;
        len_Lbyte = *(pread+1);     *(pread+1) = 0;
        len = (unsigned short)len_Hbyte << 8 | len_Lbyte;

        pread += 2;
        /*判断是否要分两次读取*/ 
        if(pendaddr - pread + 1 > len)//
        {
            memcpy(pdata,pread,len);memset(pread,0,len);
            pread += len;
            num--;
            remainbyte += (len+2);
        }else if(pendaddr - pread + 1 == len)
        {
            memcpy(pdata,pread,len);memset(pread,0,len);
            pread = pstartaddr;
            num--;
            remainbyte += (len+2);
        }else{//需要两次读取
            unsigned short len1;
            unsigned short len2;
            len1 = len - (pendaddr - pread + 1);
            len2 = len - len1;

            memcpy(pdata,pread,len1);memset(pread,0,len1);
            memcpy(pdata+len1,pstartaddr,len2);memset(pread,0,len2);

            pread = pstartaddr+len2;

            num--;
            remainbyte += (len+2);
        }
    }
}

static void loopbuf_test(void)
{
    printf("pstart = 0x%x   pend = 0x%x\r\n",pstartaddr,pendaddr);


    /*第一次添加数据*/
    {
        unsigned char buf[3] = {0x11,0x12,0x13};
        add_toLoopBuf(buf,sizeof(buf));
    }
    /*第2次添加数据*/
    {
        unsigned char buf[3] = {0x21,0x22,0x23};
        add_toLoopBuf(buf,sizeof(buf));
    } 

    /*第3次添加数据*/
    {
        unsigned char buf[4] = {0x41,0x42,0x43,0x44};
        add_toLoopBuf(buf,sizeof(buf));
    } 


    /*第4次添加数据*/
    {
        printf("***************four add**************************\r\n");
        unsigned char buf[5] = {0x51,0x52,0x53,0x54,0x55};
        add_toLoopBuf(buf,sizeof(buf));
        printf("pread = 0x%x   pwrite = 0x%x\r\n",pread,pwrite);
    } 


    // /*获取一次数据*/
    // {
    //     unsigned char buf[10];
    //     get_loopBuf(buf);
    //     /*打印获取到的数据*/
    //     printf("***************first get**************************\r\n");
    //     printf("buf : ");
    //     for(unsigned short i = 0;i<sizeof(buf);i++)
    //     {
    //         printf("[%d] = 0x%x ",i,buf[i]);
    //     } 
    //     printf("\r\n");
    //     printf("*****************************************\r\n");         
    // }
       
    /*打印整个内存情况*/
    printf("loopbuf : ");
    for(unsigned short i = 0;i<LOOPBUF_SIZE;i++)
    {
        printf("[%d] = 0x%x ",i,loopbuf[i]);
    }   
    printf("\r\n");
    printf("num = %d    remianbyte = %d\r\n",num,remainbyte);
}
