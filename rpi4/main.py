import paho.mqtt.client as mqtt
from cam import cam_inference, NO_LABEL
from predict import DecisionTreeClassifier

HEART_TOPIC = "sensor3/heart"
GALVANIC_TOPIC = "sensor3/galvanic"
ENCODING="utf-8"

class Predict:
    heart_lecture: str | None = None
    galvanic_lecture: str | None = None
    label: str | None = None
    confidence: str | None = None
    client = mqtt.Client

    def __init__(self) -> None:
        self.client = mqtt.Client()
        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message
        self.client.connect("192.168.33.213", 1883, 60)
        self.client.loop_forever()

    @staticmethod
    def _on_connect(client, userdata,  flags, rc):
        client.subscribe(HEART_TOPIC)
        client.subscribe(GALVANIC_TOPIC)

    def _on_message(self,  client, userdata, msg):
        update_func = self.update_heart if msg.topic == HEART_TOPIC else self.update_galvanic
        update_func(self.get_message_from_bytes(msg.payload))
        if self.should_take_pic():
            print("About to inference in cam")
            self.label, confidence = cam_inference()
            self.confidence = str(confidence)
            print(f"Got {self.label} prediction with ${self.confidence}")
            prediction: str = ""
            if self.label == NO_LABEL:
                print("No label detected")
                prediction = "Unknown"
            else:
                print("Detected someone")
                print("Calling model...")
                classifier = DecisionTreeClassifier()
                sample = self.get_sample()
                print(sample)
                prediction = classifier.predict(sample)[0]
            client.publish("rpi/prediction", prediction)
            self.reset_atributes()


    def update_heart(self, value: str):
        self.heart_lecture = value

    def update_galvanic(self, value: str):
        self.galvanic_lecture = value
    
    def should_take_pic(self) -> bool:
        return self.heart_lecture is not None and self.galvanic_lecture is not None

    @staticmethod
    def get_message_from_bytes(bytes: bytes) -> str:
        return bytes.decode(ENCODING)
    
    def get_sample(self) -> str:
        return f"{self.label.capitalize()},{self.confidence},{self.galvanic_lecture},{self.heart_lecture}"
    
    def reset_atributes(self):
        self.heart_lecture = None
        self.galvanic_lecture = None
        self.label = None
        self.confidence = None

if __name__ == '__main__':
    Predict()
