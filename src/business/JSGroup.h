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
 *  ����: ����������
 *  ����: ��
 *  ����: -1 ʧ�ܣ�>=0 ��������
 */
int JsGroup_GetNum(void);

/*
 *  ����: ��ָ����ŵ����ַ
 *  ����: �豸ID DpId >= 1
 *  ����: -1 ʧ��,>0 ���ַ
 */
int JsGroup_GetAddrByIndex(int Index);

/*
 *  ����: ��ָ����ŵ�DP��Ϣ
 *  ����: GAddr ���ַ��DpIndex DP���
 *  ����: NULL ʧ��
 */
cJSON* JsGroup_GetDpList(int GAddr);

/*
 *  ����: ������ַ
 *  ����: GAddr ���ַ,0:�Զ�����;>0,�½���ַ
 *  ����: -1 �Ѵ��ڣ�>0 ���ַ
 */
int JsGroup_AddAddr(int GAddr);

/*
 *  ����: ���DP
 *  ����: GAddr ���ַ,pDp Ҫ��ӵĽڵ�,pDp->DevId��pDp->DpDd���붼����0
 *  ����: -1 ʧ�ܣ�0 �ɹ�
 */
int JsGroup_AddDp(int GAddr,SGroup_Dp* pDp);

/*
 *  ����: ɾ����
 *  ����: GAddr ���ַ
 *  ����: -1 ʧ�ܣ�0 �ɹ�
 */
int JsGroup_DelAddr(int GAddr);

/*
 *  ����: ������ɾ��ָ���豸��Dp�˿�
 *  ����: GAddr ���ַ(0xFFFFFFFF,������);DevId �豸ID��DpId (0xFFFFFFFF ָ���豸������DP)
 *  ����: -1 ʧ�ܣ�0 �ɹ�
 */
int JsGroup_DelGroupDp(int GAddr,int DevId, int DpId);

/*
 *  ����: ����������ɾ��ָ���豸
 *  ����: DevId �豸ID
 *  ����: -1 ʧ�ܣ�0 �ɹ�
 */
int JsGroup_DelDev(int DevId);

/*
 *  ����: �����ļ�
 *  ����: �ļ���ַ
 *  ����: -1 ʧ�ܣ�0 �ɹ�
 */
int JSGroup_SaveFile(void);

/*
 *  ����: ���ļ�
 *  ����: �ļ���ַ
 *  ����: -1 ʧ�ܣ�0 �ɹ�
 */
int JsGroup_Open(char* sFile);

/*
 *  ����: �ر��ļ�
 *  ����: ��
 *  ����: -1 ʧ�ܣ�0 �ɹ�
 */
int JsGroup_Close(void);

#ifdef __cplusplus
    }
#endif


#endif


