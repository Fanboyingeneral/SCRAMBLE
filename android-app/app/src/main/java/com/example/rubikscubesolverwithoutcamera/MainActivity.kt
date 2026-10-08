package com.example.rubikscubesolverwithoutcamera

import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.Color
import android.graphics.Matrix
import android.graphics.Rect
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.camera.core.*
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import okhttp3.*
import java.io.IOException
import java.util.*
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import kotlin.math.pow
import kotlin.math.sqrt

class MainActivity : AppCompatActivity() {

    // --- UI Elements ---
    private lateinit var scanningUiContainer: View
    private lateinit var confirmationScreenContainer: View
    private lateinit var solutionScreenContainer: View

    // Scanning UI
    private lateinit var viewFinder: PreviewView
    private lateinit var colorPreviewOverlay: ColorPreviewOverlayView
    private lateinit var instructionTextView: TextView
    private lateinit var captureFaceButton: Button

    // Confirmation UI
    private lateinit var confirmationTitleTextView: TextView
    private lateinit var confirmationGridView: InteractiveGridView
    private lateinit var retakeFaceButton: Button
    private lateinit var acceptFaceButton: Button

    // Solution UI
    private lateinit var solutionTextView: TextView
    private lateinit var moveTrackerTextView: TextView
    private lateinit var startOverButton: Button
    private lateinit var programButton: Button
    private lateinit var sendNextMoveButton: Button
    private lateinit var executeSequenceButton: Button

    // --- State Management ---
    private var currentFaceIndex = 0
    private val faceOrder = listOf(CubeColor.U, CubeColor.R, CubeColor.F, CubeColor.D, CubeColor.L, CubeColor.B)
    private val cubeState = mutableMapOf<CubeColor, Array<CubeColor>>()
    private var lastDetectedColors = Array(9) { CubeColor.UNKNOWN }
    private val client = OkHttpClient()

    // --- Camera & Analysis ---
    private lateinit var cameraExecutor: ExecutorService
    private val referenceColorsRgb = mapOf(
        CubeColor.F to intArrayOf(201, 23, 37),
        CubeColor.B to intArrayOf(214, 69, 3),
        CubeColor.D to intArrayOf(167, 167, 2),
        CubeColor.L to intArrayOf(4, 172, 24),
        CubeColor.R to intArrayOf(15, 83, 163),
        CubeColor.U to intArrayOf(171, 167, 152)
    )

    // --- Bluetooth ---
    private lateinit var bluetoothAdapter: BluetoothAdapter
    private var solutionMoves = listOf<String>()
    private var currentMoveIndex = 0
    private val requestBluetoothPermissionLauncher =
        registerForActivityResult(ActivityResultContracts.RequestMultiplePermissions()) { permissions ->
            if (permissions.entries.any { !it.value }) {
                Toast.makeText(this, "Bluetooth permission is required", Toast.LENGTH_LONG).show()
            }
        }

    // Store locked-in colors when user presses "Capture Face"
    private var lockedInColors = Array(9) { CubeColor.UNKNOWN }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // --- Initialize ALL UI elements from the 3 containers ---
        // Scanning UI
        scanningUiContainer = findViewById(R.id.scanning_ui_container)
        viewFinder = findViewById(R.id.viewFinder)
        colorPreviewOverlay = findViewById(R.id.color_preview_overlay)
        instructionTextView = findViewById(R.id.instruction_text_view)
        captureFaceButton = findViewById(R.id.capture_face_button)

        // Confirmation UI
        confirmationScreenContainer = findViewById(R.id.confirmation_screen_container)
        confirmationTitleTextView = findViewById(R.id.confirmation_title_textview)
        confirmationGridView = findViewById(R.id.confirmation_grid_view)
        retakeFaceButton = findViewById(R.id.retake_face_button)
        acceptFaceButton = findViewById(R.id.accept_face_button)

        // Solution UI
        solutionScreenContainer = findViewById(R.id.solution_screen_container)
        solutionTextView = solutionScreenContainer.findViewById(R.id.solution_text_view)
        moveTrackerTextView = solutionScreenContainer.findViewById(R.id.move_tracker_textview)
        val finalButtonsContainer = solutionScreenContainer.findViewById<View>(R.id.final_buttons_container)
        startOverButton = finalButtonsContainer.findViewById(R.id.start_over_button)
        programButton = finalButtonsContainer.findViewById(R.id.program_button)
        sendNextMoveButton = finalButtonsContainer.findViewById(R.id.send_next_move_button)
        executeSequenceButton = finalButtonsContainer.findViewById(R.id.execute_sequence_button)

        // --- Setup ---
        cameraExecutor = Executors.newSingleThreadExecutor()
        val bluetoothManager = getSystemService(BLUETOOTH_SERVICE) as BluetoothManager
        bluetoothAdapter = bluetoothManager.adapter

        if (allPermissionsGranted()) {
            startCamera()
        } else {
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.CAMERA), 10)
        }

        setupListeners()
        updateUIForCurrentFace()
        requestBluetoothPermissions()
    }

    private fun setupListeners() {
        // --- SCANNING LISTENER ---
        captureFaceButton.setOnClickListener {
            if (lastDetectedColors.any { it == CubeColor.UNKNOWN }) {
                Toast.makeText(this, "Could not identify all colors clearly.", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            // Lock in the colors when the "Capture Face" button is clicked
            lockedInColors = lastDetectedColors.clone()

            // Switch to confirmation UI
            showConfirmationScreen()
        }

        // --- CONFIRMATION LISTENERS ---
        retakeFaceButton.setOnClickListener {
            // Just switch back to scanning UI without saving
            showScanningScreen()
        }

        acceptFaceButton.setOnClickListener {
            // Save the colors and move to the next step
            cubeState[faceOrder[currentFaceIndex]] = lockedInColors.clone() // Save the locked-in colors
            currentFaceIndex++

            if (currentFaceIndex < faceOrder.size) {
                updateUIForCurrentFace()
                showScanningScreen()
            } else {
                solveCube()
            }
        }

        // --- SOLUTION LISTENERS (with retry logic) ---
        startOverButton.setOnClickListener { resetApp() }

        programButton.setOnClickListener {
            Toast.makeText(this, "Sending Program Command ('P')...", Toast.LENGTH_SHORT).show()
            Thread { sendBluetoothCommand("P") }.start()
        }

        sendNextMoveButton.setOnClickListener {
            if (solutionMoves.isEmpty() || currentMoveIndex >= solutionMoves.size) {
                Toast.makeText(this, "No more moves to send.", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            var moveToSend = solutionMoves[currentMoveIndex]
            if (currentMoveIndex == solutionMoves.size - 1) {
                moveToSend += ")"
            }

            Toast.makeText(this, "Sending: $moveToSend", Toast.LENGTH_SHORT).show()
            Thread {
                val success = sendBluetoothCommand(moveToSend)
                if (success) {
                    // Only advance to the next move if the send was successful
                    runOnUiThread {
                        currentMoveIndex++
                        updateMoveTracker()
                    }
                }
            }.start()
        }

        executeSequenceButton.setOnClickListener {
            Toast.makeText(this, "Sending Execute Command ('S')...", Toast.LENGTH_SHORT).show()
            Thread { sendBluetoothCommand("S") }.start()
        }
    }

    // --- UI State Changers ---
    private fun showScanningScreen() {
        scanningUiContainer.visibility = View.VISIBLE
        confirmationScreenContainer.visibility = View.GONE
    }

    private fun showConfirmationScreen() {
        confirmationTitleTextView.text = "Confirm ${faceOrder[currentFaceIndex].displayName} Face"
        confirmationGridView.setFaceData(lockedInColors) // Display the locked-in colors
        scanningUiContainer.visibility = View.GONE
        confirmationScreenContainer.visibility = View.VISIBLE
    }

    // Modified Bluetooth function to return success/failure
    private fun sendBluetoothCommand(command: String): Boolean {
        val deviceName = "HC-05"
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            runOnUiThread { Toast.makeText(this, "Bluetooth permission denied.", Toast.LENGTH_SHORT).show() }
            return false
        }
        bluetoothAdapter.cancelDiscovery()
        val targetDevice = bluetoothAdapter.bondedDevices.find { it.name == deviceName }
        if (targetDevice == null) {
            runOnUiThread { moveTrackerTextView.text = "Status: Device '$deviceName' not found." }
            return false
        }
        var socket: BluetoothSocket? = null
        try {
            val sppUuid: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
            socket = targetDevice.createInsecureRfcommSocketToServiceRecord(sppUuid)
            socket.connect()
            val outputStream = socket.outputStream
            outputStream.write(command.toByteArray())
            outputStream.flush()
            Thread.sleep(200)
            Log.i("BLUETOOTH_SEND", "Sent: $command")
            return true // Return true on success
        } catch (e: Exception) {
            Log.e("BLUETOOTH_SEND", "Error sending command: $command", e)
            runOnUiThread { moveTrackerTextView.text = "Status: BT Error - ${e.message}" }
            return false // Return false on failure
        } finally {
            try { socket?.close() } catch (e: IOException) { Log.e("BLUETOOTH_SEND", "Could not close socket", e) }
        }
    }

    // --- All other functions (server requests, analysis, etc.) are unchanged ---
    private fun sendRequestToServer(cubeString: String) {
        val serverUrl = "http://172.20.10.12:8080/$cubeString"
        val request = Request.Builder().url(serverUrl).build()
        client.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                runOnUiThread { showFinalState("Connection Failed:\n${e.message}") }
            }
            override fun onResponse(call: Call, response: Response) {
                val rawHtml = response.body?.string()
                runOnUiThread {
                    if (response.isSuccessful && rawHtml != null) {
                        val cleanSolution = parseSolutionFromHtml(rawHtml)
                        if (cleanSolution != null) {
                            showFinalState("Solution:\n$cleanSolution")
                        } else {
                            showFinalState("Error: Could not parse solution from server.")
                        }
                    } else {
                        showFinalState("Error from server.")
                    }
                }
            }
        })
    }

    private fun showFinalState(message: String) {
        solutionScreenContainer.visibility = View.VISIBLE
        scanningUiContainer.visibility = View.GONE
        confirmationScreenContainer.visibility = View.GONE
        solutionTextView.text = message
        val cleanSolution = message.substringAfter(":\n").substringBefore(" (")
        solutionMoves = cleanSolution.split(" ").filter { it.isNotEmpty() }
        currentMoveIndex = 0
        updateMoveTracker()
    }

    private fun updateMoveTracker() {
        moveTrackerTextView.text = "Sent ${currentMoveIndex} / ${solutionMoves.size} moves"
        if (currentMoveIndex >= solutionMoves.size) {
            sendNextMoveButton.text = "All Moves Sent"
            sendNextMoveButton.isEnabled = false
        } else {
            sendNextMoveButton.text = "Send Next Move (${solutionMoves[currentMoveIndex]})"
            sendNextMoveButton.isEnabled = true
        }
    }

    private fun resetApp() {
        if (cameraExecutor.isShutdown) {
            cameraExecutor = Executors.newSingleThreadExecutor()
            if (allPermissionsGranted()) startCamera()
        }
        currentFaceIndex = 0
        cubeState.clear()
        solutionMoves = listOf()
        currentMoveIndex = 0
        solutionScreenContainer.visibility = View.GONE
        confirmationScreenContainer.visibility = View.GONE
        scanningUiContainer.visibility = View.VISIBLE
        updateUIForCurrentFace()
    }

    private fun startCamera() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)
        cameraProviderFuture.addListener({
            val cameraProvider: ProcessCameraProvider = cameraProviderFuture.get()
            val preview = Preview.Builder().build().also { it.setSurfaceProvider(viewFinder.surfaceProvider) }
            val imageAnalyzer = ImageAnalysis.Builder()
                .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
                .build()
                .also {
                    it.setAnalyzer(cameraExecutor) { imageProxy ->
                        processImage(imageProxy)
                    }
                }
            try {
                cameraProvider.unbindAll()
                cameraProvider.bindToLifecycle(this, CameraSelector.DEFAULT_BACK_CAMERA, preview, imageAnalyzer)
            } catch (exc: Exception) { Log.e("CameraX", "Use case binding failed", exc) }
        }, ContextCompat.getMainExecutor(this))
    }

    @SuppressLint("UnsafeOptInUsageError")
    private fun processImage(imageProxy: ImageProxy) {
        val bitmap = imageProxyToBitmap(imageProxy)
        imageProxy.close()
        val rois = calculateROIs(bitmap.width, bitmap.height)
        val detectedColors = Array(9) { CubeColor.UNKNOWN }
        for (i in rois.indices) {
            val avgColor = getAverageColor(bitmap, rois[i])
            detectedColors[i] = identifyColor(avgColor)
        }
        lastDetectedColors = detectedColors
        runOnUiThread { colorPreviewOverlay.updateColors(detectedColors) }
    }

    private fun identifyColor(rgb: IntArray): CubeColor {
        var minDistance = Double.MAX_VALUE
        var closestColor = CubeColor.UNKNOWN
        for ((colorEnum, refRgb) in referenceColorsRgb) {
            val distance = sqrt(
                (rgb[0] - refRgb[0]).toDouble().pow(2) +
                        (rgb[1] - refRgb[1]).toDouble().pow(2) +
                        (rgb[2] - refRgb[2]).toDouble().pow(2)
            )
            if (distance < minDistance) {
                minDistance = distance
                closestColor = colorEnum
            }
        }
        return closestColor
    }

    private fun solveCube() {
        cameraExecutor.shutdown()
        val solverString = generateSolverString()
        if (solverString == null) {
            Toast.makeText(this, "Error generating solver string.", Toast.LENGTH_LONG).show()
            return
        }
        Log.i("SolverString", "Generated: $solverString")
        instructionTextView.text = "Solving..."
        scanningUiContainer.visibility = View.GONE
        sendRequestToServer(solverString)
    }

    private fun parseSolutionFromHtml(html: String): String? {
        return try {
            val bodyStart = html.indexOf("<body>") + "<body>".length
            val bodyEnd = html.indexOf("</body>")
            if (bodyStart == -1 || bodyEnd == -1) return null
            html.substring(bodyStart, bodyEnd).trim()
        } catch (e: Exception) { null }
    }

    private fun requestBluetoothPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            requestBluetoothPermissionLauncher.launch(
                arrayOf(Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_CONNECT)
            )
        } else if (!bluetoothAdapter.isEnabled) {
            Toast.makeText(this, "Please enable Bluetooth", Toast.LENGTH_LONG).show()
        }
    }

    private fun updateUIForCurrentFace() {
        val currentFace = faceOrder[currentFaceIndex]
        instructionTextView.text = "Scan ${currentFace.displayName} Face"
        if (currentFaceIndex == faceOrder.size - 1) {
            captureFaceButton.text = "Capture and Solve"
        } else {
            captureFaceButton.text = "Capture Face"
        }
    }

    private fun generateSolverString(): String? {
        val builder = StringBuilder()
        for (face in faceOrder) {
            val faceColors = cubeState[face] ?: return null
            for (color in faceColors) {
                builder.append(color.name)
            }
        }
        return builder.toString()
    }

    private fun calculateROIs(width: Int, height: Int): List<Rect> {
        val rois = mutableListOf<Rect>()
        val squareSize = kotlin.comparisons.minOf(width, height)
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

    private fun getAverageColor(bitmap: Bitmap, roi: Rect): IntArray {
        var redSum = 0L; var greenSum = 0L; var blueSum = 0L; var pixelCount = 0
        for (y in roi.top until roi.bottom) {
            for (x in roi.left until roi.right) {
                val color = bitmap.getPixel(x, y)
                redSum += Color.red(color); greenSum += Color.green(color); blueSum += Color.blue(color)
                pixelCount++
            }
        }
        return if (pixelCount == 0) intArrayOf(0, 0, 0) else intArrayOf(
            (redSum / pixelCount).toInt(), (greenSum / pixelCount).toInt(), (blueSum / pixelCount).toInt()
        )
    }

    private fun imageProxyToBitmap(image: ImageProxy): Bitmap {
        val matrix = Matrix().apply { postRotate(image.imageInfo.rotationDegrees.toFloat()) }
        @SuppressLint("UnsafeOptInUsageError")
        val rotatedBitmap = Bitmap.createBitmap(image.toBitmap(), 0, 0, image.width, image.height, matrix, true)
        return rotatedBitmap
    }

    private fun allPermissionsGranted() = ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == 10 && allPermissionsGranted()) {
            startCamera()
        } else {
            Toast.makeText(this, "Camera permission is required.", Toast.LENGTH_SHORT).show()
            finish()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        cameraExecutor.shutdown()
    }
}
