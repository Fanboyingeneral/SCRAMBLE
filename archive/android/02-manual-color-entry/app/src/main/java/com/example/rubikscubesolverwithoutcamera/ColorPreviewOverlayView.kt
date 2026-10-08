package com.example.rubikscubesolverwithoutcamera

import android.content.Context
import android.graphics.Canvas
import android.graphics.Paint
import android.graphics.RectF
import android.util.AttributeSet
import android.view.View
import kotlin.comparisons.minOf

/**
 * This view draws a semi-transparent 3x3 grid of colors on top of the camera preview.
 */
class ColorPreviewOverlayView(context: Context, attrs: AttributeSet?) : View(context, attrs) {

    private val faceletRects = Array(9) { RectF() }
    private var faceletColors = Array<CubeColor>(9) { CubeColor.UNASSIGNED }

    // Create semi-transparent paint objects for each color
    private val colorPaints = CubeColor.values().associateWith { color ->
        Paint().apply {
            this.color = color.displayColor
            this.alpha = 150 // Set transparency (0-255)
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
            // Don't draw anything for unassigned colors
            if (faceletColors[i] != CubeColor.UNASSIGNED) {
                val paint = colorPaints[faceletColors[i]]
                if (paint != null) {
                    canvas.drawRect(faceletRects[i], paint)
                }
            }
        }
    }

    /**
     * Updates the colors to be drawn on the overlay and redraws the view.
     */
    fun updateColors(newColors: Array<CubeColor>) {
        if (newColors.size == 9) {
            faceletColors = newColors
            invalidate() // Request a redraw
        }
    }
}