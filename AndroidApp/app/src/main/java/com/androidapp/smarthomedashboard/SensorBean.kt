package com.androidapp.smarthomedashboard

class SensorBean(
    val Hum: Int = 0,
    val Temp: Int = 0,
    val Light: Int = 0,
    val Led: Int = 0,
    val Beep: Int = 0,
) {
    override fun toString(): String {
        return "SensorBean(Hum=$Hum, " +
                "Temp=$Temp, " +
                "Light=$Light, " +
                "Led=$Led, " +
                "Beep=$Beep)"
    }
}
