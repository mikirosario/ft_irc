#!/bin/bash

# Conectar a servidor

# Probar a conectarse al servidor o registrarse si falla

#Lo siguiente consistira en un bucle que mire los canales activos y se intente conectar

while true;
do
	# guardar en tmpVar todos los canales
	tmpVar=$(( ( RANDOM % 10 )  + 1 ))
	#Enviar mensaje de spam

	if [ $tmpVar -ge 10 ] 
	then
		echo Mensaje pidiendo pasta por irc
		sleep 10s
	else
		echo $tmpVar
	fi
	sleep 0.5s
done