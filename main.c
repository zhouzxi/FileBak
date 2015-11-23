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
#include "FileBak.h"


// 全局变量
extern INT8  g_szLocalFile[500];


// 函数声明
void Sleep(UINT32 iCountMs);
void WriteFileNonStop(void);


/**********************************************************************
* 功能描述：主函数
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期        版本号     修改人            修改内容
* -------------------------------------------------------------------
* 20150917        V1.0     Zhou Zhaoxiong        创建
***********************************************************************/
INT32 main()
{
    WriteFileNonStop();   // 写文件

    return 0;
}


/**********************************************************************
 * 功能描述： 把内容写到本地文件中
 * 输入参数： pszContentLine-一条文件记录
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 * 修改日期            版本号            修改人           修改内容
 * ----------------------------------------------------------------------
 * 20151123             V1.0          Zhou Zhaoxiong       创建
 ************************************************************************/
void WriteFileNonStop(void)
{
    FILE  *fp                     = NULL;
    INT8   szContentBuf[200]      = {0};
    INT8   szBakName[MAX_PATHLEN] = {0};
    
    snprintf(g_szLocalFile, sizeof(g_szLocalFile)-1, "%s/zhouzhaoxiong/zzx/FileBak/File/File.txt", getenv("HOME"));
    fp = fopen(g_szLocalFile, "a+");
    if (fp == NULL)
    {
         printf("WriteFileNonStop: open local file failed, file=%s\n", g_szLocalFile);
         return;
    }

    while (1)      // 不停地写文件
    {
        // 先判断文件大小是否超过设定值, 如果是, 则生成新的文件
	if (ftell(fp) >= MAX_FILESIZE)      // 设定每个文件的大小上限
        {
            fclose(fp);
            fp = NULL;
			
            GetBackupName(GetNextFileIndex(), szBakName);        // 获得备份文件名
            unlink(szBakName);                                   // 如该备份文件存在, 则删除
            rename(g_szLocalFile, szBakName);                    // 原有文件改名为备份名称
            fp = fopen(g_szLocalFile, "wt+");                    // 新建空文件
        }
		
	strcpy(szContentBuf, "hello, world!\n");
        printf("WriteFileNonStop: LocalFile=%s, ContentBuf=%s", g_szLocalFile, szContentBuf);

        fputs(szContentBuf, fp);
        fflush(fp);
		
	Sleep(1 * 1000);   // 每1s写一次
    }

    fclose(fp);
    fp = NULL;
}

 
/**********************************************************************
* 功能描述： 程序休眠
* 输入参数： iCountMs-休眠时间(单位:ms)
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151123          V1.0     Zhou Zhaoxiong           创建
********************************************************************/
void Sleep(UINT32 iCountMs)
{
    struct timeval t_timeout = {0};
 
    if (iCountMs < 1000)
    {
        t_timeout.tv_sec = 0;
        t_timeout.tv_usec = iCountMs * 1000;
    }
    else
    {
        t_timeout.tv_sec = iCountMs / 1000;
        t_timeout.tv_usec = (iCountMs % 1000) * 1000;
    }
    select(0, NULL, NULL, NULL, &t_timeout);   // 调用select函数阻塞程序
}

