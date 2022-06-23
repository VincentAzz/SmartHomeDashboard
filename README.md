# SmartHomeDashboard
 基于STM32、ESP8266、EMQX和Android的智能家居系统

## 🏷️ 项目工作
硬件选型，硬件连线，传感器数据采集，数据封装上行，下行数据解析，服务器搭建，Android APP开发与调试

## 🏷️ 系统功能
模拟智能家居场景，包含环境测量、警报与照明控制功能。<br>
传感器测量环境数据（温度、湿度、光照度），实时监控并上行到服务器。当测量值超过设定阈值时触发蜂鸣器报警。<br>
用户可以通过Android客户端查看测量数据，控制警报与照明开关，也可通过实体按钮进行控制。<br>

## 🏷️ 报警逻辑

	if((humidityH < 90) && (temperatureH < 30) && (Light < 2000))alarmFlag = 0;
	else alarmFlag = 1;
    
正常范围：湿度<90% 且 温度<30C 且 光照度<2000lx<br>
触发警报：任意测量值超出正常范围则触发警报<br>

1：超出阈值或手动控制；0：未超出阈值或未手动控制
| <img src="https://user-images.githubusercontent.com/95619684/175338630-62d87ab8-a492-4ce5-a464-326d15e7ddef.png" width="200"/> |
|:---:|
| 报警逻辑 |




## 🏷️ 硬件设备
STM32F103C8T6开发板<br>
ESP826601S Wi-Fi模块<br>
ST-LINK V2 下载器<br>
CP2102 USB-TTL串口模块<br>
DHT11 温湿度传感器<br>
BHT1750 光照度传感器<br>
蜂鸣器<br>
LED<br>
开关<br>

## 🏷️ 开发环境与工具
### Windows 11:
Keil uVision5<br>
SSCOM 串口调试工具<br>
Android Studio<br>
MQTT X https://mqttx.app/zh <br>
EMQX消息面板 http://tools.emqx.io <br>
### wsl子系统(Ubuntu 20.04):
EMQX MQTT消息服务器 https://www.emqx.io/docs/zh/v4.4/ <br>

## 🏷️ 硬件连线
| <img src="https://user-images.githubusercontent.com/95619684/174008419-eba98939-f096-46cc-8981-ce57b219069b.png" width="600"/> |
|:---:|
| 硬件连线 |

## 🏷️ 系统框图
| <img src="https://user-images.githubusercontent.com/95619684/175328014-f4e55772-4e63-4e93-8b71-e890c042d333.jpg" width="600"/> |
|:---:|
| 系统框图 |



## 🏷️ Android 客户端：Dashboard
Android客户端可查看环境数据、控制警报与照明、手动连接与断开与服务器连接

### 查看数据与控制设备
| <img src="https://user-images.githubusercontent.com/95619684/174006773-f9d2d6a9-e7b6-4c27-b2b3-16e00ddc4b60.png" width="200"/> | <img src="https://user-images.githubusercontent.com/95619684/174006794-ca4a0b00-2f8c-48dc-b149-a0114600afbd.png" width="200"/> |
|:---:|:---:|
| 查看数据 | 控制设备 |

### 警报与通知
| <img src="https://user-images.githubusercontent.com/95619684/174006901-16c740cc-f712-4fca-8953-d54565af7238.png" width="200"/> |
|:---:|
| 警报与通知 |

### 连接与断开连接
| <img src="https://user-images.githubusercontent.com/95619684/174006123-44b62dc6-e98f-4d11-a751-73b6e7142316.png" width="200"/> | <img src="https://user-images.githubusercontent.com/95619684/174006523-4bb2224a-34fc-49b5-8216-70ad6d69a0e7.png" width="200"/> |
|:---:|:---:|
| 断开服务器 | 连接服务器 |

## 🏷️ 本地串口调试: SSCOM
| <img src="https://user-images.githubusercontent.com/95619684/174009050-d59fc82b-838e-40b0-9514-7aba948c9a75.png" width="600"/> |
|:---:|
| SSCOM |

## 🏷️ EMQX 服务器消息在线工具: http://tools.emqx.io

| <img src="https://user-images.githubusercontent.com/95619684/174007373-ec558390-a037-4b54-9bae-3cb608fbdeb5.png" width="1000"/> |
|:---:|
| EMQX 服务器消息在线工具 |

## 🏷️ Android Studio Logcat 日志信息

| <img src="https://user-images.githubusercontent.com/95619684/174008070-a225ca8e-e555-43ef-a667-a127cefef5a0.png" width="1000"/> |
|:---:|
| Logcat 日志信息 |





