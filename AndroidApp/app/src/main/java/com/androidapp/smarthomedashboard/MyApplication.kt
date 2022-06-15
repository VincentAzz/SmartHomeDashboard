package com.androidapp.smarthomedashboard

import android.annotation.SuppressLint
import android.app.Application
import android.content.Context
//全局获得Context
class MyApplication : Application() {
    companion object {
        @SuppressLint("StaticFieldLeak")
        lateinit var context: Context
    }

    override fun onCreate() {
        super.onCreate()
        context = applicationContext
    }
}