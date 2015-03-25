/*
******************************
*Company:Lumitek
*Data:2011-01-03
*Author:Meiyusong
******************************
*/

#ifndef __LUMITEK_COMMON_CONFIG_H__
#define __LUMITEK_COMMON_CONFIG_H__

//������Կ���
#define LUMITEK_DEBUG_SWITCH

//�������֧�ֶϵ�����
//#define HTTP_DOWNLOAD_SUPPORT_RESUMING

//ͨ������BIN�ļ�����
#define DEVICE_UPGRADE_BY_DOWNLOAD_BIN

//ͨ������Config�ļ�����
//#define DEVICE_UPGRADE_BY_CONFIG

//ͨ������������������
//#define ENTER_UPGRADE_BY_AMARM

//UDP�������¼��̶�30��
#define LUM_UDP_HEART_INTERVAL_30S

//socket log ���index��ʶ
//#define LUN_SOCKET_SHOW_INDEX

#ifdef LUMITEK_DEBUG_SWITCH
//���淢�ͺͽ��յ�Socket��Ϣ��Flash
//#define SAVE_LOG_TO_FLASH

//���ڴ�ӡSOCKET��Ϣ
//#define LUM_UART_SOCKET_LOG

//ͨ��UDP ��ӡLOG��Ϣ
#define LUM_UDP_SOCKET_LOG
#endif

#define LUMITEK_DEBUG
#if defined(LUMITEK_DEBUG) && !defined(UART_NOT_SUPPORT)

#define lumi_debug(...)	HF_Debug(DEBUG_LEVEL_USER, __VA_ARGS__)

#define lumi_error(...)	HF_Debug(DEBUG_LEVEL_USER, "========> ERROR func=%s, (line = %d)", __FUNCTION__,__LINE__); \
						HF_Debug(DEBUG_LEVEL_USER, __VA_ARGS__)

#else
#define lumi_debug(...)
#define lumi_error(...)
#endif
#endif

