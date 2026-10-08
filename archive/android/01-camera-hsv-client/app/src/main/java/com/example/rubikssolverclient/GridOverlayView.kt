package com.example.rubikssolverclient

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import android.view.View
import kotlin.math.min

class GridOverlayView(context: Context, attrs: AttributeSet?) : View(context, attrs) {

    private val paint = Paint().apply {
        color = Color.WHITE
        style = Paint.Style.STROKE
        // Make the lines a bit thicker for better visibility
        strokeWidth = 8f
        setShadowLayer(12f, 0f, 0f, Color.BLACK)
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)

        val squareSize = min(width, height).toFloat()
        val startX = (width - squareSize) / 2
        val startY = (height - squareSize) / 2

        // --- NEW ---
        // Draw the outer boundary rectangle for the grid
        canvas.drawRect(startX, startY, startX + squareSize, startY + squareSize, paint)

        // Calculate the positions for the inner grid lines
        val line1 = squareSize / 3
        val line2 = squareSize * 2 / 3

        // Draw vertical grid lines
        canvas.drawLine(startX + line1, startY, startX + line1, startY + squareSize, paint)
        canvas.drawLine(startX + line2, startY, startX + line2, startY + squareSize, paint)

        // Draw horizontal grid lines
        canvas.drawLine(startX, startY + line1, startX + squareSize, startY + line1, paint)
        canvas.drawLine(startX, startY + line2, startX + squareSize, startY + line2, paint)
    }
}