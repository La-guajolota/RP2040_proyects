import matplotlib
matplotlib.use('Agg')  # Cambiar el backend de matplotlib   
import serial
import numpy as np
import matplotlib.pyplot as plt
from time import sleep

def adquirir_datos(puerto='COM3', muestras=1000):
    ser = serial.Serial(puerto, 115200)
    sleep(2)  # Esperar a que el puerto serial esté listo

    # Leer datos
    datos = []
    for i in range(muestras):
        if ser.in_waiting:
            linea = ser.readline()
            valor = int(linea.decode().strip())
            datos.append(valor)

    ser.close()
    return np.array(datos)

def cuantizar(datos, bits):
    niveles = 2**bits
    max_val = np.max(datos)
    min_val = np.min(datos)
    paso = (max_val - min_val) / niveles
    return np.round(datos / paso) * paso

# Configuración de parámetros
PUERTO = "/dev/ttyACM0"
MUESTRAS = 100  
BITS_DE_CUANTIZACION = 4

# Adquirir datos desde el puerto serial
datos = adquirir_datos(PUERTO, MUESTRAS)

# Cuantizar los datos adquiridos
datos_cuantizados = cuantizar(datos, BITS_DE_CUANTIZACION)

# Graficar los datos originales y cuantizados
plt.figure(figsize=(12, 6))    
plt.plot(datos, 'b-',label='Datos originales')
plt.plot(datos_cuantizados, 'r-', label='Datos cuantizados')
plt.grid(True)
plt.legend()
plt.savefig('./pics_practica/img4_3.png')