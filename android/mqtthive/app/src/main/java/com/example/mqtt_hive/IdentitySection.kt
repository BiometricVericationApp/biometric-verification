package com.example.mqtt_hive

import android.text.Layout.Alignment
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
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
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun IdentitySection(mqttManager: MqttImp) {
    val predictionTopic = "rpi/prediction"
    var person by remember { mutableStateOf("Unknown") }
    DisposableEffect(Unit) {
        mqttManager.subscribeToTopic(predictionTopic) { message ->
            person = message
        }
        onDispose {}
    }

    val lockedColor = Color.LightGray
    val unlockedColor = Color(0xFFB0FFB3)
    val color by remember(person) {
        mutableStateOf(
            if (person == "Unknown") lockedColor else unlockedColor
        )
    }

    val imgDrawable by remember(person) {
        mutableIntStateOf(
            if (person == "Unknown") R.drawable.lock else R.drawable.olock
        )
    }

    Row(
        modifier = Modifier
            .fillMaxWidth(),
    ) {
        Card(
            elevation = CardDefaults.cardElevation(
                defaultElevation = 4.dp,
            ),
            modifier = Modifier
                .fillMaxWidth()
                .padding(20.dp),
        ) {
            Box(
                modifier = Modifier
                    .background(color)
            ) {
                Column(
                    modifier = Modifier
                        .padding(vertical = 10.dp)
                        .fillMaxWidth()
                ) {
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(8.dp),
                        verticalAlignment = androidx.compose.ui.Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.Center,
                    ) {
                        Text(
                            text = "Identity",
                            fontSize = 30.sp,
                            fontWeight = FontWeight.ExtraBold,
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Image(
                            modifier = Modifier.size(36.dp),
                            painter = painterResource(id = R.drawable.man),
                            contentDescription = "User",
                        )
                    }
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(8.dp),
                        verticalAlignment = androidx.compose.ui.Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.SpaceAround,
                    ) {
                        Text(
                            text = person,
                            fontSize = 18.sp,
                            fontWeight = FontWeight.ExtraBold,
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Image(
                            modifier = Modifier.size(36.dp),
                            painter = painterResource(id = imgDrawable),
                            contentDescription = "Lock",
                        )
                    }
                }
            }
        }
    }
}