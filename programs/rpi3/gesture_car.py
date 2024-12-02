#!/usr/bin/env python3
# encoding: utf-8

import time
import threading
import queue
import cv2 as cv
import mediapipe as mp
import rclpy
from std_msgs.msg import Int32

class handDetector:
    def __init__(self, mode=False, maxHands=1, detectorCon=0.85, trackCon=0.85):
        self.tipIds = [4, 8, 12, 16, 20]
        self.mpHand = mp.solutions.hands
        self.hands = self.mpHand.Hands(
            static_image_mode=mode,
            max_num_hands=maxHands,
            min_detection_confidence=detectorCon,
            min_tracking_confidence=trackCon
        )
        self.lmList = []

    def findHands(self, frame, draw=False):
        self.lmList = []
        img_RGB = cv.cvtColor(frame, cv.COLOR_BGR2RGB)
        self.results = self.hands.process(img_RGB)
        if self.results.multi_hand_landmarks:
            for handLms in self.results.multi_hand_landmarks:
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

    def publish_command(self, gesture):
        cmd = None
        if gesture == "One":
            cmd = 1
        elif gesture == "Two":
            cmd = 2
        elif gesture == "Three":
            cmd = 3
        elif gesture == "Four":
            cmd = 4
        else:
            return
        msg = Int32()
        msg.data = cmd
        self.publisher.publish(msg)
        print(f"Enviando comando: {cmd}")

def capture_frames(capture, frame_queue):
    while True:
        ret, frame = capture.read()
        if not ret:
            break
        if not frame_queue.full():
            frame_queue.put(frame)

def main():
    rclpy.init()
    node = rclpy.create_node('gesture_publisher')
    publisher = node.create_publisher(Int32, '/motor_cmd', 1)

    capture = cv.VideoCapture(0)
    capture.set(cv.CAP_PROP_FRAME_WIDTH, 320)
    capture.set(cv.CAP_PROP_FRAME_HEIGHT, 240)

    hand_detector = handDetector()
    hand_detector.publisher = publisher

    frame_queue = queue.Queue(maxsize=1)
    threading.Thread(target=capture_frames, args=(capture, frame_queue), daemon=True).start()

    previous_gesture = None
    while True:
        if not frame_queue.empty():
            frame = frame_queue.get()
            frame = hand_detector.findHands(frame, draw=False)
            gesture = hand_detector.get_gesture()
            if gesture != previous_gesture:
                hand_detector.publish_command(gesture)
                previous_gesture = gesture

if __name__ == '__main__':
    main()
