#ifndef __JS_GROUPADDR_H_
#define __JS_GROUPADDR_H_


#ifdef __cplusplus
    extern "C" {
#endif


/********************************************************************************
**  GroupAddr.xml
********************************************************************************/

typedef struct __SGroup_Dp{
    int     DevId;
    int     DpId;
    char    Name[20];
}SGroup_Dp;

/*
 *  功能: 读分组总数
 *  输入: 无
 *  返回: -1 失败；>=0 分组总数
 */
int JsGroup_GetNum(void);

/*
 *  功能: 读指定序号的组地址
 *  输入: 设备ID DpId >= 1
 *  返回: -1 失败,>0 组地址
 */
int JsGroup_GetAddrByIndex(int Index);

/*
 *  功能: 读指定序号的DP信息
 *  输入: GAddr 组地址，DpIndex DP序号
 *  返回: NULL 失败
 */
cJSON* JsGroup_GetDpList(int GAddr);

/*
 *  功能: 添加组地址
 *  输入: GAddr 组地址,0:自动分配;>0,新建地址
 *  返回: -1 已存在；>0 组地址
 */
int JsGroup_AddAddr(int GAddr);

/*
 *  功能: 添加DP
 *  输入: GAddr 组地址,pDp 要添加的节点,pDp->DevId和pDp->DpDd必须都大于0
 *  返回: -1 失败；0 成功
 */
int JsGroup_AddDp(int GAddr,SGroup_Dp* pDp);

/*
 *  功能: 删除组
 *  输入: GAddr 组地址
 *  返回: -1 失败；0 成功
 */
int JsGroup_DelAddr(int GAddr);

/*
 *  功能: 从组里删除指定设备的Dp端口
 *  输入: GAddr 组地址(0xFFFFFFFF,所有组);DevId 设备ID；DpId (0xFFFFFFFF 指定设备的所有DP)
 *  返回: -1 失败；0 成功
 */
int JsGroup_DelGroupDp(int GAddr,int DevId, int DpId);

/*
 *  功能: 从所有组里删除指定设备
 *  输入: DevId 设备ID
 *  返回: -1 失败；0 成功
 */
int JsGroup_DelDev(int DevId);

/*
 *  功能: 保存文件
 *  输入: 文件地址
 *  返回: -1 失败；0 成功
 */
int JSGroup_SaveFile(void);

/*
 *  功能: 打开文件
 *  输入: 文件地址
 *  返回: -1 失败；0 成功
 */
int JsGroup_Open(char* sFile);

/*
 *  功能: 关闭文件
 *  输入: 无
 *  返回: -1 失败；0 成功
 */
int JsGroup_Close(void);

#ifdef __cplusplus
    }
#endif


#endif


