package com.example.rubikscubesolverwithoutcamera

import android.content.Context
import android.graphics.Canvas
import android.graphics.Paint
import android.graphics.RectF
import android.util.AttributeSet
import android.view.View
import kotlin.comparisons.minOf

class ColorPreviewOverlayView(context: Context, attrs: AttributeSet?) : View(context, attrs) {

    private val faceletRects = Array(9) { RectF() }
    private var faceletColors = Array<CubeColor>(9) { CubeColor.UNKNOWN } // Changed here

    private val colorPaints = CubeColor.values().associateWith { color ->
        Paint().apply {
            this.color = color.displayColor
            alpha = 150 // Set transparency
            style = Paint.Style.FILL
        }
    }

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh)
        val squareSize = minOf(w, h).toFloat()
        val cellSize = squareSize / 3
        val startX = (w - squareSize) / 2
        val startY = (h - squareSize) / 2

        for (i in 0..8) {
            val row = i / 3
            val col = i % 3
            val left = startX + col * cellSize
            val top = startY + row * cellSize
            faceletRects[i] = RectF(left, top, left + cellSize, top + cellSize)
        }
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        for (i in 0..8) {
            if (faceletColors[i] != CubeColor.UNKNOWN) { // Changed here
                val paint = colorPaints[faceletColors[i]]
                if (paint != null) {
                    canvas.drawRect(faceletRects[i], paint)
                }
            }
        }
    }

    fun updateColors(newColors: Array<CubeColor>) {
        if (newColors.size == 9) {
            faceletColors = newColors
            invalidate() // Request a redraw
        }
    }
}