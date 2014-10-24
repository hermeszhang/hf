#ifndef __SERVER_SOCKET_TCP_H__
#define __SERVER_SOCKET_TCP_H__

#include <hsf.h>


U32 USER_FUNC sendTcpData(U8* sendBuf, U32 dataLen);
void USER_FUNC closeSocketFd(CLOSE_SOCKET_TYPE closeType);
void USER_FUNC tcpSocketServerInit(void);
void USER_FUNC afterGetServerAddr(SOCKET_ADDR* socketAddr);

void USER_FUNC deviceServerTcpThread(void);

#endif
