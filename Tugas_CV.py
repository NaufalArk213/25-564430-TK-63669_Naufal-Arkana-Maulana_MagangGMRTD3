import cv2 as cv
from ultralytics import YOLO

model = YOLO("yolov8n.pt")
TARGET_CLASSES = [0, 39, 64]

cap = cv.VideoCapture(0)
if not cap.isOpened():
    print("Cannot open camera")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("Can't receive frame. Exiting ...")
        break

    results = model.predict(source=frame, classes=TARGET_CLASSES, conf=0.4, verbose=False)
    annotated_frame = results[0].plot()

    cv.imshow('Deteksi 3 Objek - YOLOv8n', annotated_frame)
    if cv.waitKey(1) == ord('q'):
        break

cap.release()
cv.destroyAllWindows()
