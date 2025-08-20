# Script para grabar y detener

Antes conectarse con ssh y vnc. Hay que iniciar la grabación desde VNC para que se vea la imagen en la proyección cambiando a la carpeta desktop (`cd Desktop/`)con `./camara.sh start`

Este script crea los videos en la carpeta /home/pi/videos

```bash
#!/bin/bash

VIDEO_DIR="/home/pi/videos"
PID_FILE="/tmp/cam_pid"
NAME_FILE="/tmp/cam_file"   # archivo para guardar el nombre del h264

mkdir -p "$VIDEO_DIR"

if [ "$1" == "start" ]; then
    FILE_H264="$VIDEO_DIR/captura_$(date +%Y%m%d_%H%M%S).h264"
    
    libcamera-vid --fullscreen -t 0 -o "$FILE_H264" &
    
    echo $! > "$PID_FILE"
    echo "$FILE_H264" > "$NAME_FILE"   # guardamos el nombre
    echo "Grabación iniciada: $FILE_H264"

elif [ "$1" == "stop" ]; then
    if [ -f "$PID_FILE" ]; then
        kill $(cat "$PID_FILE")
        rm "$PID_FILE"
        echo "Grabación detenida"

        # Leer el archivo h264 guardado
        if [ -f "$NAME_FILE" ]; then
            FILE_H264=$(cat "$NAME_FILE")
            FILE_MP4="${FILE_H264%.h264}.mp4"

            ffmpeg -framerate 30 -i "$FILE_H264" -c copy "$FILE_MP4"
            echo "Archivo convertido a MP4: $FILE_MP4"

            rm "$NAME_FILE"  # limpiar archivo temporal
        else
            echo "No se encontró el archivo H264 para convertir"
        fi

    else
        echo "No hay grabación en curso"
    fi

else
    echo "Uso: $0 start|stop"
fi


```

Será recomendable en la instalación tener un router y las tres raspberrys con ips determinadas para preparar la grabación con calma, y los stop desde un MAC vía ssh

