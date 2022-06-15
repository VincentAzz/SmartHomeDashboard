#include "led.h"
#include "key.h"
#include "beep.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "usart.h"
#include "dht11.h"
#include "bh1750.h"
//#include "oled.h"
#include "exti.h"
#include "stdio.h"
#include "esp8266.h"
#include "onenet.h"

u8 alarmFlag = 0;//是否报警的标志
u8 alarm_is_free = 10;//报警器是否被手动操作，如果被手动操作即设置为0

u8 humidityH;	  //湿度整数部分
u8 humidityL;	  //湿度小数部分
u8 temperatureH;   //温度整数部分
u8 temperatureL;   //温度小数部分
float Light = 0; //光照度
u8 Led_Status = 0;

//extern char oledBuf[20];//
char PUB_BUF[256];//上传数据的buf

const char *devSubTopic[] = {"/sub"};
const char devPubTopic[] = "/pub";

u8 ESP8266_INIT_OK = 0;//esp8266初始化完成标志



 int main(void)
 {	
	unsigned short timeCount = 0;	//发送间隔变量
	unsigned char *dataPtr = NULL;
	//static u8 lineNow;
	Usart1_Init(115200);//debug串口
		DEBUG_LOG("\r\n");
		DEBUG_LOG("UART1初始化			[OK]");
	delay_init();	    	 //延时函数初始化	
		DEBUG_LOG("延时函数初始化			[OK]");
	//OLED_Init();
	//OLED_ColorTurn(0);//0正常显示，1 反色显示
  //OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	//OLED_Clear();
	 	//DEBUG_LOG("OLED1初始化			[OK]");
		//OLED_Refresh_Line("OLED");

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
		DEBUG_LOG("中断优先初始化			[OK]");
		//OLED_Refresh_Line("NVIC");

	LED_Init();		  	//初始化与LED连接的硬件接口
		DEBUG_LOG("LED初始化			[OK]");
		//OLED_Refresh_Line("Led");
	KEY_Init();          	//初始化与按键连接的硬件接口
		DEBUG_LOG("按键初始化			[OK]");
		//OLED_Refresh_Line("Key");
	EXTIX_Init();		//外部中断初始化
		DEBUG_LOG("外部中断初始化			[OK]");
		//OLED_Refresh_Line("EXIT");
	BEEP_Init();
		DEBUG_LOG("蜂鸣器初始化			[OK]");
		//OLED_Refresh_Line("Beep");
	DHT11_Init();
		DEBUG_LOG("DHT11初始化			[OK]");
		//OLED_Refresh_Line("DHT11");
  BH1750_Init();
		DEBUG_LOG("BH1750初始化			[OK]");
		//OLED_Refresh_Line("BH1750");
	Usart2_Init(115200);//stm32-8266通讯串口
		DEBUG_LOG("UART2初始化			[OK]");
		//OLED_Refresh_Line("Uart2");
	
		DEBUG_LOG("硬件初始化			[OK]");
		
		
	DEBUG_LOG("初始化ESP8266 WIFI模块...");
	if(!ESP8266_INIT_OK){
		//OLED_Clear();
		//sprintf(oledBuf,"Waiting For");
		//OLED_ShowString(16,0,(u8*)oledBuf,16);//8*16 “ABC”
		//sprintf(oledBuf,"WiFi");
		//OLED_ShowString(48,18,(u8*)oledBuf,16);//8*16 “ABC”
		//sprintf(oledBuf,"Connection");
		//OLED_ShowString(24,36,(u8*)oledBuf,16);//8*16 “ABC”
		//OLED_Refresh();
	}
	ESP8266_Init();					//初始化ESP8266
	//OLED_Clear();

	
	
	//sprintf(oledBuf,"Waiting For");
	//OLED_ShowString(16,0,(u8*)oledBuf,16);//8*16 “ABC”
	//sprintf(oledBuf,"MQTT Server");
	//OLED_ShowString(16,18,(u8*)oledBuf,16);//8*16 “ABC”
	//sprintf(oledBuf,"Connection");
	//OLED_ShowString(24,36,(u8*)oledBuf,16);//8*16 “ABC”
	//OLED_Refresh();	
	
//接入mqtt服务器
	while(OneNet_DevLink()){
		delay_ms(500);
	}		
	
	//OLED_Clear();	

	TIM2_Int_Init(4999,7199);
	TIM3_Int_Init(2499,7199);
	
	BEEP = 0;//鸣叫提示接入成功
	delay_ms(250);
	BEEP = 1;
	
	//订阅话题
	OneNet_Subscribe(devSubTopic, 1);
	
	while(1)
	{
		if(timeCount % 40 == 0)//1000ms / 25 = 40 一秒执行一次
		{
			/********** 温湿度传感器获取数据**************/
			DHT11_Read_Data(&humidityH,&humidityL,&temperatureH,&temperatureL);
			
			/********** 光照度传感器获取数据**************/
			if (!i2c_CheckDevice(BH1750_Addr))
			{
				Light = LIght_Intensity();
			}
			
			/********** 读取LED0的状态 **************/
			Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);
			
			/********** 报警逻辑 **************/
			if(alarm_is_free == 10)//报警器控制权是否空闲 alarm_is_free == 10 初始值为10
			{
				if((humidityH < 90) && (temperatureH < 30) && (Light < 2000))alarmFlag = 0;
				else alarmFlag = 1;
			}
			if(alarm_is_free < 10)alarm_is_free++;
			DEBUG_LOG("alarm_is_free = %d", alarm_is_free);
			DEBUG_LOG("alarmFlag = %d\r\n", alarmFlag);

			
			/********** 输出调试信息 **************/
			DEBUG_LOG(" | 湿度：%d.%d %% | 温度：%d.%d C | 光照度：%.1f lx | 指示灯：%s | 报警器：%s | ",humidityH,humidityL,temperatureH,temperatureL,Light,Led_Status?"关闭":"【启动】",alarmFlag?"【启动】":"停止");
		}
		if(++timeCount >= 200)	// 5000ms / 25 = 200 发送间隔5000ms
		{
			Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);//读取LED0的状态
			DEBUG_LOG("==================================================================================");
			DEBUG_LOG("发布数据 ----- OneNet_Publish");
			sprintf(PUB_BUF,"{\"Hum\":%d.%d,\"Temp\":%d.%d,\"Light\":%.1f,\"Led\":%d,\"Beep\":%d}",
				humidityH,humidityL,temperatureH,temperatureL,Light,Led_Status?0:1,alarmFlag);
			OneNet_Publish(devPubTopic, PUB_BUF);
			DEBUG_LOG("==================================================================================");
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		delay_ms(10);
	}
}

