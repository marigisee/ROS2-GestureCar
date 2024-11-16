# Como compilar el firmware microros para tu raspberry pi pico
## Crear area de trabajo
Si estamos trabajando con docker crearemos un contenedor con los requerimientos necesarios para compilar el firmware.
Puedes abrir el proyecto utilizando los files de la carpeta .devcontainer
### Ajustes extras
En el archivo 'src/micro_ros_raspberrypi_pico/CMakeLists.txt' agregar las líneas (en orden)

`
set(PICO_SDK_PATH "/micro_ros_ws/pico-sdk")
include (${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
`
## Programa
El codigo a modificar sera el archivo **pico_micro_ros_example.c**, aqui pondremos la logica de programación de acuerdo a nuestros requerimientos, en este caso el codigo para mover los motores se encuentra en este directorio

## Compilación
`cd micro_ros_raspberrypi_pico_sdk`

`mkdir build && cd build`

`cmake ..`

`make`

Esto generará un archivo **.uf2** dentro de la carpeta build, este será el firmware de nuestro microcontrolador.

Para copiar el archivo desde el contenedor docker a nuestra carpeta corremos el siguiente comando en cualquier cmd de windows:

`docker cp cranky_burnell:/micro_ros_ws/src/micro_ros_raspberrypi_pico_sdk/build/pico_micro_ros_example.uf2 ruta_destino`

## Flasheo a la pico
Para poder subir el firmware conectamos la pico por cable USB a la PC manteniendo el boton de *BOOTSEL* presionado, esto la dejará lista para cargar el programa.

Copiar el archivo .uf2 a la carpeta de la rpi pico y expulsar dispositivo.
