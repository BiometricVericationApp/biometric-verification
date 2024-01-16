package com.example.mqtt_hive

import com.hivemq.client.mqtt.mqtt5.Mqtt5BlockingClient
import com.hivemq.client.mqtt.mqtt5.Mqtt5Client
import com.hivemq.client.mqtt.mqtt5.message.publish.Mqtt5Publish

class MqttImp {
    private lateinit var mqttClient: Mqtt5Client

    init {
        setupMqttClient()
    }

    private fun setupMqttClient() {
        mqttClient = Mqtt5Client.builder()
            .identifier("android")
            .serverHost("192.168.33.213")
            .serverPort(1883)
            .buildBlocking()

        (mqttClient as Mqtt5BlockingClient).connect()
    }

    fun subscribeToTopic(topic: String, messageHandler: (String) -> Unit) {
        mqttClient.toAsync().subscribeWith()
            .topicFilter(topic)
            .callback { publish ->
                messageHandler(publish.payloadAsBytes.decodeToString())
            }
            .send()
    }
}