#ifndef __DEVICE_RN8209C_H__
#define __DEVICE_RN8209C_H__

#include <hsf.h>

#ifdef RN8209C_SUPPORT
//Uart define
#define RN8209C_UART_NO			0
#define RN8209C_UART_TIMEOUT	5000
#define RN9029C_MAX_DATA_LEN	100

//#define RN8209C_READ_DATA_TIMEOUT	8000
#define RN8209C_READ_DATA_TIMEOUT	5000




//------------------------------------------------------------------------
//				RN8209�Ĵ�������
//------------------------------------------------------------------------
//У������ͼ�������
#define RN8209C_SYSCON     0x00
#define RN8209C_EMUCON     0x01
#define RN8209C_HFConst    0x02
#define RN8209C_PStart     0x03
#define RN8209C_GPQA       0x05
#define RN8209C_PhsA       0x07
#define RN8209C_APOSA      0x0A
#define RN8209C_IARMSOS    0x0E
#define RN8209C_IBGain     0x10

//����������״̬�Ĵ���
#define RN8209C_PFCnt      0x20
#define RN8209C_IARMS      0x22
#define RN8209C_IBRMS      0x23
#define RN8209C_URMS       0x24
#define RN8209C_UFreq      0x25
#define RN8209C_PowerPA    0x26
#define RN8209C_PowerPB    0x27
#define RN8209C_PowerQ     0x28
#define RN8209C_EnergyP    0x29
#define RN8209C_EnergyP2   0x2A
#define RN8209C_EnergyQ    0x2B
#define RN8209C_EnergyQ2   0x2C
#define RN8209C_EMUStatus  0x2D

//�жϼĴ���
#define RN8209C_IE         0X40
#define RN8209C_IF         0X41
#define RN8209C_RIF        0X42

//ϵͳ״̬�Ĵ���
#define RN8209C_SysStatus  0x43
#define RN8209C_RData      0x44
#define RN8209C_WData      0x45
#define RN8209C_DeviceID   0x7F

//��������
#define RN8209C_EA		   0xEA

#define RN8209C_PATH_A	0x5A
#define RN8209C_PATH_B	0xA5
#define RN9208C_WRITE_EN		0xE5
#define RN8209C_WRITE_PROTECT	0xDC
#define RN8209C_CMD_RESET		0xFA



typedef struct
{
	int reco_irms; //������Чֵ
	int reco_urms; //��ѹ��Чֵ
	int reco_freq;//Ƶ��
	int reco_powerp;//�й�����
	int reco_powerq;//�޹�����
	int reco_energyp;//�й�����
	int reco_energyq;//�޹�����
} MeasureDataInfo;


void USER_FUNC rn8209cCreateThread(void);

#endif /* RN8209C_SUPPORT */

#endif

