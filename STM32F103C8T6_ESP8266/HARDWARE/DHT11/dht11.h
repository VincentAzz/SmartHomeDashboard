#ifndef __DHT11_H
#define __DHT11_H	 
#include "sys.h"
 
//IO方向设置
#define DHT11_IO_IN()  {GPIOA->CRH&=0XFFFFFFF0;GPIOA->CRH|=8;}
#define DHT11_IO_OUT() {GPIOA->CRH&=0XFFFFFFF0;GPIOA->CRH|=3;}
////IO操作函数											   
#define	DHT11_DQ_OUT PAout(8) //数据端口	PA8出方向 
#define	DHT11_DQ_IN  PAin(8)  //数据端口	PA8入方向


u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u8 *humiH,u8 *humiL,u8 *tempH,u8 *tempL);//读取温湿度
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11  
		 				    
#endif
