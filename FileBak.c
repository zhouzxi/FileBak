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
INT8   g_szLocalFile[500]                  = {0};
static INT8 g_szField[1024]                = {0};
static struct dirent *direntp              = NULL;
static DIR  *dirp                          = NULL;
static INT8  g_szScanFilename[MAX_PATHLEN] = {0};
static INT8  g_szDirPath[MAX_PATHLEN]      = {0};


/**********************************************************************
* 功能描述： 获得下一个日志文件序号
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 文件序号
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151123          V1.0     Zhou Zhaoxiong           创建
**********************************************************************/
INT32 GetNextFileIndex(void)
{
    struct finddata_t  tfile;
	
    LONG               hFile     = 0;
    INT32              iNext     = -1;
    time_t             lLastTime =  0;
	
    INT8               szLastName[MAX_PATHLEN] = {0};
    INT8               szFindPara[MAX_PATHLEN] = {0};
	
    INT8              *pNo       = NULL;
    
    // 搜索匹配szFindPara的文件
    sprintf(szFindPara, "%s_*", g_szLocalFile);
    if ((hFile = FindFirst(szFindPara, &tfile)) != -1L)
    {
        strcpy(szLastName, tfile.szFileName);
        lLastTime = tfile.time_write;

        while (FindNext(&tfile) == 0)
        {
            // 查找最新修改过的文件
            if (tfile.time_write > lLastTime)
            {
                snprintf(szLastName, sizeof(szLastName)-1, "%s", tfile.szFileName);
                lLastTime = tfile.time_write;
            }
        }
        FindClose();
    }

    if (strlen(szLastName) > 0)
    {
        // 取得最新修改过的文件编号
        pNo = GetField(szLastName, "_", GetFieldNum(szLastName, "_"));

        if (pNo != NULL)
        {
            iNext = atoi(pNo) + 1;
            if (iNext > MAX_BAKCOUNT)    // 超过最多备份个数, 则从1开始重新备份
            {
                iNext = 1;
            }
            return iNext;
        }
    }

    // 未找到符合格式的文件时, 默认返回1
    return 1;
}


/**********************************************************************
* 功能描述： 获取输入szBuf中以指定字符串szDivStr分隔的第iIndex个数据域
* 输入参数： szBuf-输入字符串
             szDivStr-分隔字符串
             iIndex-数据域序号(从1开始)
* 输出参数： 无
* 返 回 值： 指向数据域的指针, 未找到返回空字符串"\0"
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151123          V1.0     Zhou Zhaoxiong           创建
**********************************************************************/
INT8 *GetField(INT8 *szBuf, INT8 *szDivStr, INT8 iIndex)
{
    INT8  *pStr      = NULL;
    INT32  iLoopFlag = 0;
	
	memset(g_szField, 0x00, sizeof(g_szField));	
	
    do
	{
        if (NULL == szBuf || strlen(szBuf) == 0 || iIndex <= 0 || iIndex > GetFieldNum(szBuf, szDivStr))
        {
            break;
        }
		
        if ((pStr = StrTokDiv(szBuf, szDivStr)) == NULL)
        {
            break;
        }

        if (iIndex == 1)
        {
            strncpy(g_szField, pStr, sizeof(g_szField)-1);
            break;
        }

        for (iLoopFlag = 1; iLoopFlag < iIndex; iLoopFlag ++)
        {
            if ((pStr = StrTokDiv(NULL, szDivStr)) == NULL)
            {
                break;
            }
        }

        if (pStr == NULL)
        {
            break;
        }

        strcpy(g_szField, pStr);
    } while (0);
    
    return g_szField;    
}


/**********************************************************************
* 功能描述： 分解输入szBuf中以字符串szDivStr分隔的域
* 输入参数： szBuf-输入字符串
             szDivStr-分隔字符串
* 输出参数： 无
* 返 回 值： 指向数据域的指针, 未找到返回NULL
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151123          V1.0     Zhou Zhaoxiong           创建
**********************************************************************/
INT8 *StrTokDiv(INT8 *szBuf, INT8 *szDivStr) 
{
    static INT8 szTemp1[1024] = {0};
    static INT8 szTemp2[1024] = {0};
	
    INT8  *pStr    = NULL;
    INT32  iOffset = 0;

    if (szBuf != NULL)
    {
        memset(szTemp1, 0x00, sizeof(szTemp1));
        memset(szTemp2, 0x00, sizeof(szTemp2));
        strcpy(szTemp1, szBuf);
    }

    if (strlen(szTemp1) == 0)
    {
        return NULL;
    }

    if ((pStr = strstr(szTemp1, szDivStr)) == NULL)
    {
        // 如果pStr指向一个数据而后面没有分隔串, 则返回pStr指向的这个串
        strcpy(szTemp2, szTemp1);
        memset(szTemp1, 0x00, sizeof(szTemp1));
    }
    else
    {
        iOffset = pStr - szTemp1;
        strcpy(szTemp2, szTemp1);
        szTemp2[iOffset] = '\0';
        strcpy(szTemp1, &szTemp1[iOffset + strlen(szDivStr)]);
    }
    pStr = szTemp2;

    return pStr;
}


/**********************************************************************
* 功能描述： 获取输入szBuf中以指定字符串szDivStr分隔的数据域数
* 输入参数： szBuf-输入字符串
             szDivStr-分隔字符串
* 输出参数： 无
* 返 回 值： 数据域数(源串不含分隔字符串时返回1)
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151123          V1.0     Zhou Zhaoxiong           创建
**********************************************************************/
INT32 GetFieldNum(const INT8 *szBuf, INT8 *szDivStr)
{
    INT32  iNum         = 0;
    INT8   szTemp[1024] = {0};

    snprintf(szTemp, sizeof(szTemp)-1, "%s", szBuf);    

    if ((StrTokDiv(szTemp, szDivStr)) != NULL)
    {
        iNum ++;
        while (StrTokDiv(NULL, szDivStr) != NULL)
        {
            iNum ++;
        }
    }

    return iNum;
}


/**********************************************************************
* 功能描述： 获得当前备份文件名
* 输入参数： iIndex-文件序号
* 输出参数： szOutput-备份文件名
* 返 回 值： 无
* 其它说明： 备份文件的样式为: File.txt_XXX
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151123          V1.0     Zhou Zhaoxiong           创建
**********************************************************************/
void GetBackupName(INT32 iIndex, INT8 *szOutput)
{
    INT8 szMaxIndex[100] = {0};
    INT8 szIndex[100]    = {0};

    snprintf(szIndex,    sizeof(szIndex)-1,    "%d", iIndex); 
    snprintf(szMaxIndex, sizeof(szMaxIndex)-1, "%d", MAX_BAKCOUNT);  
    
    snprintf(szOutput, MAX_PATHLEN, "%s_%0*d%s", g_szLocalFile, strlen(szMaxIndex)-strlen(szIndex), 0, szIndex);  
}


/**********************************************************************
* 功能描述： 寻找满足条件的第一个文件
* 输入参数： pszFindPara-需要匹配的内容
* 输出参数： tfinddata-文件名结构体
* 返 回 值： 0-成功  其它-失败
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151123          V1.0     Zhou Zhaoxiong           创建
**********************************************************************/
LONG FindFirst(INT8 *pszFindPara, struct finddata_t *tfinddata)
{
    struct stat tStatBuf;
	
    INT8   szFileName[MAX_PATHLEN] = {0};
    INT32  iStrLen                 = 0;
    INT32  iCmpStrLen              = 0;
    
    memset(tfinddata, 0x00, sizeof(struct finddata_t));
    memset(g_szScanFilename, 0x00, MAX_PATHLEN);

    strncpy(g_szDirPath, pszFindPara, MAX_PATHLEN);
    strncpy(g_szScanFilename, GetField(pszFindPara, "*", 1), MAX_PATHLEN);  // 用于比较,去掉通配符*

    iCmpStrLen = strlen(g_szScanFilename);

    // 获得目录路径
    iStrLen = strlen(g_szDirPath);
    for (; g_szDirPath[iStrLen] != '/'; iStrLen --)    // 寻找目录串
    {
        ;
    }
    g_szDirPath[iStrLen]='\0';

    if (dirp != NULL)
    {
        closedir(dirp);
        dirp = NULL;
    }

    if ((dirp = opendir(g_szDirPath)) == NULL)   // 打开目录
    {
        return -1L;
    }

    // 读取目录数据进行比较
    while ((direntp = readdir(dirp)) != NULL)
    {
        snprintf(szFileName, MAX_PATHLEN, "%s/%s", g_szDirPath, direntp->d_name);
        if (strncmp(szFileName, g_szScanFilename, iCmpStrLen) == 0)
        {
            strncpy(tfinddata->szFileName, szFileName, MAX_PATHLEN);
            stat(szFileName, &tStatBuf);
            tfinddata->time_write = tStatBuf.st_mtime;
            
            return 0L;
        }
    }
    if (NULL != dirp)
    {
        closedir(dirp);
        dirp = NULL;
    }
    return -1L;
}


/**********************************************************************
* 功能描述： 寻找满足条件的下一个文件
* 输入参数： 无
* 输出参数： tfinddata-文件名结构体
* 返 回 值： 0-成功  其它-失败
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151123          V1.0     Zhou Zhaoxiong           创建
**********************************************************************/
LONG FindNext(struct finddata_t *tfinddata)
{
    INT32  iCmpLen                 = 0;
    INT8   szFileName[MAX_PATHLEN] = {0};
    
    struct stat tStatBuf;

    if (NULL == dirp)
    {
        return -1L;
    }

    memset(tfinddata, 0x00, sizeof(struct finddata_t));

    iCmpLen = strlen(g_szScanFilename);

    while ((direntp = readdir(dirp)) != NULL)
    {
        snprintf(szFileName, MAX_PATHLEN, "%s/%s", g_szDirPath, direntp->d_name);
        if (strncmp(szFileName, g_szScanFilename, iCmpLen) == 0)
        {
            strncpy(tfinddata->szFileName, szFileName, MAX_PATHLEN);
            stat(szFileName, &tStatBuf);
            tfinddata->time_write = tStatBuf.st_mtime;
            return 0L;
        }
    }

    if (NULL != dirp)
    {
        closedir(dirp);
        dirp = NULL;
    }
    return -1L;
}


/**********************************************************************
* 功能描述： 关闭目录
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 0-成功
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151123          V1.0     Zhou Zhaoxiong           创建
**********************************************************************/
LONG FindClose(void)
{
    if (NULL != dirp)
    {
        closedir(dirp);
        dirp = NULL;
    }
    
    return 0L;
}

