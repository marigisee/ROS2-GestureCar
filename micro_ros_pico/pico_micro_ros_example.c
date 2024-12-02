#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <rmw_microros/rmw_microros.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico_uart_transports.h"

// Pines de control para el L298N
#define IN1 2  // Pin de dirección del motor izquierdo
#define IN2 3  // Pin de dirección del motor izquierdo
#define IN3 6  // Pin de dirección del motor derecho
#define IN4 7  // Pin de dirección del motor derecho
#define ENA 10 // PWM para control de velocidad del motor izquierdo
#define ENB 11 // PWM para control de velocidad del motor derecho

// Inicialización de los pines para el controlador de motores
void init_motor_pins() {
    // Configurar los pines de dirección como salidas
    gpio_init(IN1);
    gpio_set_dir(IN1, GPIO_OUT);
    gpio_put(IN1, 0);

    gpio_init(IN2);
    gpio_set_dir(IN2, GPIO_OUT);
    gpio_put(IN2, 0);

    gpio_init(IN3);
    gpio_set_dir(IN3, GPIO_OUT);
    gpio_put(IN3, 0);

    gpio_init(IN4);
    gpio_set_dir(IN4, GPIO_OUT);
    gpio_put(IN4, 0);

    // Configurar los pines de PWM como funciones de PWM
    gpio_set_function(ENA, GPIO_FUNC_PWM);
    gpio_set_function(ENB, GPIO_FUNC_PWM);

    // Obtener los slices de PWM asociados a los pines ENA y ENB
    uint slice_num_ena = pwm_gpio_to_slice_num(ENA);
    uint slice_num_enb = pwm_gpio_to_slice_num(ENB);

    // Configurar los slices de PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f); // Divisor de reloj para ajustar la frecuencia del PWM

    pwm_init(slice_num_ena, &config, true); // Iniciar PWM para el motor izquierdo
    pwm_init(slice_num_enb, &config, true); // Iniciar PWM para el motor derecho
}

// Función para ajustar la velocidad del motor
void set_motor_speed(uint gpio_pin, uint16_t speed) {
    // Establece el nivel de PWM (0-65535) en el pin especificado
    pwm_set_gpio_level(gpio_pin, speed);
}

// Funciones de movimiento
void move_forward(uint16_t speed) {
    // Configurar los pines para mover ambos motores hacia adelante
    gpio_put(IN1, 0);
    gpio_put(IN2, 1);
    gpio_put(IN3, 0);
    gpio_put(IN4, 1);
    set_motor_speed(ENA, speed); // Velocidad del motor izquierdo
    set_motor_speed(ENB, speed); // Velocidad del motor derecho
}

void move_backward(uint16_t speed) {
    // Configurar los pines para mover ambos motores hacia atrás
    gpio_put(IN1, 1);
    gpio_put(IN2, 0);
    gpio_put(IN3, 1);
    gpio_put(IN4, 0);
    set_motor_speed(ENA, speed);
    set_motor_speed(ENB, speed);
}

void turn_left(uint16_t speed) {
    // Configurar los pines para girar a la izquierda
    gpio_put(IN1, 0);
    gpio_put(IN2, 1);
    gpio_put(IN3, 1);
    gpio_put(IN4, 0);
    set_motor_speed(ENA, speed);
    set_motor_speed(ENB, speed);
}

void turn_right(uint16_t speed) {
    // Configurar los pines para girar a la derecha
    gpio_put(IN1, 1);
    gpio_put(IN2, 0);
    gpio_put(IN3, 0);
    gpio_put(IN4, 1);
    set_motor_speed(ENA, speed);
    set_motor_speed(ENB, speed);
}

void stop_motors() {
    // Detener ambos motores
    gpio_put(IN1, 0);
    gpio_put(IN2, 0);
    gpio_put(IN3, 0);
    gpio_put(IN4, 0);
    set_motor_speed(ENA, 0);
    set_motor_speed(ENB, 0);
}

// Callback para recibir comandos del tópico
void motor_cmd_callback(const void *msg_in) {
    const std_msgs__msg__Int32 *msg = (const std_msgs__msg__Int32 *)msg_in;

    uint16_t speed = 49152; // Velocidad por defecto al 75% de la máxima (65535)

    switch (msg->data) {
        case 0:
            stop_motors();
            break;
        case 1:
            move_forward(speed);
            sleep_ms(1000); // Mover durante 1 segundo
            stop_motors();
            break;
        case 2:
            move_backward(speed);
            sleep_ms(1000);
            stop_motors();
            break;
        case 3:
            turn_left(speed);
            sleep_ms(1000);
            stop_motors();
            break;
        case 4:
            turn_right(speed);
            sleep_ms(1000);
            stop_motors();
            break;
        default:
            stop_motors(); // Si el comando no es reconocido, detener motores
            break;
    }
}

int main() {
    stdio_init_all(); // Inicializar stdio (para depuración serial)
    sleep_ms(6000);   // Esperar un momento para la inicialización

    struct uxrCustomTransport transport;
    rmw_uros_set_custom_transport(
        true,
        &transport, // Transporte UART personalizado
        pico_serial_transport_open,
        pico_serial_transport_close,
        pico_serial_transport_write,
        pico_serial_transport_read
    );

    init_motor_pins(); // Configurar los pines del motor

    // Intentar conexión al agente micro-ROS
    const int timeout_ms = 1000;
    const uint8_t attempts = 120;
    while (rmw_uros_ping_agent(timeout_ms, attempts) != RCL_RET_OK) {
        // Reiniciar el transporte UART si la conexión falla
        pico_serial_transport_close(&transport);
        pico_serial_transport_open(&transport);

        // Reintentar después de un segundo
        sleep_ms(1000);
    }

    // Configurar los nodos y suscripciones de ROS 2
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rclc_support_t support;
    rcl_node_t node;
    rcl_subscription_t subscriber;
    rclc_executor_t executor;
    std_msgs__msg__Int32 msg;

    rclc_support_init(&support, 0, NULL, &allocator);
    rclc_node_init_default(&node, "motor_controller_node", "", &support);

    rclc_subscription_init_default(
        &subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "motor_cmd"
    );

    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_subscription(&executor, &subscriber, &msg, &motor_cmd_callback, ON_NEW_DATA);

    // Bucle principal
    while (true) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    }

    // Limpiar recursos antes de salir
    rcl_subscription_fini(&subscriber, &node);
    rcl_node_fini(&node);
    return 0;
}
