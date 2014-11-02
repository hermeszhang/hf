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
#include "../inc/asyncMessage.h"
#include "../inc/socketSendList.h"



//global data
static int g_tcp_socket_fd = -1;
static hfthread_mutex_t g_tcp_socket_mutex;



static BOOL initTcpSockrtMutex(void)
{
	BOOL ret = TRUE;
	if((hfthread_mutext_new(&g_tcp_socket_mutex)!= HF_SUCCESS))
	{
		HF_Debug(DEBUG_ERROR, "failed to create TCP socketMutex");
		ret = FALSE;
	}
	return ret;
}


static void USER_FUNC tcpCreateSocketAddr(struct sockaddr_in* addr, SOCKET_ADDR* pSocketAddr)
{
	memset(addr, 0,  sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	//addr->sin_port = htons(pSocketAddr->port);
	//addr->sin_addr.s_addr = htonl(pSocketAddr->ipAddr);
	addr->sin_port = pSocketAddr->port;
	addr->sin_addr.s_addr = pSocketAddr->ipAddr;
}



static void USER_FUNC setSocketOption(S32 sockFd)
{
	S32 optData;

	
	optData = 1;
	if(setsockopt(sockFd, SOL_SOCKET,SO_KEEPALIVE,&optData,sizeof(optData))<0)
	{
		lumi_debug("set SO_KEEPALIVE fail\n");
	}
	
	optData = 60;//60s
	if(setsockopt(sockFd, IPPROTO_TCP,TCP_KEEPIDLE,&optData,sizeof(optData))<0)
	{
		lumi_debug("set TCP_KEEPIDLE fail\n");
	}
	
	optData = 6;
	if(setsockopt(sockFd, IPPROTO_TCP,TCP_KEEPINTVL,&optData,sizeof(optData))<0)
	{
		lumi_debug("set TCP_KEEPINTVL fail\n");
	}
	
	optData = 5;
	if(setsockopt(sockFd, IPPROTO_TCP,TCP_KEEPCNT,&optData,sizeof(optData))<0)
	{
		lumi_debug("set TCP_KEEPCNT fail\n");
	}
}



static void USER_FUNC setNonBlockingOption(S32 sockFd)
{
	S32 optData = 1;
	
	ioctlsocket(sockFd, FIONBIO, &optData);
}


#if 0
static BOOL USER_FUNC nonFatalError(void)
{
   int err = errno;
   
   return (err == EINPROGRESS || err == EAGAIN || err == EWOULDBLOCK || err == EINTR);
}
#endif


static BOOL USER_FUNC connectServerSocket(SOCKET_ADDR* pSocketAddr)
{
	struct sockaddr_in socketAddrIn;
	BOOL ret = TRUE;


	tcpCreateSocketAddr(&socketAddrIn, pSocketAddr);
	if(connect(g_tcp_socket_fd, (struct sockaddr *)&socketAddrIn, sizeof(socketAddrIn)) < 0)
	{
		ret = FALSE;
	}
	else
	{
		setNonBlockingOption(g_tcp_socket_fd);
	}
	lumi_debug("ip=0x%x, port=0x%x ret=%d\n", socketAddrIn.sin_addr.s_addr, socketAddrIn.sin_port, ret);
	
	return ret;
}


static void USER_FUNC tcpSocketInit(SOCKET_ADDR* pSocketAddr, S32* sockFd)
{
	struct sockaddr_in socketAddrIn;


	while (1)
	{
		*sockFd = socket(AF_INET, SOCK_STREAM, 0);
		if (*sockFd < 0)
		{
			msleep(1000);
			continue;
		}
		else
		{
			break;
		}
	}
	tcpCreateSocketAddr(&socketAddrIn, pSocketAddr);
	setSocketOption(*sockFd);
	lumi_debug("sockFd = %d \n", *sockFd);
}



static void USER_FUNC tcpSocketServerInit(void)
{
	SOCKET_ADDR socketAddr;

	
	getServerAddr(&socketAddr);
	if(g_tcp_socket_fd != -1)
	{
		close(g_tcp_socket_fd);
		g_tcp_socket_fd = -1;
	}
	tcpSocketInit(&socketAddr, &g_tcp_socket_fd);
	clearServerAesKey(FALSE);
	setDeviceConnectInfo(NEED_RECONN_BIT, FALSE);
}


static U8 USER_FUNC tcpSockSelect(struct timeval* pTimeout, S32 sockFd)
{
	fd_set fdRead;
	fd_set fdWrite;
	S32 ret;
	U8 sel= 0;

	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);
	if (sockFd != -1)
	{
		FD_SET(sockFd,&fdRead);
	}
	ret= select((sockFd + 1), &fdRead, &fdWrite, NULL, pTimeout);
	if (ret <= 0)
	{
		//return 0;
	}
	else
	{
		if (FD_ISSET(sockFd, &fdRead))
		{
			sel = SOCKET_READ_ENABLE;
		}
		if (FD_ISSET(sockFd, &fdRead))
		{
			sel &= SOCKET_WRITE_ENABLE;
		}
	}
	
	return sel;
}



static S32 USER_FUNC tcpSocketRecvData( S8 *buffer, S32 bufferLen, S32 socketFd)
{
	S32 recvCount;

	hfthread_mutext_lock(g_tcp_socket_mutex);
	recvCount = recv(socketFd, buffer, bufferLen, 0);
	lumi_debug("recv len=%d \n", recvCount);
	hfthread_mutext_unlock(g_tcp_socket_mutex);
	return recvCount;
}



static S32 USER_FUNC tcpSocketSendData(U8 *SocketData, S32 bufferLen, S32 socketFd)
{
	int sendCount;

	hfthread_mutext_lock(g_tcp_socket_mutex);
	sendCount = send(socketFd, SocketData, bufferLen, 0);
	hfthread_mutext_unlock(g_tcp_socket_mutex);
	lumi_debug("need send len=%d, send len=%d\n", bufferLen, sendCount);
	return sendCount;
}



static S8* USER_FUNC recvTcpData(U32* recvCount)
{
	S8* recvBuf;
	S32 count;

	recvBuf = getTcpRecvBuf(TRUE);
	count = tcpSocketRecvData(recvBuf, NETWORK_MAXRECV_LEN, g_tcp_socket_fd);
	//showHexData("Tcp recv data", (U8*)recvBuf, count);
	if(count <= 0 /*count == -1*/) //server socket closed
	{
		setDeviceConnectInfo(SERVER_CONN_BIT, FALSE);
		setDeviceConnectInfo(NEED_RECONN_BIT, TRUE);
		recvBuf = NULL;
	}
	else if(!checkRecvSocketData((U32)count, recvBuf))
	{
		recvBuf = NULL;
	}
	*recvCount = (U32)count;
	return recvBuf;
}



BOOL USER_FUNC sendTcpData(U8* sendBuf, U32 dataLen)
{
	tcpSocketSendData(sendBuf, (S32)dataLen, g_tcp_socket_fd);
	return TRUE;
}


static BOOL USER_FUNC checkTcpConnStatus(SOCKET_ADDR* pSocketAddr)
{
	BOOL needContinue = FALSE;


	if(!getDeviceConnectInfo(SERVER_CONN_BIT))
	{
		if(!getDeviceConnectInfo(DHPC_OK_BIT)) //not got IP from AP
		{
			msleep(3000);
			needContinue = TRUE;
		}
		else if(!getDeviceConnectInfo(BALANCE_CONN_BIT)) // not connect with balance server
		{
			if(connectServerSocket(pSocketAddr))
			{
				setDeviceConnectInfo(BALANCE_CONN_BIT, TRUE);
				insertLocalMsgToList(MSG_LOCAL_EVENT, NULL, 0, MSG_CMD_GET_SERVER_ADDR);
			}
			else
			{
				msleep(3000);
				needContinue = TRUE;
			}
		}
		else if(!getDeviceConnectInfo(SERVER_CONN_BIT) && getDeviceConnectInfo(SERVER_ADDR_BIT)) // get balance addr but not connect it
		{
			if(getDeviceConnectInfo(NEED_RECONN_BIT))
			{
				tcpSocketServerInit();
			}
			getServerAddr(pSocketAddr);
			if(connectServerSocket(pSocketAddr))
			{
				setDeviceConnectInfo(SERVER_CONN_BIT, TRUE);
				insertLocalMsgToList(MSG_LOCAL_EVENT, NULL, 0, MSG_CMD_REQUST_CONNECT);
			}
			else
			{
				msleep(3000);
				needContinue = TRUE;
			}
		}
	}
	
	return needContinue;
}



void USER_FUNC afterGetServerAddr(SOCKET_ADDR* socketAddr)
{
	hfthread_mutext_lock(g_tcp_socket_mutex);
	setServerAddr(socketAddr);
	setDeviceConnectInfo(SERVER_ADDR_BIT, TRUE);
	setDeviceConnectInfo(NEED_RECONN_BIT, TRUE);
	hfthread_mutext_unlock(g_tcp_socket_mutex);
}


void USER_FUNC deviceServerTcpThread(void)
{
	U32 recvCount;
	S8* recvBuf;
	struct timeval timeout;
	SOCKET_ADDR socketAddr;
	U8 selectRet;

	initTcpSockrtMutex();

	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	socketAddr.ipAddr = inet_addr(TCP_SERVER_IP);
	socketAddr.port = htons(TCP_SOCKET_PORT);

	tcpSocketInit(&socketAddr, &g_tcp_socket_fd);

	hfthread_enable_softwatchdog(NULL, 30); //Start watchDog
	while(1)
	{

		//lumi_debug(" deviceServerTcpThread \n");
		hfthread_reset_softwatchdog(NULL); //tick watchDog

		if(checkTcpConnStatus(&socketAddr))
		{
			continue;
		}
		selectRet = tcpSockSelect(&timeout, g_tcp_socket_fd);
		if((selectRet&SOCKET_READ_ENABLE) != 0) //check socket buf
		{
			recvBuf = recvTcpData(&recvCount);
			if(recvBuf != NULL)
			{
				insertSocketMsgToList(MSG_FROM_TCP, (U8*)recvBuf, recvCount, 0); // insert msg to msg list
			}
		}
		if((selectRet&SOCKET_WRITE_ENABLE) != 0)
		{
			sendSocketData(MSG_FROM_TCP);
		}
		msleep(100);
	}
}

#endif
