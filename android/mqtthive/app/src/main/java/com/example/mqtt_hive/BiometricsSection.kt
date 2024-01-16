package com.example.mqtt_hive

import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun BiometricsSection(mqttManager: MqttImp) {
    val heartTopic = "sensor3/heart"
    var heartLecture by remember { mutableStateOf("")}
    DisposableEffect(Unit) {
        mqttManager.subscribeToTopic(heartTopic) { message ->
            heartLecture = message
        }
        onDispose {}
    }

    val rightTopic = "sensor3/galvanic"
    var galvanicLecture by remember { mutableStateOf("")}
    DisposableEffect(Unit) {
        mqttManager.subscribeToTopic(rightTopic) { message ->
            galvanicLecture = message
        }
        onDispose {}
    }



    Row(
        modifier = Modifier
            .fillMaxWidth(),
    ) {
        Card(
            elevation = CardDefaults.cardElevation(
                defaultElevation = 4.dp
            ),
            modifier = Modifier
                .fillMaxWidth()
                .padding(20.dp)
        ) {
            Box(
                modifier = Modifier
                    .background(
                        Color(0xFF73BBDB)
                    )
            ) {
                Column(
                    modifier = Modifier
                        .padding(vertical = 10.dp)
                        .fillMaxWidth(),
                ) {
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(8.dp),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.Center,
                    ) {
                        Text(
                            text = "Biometrics",
                            fontSize = 30.sp,
                            fontWeight = FontWeight.ExtraBold,
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Image(
                            modifier = Modifier.size(36.dp),
                            painter = painterResource(id = R.drawable.biometrics),
                            contentDescription = "Biometrics",
                        )
                    }
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(8.dp),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.SpaceAround,
                    ) {
                        Image(
                            modifier = Modifier.size(24.dp),
                            painter = painterResource(id = R.drawable.heart),
                            contentDescription = "Heart rate",
                        )
                        Image(
                            modifier = Modifier.size(24.dp),
                            painter = painterResource(id = R.drawable.resistance),
                            contentDescription = "Resistance",
                        )
                    }
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(8.dp),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.SpaceAround,
                    ) {
                        Text(
                            text = "$heartLecture BPM",
                            fontSize = 18.sp,
                        )
                        Text(
                            text = "$galvanicLecture V",
                            fontSize = 18.sp,
                        )
                    }
                }
            }
        }
    }
}
