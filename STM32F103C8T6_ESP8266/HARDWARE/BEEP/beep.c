/*
 * @Author: your name
 * @Date: 2020-04-06 02:16:49
 * @LastEditTime: 2020-04-06 02:18:40
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \undefinedc:\Users\Administrator\Desktop\sunjianping\HARDWARE\BEEP\beep.c
 */

#include "beep.h"
		    
//BEEP IO初始化
void BEEP_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //BEEP-->PA.0 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.0
 GPIO_SetBits(GPIOA,GPIO_Pin_0);						 //PA.0 初始化为高电平
}
 
