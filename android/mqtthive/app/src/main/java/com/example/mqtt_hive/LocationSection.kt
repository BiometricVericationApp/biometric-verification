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
fun LocationSection(mqttManager: MqttImp) {
    val leftTopic = "sensor1/distance"
    var leftDistance by remember { mutableStateOf("") }
    DisposableEffect(Unit) {
        mqttManager.subscribeToTopic(leftTopic) { message ->
            leftDistance = message
        }
        onDispose {}
    }

    val rightTopic = "sensor2/distance"
    var rightDistance by remember { mutableStateOf("") }
    DisposableEffect(Unit) {
        mqttManager.subscribeToTopic(rightTopic) { message ->
            rightDistance = message
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
                            text = "Location",
                            fontSize = 30.sp,
                            fontWeight = FontWeight.ExtraBold,
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Image(
                            modifier = Modifier.size(36.dp),
                            painter = painterResource(id = R.drawable.ruler),
                            contentDescription = "Ruler",
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
                            painter = painterResource(id = R.drawable.left),
                            contentDescription = "Left arrow",
                        )
                        Image(
                            modifier = Modifier.size(24.dp),
                            painter = painterResource(id = R.drawable.right),
                            contentDescription = "Right arrow",
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
                            text = "$leftDistance cm",
                            fontSize = 18.sp,
                        )
                        Text(
                            text = "$rightDistance cm",
                            fontSize = 18.sp,
                        )
                    }
                }
            }
        }
    }
}
