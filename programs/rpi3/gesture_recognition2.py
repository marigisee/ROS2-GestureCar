import cv2
import mediapipe as mp

# Inicializar MediaPipe Hands
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode=False, max_num_hands=1, min_detection_confidence=0.7)
mp_draw = mp.solutions.drawing_utils

# Iniciar captura de video
cap = cv2.VideoCapture(0)

# Función para contar dedos levantados usando coordenadas de landmarks
def detect_gesture(hand_landmarks):
    # Coordenadas de puntos clave
    thumb_tip = hand_landmarks.landmark[4]
    index_tip = hand_landmarks.landmark[8]
    middle_tip = hand_landmarks.landmark[12]
    ring_tip = hand_landmarks.landmark[16]
    pinky_tip = hand_landmarks.landmark[20]

    # Coordenadas del centro de la palma (landmark 0)
    palm_center = hand_landmarks.landmark[0]

    # Variables para el estado de los dedos
    thumb_up = thumb_tip.x < palm_center.x  # Pulgar está a la izquierda del centro
    index_up = index_tip.y < hand_landmarks.landmark[6].y  # Índice está levantado
    middle_up = middle_tip.y < hand_landmarks.landmark[10].y  # Medio está levantado
    ring_up = ring_tip.y < hand_landmarks.landmark[14].y  # Anular está levantado
    pinky_up = pinky_tip.y < hand_landmarks.landmark[18].y  # Meñique está levantado

    # Reconocimiento de gestos
    if not thumb_up and not index_up and not middle_up and not ring_up and not pinky_up:
        return "Puño cerrado (Detenerse)"
    elif thumb_up and index_up and middle_up and ring_up and pinky_up:
        return "Mano abierta (Avanzar)"
    elif thumb_up and not index_up and not middle_up and not ring_up and not pinky_up:
        return "Pulgar arriba (Marcha atrás)"
    elif index_up and middle_up and not ring_up and not pinky_up:
        return "Peace (Girar izquierda)"
    elif thumb_up and index_up and not middle_up and not ring_up and not pinky_up:
        return "OK (Girar derecha)"
    else:
        return "Gestos no detectados"

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        print("Error al capturar el frame.")
        break

    # Convertir el frame a RGB
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(frame_rgb)

    # Verificar si se detectaron manos
    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            mp_draw.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            # Detectar el gesto
            gesture = detect_gesture(hand_landmarks)
            cv2.putText(frame, gesture, (10, 70), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

    # Mostrar el frame con anotaciones
    cv2.imshow("Reconocimiento de Gestos", frame)

    # Presiona 'q' para salir
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Liberar recursos
cap.release()
cv2.destroyAllWindows()
