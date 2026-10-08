package com.example.rubikscubesolverwithoutcamera

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import android.view.View
import kotlin.comparisons.minOf

class GridOverlayView(context: Context, attrs: AttributeSet?) : View(context, attrs) {
    private val paint = Paint().apply {
        color = Color.BLACK
        style = Paint.Style.STROKE
        strokeWidth = 8f
        alpha = 180 // Slightly transparent
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        val squareSize = minOf(width, height).toFloat()
        val startX = (width - squareSize) / 2
        val startY = (height - squareSize) / 2

        canvas.drawRect(startX, startY, startX + squareSize, startY + squareSize, paint)
        val line1 = squareSize / 3
        val line2 = squareSize * 2 / 3
        canvas.drawLine(startX + line1, startY, startX + line1, startY + squareSize, paint)
        canvas.drawLine(startX + line2, startY, startX + line2, startY + squareSize, paint)
        canvas.drawLine(startX, startY + line1, startX + squareSize, startY + line1, paint)
        canvas.drawLine(startX, startY + line2, startX + squareSize, startY + line2, paint)
    }
}