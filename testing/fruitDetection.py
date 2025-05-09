import cv2
from ultralytics import YOLO
import threading
import queue

# load mode
model = YOLO("yolov8n.pt")
frame_count = 0
fruit_classes = {"apple", "banana", "orange"}

# 2 global queues for frame and result.
frame_queue = queue.Queue(maxsize=6)
result_queue = queue.Queue(maxsize=6)


cap = cv2.VideoCapture("http://10.43.0.137:7123/stream.mjpg") 
# this is the old ip address of the camera, put 0 here for local test.

def capture_frames():
    global frame_count
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        frame_count += 1
        if frame_count % 2 == 0:
            continue  # skip every 5 frame
        frame_queue.put(frame)

# reconize the picture of the frame
def analyze_frames():
    while True:
        frame = frame_queue.get()
        results = model(frame)
        annotated_frame = frame.copy()

# skip frames sometime every 2 to 3 frams

        for result in results:
            boxes = result.boxes
            for box in boxes:
                cls_id = int(box.cls.cpu().numpy()[0])
                conf = float(box.conf.cpu().numpy()[0])
                cls_name = model.names.get(cls_id, "unknown")
                if cls_name in fruit_classes:
                    x1, y1, x2, y2 = box.xyxy.cpu().numpy()[0]
                    cv2.rectangle(annotated_frame, (int(x1), int(y1)), (int(x2), int(y2)), (0,255,0), 2)
                    cv2.putText(annotated_frame, f"{cls_name} {conf:.2f}",
                                (int(x1), int(y1)-10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0,255,0), 2)

        result_queue.put(annotated_frame)

# here for another frame maybe make another statement to just like 

# show the result of the frame
def display_output():
    while True:
        annotated_frame = result_queue.get()

#change here because it call itself everytime in the loop of call


        cv2.imshow("Fruit Detection", annotated_frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

# create threads and start them
t1 = threading.Thread(target=capture_frames)
t2 = threading.Thread(target=analyze_frames)
t3 = threading.Thread(target=display_output)

t1.start()
t2.start()
t3.start()

# wait for it to stop
t1.join()
t2.join()
t3.join()
