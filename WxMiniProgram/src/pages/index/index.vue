<template>
    <div class="wrapper">
        <!-- <div class="weather-header">
            <span>天气</span>

        </div> -->
        <div class="header-wrapper">
            <div class="header-title">
                <span>空气质量 {{ airText }}</span>
                <span>{{ area }} {{ city }} {{ district }}</span>
            </div>
            <div class="header-text">
                <span><span class="aqi-span">AQI</span> {{ airValue }}</span>
                <span>{{ temperature }}℃ {{ weather }}</span>
            </div>
            <!-- <div class="weather-advice">text</div> -->
            <view class="divide-line"></view>
            <button class="btn" @click="getUserLocation()">debugBtn: relocate</button>
            <button class="btn" @click="updateWeather()">debugBtn: updateWeather</button>
            <!-- <button class="btn" @click="updateSensor()">debugBtn: updateSensorData</button> -->
            <!-- <button class="btn" @click="clearLog()">debugBtn: clearLog</button> -->

        </div>

        <div class="body-wrapper">
            <div class="body">

                <div class="data-header"><span>环境数据</span></div>

                <div class="data-wrapper">
                    <div class="data-temp">
                        <img class="data-logo" src="/static/images/temp.png"/>
                        <div class="data-text">
                            <div class="data-title">温度</div>
                            <div class="data-value">{{temp}}</div>
                        </div>
                    </div>
                    <div class="data-humi">
                        <img class="data-logo" src="/static/images/humi.png"/>
                        <div class="data-text">
                            <div class="data-title">湿度</div>
                            <div class="data-value">{{humi}}</div>
                        </div>
                    </div>
                </div>

                <div class="data-wrapper">
                    <div class="data-light">
                        <img class="data-logo" src="/static/images/light.png"/>
                        <div class="data-text">
                            <div class="data-title">光照</div>
                            <div class="data-value">{{light}}</div>
                        </div>
                    </div>
                    <div class="data-air">
                        <img class="data-logo" src="/static/images/fire.png"/>
                        <div class="data-text">
                            <div class="data-title">可燃气体</div>
                            <div class="data-value"></div>
                        </div>
                    </div>
                </div>

                <div class="data-header"><span>设备控制</span></div>

                <div class="data-wrapper">
                    <div class="control-led">
                        <img class="data-logo" src="/static/images/led.png"/>
                        <div class="data-text">
                            <div class="data-title">LED</div>
                            <div class="data-value">
                                <switch class="led-sw" @change="onLedChange" :checked="led" color="#4785f4"/>
                            </div>
                        </div>
                    </div>
                    <div class="control-alarm">
                        <img class="data-logo" src="/static/images/alarm.png"/>
                        <div class="data-text">
                            <div class="data-title">报警器</div>
                            <div class="data-value">
                                <switch class="alarm-sw" @change="onAlarmChange" :checked="alarm" color="#4785f4"/>
                            </div>
                        </div>
                    </div>
                </div>

                <div class="data-header"><span>消息日志</span></div>

                <div class="data-wrapper">
                    <div class="logsList">
                        <!-- <div>
                            <ul class="container log-list">
                            <li v-for="(log, index) in logs" :class="{ red: aa }" :key="index" class="log-item">
                                <card :text="(index + 1) + ' . ' + log"></card>
                            </li>
                            </ul>
                        </div> -->

                        <div>
                            <ul class="container log-list">
                            <li v-for="(log, index) in logs" :class="{ red: aa }" :key="index" class="log-item">
                                <card :text="(index + 1) + ' . ' + log"></card>
                            </li>
                            </ul>
                        </div>
                    </div>
                </div>


            </div>
        </div>
    </div>
</template>
<!---------------------------------------------------------------->
<script>
// import { connect } from "mqtt"
import { connect } from 'mqtt/dist/mqtt.js'
//import { connect } from 'mqtt/mqtt.js'
// import { connect } from '@/utils/mqtt410.js'
// import { formatTime } from '@/utils/index'


import card from '@/components/card'

//配置mqtt服务器url 
// var mqtt = require('mqtt')
// import mqtt from '../../utils/mqtt401.js'
// var mqtt = require('../../utils/mqtt401')
var mqttURL = "wxs://mqtt.mqttemqx.xyz:8084/mqtt";
//var mqttURL = "wxs://mqtt.mqttemqx.xyz/mqtt";
//地理位置：经度 纬度 数据保留两位小数
var _longitude = 0;
var _latitude = 0;

//和风天气 key
var weatherKey = '0e7717d92a3a4e29b03b5721516a0169'

export default {
    data() {
        return {
            //模型数据
            client: {},
            temp: '',
            humi: '',
            light: '',
            led: 0,
            alarm: 0,
            area: '',
            city: '',
            district: '',
            airText: ' ',
            airValue: ' ',
            temperature: '正在更新 ',
            weather: '',
            // logsList:[],
            logs: []
            // mqttOptions: {
            //     protocolVersion: 4, //MQTT连接协议版本
            //     clientId: 'emqx_cloud_miniprogram',
            //     clean: true,
            //     password: '',
            //     username: '',
            //     reconnectPeriod: 1000, // 1000毫秒，两次重新连接之间的间隔
            //     connectTimeout: 30 * 1000, // 1000毫秒，两次重新连接之间的间隔
            //     resubscribe: true // 如果连接断开并重新连接，则会再次自动订阅已订阅的主题（默认true）
            // },
        };
    },

    components: {
        card
    },

    methods: {
        // TODO LED开关执行方法
        onLedChange(event) {
            var _this = this
            var switchState = event.mp.detail.value
            console.log(switchState)
            if (switchState) {
                console.log("下发指令：开灯")
                var msg = {"LED_SW": 1}
                _this.client.subscribe(
                    '/sub133',
                    msg,
                    function (error) {
                        if (!error) {
                            console.log("成功下发指令到设备端：开灯")
                        }
                    })
            } else {
                console.log("下发指令：关灯")
                _this.client.subscribe(
                    '/sub133',
                    '{"LED_SW":0}',
                    function (error) {
                        if (!error) {
                            console.log("成功下发指令到设备端：关灯")
                        }
                    })
            }
        },

        //TODO 报警器开关执行方法 
        onAlarmChange(event) {
            var _this = this;
            var switchState = event.mp.detail.value
            console.log(switchState)
            if (switchState) {
                console.log("下发指令：开启报警")
                _this.client.subscribe(
                    '/sub133',
                    '{"Beep_SW":1}',
                    function (error) {
                        if (!error) {
                            console.log("成功下发指令到设备端：开启报警")
                        }
                    })
            } else {
                console.log("下发指令：关闭报警")
                _this.client.subscribe(
                    '/sub133',
                    '{"Beep_SW":0}',
                    function (error) {
                        if (!error) {
                            console.log("成功下发指令到设备端：关闭报警")
                        }
                    })
            }
        },

        //GPS用户授权
        gpsAuthorize(){
            var _this = this
            wx.getSetting({
                success(res) {
                    if (!res.authSetting['scope.userLocation']) {
                        wx.authorize({
                            scope: 'scope.userLocation',
                            success() {
                                _this.getUserLocation()
                            }
                        })
                    }
                }
            })
        },

        //调用wxapi获取位置
        getUserLocation() {
            var _this = this
            wx.getLocation({
                type: 'wgs84',
                success(res) {
                    //获取经度 纬度 保留两位小数
                    _longitude = res.longitude.toFixed(2)
                    _latitude = res.latitude.toFixed(2)
                    _this.updateWeather()
                }
            })
        },

        //查询天气
        updateWeather() {
            var _this = this
            console.log(" Location: " + _longitude + " , " + _latitude)
            wx.request({
                url: `https://free-api.qweather.com/s6/weather/now?location=${_longitude},${_latitude}&key=${weatherKey}`,
                success(res) {
                    console.log(res.data)
                    console.log("updateWeather Successful")
                    
                    //取res.data响应数组的第0项中的basic,now部分
                    const{basic,now} = res.data.HeWeather6[0]
                    console.log(basic)
                    console.log(now)

                    _this.area=basic.admin_area
                    _this.city=basic.parent_city
                    _this.district=basic.location + "区"
                    _this.temperature=now.tmp
                    _this.weather=now.cond_txt
                    // _this.addToLog(basic)

                    //查询空气质量
                    wx.request({
                        url: `https://free-api.qweather.com/s6/air/now?location=${_this.city}&key=${weatherKey}`,

                        success(res) {
                            console.log(res.data)
                            console.log("updateAirQuality Successful")
                            
                            //解构数据
                            const{air_now_city} = res.data.HeWeather6[0]
                            const{aqi, qlty} = air_now_city
                            _this.airValue=aqi
                            _this.airText=qlty
                        },
                        fail: function (errInfo) {
                            console.info(errInfo)
                        }
                    })
                },
                fail: function (errInfo) {
                    console.info(errInfo)
                }
            })  
        },

        //显示日志
        // addlog(){
        //     const _this = this;
        //     var logs
        //     if (mpvuePlatform === 'my') {
        //         logs = mpvue.getStorageSync({key: 'logs'}).data || []
        //     } else {
        //         logs = mpvue.getStorageSync('logs') || []
        //     }
        //     _this.logs = logs.map(log => formatTime(new Date(log)))
        // },

        // addlog(obj){
        //     const _this = this;
        //     var logs = mpvue.getStorageSync('logs') || []
        //     _this.logs = logs.map(JSON.stringify(obj))
        //     mpvue.setStorageSync('logs', logs)
        //     console.log(logs)
        //     // _this.logs = logs.map(log => formatTime(new Date(log)))
        // },



        // addToLog(obj){
        //     var _this = this
        //     var logs
        //     // if (mpvuePlatform === 'my') {
        //         // logs = wx.getStorageSync({key: 'logs'}).data || []
        //         // console.log(1)
        //     // } else {
        //         logs = wx.getStorageSync('logs') || []
        //         // console.log(2)
        //     // }
        //     // console.log(3)
        //     _this.logs = logs.map(log => formatTime(new Date(log)) +": "+ JSON.stringify(obj))

        //     wx.setStorageSync('logs', logs)
        //     console.log(logs)
           
            
        // },

        //清除日志
        // clearLog(){
        //     var _this = this
        //     _this.logs = []
        //     wx.removeStorageSync('logs')
        //     wx.showToast({
        //             title: '日志清除成功',
        //             icon: 'success',
        //             duration: 1000,
        //             mask: true 
        //     })
        // }
    },

    created () {
        var _this = this
        _this.gpsAuthorize()//用户授权获取地理位置并首次获取天气
        _this.getUserLocation()
    },

    onShow() {
         var _this = this
        _this.client = connect(mqttURL)//连接mqtt服务器

        _this.client.on("connect",function () {
                console.log("成功连接 mqtt 服务器")
                _this.client.subscribe("/pub133",function (error) {
                        if (!error) {
                            console.log("成功订阅设备数据上行到小程序topic")
                        }
                    }
                )
            }
        )

        _this.client.subscribe("pub133",function (error) {
                if (!error) {
                    console.log("成功订阅设备数据上行到小程序topic")
                }
            }
        )

        _this.client.on(
            "message",
            // _this.getSensorData(topic, message),
            function (topic, message) {
                console.log('-----------------------------------------')
                console.log('设备数据上行 topic:' + topic)
                // console.log('JSON Hex Buffer:')
                // console.log(message)
                //JSON字符串转换为JSON对象以提取数据
                // console.log('转换为JSON对象:')
                var sensorData = JSON.parse(message)
                console.log(sensorData)
                console.log(sensorData.Hum)
                console.log(sensorData.Temp)
                console.log(sensorData.Light)
                console.log(sensorData.Led)
                console.log(sensorData.Beep)

                //获取数据 绑定数据
                _this.temp = sensorData.Temp + " ℃"
                _this.humi = sensorData.Hum + " %"
                _this.light = sensorData.Light.toFixed(0) + " lx"
                _this.led = sensorData.Led
                _this.alarm = sensorData.Beep
            }
        )
    }
}
</script>

<style lang="scss" scoped>
.wrapper {
    padding: 12px;
    background-color: #f1f2f6;
    .btn {
        margin-top: 16px;
        border-radius: 10px;
        border: 2px solid #505050;
        color: #000000;
        background-color: #ffffff;
    }

    .header-wrapper {
        // background-color: #4285F4;
        background-color: #ffffff;
        border-radius: 20px;
        color: #000000;
        // box-shadow: #d6d6d6 0 0 5px;
        padding: 15px 15px;

        .header-title {
            display: flex;
            justify-content: space-between;
        }

        .header-text {
            font-size: 32px;
            font-weight: 400;
            display: flex;
            justify-content: space-between;
        }

        .weather-advice {
            margin-top: 20px;
            font-size: 12px;
        }
    }

    .data-wrapper {
        margin-top: 8px;
        display: flex;
        justify-content: space-between;


        .data-temp {
            background-color: #fdefc2;
            width: 49%;
            height: 80px;
            border-radius: 20px;
            display: flex;
            justify-content: space-around;
            // padding: 0 10px;

            .data-logo {
                height: 32px;
                width: 32px;
                margin-top: 24px;
            }

            .data-text {
                margin-top: 15px;
                color: #202020;

                .data-title {
                    text-align: right;
                }

                .data-value {
                    font-size: 26px;
                }
            }
        }

        .data-humi {
            background-color: #d2e4fc;
            width: 49%;
            height: 80px;
            border-radius: 20px;
            display: flex;
            justify-content: space-around;
            // padding: 0 10px;

            .data-logo {
                height: 32px;
                width: 32px;
                margin-top: 24px;
            }

            .data-text {
                margin-top: 15px;
                color: #202020;

                .data-title {
                    text-align: right;
                }

                .data-value {
                    font-size: 26px;
                }
            }
        }

        .data-light {
            background-color: #d1f2da;
            width: 49%;
            height: 80px;
            border-radius: 20px;
            display: flex;
            justify-content: space-around;
            // padding: 0 10px;

            .data-logo {
                height: 30px;
                width: 30px;
                margin-top: 24px;
                margin-left: 5px;
            }

            .data-text {
                margin-top: 15px;
                color: #202020;

                .data-title {
                    text-align: right;
                }

                .data-value {
                    font-size: 26px;
                }
            }
        }

        .data-air {
            background-color: #fdd1cc;
            width: 49%;
            height: 80px;
            border-radius: 20px;
            display: flex;
            justify-content: space-around;
            // padding: 0 10px;

            .data-logo {
                height: 32px;
                width: 32px;
                margin-top: 24px;
            }

            .data-text {
                margin-top: 15px;
                color: #202020;

                .data-title {
                    text-align: right;
                }

                .data-value {
                    font-size: 26px;
                }
            }
        }

        .control-led {
            background-color: #d2e4fc;
            width: 49%;
            height: 150px;
            border-radius: 20px;
            display: flex;
            justify-content: space-around;
            // padding: 0 10px;

            .led-sw{
                margin-top: 24px;
                // margin-right: -10px;
            }

            .data-logo {
                height: 32px;
                width: 32px;
                margin-top: 24px;
            }

            .data-text {
                margin-top: 15px;
                color: #202020;

                .data-title {
                    text-align: right;
                }

                .data-value {
                    font-size: 26px;
                }
            }
        }

        .control-alarm {
            background-color: #fdd1cc;
            width: 49%;
            height: 150px;
            border-radius: 20px;
            display: flex;
            justify-content: space-around;
            // padding: 0 10px;

            .alarm-sw{
                margin-top: 24px;
                // margin-right: -10px;
            }

            .data-logo {
                height: 32px;
                width: 32px;
                margin-top: 24px;
            }

            .data-text {
                margin-top: 15px;
                color: #202020;

                .data-title {
                    text-align: right;
                }

                .data-value {
                    font-size: 26px;
                }
            }
        }


    }

    .aqi-span{
        font-size: 20px;
    }

    .weather-header {
        margin-left: 5px;
        // margin-top: 5px;
        margin-right: 10px;
        margin-bottom: 10px;
        font-size: 20px;
    }

    .data-header {
        margin-left: 5px;
        margin-top: 30px;
        margin-right: 10px;
        // margin-bottom: 5px;
        font-size: 20px;
    }

    //.divide-line {
    //    margin-top: 20px;
    //
    //    background: #e0e3da;
    //    width: 50%;
    //    height: 2px;
    //    display: flex;
    //    flex-direction: row;
    //
    //    justify-content: center;
    //}

    .divide-line {
        margin-top: 15px;
        background: #000000;
        width: 100%;
        height: 1px;
    }


}

// .logsList {
    // background-color: #d2e4fc;
    // width: 100%;
    // height: 300px;
    // border-radius: 20px;
    // display: flex;
    // padding: 20px 20px 20px 20px;
    // .data-text {
    //     margin: 15px;
    //     color: #202020;

    //     .data-value {
    //         font-size: 12px;
    //         // text-align: left;
    //     }
    // }
// }

.log-list {
    display: flex;
    flex-direction: column;
    padding: 2px;
}

.log-item {
    // margin: 10px;
    color: #202020;
    text-align: left;
    margin: 2px;
    font-size: 12px;
}

</style>
