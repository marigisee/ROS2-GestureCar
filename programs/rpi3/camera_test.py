#!/usr/bin/env python3
# encoding: utf-8

import cv2 as cv
import mediapipe as mp

class handDetector:
    def __init__(self, mode=False, maxHands=2, detectorCon=0.85, trackCon=0.85):
        self.tipIds = [4, 8, 12, 16, 20]
        self.mpHand = mp.solutions.hands
        self.hands = self.mpHand.Hands(
            static_image_mode=mode,
            max_num_hands=maxHands,
            min_detection_confidence=detectorCon,
            min_tracking_confidence=trackCon
        )
        self.lmList = []

    def findHands(self, frame, draw=True):
        self.lmList = []
        img_RGB = cv.cvtColor(frame, cv.COLOR_BGR2RGB)
        self.results = self.hands.process(img_RGB)
        if self.results.multi_hand_landmarks:
            for handLms in self.results.multi_hand_landmarks:
                if draw:
                    mp.solutions.drawing_utils.draw_landmarks(
                        frame, handLms, self.mpHand.HAND_CONNECTIONS)
                for id, lm in enumerate(handLms.landmark):
                    h, w, _ = frame.shape
                    cx, cy = int(lm.x * w), int(lm.y * h)
                    self.lmList.append([id, cx, cy])
        return frame

    def fingersUp(self):
        if len(self.lmList) < 21:
            return []
        fingers = []
        fingers.append(1 if self.lmList[self.tipIds[0]][2] < self.lmList[self.tipIds[1]][2] else 0)
        for id in range(1, 5):
            fingers.append(1 if self.lmList[self.tipIds[id]][2] < self.lmList[self.tipIds[id] - 2][2] else 0)
        return fingers

    def get_gesture(self):
        gesture = ""
        fingers = self.fingersUp()
        if fingers.count(1) == 5:
            gesture = "Five"
        elif fingers.count(1) == 0:
            gesture = "Zero"
        elif fingers.count(1) == 1:
            gesture = "One"
        elif fingers.count(1) == 2:
            gesture = "Two"
        elif fingers.count(1) == 3:
            gesture = "Three"
        elif fingers.count(1) == 4:
            gesture = "Four"
        return gesture


def main():
    # Abrir la cámara
    capture = cv.VideoCapture(0)
    capture.set(cv.CAP_PROP_FRAME_WIDTH, 640)
    capture.set(cv.CAP_PROP_FRAME_HEIGHT, 480)

    # Crear una ventana para la visualización
    cv.namedWindow("Detección de Gestos", cv.WINDOW_NORMAL)

    # Inicializar el detector de manos
    hand_detector = handDetector()

    while capture.isOpened():
        ret, frame = capture.read()
        if not ret:
            print("Error al capturar el frame")
            break

        # Detectar manos y obtener el gesto
        frame = hand_detector.findHands(frame, draw=True)
        gesture = hand_detector.get_gesture()

        # Mostrar el gesto detectado en la ventana
        cv.putText(frame, f'Gesto: {gesture}', (10, 50), cv.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

        # Mostrar el video en tiempo real
        cv.imshow("Detección de Gestos", frame)

        # Salir si se presiona 'q'
        if cv.waitKey(1) & 0xFF == ord('q'):
            break

    # Liberar la cámara y cerrar la ventana
    capture.release()
    cv.destroyAllWindows()


if __name__ == '__main__':
    main()
