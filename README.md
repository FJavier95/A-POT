# SBC-POT (MACETERO SBC)

[![Latest Version on Packagist][ico-version]][link-packagist]
[![Build Status][ico-travis]][link-travis]
[![Coverage Status][ico-scrutinizer]][link-scrutinizer]
[![Quality Score][ico-code-quality]][link-code-quality]
[![Total Downloads][ico-downloads]][link-downloads]

Macetero inteligente creado para la asignatura de Sistemas basados en computador.
Gracias a las lecturas de los diferentes sensores se realizan unas diversas
acciones sobre la planta y se gestiona de manera completamente automática.

## Structure

```
/SBC-Pot/
|-- src-arduino
|-- src-python
    `-- upload_data_BBDD
    `-- upsquare-build
```


## Install

Existen dos implementaciones del macetero, una a través de la placa UpSquare
y otra a través del microcontrolador SparkFun ESP32. A continuación, se detalla
como realizar las instalaciones para ambas opciones.

### python option
Lo primero es instalar todas las dependencias de librerías encontradas en el fichero
requirements.txt a través de la herramienta pip
``` bash
$ pip install -r requirements.txt
```

Para ejecutar el proceso es necesario lanzar el siguiente comando:
``` bash
$ cd src-python/upsquare-build/ && python3 send_data_from_device.py
```
* Quizás sea necesario ejecutar este último comando siendo administrador.

### arduino option
Todas las librerías externas necesarias para el correcto funcionamiento 
se encuentran dentro de la carpeta src-arduino/libraries


## Contributing

Proyecto realizado por:
- Julián González Dos Reis
- Francisco Javier García Álvarez
- David Sánchez Fernández
- Aitor Navarro Sanz

## Security

Este proyecto realiza una conexión a una base de datos personal para almacenar
todos los valores leidos por los sensores.