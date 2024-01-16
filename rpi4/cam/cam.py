from collections import Counter
from edge_impulse_linux.image import ImageImpulseRunner
import os
import cv2
import time
import statistics

FRAME_DELAY = 0.2
OCCURENCE_THRESHOLD  = 10
occurrences = Counter()
confidences: dict[str, list[float]] = {}

dir_path = os.path.dirname(os.path.realpath(__file__))
model = "model.eim"
model_file = os.path.join(dir_path, model)

NO_LABEL = "No one from the allowed persons detected"

def cam_inference() -> tuple[str, float | None]:
    with ImageImpulseRunner(model_file) as runner:
        label = ""
        mean_confidence = None
        try:
            runner.init()
            camera = cv2.VideoCapture(0)
            ret = camera.read()[0]
            if ret:
                camera.release()
            else:
                raise Exception("Couldn't initialize selected camera")
                
            for res, _ in runner.classifier(0):
                if "bounding_boxes" in res["result"].keys():
                    try:
                        label = res["result"]["bounding_boxes"][0]["label"]
                        confidence = res["result"]["bounding_boxes"][0]["value"]
                        if label not in confidences.keys():
                            confidences[label] = []
                        confidences[label].append(confidence)
                    except IndexError:
                        label = NO_LABEL
                    finally:
                        occurrences[label] += 1
                        if occurrences[label] == OCCURENCE_THRESHOLD:
                            if label != NO_LABEL:
                                mean_confidence = statistics.mean(confidences[label])
                            break
                        time.sleep(FRAME_DELAY)

        finally:
            if (runner):
                    runner.stop()
            return label, mean_confidence

if __name__ == '__main__':
    label, confidence = cam_inference()
    print(label, confidence)
