package com.example.mqtt_hive

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

@Composable
fun BiometricUI(mqttManager: MqttImp) {
    Column(modifier = Modifier.fillMaxSize()) {
        BiometricVerificationSection()
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(8.dp),
            verticalArrangement = Arrangement.SpaceAround
        ) {
            LocationSection(mqttManager)
            BiometricsSection(mqttManager)
            IdentitySection(mqttManager)
        }
    }
}