#ifndef __LUMITEK_CONFIG_H__
#define __LUMITEK_CONFIG_H__

#define CONFIG_LUMITEK_DEVICE

//������Կ���
#define LUMITEK_DEBUG_SWITCH

//������ģ��ǽ��
//#define LPB100_DEVLOPMENT_BOARD

//ǽ���豸
#define DEEVICE_LUMITEK_P1

//�������֧�ֶϵ�����
//#define HTTP_DOWNLOAD_SUPPORT_RESUMING

//ͨ������BIN�ļ�����
#define DEVICE_UPGRADE_BY_DOWNLOAD_BIN

//ͨ������Config�ļ�����
//#define DEVICE_UPGRADE_BY_CONFIG

//ͨ������������������
//#define ENTER_UPGRADE_BY_AMARM

//֧���ⲿ����
#define EXTRA_SWITCH_SUPPORT

#ifdef LUMITEK_DEBUG_SWITCH
//���淢�ͺͽ��յ�Socket��Ϣ��Flash
#define SAVE_LOG_TO_FLASH

//ͨ��UDP ��ӡLOG��Ϣ
#define SEND_LOG_BY_UDP
#endif

#define LUMITEK_DEBUG
#ifdef LUMITEK_DEBUG

#define lumi_debug(...)	HF_Debug(DEBUG_LEVEL_USER, "%d========>", time(NULL)); \
						HF_Debug(DEBUG_LEVEL_USER, __VA_ARGS__)

#define lumi_error(...)	HF_Debug(DEBUG_LEVEL_USER, "%d========> ERROR func=%s, (line = %d)", time(NULL),__FUNCTION__,__LINE__); \
						HF_Debug(DEBUG_LEVEL_USER, __VA_ARGS__)

#else
#define lumi_debug(...)
#define lumi_error(...)
#endif

#endif
