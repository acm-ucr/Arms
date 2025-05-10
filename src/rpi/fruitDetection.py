import cv2
from ultralytics import YOLO

# Load the YOLOv8 model
model = YOLO("yolov8n.pt")

# Define your fruit classes (adjust names as per model.names)
fruit_classes = {"apple", "banana", "orange"}

# change to the ip address
cap = cv2.VideoCapture("http://10.43.0.59:7123/stream.mjpg")

while True:
    ret, frame = cap.read()
    if not ret:
        break

    results = model(frame)

    # Copy the frame to draw filtered detections
    annotated_frame = frame.copy()

    # Loop over the results (assuming one result per image/frame)
    for result in results:
        boxes = result.boxes  # bounding boxes
        for box in boxes:
            # Get the predicted class index and confidence
            cls_id = int(box.cls.cpu().numpy()[0])
            conf = float(box.conf.cpu().numpy()[0])
            # Get the class name from the model's names dictionary
            cls_name = model.names.get(cls_id, "unknown")
            
            # Check if the detected object is in your fruit list
            if cls_name in fruit_classes:
                # Extract box coordinates and draw bounding box
                x1, y1, x2, y2 = box.xyxy.cpu().numpy()[0]
                cv2.rectangle(annotated_frame, (int(x1), int(y1)), (int(x2), int(y2)), (0,255,0), 2)
                cv2.putText(annotated_frame, f"{cls_name} {conf:.2f}",
                            (int(x1), int(y1)-10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0,255,0), 2)

    cv2.imshow("Fruit Detection", annotated_frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()