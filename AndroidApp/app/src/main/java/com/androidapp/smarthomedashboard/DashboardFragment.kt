package com.androidapp.smarthomedashboard

import android.annotation.SuppressLint
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Context.NOTIFICATION_SERVICE
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat
import androidx.core.view.isVisible
import androidx.fragment.app.Fragment
import com.alibaba.fastjson.JSON
import com.google.android.material.snackbar.Snackbar
import kotlinx.android.synthetic.main.fragment_dashboard.*
import org.eclipse.paho.android.service.MqttAndroidClient
import org.eclipse.paho.client.mqttv3.*
import java.time.LocalDateTime
import java.time.format.DateTimeFormatter

class DashboardFragment : Fragment() {

    //创建MQTT客户端
    private lateinit var mqttClient: MqttAndroidClient

    //服务器连接状态标志
    var connectedFlag: Boolean = false

    //服务器URL
    private val serverURI: String = "tcp://192.168.34.102:1883"
//    "tcp://broker.emqx.io:1883"
//    "http://broker.emqx.io:8083"
//    "tcp://mqtt.mqttemqx.xyz:1883"
//    "http://192.168.239.102:8083"

    //topic 话题
    private val subTopic: String = "/sub"
    private val pubTopic: String = "/pub"

    //message 下发消息
    private val msgLedOn: String = "{\"device\":\"LED\",\"value\":1}"
    private val msgLedOff: String = "{\"device\":\"LED\",\"value\":0}"
    private val msgBeepOn: String = "{\"device\":\"BEEP\",\"value\":1}"
    private val msgBeepOff: String = "{\"device\":\"BEEP\",\"value\":0}"

    //message
//    private val msgLedOn = "{\"LED_SW\": 1}"
//    private val msgLedOff = "{\"LED_SW\": 0}"
//    private val msgBeepOn = "{\"BEEP_SW\": 1}"
//    private val msgBeepOff = "{\"BEEP_SW\": 0}"

    //日志TAG
    companion object {
        const val TAG = "AndroidMqttClient"
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_dashboard, container, false)
    }

    @SuppressLint("SetTextI18n")
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        //注册通知
        createNotificationChannel()

        //页面元素初始化显示状态
        topAppBarDashboard.menu.findItem(R.id.icoOnline).isVisible = false
        topAppBarDashboard.menu.findItem(R.id.icoOffline).isVisible = true
        ledSwitch.isVisible = false
        alarmPanel.isVisible = false
        alarmOffText.isVisible = false
        connectState.isVisible = true

        //连接服务器 订阅话题
        connect(MyApplication.context)

        //手动连接与断开按钮
        topAppBarDashboard.setOnMenuItemClickListener {
            when (it.itemId) {
                //未连接服务器->手动连接
                R.id.icoOffline -> {
                    connect(MyApplication.context)//连接服务器
                    true
                }
                //已连接服务器->手动断开
                R.id.icoOnline -> {
                    disconnect()//断开连接
                    Snackbar.make(
                        requireActivity().findViewById(android.R.id.content),
                        "与服务器连接断开",
                        Snackbar.LENGTH_LONG
                    )
                        .setAction("重新连接") {
                            connect(MyApplication.context)//断开后重新连接服务器
                        }
                        .show()
                    true
                }
                else -> false
            }
        }

        //LED开关控制
        ledSwitch.setOnCheckedChangeListener { buttonView, isChecked ->
            if (connectedFlag) {
                if (isChecked) {
                    publish(subTopic, msgLedOn, 0, false)
                    //页面日志
                    ledStatusText.text = "照明：开"
                    val current = LocalDateTime.now()
                    tvSendLogTime.text = "$current  发送数据"
                    tvLogPanel3.text = "send to ${subTopic}:  $msgLedOn"
                } else {
                    publish(subTopic, msgLedOff, 0, false)
                    //页面日志
                    ledStatusText.text = "照明：关"
                    val current = LocalDateTime.now()
                    tvSendLogTime.text = "$current  发送数据"
                    tvLogPanel3.text = "send to ${subTopic}:  $msgLedOff"
                }
            } else {
                ledSwitch.isChecked = false
                Snackbar.make(
                    requireActivity().findViewById(android.R.id.content),
                    "未连接服务器",
                    Snackbar.LENGTH_LONG
                )
                    .setAction("连接") {
                        connect(MyApplication.context)//断开后提示重新连接服务器
                    }
                    .show()
            }
        }

        //警报开关控制
        alarmSwitch.setOnCheckedChangeListener { buttonView, isChecked ->
            if (connectedFlag) {
                if (isChecked) {
                    publish(subTopic, msgBeepOn, 0, false)
                    //页面日志
                    alarmStatusText.text = "警报：开"
                    val current = LocalDateTime.now()
                    tvSendLogTime.text = "$current  发送数据"
                    tvLogPanel3.text = "send to $subTopic:  $msgBeepOn"
                } else {
                    publish(subTopic, msgBeepOff, 0, false)
                    //页面日志
                    alarmStatusText.text = "警报：关"
                    val current = LocalDateTime.now()
                    tvSendLogTime.text = "$current  发送数据"
                    tvLogPanel3.text = "send to $subTopic:  $msgBeepOff"
                }
            } else {
                alarmSwitch.isChecked = false
                Snackbar.make(
                    requireActivity().findViewById(android.R.id.content),
                    "未连接服务器",
                    Snackbar.LENGTH_LONG
                )
                    .setAction("连接") {
                        connect(MyApplication.context)//断开后重新连接服务器
                    }
                    .show()
            }
        }
    }

    //设置topAppBar连接图标
    fun changeTopAppBarIcon() {
        if (connectedFlag) {
            topAppBarDashboard.menu.findItem(R.id.icoOnline).isVisible = true
            topAppBarDashboard.menu.findItem(R.id.icoOffline).isVisible = false
        } else {
            topAppBarDashboard.menu.findItem(R.id.icoOnline).isVisible = false
            topAppBarDashboard.menu.findItem(R.id.icoOffline).isVisible = true
        }
    }

    //注册通知
    private fun createNotificationChannel() {
        val name = "警报"
        val descriptionText = "触发警报"
        val importance = NotificationManager.IMPORTANCE_HIGH
        val channel = NotificationChannel("alarmNotification", name, importance).apply {
            description = descriptionText
        }
        val notificationManager: NotificationManager =
            activity?.getSystemService(NOTIFICATION_SERVICE) as NotificationManager
        notificationManager.createNotificationChannel(channel)
    }

    //创建通知
    private fun alarmNotification(title: String, content: String) {
        val intent = Intent(MyApplication.context, DashboardFragment::class.java).apply {
            flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
        }
        val pendingIntent: PendingIntent =
            PendingIntent.getActivity(MyApplication.context, 0, intent, 0)
        val builder = NotificationCompat.Builder(MyApplication.context, "alarmNotification")
            .setSmallIcon(R.drawable.ic_outline_error_outline_24)
            .setContentTitle(title)
            .setContentText(content)
            //通知优先级
            .setPriority(NotificationCompat.PRIORITY_MAX)
            .setCategory(NotificationCompat.CATEGORY_MESSAGE)
            .setContentIntent(pendingIntent)
            .setAutoCancel(true)
            .setVisibility(NotificationCompat.VISIBILITY_PUBLIC)
        //显示通知
        with(NotificationManagerCompat.from(MyApplication.context)) {
            notify(1, builder.build())
        }
    }


    //JSON解析与数据封装
    @SuppressLint("SetTextI18n")
    fun getSensorDataByJsonStr(topic: String?, message: MqttMessage?) {
        //message.payload: byte[] -> String
        val jsonStrMsg = String(message!!.payload)
        Log.d("message", jsonStrMsg + topic)
        val sensorBean: SensorBean = JSON.parseObject(jsonStrMsg, SensorBean::class.java)
        //日志
        Log.d("sensorBean", sensorBean.toString())
        Log.d("Hum", sensorBean.Hum.toString())
        Log.d("Temp", sensorBean.Temp.toString())
        Log.d("Light", sensorBean.Light.toString())
        Log.d("Led", sensorBean.Led.toString())
        Log.d("Beep", sensorBean.Beep.toString())
        //绑定数据
        tempData.text = "${sensorBean.Temp}℃"
        humiData.text = "${sensorBean.Hum}%"
        lightData.text = "${sensorBean.Light}lx"
        airData.text = "良好"
        //根据数据设置开关与文本
        ledSwitch.isVisible = true
        ledSwitch.isChecked = sensorBean.Led != 0
        alarmSwitch.isChecked = sensorBean.Beep != 0
        if (sensorBean.Beep != 0) {
            alarmPanel.isVisible = true
            alarmOffText.isVisible = false
        } else {
            alarmPanel.isVisible = false
            alarmOffText.isVisible = true
            alarmStatusText.text = "警报"
        }

        Log.d("----", "------------------------------------------------------")

        //页面日志
        val current = LocalDateTime.now()
        val currentTimeFormatted = current.format(DateTimeFormatter.ofPattern("HH:mm:ss"))
        sensorDataUpdateTime.text = "更新时间：$currentTimeFormatted"
        tvGetLogTime.text = "$current  接收数据"
        tvLogPanel1.text = "message from $pubTopic:  $jsonStrMsg"
        tvLogPanel2.text = "bean:  $sensorBean"

        //警报通知
        if (sensorBean.Beep != 0) {
            alarmNotification("警报", "$currentTimeFormatted 触发警报")
        }
    }

    //连接 MQTT 服务器
    fun connect(context: Context) {
        mqttClient = MqttAndroidClient(context, serverURI, "kotlin_client")
        //回调方法
        mqttClient.setCallback(object : MqttCallback {
            override fun messageArrived(topic: String?, message: MqttMessage?) {
                getSensorDataByJsonStr(topic, message)
            }

            override fun connectionLost(cause: Throwable?) {
                Log.d(TAG, "连接丢失 ${cause.toString()}")
                connectedFlag = false
                changeTopAppBarIcon()
            }

            override fun deliveryComplete(token: IMqttDeliveryToken?) {}
        })
        val options = MqttConnectOptions()
        try {
            mqttClient.connect(options, null, object : IMqttActionListener {
                @SuppressLint("SetTextI18n")
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(TAG, "服务器连接成功")
                    //连接成功标识
                    connectedFlag = true
                    connectState.isVisible = false
                    //页面日志
                    val current = LocalDateTime.now()
                    tvServerLogTime.text = "$current  服务器：已连接"
                    tvServerLogPanel.text = "URL: $serverURI  connectedFlag: $connectedFlag"
                    subscribe(pubTopic, 0)//订阅topic 获取数据
                    Snackbar.make(
                        activity!!.findViewById(android.R.id.content),
                        "服务器连接成功",
                        Snackbar.LENGTH_LONG
                    )
                        .show()
                    changeTopAppBarIcon()
                }

                @SuppressLint("SetTextI18n")
                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(TAG, "服务器连接失败")
                    connectedFlag = false
                    //页面日志
                    val current = LocalDateTime.now()
                    tvServerLogTime.text = "$current  服务器连接失败"
                    tvServerLogPanel.text = "URL: $serverURI  connectedFlag: $connectedFlag"
                    changeTopAppBarIcon()
                }
            })
        } catch (e: MqttException) {
            e.printStackTrace()
        }

    }

    //创建订阅
    fun subscribe(topic: String, qos: Int = 0) {
        try {
            mqttClient.subscribe(topic, qos, null, object : IMqttActionListener {
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(TAG, "订阅 $topic")
                    Log.d(TAG, "订阅成功")
                    Toast.makeText(MyApplication.context, "订阅 $topic", Toast.LENGTH_SHORT).show()
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(TAG, "订阅失败 $topic")
                }
            })
        } catch (e: MqttException) {
            e.printStackTrace()
        }
    }

    //取消订阅
    fun unsubscribe(topic: String) {
        try {
            mqttClient.unsubscribe(topic, null, object : IMqttActionListener {
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(TAG, "取消订阅： $topic")
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(TAG, "取消订阅失败： $topic")
                }
            })
        } catch (e: MqttException) {
            e.printStackTrace()
        }
    }

    //发布消息
    fun publish(topic: String, msg: String, qos: Int = 1, retained: Boolean = false) {
        try {
            val message = MqttMessage()
            message.payload = msg.toByteArray()
            message.qos = qos
            message.isRetained = retained
            mqttClient.publish(topic, message, null, object : IMqttActionListener {
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(TAG, "$msg published to $topic")
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(TAG, "Failed to publish $msg to $topic")
                }
            })
        } catch (e: MqttException) {
            e.printStackTrace()
        }
    }

    //断开MQTT连接
    fun disconnect() {
        try {
            mqttClient.disconnect(null, object : IMqttActionListener {
                @SuppressLint("SetTextI18n")
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(TAG, "断开连接")
                    connectedFlag = false
                    //页面日志
                    val current = LocalDateTime.now()
                    tvServerLogTime.text = "$current  服务器：断开连接"
                    tvServerLogPanel.text = "URL: $serverURI  connectedFlag: $connectedFlag"
                    changeTopAppBarIcon()
                    connectState.isVisible = true
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(TAG, "断开连接失败")
                    changeTopAppBarIcon()
                }
            })
        } catch (e: MqttException) {
            e.printStackTrace()
        }
    }
}

//JSON解析
//fun getSensorData(topic: String?, message: MqttMessage?) {
//    val jsonMsg = message!!.getPayload().toString(Charsets.UTF_8)
//    Log.d("message", String(message.payload))
////        val jsonMsg = message.toString()
//    Log.d("message", jsonMsg + topic)
//    val sensorBean: SensorBean = JSON.parseObject(jsonMsg, SensorBean::class.java)
//    Log.d("sensorBean", sensorBean.toString())
//    Log.d("Hum", sensorBean.Hum.toString())
//    Log.d("Temp", sensorBean.Temp.toString())
//    Log.d("Light", sensorBean.Light.toString())
//    Log.d("Led", sensorBean.Led.toString())
//    Log.d("Beep", sensorBean.Beep.toString())
//}