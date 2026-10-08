package com.example.rubikscubesolverwithoutcamera

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.pm.PackageManager
import android.graphics.Color
import android.graphics.drawable.GradientDrawable
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.app.ActivityCompat
import androidx.core.view.setMargins
import okhttp3.*
import java.io.IOException
import java.util.*

class MainActivity : AppCompatActivity() {

    // --- UI Elements ---
    private lateinit var instructionTextView: TextView
    private lateinit var interactiveGridView: InteractiveGridView
    private lateinit var paletteContainer: LinearLayout
    private lateinit var nextFaceButton: Button
    private lateinit var solutionTextView: TextView
    private lateinit var startOverButton: Button
    private lateinit var retryConfigButton: Button
    private lateinit var sendStartCommandButton: Button
    private lateinit var solutionScreenContainer: View

    // --- State & Bluetooth ---
    private var selectedColor: CubeColor = CubeColor.UNASSIGNED
    private var currentFaceIndex = 0
    private val faceOrder = listOf(CubeColor.U, CubeColor.R, CubeColor.F, CubeColor.D, CubeColor.L, CubeColor.B)
    private val cubeState = mutableMapOf<CubeColor, Array<CubeColor>>()
    private var lastSolutionString: String? = null
    private val client = OkHttpClient()
    private lateinit var bluetoothAdapter: BluetoothAdapter
    private val requestBluetoothPermissionLauncher =
        registerForActivityResult(ActivityResultContracts.RequestMultiplePermissions()) { permissions ->
            if (permissions.entries.any { !it.value }) {
                Toast.makeText(this, "Bluetooth permission is required", Toast.LENGTH_LONG).show()
            }
        }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val bluetoothManager = getSystemService(BLUETOOTH_SERVICE) as BluetoothManager
        bluetoothAdapter = bluetoothManager.adapter

        // Initialize all UI elements
        instructionTextView = findViewById(R.id.instruction_text_view)
        interactiveGridView = findViewById(R.id.interactive_grid_view)
        paletteContainer = findViewById(R.id.palette_container)
        nextFaceButton = findViewById(R.id.next_face_button)
        solutionTextView = findViewById(R.id.solution_text_view)
        startOverButton = findViewById(R.id.start_over_button)
        retryConfigButton = findViewById(R.id.retry_config_button)
        sendStartCommandButton = findViewById(R.id.send_start_command_button)
        solutionScreenContainer = findViewById(R.id.solution_screen_container)

        setupPalette()
        setupListeners()
        updateUIForCurrentFace()
        requestBluetoothPermissions()
    }

    private fun setupListeners() {
        // ... (interactiveGridView and nextFaceButton listeners are unchanged)
        interactiveGridView.onFaceletClicked = { index ->
            if (selectedColor != CubeColor.UNASSIGNED) {
                interactiveGridView.setFaceletColor(index, selectedColor)
            } else {
                Toast.makeText(this, "Please select a color first", Toast.LENGTH_SHORT).show()
            }
        }
        nextFaceButton.setOnClickListener {
            val currentFaceColors = interactiveGridView.getFaceData().clone()
            if (currentFaceColors.any { it == CubeColor.UNASSIGNED }) {
                Toast.makeText(this, "Please fill all 9 cells", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            cubeState[faceOrder[currentFaceIndex]] = currentFaceColors
            currentFaceIndex++
            if (currentFaceIndex < faceOrder.size) {
                updateUIForCurrentFace()
            } else {
                solveCube()
            }
        }

        startOverButton.setOnClickListener {
            resetApp()
        }

        retryConfigButton.setOnClickListener {
            lastSolutionString?.let { solution ->
                Toast.makeText(this, "Retrying configuration...", Toast.LENGTH_SHORT).show()
                Thread { sendConfigurationSequence(solution) }.start()
            } ?: Toast.makeText(this, "No solution available to send", Toast.LENGTH_SHORT).show()
        }

        sendStartCommandButton.setOnClickListener {
            Toast.makeText(this, "Sending Start Command ('S')...", Toast.LENGTH_SHORT).show()
            Thread { sendStartCommand() }.start()
        }
    }

    private fun sendRequestToServer(cubeString: String) {
        val laptopIp = "172.20.10.12"
        val serverUrl = "http://$laptopIp:8080/$cubeString"
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
                            lastSolutionString = cleanSolution
                            val message = "Solution:\n$cleanSolution"
                            showFinalState(message)
                            // Automatically send the configuration after solving
                            Thread { sendConfigurationSequence(cleanSolution) }.start()
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

    // --- NEW BLUETOOTH PROTOCOL FUNCTIONS ---

    private fun sendConfigurationSequence(solution: String) {
        val deviceName = "HC-06"
        val dataToSend = (solution + "\n").toByteArray() // Add newline character

        // Establish connection and send data
        connectAndSendData(deviceName) { socket ->
            try {
                val outputStream = socket.outputStream
                // 1. Send the 'C' command
                outputStream.write('C'.code)
                outputStream.flush()
                Thread.sleep(100) // Short delay

                // 2. Send the solution string
                outputStream.write(dataToSend)
                outputStream.flush()
                Thread.sleep(500) // Delay to ensure transmission

                Log.i("BLUETOOTH_CONFIG", "Configuration sent successfully.")
                runOnUiThread {
                    solutionTextView.append("\n\nConfiguration sent to HC-06!")
                }
            } catch (e: Exception) {
                Log.e("BLUETOOTH_CONFIG", "Failed to send configuration", e)
                runOnUiThread {
                    solutionTextView.append("\n\nError sending configuration: ${e.message}")
                }
            }
        }
    }

    private fun sendStartCommand() {
        val deviceName = "HC-06"
        connectAndSendData(deviceName) { socket ->
            try {
                val outputStream = socket.outputStream
                // Send the 'S' command
                outputStream.write('S'.code)
                outputStream.flush()
                Thread.sleep(200)

                Log.i("BLUETOOTH_START", "Start command sent.")
                runOnUiThread {
                    solutionTextView.append("\n\n'S' command sent!")
                }
            } catch (e: Exception) {
                Log.e("BLUETOOTH_START", "Failed to send start command", e)
                runOnUiThread {
                    solutionTextView.append("\n\nError sending start command: ${e.message}")
                }
            }
        }
    }

    // --- GENERIC BLUETOOTH HELPER FUNCTION ---

    private fun connectAndSendData(deviceName: String, onConnected: (BluetoothSocket) -> Unit) {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            runOnUiThread { Toast.makeText(this, "Bluetooth permission denied.", Toast.LENGTH_SHORT).show() }
            return
        }

        bluetoothAdapter.cancelDiscovery()
        val targetDevice = bluetoothAdapter.bondedDevices.find { it.name == deviceName }

        if (targetDevice == null) {
            runOnUiThread { solutionTextView.append("\n\nError: Device '$deviceName' not found.") }
            return
        }

        var socket: BluetoothSocket? = null
        try {
            val sppUuid: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
            socket = targetDevice.createInsecureRfcommSocketToServiceRecord(sppUuid)
            socket.connect()
            onConnected(socket) // Execute the specific send logic
        } catch (e: IOException) {
            Log.e("BLUETOOTH_HELPER", "Connection or send error", e)
            runOnUiThread { solutionTextView.append("\n\nBT Error: ${e.message}") }
        } finally {
            try {
                socket?.close()
            } catch (e: IOException) {
                Log.e("BLUETOOTH_HELPER", "Could not close socket", e)
            }
        }
    }

    // ... (All other functions like showFinalState, resetApp, parsing, etc., are unchanged)

    private fun showFinalState(message: String) {
        solutionTextView.text = message
        solutionScreenContainer.visibility = View.VISIBLE
    }

    private fun resetApp() {
        currentFaceIndex = 0
        cubeState.clear()
        selectedColor = CubeColor.UNASSIGNED
        lastSolutionString = null
        solutionScreenContainer.visibility = View.GONE
        paletteContainer.visibility = View.VISIBLE
        nextFaceButton.visibility = View.VISIBLE
        interactiveGridView.visibility = View.VISIBLE
        nextFaceButton.text = "Next Face"
        updateUIForCurrentFace()
        highlightSelectedColor(View(this))
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

    private fun setupPalette() {
        val colorsToDisplay = CubeColor.values().filter { it != CubeColor.UNASSIGNED }
        for (color in colorsToDisplay) {
            val colorView = View(this)
            val size = (resources.displayMetrics.density * 48).toInt()
            val margin = (resources.displayMetrics.density * 4).toInt()
            val layoutParams = LinearLayout.LayoutParams(size, size)
            layoutParams.setMargins(margin)
            colorView.layoutParams = layoutParams
            val shape = GradientDrawable().apply {
                shape = GradientDrawable.RECTANGLE
                setColor(color.displayColor)
                cornerRadius = 8f
                setStroke(6, Color.TRANSPARENT)
            }
            colorView.background = shape
            colorView.tag = color
            colorView.setOnClickListener {
                selectedColor = it.tag as CubeColor
                highlightSelectedColor(it)
            }
            paletteContainer.addView(colorView)
        }
    }

    private fun highlightSelectedColor(selectedView: View) {
        for (i in 0 until paletteContainer.childCount) {
            val view = paletteContainer.getChildAt(i)
            val drawable = view.background as GradientDrawable
            if (view == selectedView) drawable.setStroke(8, Color.BLACK)
            else drawable.setStroke(6, Color.TRANSPARENT)
        }
    }

    private fun updateUIForCurrentFace() {
        val currentFace = faceOrder[currentFaceIndex]
        instructionTextView.text = "Input ${currentFace.displayName} Face"
        interactiveGridView.setFaceData(Array(9) { CubeColor.UNASSIGNED })
        if (currentFaceIndex == faceOrder.size - 1) {
            nextFaceButton.text = "Solve Cube"
        }
    }

    private fun solveCube() {
        val solverString = generateSolverString()
        if (solverString == null) {
            Toast.makeText(this, "Error generating solver string.", Toast.LENGTH_LONG).show()
            return
        }
        Log.i("SolverString", "Generated: $solverString")
        instructionTextView.text = "Solving..."
        paletteContainer.visibility = View.GONE
        nextFaceButton.visibility = View.GONE
        interactiveGridView.visibility = View.GONE
        sendRequestToServer(solverString)
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
}