#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <rmw_microros/rmw_microros.h>

#include "pico/stdlib.h"
#include "pico_uart_transports.h"

// Pines de control para el L298N
#define IN1 2   // Motor izquierdo
#define IN2 3   // Motor izquierdo
#define IN3 6   // Motor derecho
#define IN4 7   // Motor derecho

// Inicialización de los pines para el controlador de motores
void init_motor_pins() {
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
}

// Funciones de movimiento
void move_forward() {
    gpio_put(IN1, 0);
    gpio_put(IN2, 1);
    gpio_put(IN3, 0);
    gpio_put(IN4, 1);
}

void move_backward() {
    gpio_put(IN1, 1);
    gpio_put(IN2, 0);
    gpio_put(IN3, 1);
    gpio_put(IN4, 0);
}

void turn_left() {
    gpio_put(IN1, 0);
    gpio_put(IN2, 1);
    gpio_put(IN3, 1);
    gpio_put(IN4, 0);
}

void turn_right() {
    gpio_put(IN1, 1);
    gpio_put(IN2, 0);
    gpio_put(IN3, 0);
    gpio_put(IN4, 1);
}

void stop_motors() {
    gpio_put(IN1, 0);
    gpio_put(IN2, 0);
    gpio_put(IN3, 0);
    gpio_put(IN4, 0);
}

// Callback para recibir comandos del tópico
void motor_cmd_callback(const void *msg_in) {
    const std_msgs__msg__Int32 *msg = (const std_msgs__msg__Int32 *)msg_in;

    switch (msg->data) {
        case 0:
            stop_motors();
            break;
        case 1:
            move_forward();
            sleep_ms(1000);  // Mover por 1 segundo
            stop_motors();
            break;
        case 2:
            move_backward();
            sleep_ms(1000);  // Mover por 1 segundo
            stop_motors();
            break;
        case 3:
            turn_left();
            sleep_ms(1000);  // Mover por 1 segundo
            stop_motors();
            break;
        case 4:
            turn_right();
            sleep_ms(1000);  // Mover por 1 segundo
            stop_motors();
            break;
        default:
            stop_motors();  // En caso de comando no reconocido
            break;
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);  // Esperar un momento para la inicialización

    rmw_uros_set_custom_transport(
        true,
        NULL,
        pico_serial_transport_open,
        pico_serial_transport_close,
        pico_serial_transport_write,
        pico_serial_transport_read
    );

    init_motor_pins();

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

    while (true) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    }

    rcl_subscription_fini(&subscriber, &node);
    rcl_node_fini(&node);
    return 0;
}
