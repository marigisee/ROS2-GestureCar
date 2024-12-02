El programa ``camera_test.py`` esta pensado para poder visualizar la imagen de la cámara web conectada a la rpi3. 


Para tener éxito en la visualización de la imagen, es **requesito** 

- Comunicarse con la rpi3 mediante ssh utilizando WSL2.
- Utilizar Xlaunch para visualizar la imagen en la computadora local.
- Tener instalado OpenCV en la rpi3.


 **Nota** : Si en algun momento te deja de andar un programa que antes sí te funcionaba, reinicia la rpi3.


## Pasos a seguir

1. Se debe configurar el archivo sshd_config en la rpi3. Para ello, se debe ejecutar el siguiente comando en la rpi3:

```bash
sudo nano /etc/ssh/sshd_config
```

 Y se deben descomentar las siguientes líneas:

```bash
X11Forwarding yes
X11DisplayOffset 10
X11UseLocalhost no
```

Luego, se debe reiniciar el servicio ssh.

2. Con XLaunch instalado en la computadora local, inicializamos la conexión ssh de la siguiente forma
    
```bash
     ssh -X user@<ip>
 ```

 3. Para probar que la conexión es exitosa, probamos el siguiente programa en la rpi3:  
   
```bash
     xeyes  
```

Si aparece una ventana con unos ojos, entonces la conexión es exitosa. Proccesa a ejecutar el programa `camera_test.py`