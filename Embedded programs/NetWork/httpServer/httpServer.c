/**
 * @file	httpServer.c
 * @brief	HTTP Server Utilities	
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "httpServer.h"
#include "socket.h"
#include "webpage.h"
#include "net_conf.h"
#include "cmd.h"

char httpRecvBuffer[HTTP_RX_BUFEER_SIZE];
char httpTransBuffer[HTTP_TX_BUFEER_SIZE];
st_http_request *parsed_http_req = (st_http_request *)httpRecvBuffer;
uint8_t *parsed_http_res = (uint8_t *)httpTransBuffer;
extern CmdTypeDef command;
extern PtlTypeDef protocol;
extern uint32_t cmd_status;
uint8_t temp_buf[20];
char sel_opt[11] = "x_selected";
static void ExtractParameter(uint8_t *str, uint8_t*target);
void httpParseAndResponse(uint8_t sn, uint16_t rlen)
{
	*(((uint8_t *)httpRecvBuffer)+rlen) = '\0';
	parse_http_request(parsed_http_req, (uint8_t *)httpRecvBuffer);
	char *uri_ptr;
	uint32_t html_file_len;
	uint8_t  pr_i;
	uint8_t  send_times;
	uint16_t send_remain;

	char *sel_ptr;
	
	switch(parsed_http_req->METHOD)
	{
		case METHOD_ERR:
			memcpy(httpTransBuffer, ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
		  gen_send(sn, (uint8_t *)httpTransBuffer, sizeof(ERROR_REQUEST_PAGE));
			break;
		
		case METHOD_HEAD:
			break;
		
		case METHOD_GET:
			uri_ptr = (char *)(parsed_http_req->URI);
			if(!strcmp(uri_ptr,"/index.htm") || !strcmp(uri_ptr,"/") || !strcmp(uri_ptr,"/index.html"))
			{
				html_file_len = strlen(HtmlFile);
				make_http_response_head((char *)parsed_http_res, PTYPE_HTML, html_file_len);
				gen_send(sn, (uint8_t *)parsed_http_res, strlen((const char *)parsed_http_res));
				
				sprintf(httpTransBuffer, HtmlFile, NetConf.ip[0], NetConf.ip[1], NetConf.ip[2], NetConf.ip[3],
																					 LocalPort,
																					 NetConf.gw[0], NetConf.gw[1], NetConf.gw[2], NetConf.gw[3],
																					 NetConf.mac[0],NetConf.mac[1],NetConf.mac[2],NetConf.mac[3],NetConf.mac[4],NetConf.mac[5]);
				sel_opt[0] = protocol.romBuffer[ROM_SAMPLE_FREQ_TAB] + '0';
        sel_ptr = strstr(httpTransBuffer, sel_opt);
				sel_ptr[0+1] = ' ';
				
				send_times = html_file_len / HTTP_TX_BUFEER_SIZE;
				send_remain= html_file_len % HTTP_TX_BUFEER_SIZE;
				for(pr_i = 0; pr_i < send_times; pr_i++)
				{
					gen_send(sn, (uint8_t *)( httpTransBuffer + pr_i*HTTP_TX_BUFEER_SIZE ) , HTTP_TX_BUFEER_SIZE);
				}
				gen_send(sn, (uint8_t *)( httpTransBuffer + pr_i*HTTP_TX_BUFEER_SIZE ) , send_remain);
			}
			break;
			
		case METHOD_POST:
			memset(command.cmdBuffer, 0, CMD_BUF_LEN);
			command.cmdBuffer[0] = CMD_HEADER;
		  command.cmdBuffer[1] = 0x13;
		  command.cmdBuffer[2] = CMD_DEVICE_CONF;
			ExtractParameter(parsed_http_req->URI, "xsip");
			ExtractParameter(parsed_http_req->URI, "xsport");
		  ExtractParameter(parsed_http_req->URI, "xsgw");
		  ExtractParameter(parsed_http_req->URI, "xsmac");
		  protocol.recv_len = 0x13;
		  cmd_status = ParseCmd(&protocol, &command);
		
		  memset(command.cmdBuffer, 0, CMD_BUF_LEN);
			command.cmdBuffer[0] = CMD_HEADER;
		  command.cmdBuffer[1] = 0x04;
		  command.cmdBuffer[2] = CMD_DEVICE_SETTING;
		  ExtractParameter(parsed_http_req->URI, "fs");
		  protocol.recv_len = 0x04;
		  cmd_status += ParseCmd(&protocol, &command);
		
//		  if(cmd_status == CMD_CPT_OK)
//			{
//				sprintf(httpTransBuffer, ConfigTip, "DevConfig OK!");
//			}
//			else
//			{
//				sprintf(httpTransBuffer, ConfigTip, "DevConfig ERR!");
//			}
//		  gen_send(sn, (uint8_t *)httpTransBuffer, strlen(httpTransBuffer));
//			disconnect(sn);
			/* reset mcu*/
			NVIC_SetVectorTable(NVIC_VectTab_FLASH,0);
			ExecuteAPP(NVIC_VectTab_FLASH);
			break;
		default:
			break;
	}
	disconnect(sn);
	close(sn);
}

uint16_t str2num(uint8_t src[], uint8_t n)
{
	uint8_t i = 0;
	uint16_t temp_var = 0;
	uint8_t tv;
	for(; i < n; i++)
	{
		tv = src[i];
		tv -= '0';
		temp_var = temp_var*10 + tv;
	}
	return temp_var;
}

uint8_t char2hex(uint8_t c)
{
	if((c >= '0') && (c <= '9')) return (c-'0');
	if((c >= 'a') && (c <= 'f')) return (c-'a'+10);
	if((c >= 'A') && (c <= 'F')) return (c-'A'+10);
	return 16;
}

uint8_t str2hex(uint8_t src[], uint8_t n)
{
	uint8_t i = 0;
	uint8_t temp_var = 0;
	uint8_t tv;
	for(; i < n; i++)
	{
		tv = char2hex(src[i]);
		temp_var = temp_var*16 + tv;
	}
	return temp_var;
}




void ExtractParameter(uint8_t *str, uint8_t*target)
{
	char *sub;
	uint8_t tl;
	char *ptr;
	uint8_t target_len = strlen((const char *)target);
	uint16_t tport;
	
	sub = strstr((const char *)str, (const char *)target);
	if(!sub) return;
	sub = sub + target_len + 1;
	strncpy((char *)temp_buf, (const char *)sub, 18);
	
	if(!strcmp((const char *)target, (const char *)"xsip"))
	{
		ptr = strtok((char *)temp_buf, (const char *)".");
		tl = strlen(ptr);
		command.cmdBuffer[3] = str2num((uint8_t *)ptr,tl);
		
		ptr = strtok(NULL, (const char *)".");
		tl = strlen(ptr);
		command.cmdBuffer[4] = str2num((uint8_t *)ptr,tl);
		
		ptr = strtok(NULL, (const char *)".");
		tl = strlen(ptr);
		command.cmdBuffer[5] = str2num((uint8_t *)ptr,tl);
		
		ptr = strtok(NULL, (const char *)"&");
		tl = strlen(ptr);
		command.cmdBuffer[6] = str2num((uint8_t *)ptr,tl);
	}
	else if(!strcmp((const char *)target, (const char *)"xsport"))
	{
		ptr = strtok((char *)temp_buf, (const char *)"&");
		tl = strlen(ptr);
		tport = str2num((uint8_t *)ptr,tl);
		command.cmdBuffer[7] = (uint8_t)(tport&0x00ff);
		command.cmdBuffer[8] = (uint8_t)((tport&0xff00) >> 8);
	}	
	else if(!strcmp((const char *)target, (const char *)"xsgw"))
	{
		ptr = strtok((char *)temp_buf, (const char *)".");
		tl = strlen(ptr);
		command.cmdBuffer[9] = str2num((uint8_t *)ptr,tl);
		
		ptr = strtok(NULL, (const char *)".");
		tl = strlen(ptr);
		command.cmdBuffer[10] = str2num((uint8_t *)ptr,tl);
		
		ptr = strtok(NULL, (const char *)".");
		tl = strlen(ptr);
		command.cmdBuffer[11] = str2num((uint8_t *)ptr,tl);
		
		ptr = strtok(NULL, (const char *)"&");
		tl = strlen(ptr);
		command.cmdBuffer[12] = str2num((uint8_t *)ptr,tl);
	}
	else if(!strcmp((const char *)target, (const char *)"xsmac"))
	{
		ptr = strtok((char *)temp_buf, (const char *)"-");
		tl = strlen(ptr);
		command.cmdBuffer[13] = str2hex((uint8_t *)ptr,tl);
		
		ptr = strtok(NULL, (const char *)"-");
		tl = strlen(ptr);
		command.cmdBuffer[14] = str2hex((uint8_t *)ptr,tl);
		
		ptr = strtok(NULL, (const char *)"-");
		tl = strlen(ptr);
		command.cmdBuffer[15] = str2hex((uint8_t *)ptr,tl);

		ptr = strtok(NULL, (const char *)"-");
		tl = strlen(ptr);
		command.cmdBuffer[16] = str2hex((uint8_t *)ptr,tl);

		ptr = strtok(NULL, (const char *)"-");
		tl = strlen(ptr);
		command.cmdBuffer[17] = str2hex((uint8_t *)ptr,tl);

		ptr = strtok(NULL, (const char *)"&");
		tl = strlen(ptr);
		command.cmdBuffer[18] = str2hex((uint8_t *)ptr,tl);		
	}
	else if(!strcmp((const char *)target, (const char *)"fs"))
	{
		command.cmdBuffer[3] = temp_buf[0]-'0';
	}
}
