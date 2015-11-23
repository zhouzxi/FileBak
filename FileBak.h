/**********************************************************************
* 版权所有 (C)2015, Zhou Zhaoxiong。
*
* 文件名称：FileBak.c
* 文件标识：无
* 内容摘要：测试当文件大小超过规定值之后的文件备份操作
* 其它说明：无
* 当前版本：V1.0
* 作    者：Zhou Zhaoxiong
* 完成日期：20151123
*
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/io.h>


// 数据类型重定义
typedef signed   int        INT32;
typedef signed   char       INT8;
typedef unsigned char       UINT8;
typedef unsigned short int  UINT16;
typedef unsigned int        UINT32;
typedef long                LONG;

// 宏定义
#define MAX_BAKCOUNT        100         // 备份文件的最大个数
#define MAX_PATHLEN         256         // 最大路径长度
#define MAX_FILESIZE        500         // 每个文件大小上限


// 结构体声明
struct finddata_t
{
    INT8 szFileName[MAX_PATHLEN];
    LONG time_write;
};

// 函数声明
INT32 GetNextFileIndex(void);
INT8 *GetField(INT8 *szBuf, INT8 *szDivStr, INT8 iIndex);
INT32 GetFieldNum(const INT8 *szBuf, INT8 *szDivStr);
INT8 *StrTokDiv(INT8 *szBuf, INT8 *szDivStr);
void GetBackupName(INT32 iIndex, INT8 *szOutput);
LONG FindFirst(INT8 *pszFindPara, struct finddata_t *tfinddata);
LONG FindNext(struct finddata_t *tfinddata);
LONG FindClose(void);
