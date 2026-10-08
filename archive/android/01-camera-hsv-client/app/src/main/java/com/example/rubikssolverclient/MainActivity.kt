package com.example.rubikssolverclient

import android.Manifest
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.Color
import android.graphics.Matrix
import android.graphics.Rect
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.camera.core.*
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import kotlin.comparisons.minOf

class MainActivity : AppCompatActivity() {

    // --- App State Management ---
    private enum class AppMode { CALIBRATING, SCANNING }
    private var currentMode = AppMode.CALIBRATING
    private var calibrationStep = 0
    private val calibrationOrder = listOf(CubeColor.U, CubeColor.R, CubeColor.F, CubeColor.D, CubeColor.L, CubeColor.B)
    private val referenceColors = mutableMapOf<CubeColor, FloatArray>() // Stores learned HSV values

    // --- Camera & UI ---
    private var imageCapture: ImageCapture? = null
    private lateinit var cameraExecutor: ExecutorService
    private lateinit var viewFinder: PreviewView
    private lateinit var captureButton: Button
    private lateinit var instructionTextView: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Link UI elements
        viewFinder = findViewById(R.id.viewFinder)
        captureButton = findViewById(R.id.capture_button)
        instructionTextView = findViewById(R.id.instruction_text_view)

        // Standard permission check and camera startup
        if (allPermissionsGranted()) {
            startCamera()
        } else {
            ActivityCompat.requestPermissions(
                this, REQUIRED_PERMISSIONS, REQUEST_CODE_PERMISSIONS)
        }

        captureButton.setOnClickListener { takePhoto() }
        cameraExecutor = Executors.newSingleThreadExecutor()

        // Start in calibration mode
        updateInstructionText()
    }

    private fun takePhoto() {
        val imageCapture = imageCapture ?: return
        imageCapture.takePicture(
            ContextCompat.getMainExecutor(this),
            object : ImageCapture.OnImageCapturedCallback() {
                override fun onCaptureSuccess(image: ImageProxy) {
                    val bitmap = imageProxyToBitmap(image)
                    // The action depends on the current mode
                    when (currentMode) {
                        AppMode.CALIBRATING -> handleCalibrationStep(bitmap)
                        AppMode.SCANNING -> handleScanningStep(bitmap) // Call the new function
                    }
                    image.close()
                }
                override fun onError(exc: ImageCaptureException) {
                    Log.e(TAG, "Photo capture failed: ${exc.message}", exc)
                }
            }
        )
    }

    // --- NEW: Calibration Logic ---
    private fun handleCalibrationStep(bitmap: Bitmap) {
        // In calibration, we only care about the center square of the grid
        val centerROI = calculateROIs(bitmap.width, bitmap.height)[4] // Index 4 is the center
        val averageColor = getAverageColor(bitmap, centerROI)

        // Convert the captured RGB color to HSV
        val hsv = FloatArray(3)
        Color.colorToHSV(averageColor, hsv)

        // Store this HSV value as the reference for the current color
        val colorToCalibrate = calibrationOrder[calibrationStep]
        referenceColors[colorToCalibrate] = hsv

        Log.i(TAG, "Calibrated ${colorToCalibrate.displayName} to H: ${hsv[0]}, S: ${hsv[1]}, V: ${hsv[2]}")
        Toast.makeText(this, "${colorToCalibrate.displayName} calibrated!", Toast.LENGTH_SHORT).show()

        // Move to the next step
        calibrationStep++
        if (calibrationStep < calibrationOrder.size) {
            // Still more colors to calibrate
            updateInstructionText()
        } else {
            // Calibration is finished, switch to scanning mode
            currentMode = AppMode.SCANNING
            Log.i(TAG, "--- CALIBRATION COMPLETE ---")
            Log.i(TAG, "Reference Colors: $referenceColors")
            updateInstructionText()
        }
    }
    /**
     * Analyzes a full face during the scanning phase.
     */
    private fun handleScanningStep(bitmap: Bitmap) {
        val faceletColors = mutableListOf<CubeColor>()
        // We get the ROIs for all 9 squares
        val rois = calculateROIs(bitmap.width, bitmap.height)

        // Loop through each of the 9 squares
        for (roi in rois) {
            val averageColor = getAverageColor(bitmap, roi)
            val hsv = FloatArray(3)
            Color.colorToHSV(averageColor, hsv)
            // For each square, find its closest color match from our references
            val identifiedColor = findClosestColor(hsv)
            faceletColors.add(identifiedColor)
        }

        // --- THIS IS THE KEY PART FOR TESTING ---
        // We log the results to Logcat so you can see if it's working.
        val colorNames = faceletColors.joinToString { it.displayName }
        Log.i(TAG, "Detected Face Colors: [ $colorNames ]")
        Toast.makeText(this, "Face scanned! Check Logcat.", Toast.LENGTH_SHORT).show()

        // In the final version, we would add these colors to our cube state
        // and move to the next face. For now, we just test.
    }

    // --- NEW: Update UI text based on current mode and step ---
    private fun updateInstructionText() {
        when (currentMode) {
            AppMode.CALIBRATING -> {
                val colorToCalibrate = calibrationOrder[calibrationStep]
                instructionTextView.text = "Point at the CENTER of the ${colorToCalibrate.displayName} face"
            }
            AppMode.SCANNING -> {
                // We will add the logic for scanning each of the 6 faces later
                instructionTextView.text = "Calibration done! Ready to scan."
            }
        }
    }


    // --- All other helper functions remain the same for now ---
    // (calculateROIs, imageProxyToBitmap, getAverageColor, startCamera, permissions, etc.)
    private fun calculateROIs(width: Int, height: Int): List<Rect> {
        val rois = mutableListOf<Rect>()
        val squareSize = minOf(width, height)
        val startX = (width - squareSize) / 2
        val startY = (height - squareSize) / 2
        val cellWidth = squareSize / 3
        val roiSize = cellWidth / 4
        for (row in 0..2) {
            for (col in 0..2) {
                val centerX = startX + (col * cellWidth) + (cellWidth / 2)
                val centerY = startY + (row * cellWidth) + (cellWidth / 2)
                rois.add(Rect(centerX - roiSize, centerY - roiSize, centerX + roiSize, centerY + roiSize))
            }
        }
        return rois
    }
    private fun imageProxyToBitmap(image: ImageProxy): Bitmap {
        val planeProxy = image.planes[0]
        val buffer = planeProxy.buffer
        val bytes = ByteArray(buffer.remaining())
        buffer.get(bytes)
        val initialBitmap = android.graphics.BitmapFactory.decodeByteArray(bytes, 0, bytes.size)
        val matrix = Matrix().apply { postRotate(image.imageInfo.rotationDegrees.toFloat()) }
        return Bitmap.createBitmap(initialBitmap, 0, 0, initialBitmap.width, initialBitmap.height, matrix, true)
    }
    private fun getAverageColor(bitmap: Bitmap, roi: Rect): Int {
        var redSum = 0L; var greenSum = 0L; var blueSum = 0L; var pixelCount = 0
        for (y in roi.top until roi.bottom) {
            for (x in roi.left until roi.right) {
                val color = bitmap.getPixel(x, y)
                redSum += Color.red(color); greenSum += Color.green(color); blueSum += Color.blue(color)
                pixelCount++
            }
        }
        if (pixelCount == 0) return Color.BLACK
        return Color.rgb((redSum / pixelCount).toInt(), (greenSum / pixelCount).toInt(), (blueSum / pixelCount).toInt())
    }

    /**
     * Compares a captured color to the learned reference colors and finds the closest match.
     * @param hsv The HSV values of the color to identify.
     * @return The CubeColor that is the nearest match.
     */
    private fun findClosestColor(hsv: FloatArray): CubeColor {
        var closestColor = CubeColor.UNKNOWN
        var minDistance = Float.MAX_VALUE

        // Loop through each of our learned reference colors
        for ((color, referenceHsv) in referenceColors) {
            // Calculate the distance in 3D color space
            // We give Hue more weight because it's the most important differentiator.
            val hueDistance = minOf(
                StrictMath.abs(hsv[0] - referenceHsv[0]),
                360 - StrictMath.abs(hsv[0] - referenceHsv[0])
            ) * 1.5f // Hue weight
            val saturationDistance = StrictMath.abs(hsv[1] - referenceHsv[1]) * 1.0f
            val valueDistance = StrictMath.abs(hsv[2] - referenceHsv[2]) * 1.0f

            val distance = (hueDistance * hueDistance) +
                    (saturationDistance * saturationDistance) +
                    (valueDistance * valueDistance)

            // If this color is closer than the best one we've found so far, update it
            if (distance < minDistance) {
                minDistance = distance
                closestColor = color
            }
        }
        return closestColor
    }
    private fun startCamera() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)
        cameraProviderFuture.addListener({
            val cameraProvider: ProcessCameraProvider = cameraProviderFuture.get()
            val preview = Preview.Builder().build().also { it.setSurfaceProvider(viewFinder.surfaceProvider) }
            imageCapture = ImageCapture.Builder().build()
            val cameraSelector = CameraSelector.DEFAULT_BACK_CAMERA
            try { cameraProvider.unbindAll(); cameraProvider.bindToLifecycle(this, cameraSelector, preview, imageCapture) }
            catch (exc: Exception) { Log.e(TAG, "Use case binding failed", exc) }
        }, ContextCompat.getMainExecutor(this))
    }
    private fun allPermissionsGranted() = REQUIRED_PERMISSIONS.all { ContextCompat.checkSelfPermission(baseContext, it) == PackageManager.PERMISSION_GRANTED }
    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_CODE_PERMISSIONS) {
            if (allPermissionsGranted()) { startCamera() }
            else { Toast.makeText(this, "Permissions not granted.", Toast.LENGTH_SHORT).show(); finish() }
        }
    }
    override fun onDestroy() { super.onDestroy(); cameraExecutor.shutdown() }
    companion object {
        private const val TAG = "CameraXApp"
        private const val REQUEST_CODE_PERMISSIONS = 10
        private val REQUIRED_PERMISSIONS = mutableListOf(Manifest.permission.CAMERA).toTypedArray()
    }
}

enum class CubeColor(val displayName: String) {
    U("White"),
    R("Blue"),
    F("Red"),
    D("Yellow"),
    L("Green"),
    B("Orange"),
    UNKNOWN("Unknown")
}