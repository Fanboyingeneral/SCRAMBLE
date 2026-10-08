package com.example.rubikscubesolverwithoutcamera

import android.graphics.Color

enum class CubeColor(val displayName: String, val displayColor: Int) {
    U("White", Color.WHITE),
    R("Blue", Color.BLUE),
    F("Red", Color.RED),
    D("Yellow", Color.YELLOW),
    L("Green", Color.GREEN),
    B("Orange", Color.parseColor("#FFA500")),
    UNASSIGNED("Gray", Color.LTGRAY)
}