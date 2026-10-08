package com.example.rubikscubesolverwithoutcamera

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.RectF
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import kotlin.comparisons.minOf

class InteractiveGridView(context: Context, attrs: AttributeSet?) : View(context, attrs) {

    private val faceletColors = Array(9) { CubeColor.UNASSIGNED }
    private val cellRects = Array(9) { RectF() }
    private var cellSize = 0f
    private var startX = 0f
    private var startY = 0f

    private val borderPaint = Paint().apply {
        color = Color.BLACK
        style = Paint.Style.STROKE
        strokeWidth = 8f
    }

    private val colorPaints = CubeColor.values().associateWith { color ->
        Paint().apply {
            this.color = color.displayColor
            style = Paint.Style.FILL
        }
    }

    var onFaceletClicked: ((Int) -> Unit)? = null

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh)
        val squareSize = minOf(w, h).toFloat()
        cellSize = squareSize / 3
        startX = (w - squareSize) / 2
        startY = (h - squareSize) / 2

        for (i in 0..8) {
            val row = i / 3
            val col = i % 3
            val left = startX + col * cellSize
            val top = startY + row * cellSize
            cellRects[i] = RectF(left, top, left + cellSize, top + cellSize)
        }
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        for (i in 0..8) {
            val paint = colorPaints[faceletColors[i]] ?: return
            canvas.drawRect(cellRects[i], paint)
            canvas.drawRect(cellRects[i], borderPaint)
        }
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        if (event.action == MotionEvent.ACTION_DOWN) {
            for (i in 0..8) {
                if (cellRects[i].contains(event.x, event.y)) {
                    onFaceletClicked?.invoke(i)
                    return true
                }
            }
        }
        return super.onTouchEvent(event)
    }

    fun setFaceletColor(index: Int, color: CubeColor) {
        if (index in 0..8) {
            faceletColors[index] = color
            invalidate()
        }
    }

    fun setFaceData(colors: Array<CubeColor>) {
        if (colors.size == 9) {
            for (i in 0..8) {
                faceletColors[i] = colors[i]
            }
            invalidate()
        }
    }

    fun getFaceData(): Array<CubeColor> {
        return faceletColors
    }
}