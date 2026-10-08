package com.example.bluetoothtestapp

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.pm.PackageManager
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.app.ActivityCompat
import java.io.IOException
import java.util.*

class MainActivity : AppCompatActivity() {

    private lateinit var sendButton: Button
    private lateinit var statusTextView: TextView
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

        // Initialize Bluetooth
        val bluetoothManager = getSystemService(BLUETOOTH_SERVICE) as BluetoothManager
        bluetoothAdapter = bluetoothManager.adapter

        // Find UI elements
        sendButton = findViewById(R.id.send_button)
        statusTextView = findViewById(R.id.status_text_view)

        // Set button listener
        sendButton.setOnClickListener {
            // Run the Bluetooth logic in a background thread to avoid freezing the UI
            Thread {
                sendData("Z")
            }.start()
        }

        // Request permissions when the app starts
        requestBluetoothPermissions()
    }

    private fun sendData(dataToSend: String) {
        val deviceName = "HC-06"
        var targetDevice: BluetoothDevice? = null

        // Update UI on the main thread
        runOnUiThread { statusTextView.text = "Status: Checking permissions..." }
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            runOnUiThread { statusTextView.text = "Status: Error - Permission denied." }
            return
        }

        runOnUiThread { statusTextView.text = "Status: Finding device '$deviceName'..." }
        bluetoothAdapter.cancelDiscovery()
        val pairedDevices: Set<BluetoothDevice> = bluetoothAdapter.bondedDevices
        targetDevice = pairedDevices.find { it.name == deviceName }

        if (targetDevice == null) {
            runOnUiThread { statusTextView.text = "Status: Error - '$deviceName' not found. Please pair it first." }
            return
        }

        val sppUuid: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
        var socket: BluetoothSocket? = null

        try {
            runOnUiThread { statusTextView.text = "Status: Connecting..." }
            socket = targetDevice.createInsecureRfcommSocketToServiceRecord(sppUuid)
            socket.connect() // This is the blocking call that establishes the connection

            runOnUiThread { statusTextView.text = "Status: Connected! Sending data..." }
            val outputStream = socket.outputStream
            outputStream.write(dataToSend.toByteArray())
            outputStream.flush()

            // Keep the connection open for a moment
            Thread.sleep(500)

            Log.i("BLUETOOTH_TEST", "Data sent successfully.")
            runOnUiThread { statusTextView.text = "Status: Sent '$dataToSend' successfully!" }

        } catch (e: IOException) {
            Log.e("BLUETOOTH_TEST", "Connection failed", e)
            runOnUiThread { statusTextView.text = "Status: Error - ${e.message}" }
        } catch (e: InterruptedException) {
            Thread.currentThread().interrupt()
            Log.e("BLUETOOTH_TEST", "Thread interrupted", e)
        } finally {
            try {
                socket?.close()
                Log.i("BLUETOOTH_TEST", "Socket closed.")
            } catch (e: IOException) {
                Log.e("BLUETOOTH_TEST", "Could not close socket", e)
            }
        }
    }

    private fun requestBluetoothPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            requestBluetoothPermissionLauncher.launch(
                arrayOf(Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_CONNECT)
            )
        } else {
            if (!bluetoothAdapter.isEnabled) {
                Toast.makeText(this, "Please enable Bluetooth", Toast.LENGTH_LONG).show()
            }
        }
    }
}