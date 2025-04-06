import cv2
import numpy as np
from ultralytics import YOLO

# yolov8 model with segmentation feature
model = YOLO("yolov8n-seg.pt")

# list of objects to detect
detection_list = ["apple", "banana", "orange"]

# camera currently in use is webcam
cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    results = model(frame)
    annotated_frame = frame.copy()

    # process each result
    for result in results:
        # check if segmentation masks are available
        if hasattr(result, "masks") and result.masks is not None:
            # convert segmentation masks to a NumPy array
            masks_np = result.masks.data.cpu().numpy()
            # retrieve class IDs and confidence scores from bounding boxes
            cls_ids = result.boxes.cls.cpu().numpy() if result.boxes is not None else None
            confs = result.boxes.conf.cpu().numpy() if result.boxes is not None else None

            for i, mask in enumerate(masks_np):
                # retrieve the class name from the bounding box info
                if cls_ids is not None:
                    cls_id = int(cls_ids[i])
                    cls_name = model.names.get(cls_id, "unknown")
                else:
                    cls_name = "unknown"

                # process only objects in the detection list
                if cls_name.lower() in [obj.lower() for obj in detection_list]:
                    # create a binary mask by thresholding
                    binary_mask = (mask > 0.5).astype(np.uint8)
                    # resize the binary mask to match the original frame dimensions
                    resized_mask = cv2.resize(binary_mask, (frame.shape[1], frame.shape[0]), interpolation=cv2.INTER_NEAREST)
                    
                    # find contours from the resized binary mask
                    contours, _ = cv2.findContours(resized_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
                    # draw the contours (object outline) on the annotated frame
                    cv2.drawContours(annotated_frame, contours, -1, (0, 255, 0), 2)

                    # get the confidence value for this detection (if available)
                    conf = float(confs[i]) if confs is not None else 0.0

                    # use the largest contour to place the label near the object
                    if contours:
                        largest_contour = max(contours, key=cv2.contourArea)
                        x, y, w, h = cv2.boundingRect(largest_contour)
                        label_text = f"{cls_name} {conf:.2f}"
                        cv2.putText(annotated_frame, label_text, (x, y - 10),
                                    cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

    cv2.imshow("Segmentation Detection", annotated_frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
