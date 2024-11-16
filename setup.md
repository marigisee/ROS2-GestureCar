##### Cambiar distribución de teclado

1. Edita el archivo de configuración del teclado:
````bash
    sudo nano /etc/default/keyboard
````
Cambia la línea XKBLAYOUT="us" (o lo que esté configurado) a XKBLAYOUT="es" para cambiar a español. Guarda los cambios y cierra nano.

1. Aplica la configuración:
   ````bash
   sudo dpkg-reconfigure keyboard-configuration

   ```` 
2. Reinicia el servicio de teclado para aplicar los cambios:
    ````bash
    sudo systemctl restart keyboard-setup
    ````
##### Setup wifi en la raspberry pi 3

1. Edito el archivo de netplan
```bash
sudo nano /etc/netplan/00-installer-config.yaml

```
2. Configuro la red de tal forma que el archivo se tiene que ver asi
``` bash
network:
  version: 2
  renderer: networkd
  wifis:
    wlan0:
      dhcp4: true
      access-points:
        "NOMBRE_DE_TU_RED":
          password: "CONTRASEÑA_DE_TU_RED"

```

3. Aplico cambios
```bash
sudo netplan apply
```

4. Verifico conexión
```bash
ping -c 4 google.com
```

##### Conexión ssh

1. Verifico el estado del ssh
```bash
sudo systemctl status ssh
```

2. Si no esta activo lo activo
```bash
sudo systemctl start ssh
```

3. Verifico la ip de la raspberry
```bash
hostname -I
```

4. Me conecto a la raspberry
```bash
ssh pi@IP_DE_LA_RASPBERRY
```

##### Entorno virtual de Python

1. Si no tengo un entorno virtual, creo uno
```bash
python3 -m venv nombre_del_entorno
```

2. Activo el entorno virtual
```bash
source nombre_del_entorno/bin/activate
```

3. Desactivo el entorno virtual
```bash
deactivate
```

##### Transferir imagenes al host

Muy importante, el siguiente comando lo ejecutas DESDE CMD en tu computadora, no en la raspberry pi.

```bash	

scp rpi@192.168.0.244:{DIRECCION_IMAGEN} "{DIRECCCION_DONDE_VAMOS_A_GUARDAR_IMAGEN}"
````

Ejemplo:

````bash
scp rpi@IP_DE_LA_RASPBERRY:/home/rpi/prueba-opencv/imagen_mediapipe.jpg "C:\Users\user\Pictures\imagen_opencv.jpg"
```

##### Transferir imagenes al host

1. Si instalaste numpy y te dice que la versión no es compatible

```bash
pip install numpy --upgrade
```

2. Si luego de abrir 