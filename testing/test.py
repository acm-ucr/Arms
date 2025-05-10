from queue import Queue
from threading import Thread
import cv2
from ultralytics import YOLO

# Shared queues
frame_queue = Queue()
result_queue = Queue()

# Fruit names to detect
fruit_classes = {"apple", "banana", "orange"}

# Load the YOLO model once
model = YOLO("yolov8n.pt")

# Thread 1: Capture frames
def capture_thread_func():
    cap = cv2.VideoCapture("http://10.43.0.59:7123/stream.mjpg")
    while True:
        ret, frame = cap.read()
        if not ret:
            continue
        frame_queue.put(frame)

# Thread 2: Analyze with YOLO
def analysis_thread_func():
    while True:
        frame = frame_queue.get()
        results = model(frame)
        result_queue.put((frame, results))

# Thread 3: Display the annotated frame
def output_thread_func():
    while True:
        frame, results = result_queue.get()
        annotated_frame = frame.copy()
        for result in results:
            for box in result.boxes:
                cls_id = int(box.cls.cpu().numpy()[0])
                conf = float(box.conf.cpu().numpy()[0])
                cls_name = model.names.get(cls_id, "unknown")
                if cls_name in fruit_classes:
                    x1, y1, x2, y2 = box.xyxy.cpu().numpy()[0]
                    cv2.rectangle(annotated_frame, (int(x1), int(y1)), (int(x2), int(y2)), (0,255,0), 2)
                    cv2.putText(annotated_frame, f"{cls_name} {conf:.2f}",
                                (int(x1), int(y1)-10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0,255,0), 2)
        cv2.imshow("Fruit Detection", annotated_frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

# Start the threads
if __name__ == '__main__':
    Thread(target=capture_thread_func, daemon=True).start()
    Thread(target=analysis_thread_func, daemon=True).start()
    Thread(target=output_thread_func, daemon=True).start()

    # Keep main thread running
    while True:
        pass
