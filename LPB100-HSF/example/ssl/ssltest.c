/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *-pipe -fno-strict-aliasing -Wall -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith -std=gnu99 -ffunction-sections -fdata-sections -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Wmain -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wbad-function-cast -Wwrite-strings -Wsign-compare -Waggregate-return  -Wmissing-declarations -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wcast-align --param max-inline-insns-single=500
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <hsf.h>
#include <stdlib.h>
#include <string.h>

#include <cyassl/openssl/ssl.h>
#include <cyassl/internal.h>
#include <cyassl/cyassl_config.h>

#include "../example.h"

#if (EXAMPLE_USE_DEMO==SSL_TEST_DEMO)

const int hf_gpio_fid_to_pid_map_table[HFM_MAX_FUNC_CODE]=
{
	HF_M_PIN(2),	//HFGPIO_F_JTAG_TCK
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDO
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDI
	HF_M_PIN(5),	//HFGPIO_F_JTAG_TMS
	HFM_NOPIN,		//HFGPIO_F_USBDP
	HFM_NOPIN,		//HFGPIO_F_USBDM
	HF_M_PIN(39),	//HFGPIO_F_UART0_TX
	HF_M_PIN(40),	//HFGPIO_F_UART0_RTS
	HF_M_PIN(41),	//HFGPIO_F_UART0_RX
	HF_M_PIN(42),	//HFGPIO_F_UART0_CTS
	
	HF_M_PIN(27),	//HFGPIO_F_SPI_MISO
	HF_M_PIN(28),	//HFGPIO_F_SPI_CLK
	HF_M_PIN(29),	//HFGPIO_F_SPI_CS
	HF_M_PIN(30),	//HFGPIO_F_SPI_MOSI
	
	HFM_NOPIN,	//HFGPIO_F_UART1_TX,
	HFM_NOPIN,	//HFGPIO_F_UART1_RTS,
	HFM_NOPIN,	//HFGPIO_F_UART1_RX,
	HFM_NOPIN,	//HFGPIO_F_UART1_CTS,
	
	HF_M_PIN(43),	//HFGPIO_F_NLINK
	HF_M_PIN(44),	//HFGPIO_F_NREADY
	HF_M_PIN(45),	//HFGPIO_F_NRELOAD
	HF_M_PIN(7),	//HFGPIO_F_SLEEP_RQ
	HF_M_PIN(8),	//HFGPIO_F_SLEEP_ON
		
	HF_M_PIN(15),		//HFGPIO_F_WPS
	HFM_NOPIN,		//HFGPIO_F_RESERVE1
	HFM_NOPIN,		//HFGPIO_F_RESERVE2
	HFM_NOPIN,		//HFGPIO_F_RESERVE3
	HFM_NOPIN,		//HFGPIO_F_RESERVE4
	HFM_NOPIN,		//HFGPIO_F_RESERVE5
	
	HFM_NOPIN,	//HFGPIO_F_USER_DEFINE
};

/* return 1 is a ipaddress */
int addressis_ip(const char * ipaddr)
{
	char ii, ipadd;
	int i, j;
	
	ii=0;
	for (j= 0; j< 4; j++)
	{
		ipadd=0;
		for (i=0; i< 4; i++, ii++)
		{
			if (*(ipaddr+ii)=='.')
				if (i== 0)
					return 0;		//the first shall not be '.'
				else
				{
					ii++;
					break;			//this feild finished
				}
			else if ((i==3)&&(j!=3))	//not the last feild, the number shall less than 4 bits
				return 0;
			else if ((*(ipaddr+ii) > '9')||(*(ipaddr+ii) < '0'))
			{
				if ((*(ipaddr+ii) == '\0')&&(j==3)&&(i!=0))
				{
					break;
				}
				else
					return 0;			//pls input number
			}
			else
				ipadd= ipadd*10+(*(ipaddr+ii)-'0');
			if (ipadd > 255)
				return 0;
		}
	}
	return 1;
}

int tcp_connect_ssl_server(char *url)
{
	int fd;	
	struct sockaddr_in addr;
	char *addrp=url;
	
	if((memcmp(url, "HTTPS://", 8)==0)||(memcmp(url, "https://", 8)==0))
		addrp= (char *)(url+8);

	ip_addr_t dest_addr;
	if(is_ipaddress((const char *)(addrp)) !=1 )
	{
		if(netconn_gethostbyname((const char *)(addrp), &dest_addr) !=HF_SUCCESS)
			return -1;
	}
	else
		inet_aton((char *)(addrp), (ip_addr_t *) &dest_addr);
	
	memset((char*)&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(443);
	addr.sin_addr.s_addr=dest_addr.addr;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd<0)
		return -1;
	
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr))< 0)
	{
		close(fd);
		return -1;
	}
	
	return fd;
}

char ssl_url[101];
char ssl_recvbuf[1000];
static void my_ssl_test(char *url, char *sendbuf, int sendnum)//a SSL test
{
	InitMemoryTracker();//for debug, it can show how many memory used in SSL
	CyaSSL_Debugging_ON();//for debug

	CyaSSL_Init();
	CYASSL_METHOD*  method  = 0;
	CYASSL_CTX*     ctx     = 0;
	CYASSL*         ssl     = 0;
	int sockfd;
	
	method=CyaSSLv3_client_method();
	if (method == NULL)
		HF_Debug(DEBUG_LEVEL_LOW, "unable to get method");

	ctx = CyaSSL_CTX_new(method);
	if (ctx == NULL)
	{
		HF_Debug(DEBUG_LEVEL_LOW, "unable to get ctx");
		return;
	}

	CyaSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);//disable verify certificates
	
	ssl = CyaSSL_new(ctx);
	if (ssl == NULL)
	{
		HF_Debug(DEBUG_LEVEL_LOW, "unable to get SSL object");
		goto FREE_CTX;
	}

	sockfd=tcp_connect_ssl_server(url);
	if(sockfd<0)
	{
		HF_Debug(DEBUG_LEVEL_LOW, "create socket error");
		goto FREE_SSL;
	}
	
	CyaSSL_set_fd(ssl, sockfd);
	if (CyaSSL_connect(ssl) != SSL_SUCCESS)
 	{
		int  err = CyaSSL_get_error(ssl, 0);
		char buffer[80];
		HF_Debug(DEBUG_LEVEL_LOW, "err = %d, %s\n", err,CyaSSL_ERR_error_string(err, buffer));
		HF_Debug(DEBUG_LEVEL_LOW, "SSL_connect failed");
    	}

	if (CyaSSL_write(ssl, sendbuf, sendnum) != sendnum)
       	HF_Debug(DEBUG_LEVEL_LOW,"SSL_write failed");

	int recvlen;
	recvlen = CyaSSL_read(ssl, ssl_recvbuf, sizeof(ssl_recvbuf)-1);
	if (recvlen > 0)
	{
		HF_Debug(DEBUG_LEVEL_LOW,"Server response: recv start ----------------------------------------\n");
		CyaSSL_Debugging_OFF();
		hfuart_send(HFUART0, ssl_recvbuf, recvlen,1000);
		
		while (1) 
		{
			recvlen = CyaSSL_read(ssl, ssl_recvbuf, sizeof(ssl_recvbuf)-1);
			if (recvlen > 0) 
				hfuart_send(HFUART0, ssl_recvbuf, recvlen,1000);
			else
				break;
		}
		
		CyaSSL_Debugging_ON();
		HF_Debug(DEBUG_LEVEL_LOW,"\n---------------------------------------- recv End!\n");	
	}
	else if (recvlen < 0) 
	{
		int readErr = CyaSSL_get_error(ssl, 0);
		if (readErr != SSL_ERROR_WANT_READ)
			HF_Debug(DEBUG_LEVEL_LOW, "CyaSSL_read failed");
	}

FREE_SSL:
	CyaSSL_shutdown(ssl);
	CyaSSL_free(ssl);
FREE_CTX:
	CyaSSL_CTX_free(ctx);
	close(sockfd);
	
	CyaSSL_Debugging_OFF();//close debug
	ShowMemoryTracker();//peek into how memory was used
}

static USER_FUNC int set_ssl_addr(pat_session_t s,int argc,char *argv[],char *rsp,int len)
{
	if( 0 == argc )
	{
		hffile_userbin_read(0, ssl_url, 100);	
		sprintf(rsp, "%s=%s", rsp, ssl_url);
		return 0;
	}
	else if( 1 == argc )
	{
		if((strlen(argv[0]) > 1)&&(strlen(argv[0]) < 100))
		{
			hffile_userbin_write(0, argv[0], strlen(argv[0])+1);
			return 0;
		}
		else
			return -1;
	}
	else
		return -1;
		
}

const hfat_cmd_t user_define_at_cmds_table[]=
{
	{"SSLADDR", set_ssl_addr, "   AT+SSLADDR: Get/Set address for SSL.\r\n", NULL},//add a AT cmd for SSL
	{NULL,NULL,NULL,NULL} //the last item must be null
};

static int USER_FUNC uart_recv_callback(uint32_t event,char *data,uint32_t len,uint32_t buf_len)
{
	//HF_Debug(DEBUG_LEVEL_LOW,"[%d]uart recv %d bytes data %d\n",event,len,buf_len);
	if((memcmp(data, "POST", 4)==0)||(memcmp(data, "GET", 3)==0))
	{
		hffile_userbin_read(0, ssl_url, 100);	
		my_ssl_test(ssl_url, data, len);//do SSL Get/Post
		return 0;
	}
	return len;
}

static void show_reset_reason(void)
{
	uint32_t reset_reason=0;
	
	reset_reason = hfsys_get_reset_reason();
	
	if(reset_reason&HFSYS_RESET_REASON_ERESET)
	{
		u_printf("HFSYS_RESET_REASON_ERESET\n");
	}
	if(reset_reason&HFSYS_RESET_REASON_IRESET0)
	{
		u_printf("HFSYS_RESET_REASON_IRESET0\n");
	}
	if(reset_reason&HFSYS_RESET_REASON_IRESET1)
	{
		u_printf("HFSYS_RESET_REASON_IRESET1\n");
	}
	if(reset_reason==HFSYS_RESET_REASON_NORMAL)
	{
		u_printf("HFSYS_RESET_REASON_NORMAL\n");
	}
	if(reset_reason&HFSYS_RESET_REASON_WPS)
	{
		u_printf("HFSYS_RESET_REASON_WPS\n");
	}
	if(reset_reason&HFSYS_RESET_REASON_SMARTLINK_START)
	{
		u_printf("HFSYS_RESET_REASON_SMARTLINK_START\n");
	}
	if(reset_reason&HFSYS_RESET_REASON_SMARTLINK_OK)
	{
		u_printf("HFSYS_RESET_REASON_SMARTLINK_OK\n");
	}
	
	return;
}

int USER_FUNC app_main (void)
{
	time_t now=time(NULL);
	
	u_printf("[CALLBACK DEMO]sdk version(%s),the app_main start time is %d %s\n",hfsys_get_sdk_version(),now,ctime(&now));
	if(hfgpio_fmap_check(HFM_TYPE_LPB100)!=0)
	{
		while(1)
		{
			HF_Debug(DEBUG_ERROR,"gpio map file error\n");
			msleep(1000);
		}
		return 0;
	}

	show_reset_reason();
	
	while(!hfnet_wifi_is_active())
	{
		msleep(50);
	}
	
	if(hfnet_start_assis(ASSIS_PORT)!=HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start httpd fail\n");
	}
	
	//this is a new function, can define the stack size for UART thread
	if(hfnet_start_uart_ex(HFTHREAD_PRIORITIES_LOW,(hfnet_callback_t)uart_recv_callback, 1024+256)!=HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start uart fail!\n");
	}
	
	return 1;
}

#endif

