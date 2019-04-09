#! /bin/sh
 # Las líneas que empiezan por "#" son comentarios
 # La primera línea o #! /bin/bash asegura que se interpreta como
 # un script de bash, aunque se ejecute desde otro shell.

echo "Autodock server UP"
roslaunch kobuki_auto_docking minimal.launch
