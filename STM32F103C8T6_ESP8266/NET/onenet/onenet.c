/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"

//C库
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#include "led.h"

#define PROID		"111"

#define AUTH_INFO	"222"

#define DEVID		"333"


extern unsigned char esp8266_buf[128];
extern u8 alarmFlag;//是否报警的标志
extern u8 alarm_is_free;




//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	char ProductID[64];
	char Auth_Info[64];
	char DeviceID[64];
	
	u32 MCU_ID;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	MCU_ID = *(vu32*)(0x1FFFF7E8);	//获取MCU的唯一标识
	sprintf(ProductID,"%s_%X",PROID,MCU_ID);
	sprintf(Auth_Info,"%s_%X",AUTH_INFO,MCU_ID);
	sprintf(DeviceID,"%s_%X",DEVID,MCU_ID);
	
	
	UsartPrintf(USART_DEBUG,"OneNet_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s	--- 连接中....\r\n"
                        ,ProductID, Auth_Info, DeviceID);
	
	if(MQTT_PacketConnect(ProductID, Auth_Info, DeviceID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:DEBUG_LOG("Tips:	连接成功\r\n");status = 0;break;
					
					case 1:DEBUG_LOG("WARN:	连接失败：协议错误\r\n");break;
					case 2:DEBUG_LOG("WARN:	连接失败：非法的clientid\r\n");break;
					case 3:DEBUG_LOG("WARN:	连接失败：服务器失败\r\n");break;
					case 4:DEBUG_LOG("WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:DEBUG_LOG("WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					
					default:DEBUG_LOG("ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		DEBUG_LOG("WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}

//==========================================================
//	函数名称：	OneNet_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	topics：订阅的topic
//				topic_cnt：topic个数
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_SUBSCRIBE-需要重发
//
//	说明：		
//==========================================================
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	for(; i < topic_cnt; i++)
		DEBUG_LOG("Subscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

//==========================================================
//	函数名称：	OneNet_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_PUBLISH-需要重送
//
//	说明：		
//==========================================================
void OneNet_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	DEBUG_LOG("Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据 也就是*cmd
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	cJSON *json , *json_value, *json_value_LED, *json_value_BEEP;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				DEBUG_LOG("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

				MQTT_DeleteBuffer(&mqttPacket);									//删包
			}
		break;
			
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				DEBUG_LOG("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
				// 对数据包req_payload进行JSON格式解析
				json = cJSON_Parse(req_payload);
				if (!json)UsartPrintf(USART_DEBUG,"Error before: [%s]\n",cJSON_GetErrorPtr());
				else
				{
					/*
					json_value_LED = cJSON_GetObjectItem(json , "LED_SW");
					UsartPrintf(USART_DEBUG,"json_value: [%s]\n",json_value_LED->string);
					if(json_value_LED->valueint)//json_value > 0且为整形				
					{
						LED0 = 0;//打开LED0 
					}
					else
					{
						LED0 = 1;//关闭LED0 
					}
					
					json_value_BEEP = cJSON_GetObjectItem(json , "BEEP_SW");
					UsartPrintf(USART_DEBUG,"json_value: [%s]\n",json_value_BEEP->string);
					if(json_value_BEEP->valueint)//json_value > 0且为整形				
					{
						alarmFlag = 1;
					}
					else
					{
						alarmFlag = 0;
					}
					alarm_is_free=0;
					
					*/
					
					json_value = cJSON_GetObjectItem(json , "device");
					UsartPrintf(USART_DEBUG,"json_value: [%s]\n",json_value->string);
					UsartPrintf(USART_DEBUG,"json_value: [%s]\n",json_value->valuestring);
					if(strstr(json_value->valuestring,"LED") != NULL)
					{
						json_value = cJSON_GetObjectItem(json , "value");
						if(json_value->valueint)LED0 = 0;//开灯
						else LED0 = 1;//关灯
					}
					else
					{
						json_value = cJSON_GetObjectItem(json , "value");
						if(json_value->valueint)alarmFlag = 1;//打开报警器
						else alarmFlag = 0;//关闭报警器
						alarm_is_free=0;//上位机控制跟手动控制具有一样的优先级 优先于自动控制，因此要把alarm_is_free 归0
					}
				}
				cJSON_Delete(json);
			}
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				DEBUG_LOG("Tips:	MQTT Publish Send OK\r\n");
			
		break;
			
		case MQTT_PKT_PUBREC:														//发送Publish消息，平台回复的Rec，设备需回复Rel消息
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
			{
				DEBUG_LOG("Tips:	Rev PublishRec\r\n");
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					DEBUG_LOG("Tips:	Send PublishRel\r\n");
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														//收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp
			
			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
			{
				DEBUG_LOG("Tips:	Rev PublishRel\r\n");
				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					DEBUG_LOG("Tips:	Send PublishComp\r\n");
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
		
		case MQTT_PKT_PUBCOMP:														//发送Publish消息，平台返回Rec，设备回复Rel，平台再返回的Comp
		
			if(MQTT_UnPacketPublishComp(cmd) == 0)
			{
				DEBUG_LOG("Tips:	Rev PublishComp\r\n");
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				DEBUG_LOG("Tips:	MQTT Subscribe OK\r\n");
			else
				DEBUG_LOG("Tips:	MQTT Subscribe Err\r\n");
		
		break;
			
		case MQTT_PKT_UNSUBACK:														//发送UnSubscribe消息的Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
				DEBUG_LOG("Tips:	MQTT UnSubscribe OK\r\n");
			else
				DEBUG_LOG("Tips:	MQTT UnSubscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//搜索'}'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
