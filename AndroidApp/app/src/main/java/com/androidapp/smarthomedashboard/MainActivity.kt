package com.androidapp.smarthomedashboard

import android.os.Bundle
import android.view.Window
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowInsetsControllerCompat
import androidx.fragment.app.Fragment
import com.jaeger.library.StatusBarUtil

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        StatusBarUtil.setColor(this, resources.getColor(R.color.white),0)
        lightStatusBar(window)
        setFragment(DashboardFragment())
    }

    //设置显示Fragment
    private fun setFragment(fr : Fragment){
        val frag = supportFragmentManager.beginTransaction()
        frag.replace(R.id.fragmentContainer,fr)
        frag.commit()
    }

    private fun lightStatusBar(window: Window, isLight: Boolean = true) {
        val wic = WindowInsetsControllerCompat(window,window.decorView)
        wic.isAppearanceLightStatusBars = isLight
    }
}