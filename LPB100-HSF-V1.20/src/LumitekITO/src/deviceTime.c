/*
******************************
*Company:Lumitek
*Data:2014-10-07
*Author:Meiyusong
******************************
*/


#include "../inc/lumitekConfig.h"

#ifdef CONFIG_LUMITEK_DEVICE
#include <hsf.h>
#include <string.h>
#include <stdio.h>


#include "../inc/itoCommon.h"
#include "../inc/deviceTime.h"


void USER_FUNC test2(void)
{
    char *wday[] = {"������", "����һ", "���ڶ�", "������", "������", "������", "������"};
    time_t timep;
    struct tm *p_tm;
    timep = time(NULL);
    p_tm = localtime(&timep); /*��ȡ����ʱ��ʱ��*/
    lumi_debug("%d-%d-%d %s %d:%d:%d\n", (p_tm->tm_year+1900), 
		(p_tm->tm_mon+1),
		p_tm->tm_mday,
		wday[p_tm->tm_wday],
		p_tm->tm_hour,
		p_tm->tm_min,
		p_tm->tm_sec); 
}


#if 0
int tm_sec; /* ��Cȡֵ����Ϊ[0,59] */
int tm_min; /* �� - ȡֵ����Ϊ[0,59] */
int tm_hour; /* ʱ - ȡֵ����Ϊ[0,23] */
int tm_mday; /* һ�����е����� - ȡֵ����Ϊ[1,31] */
int tm_mon; /* �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11] */
int tm_year; /* ��ݣ���ֵ��1900��ʼ */
int tm_wday; /* ���ڨCȡֵ����Ϊ[0,6]������0���������죬1��������һ���Դ����� */
int tm_yday; /* ��ÿ���1��1�տ�ʼ�������Cȡֵ����Ϊ[0,365]������0����1��1�գ�1����1��2�գ��Դ����� */
int tm_isdst; /* ����ʱ��ʶ����ʵ������ʱ��ʱ��tm_isdstΪ������ʵ������ʱ�Ľ���tm_isdstΪ0�����˽����ʱ��tm_isdst()Ϊ����*/
long int tm_gmtoff; /*ָ�������ڱ���߶���ʱ����UTC����ʱ����������UTC����ʱ���ĸ�����*/
const char *tm_zone; /*��ǰʱ��������(�뻷������TZ�й�)*/

#endif
static void USER_FUNC getLocalTime(TIME_DATA_INFO *pTimeInfo)
{
	time_t curTime;
	struct tm *p_tm;


	curTime = time(NULL);
	p_tm = localtime(&curTime);

	memset(pTimeInfo, 0, sizeof(TIME_DATA_INFO));
	if(p_tm != NULL)
	{
		pTimeInfo->year = p_tm->tm_year + 1900;
		pTimeInfo->month = p_tm->tm_mon;
		pTimeInfo->day = p_tm->tm_mday;
		pTimeInfo->week = p_tm->tm_wday;
		pTimeInfo->hour = p_tm->tm_hour;
		pTimeInfo->minute = p_tm->tm_min;
		pTimeInfo->second = p_tm->tm_sec;
		pTimeInfo->dayCount = p_tm->tm_yday;
		lumi_debug("Cur Time= %d-%d-%d (%d) %d:%d:%d  (days=%d)\n",
			pTimeInfo->year,
			pTimeInfo->month,
			pTimeInfo->day,
			pTimeInfo->week,
			pTimeInfo->hour,
			pTimeInfo->minute,
			pTimeInfo->second,
			pTimeInfo->dayCount);
	}
}


static BOOL compareWeekData(U8 alarmWeek, U8 curWeek)
{
	U8 tem;
	BOOl ret = FALSE;

	tem = ((alarmWeek&0x3F)>1)|((alarmWeek&0x4F)>>6);

	if((tem&curWeek) > 0)
	{
		ret = TRUE;
	}
	lumi_debug("alarmWeek=0x%x, tem=0x%x ret=%d\n", alarmWeek, tem, ret);
	return ret;
}


static BOOL USER_FUNC compareAlarmTime(ALARM_DATA_INFO* pAlarmInfo, TIME_DATA_INFO* pCurTime)
{
	if(pAlarmInfo->repeatData.bActive == EVENT_INCATIVE)
	{
		return FALSE;
	}
	if((pAlarmInfo->repeatData&0x7F) > 0)
	{
		if(!compareWeekData(pAlarmInfo->repeatData, pCurTime->week)
		{
			return FALSE;
		}
	}
	if(pAlarmInfo->hourData != pCurTime->hour)
	{
		return FALSE;
	}
	if(pAlarmInfo->minuteData != pCurTime->minute)
	{
		return FALSE;
	}
	return TRUE;
}


static BOOL USER_FUNC checkAlarmEvent(TIME_DATA_INFO* pTimeInfo, SWITCH_ACTION* action)
{
	ALARM_DATA_INFO* pAlarmInfo;
	TIME_DATA_INFO timeInfo;
	U8 i;
	BOOL ret = FALSE;


	getLocalTime(&timeInfo);
	for(i=0; i<MAX_ALARM_COUNT; i++)
	{
		pAlarmInfo = getAlarmData(i);
		if(compareAlarmTime(pAlarmInfo, &timeInfo))
		{
			ret = TRUE;
			*action = pAlarmInfo->action;
			break;
		}
	}
	return TRUE;
}

#endif

