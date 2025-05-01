import matplotlib
matplotlib.use('Agg')  # Cambiar el backend de matplotlib
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

def cuantizar(senal, bits):
    niveles = 2**bits
    senal_normalizada = senal * 1023/5.0
    senal_cunatizada = np.round(senal_normalizada)
    return senal_cunatizada * 5.0/1023

def simular_sennal_analogica(f0, T, fs):
    ti = np.linspace(0, T, int(T * fs))  # Corregido para que el tercer argumento sea un entero
    signal = 0.5 * np.sin(2 * np.pi * f0 * ti) + 0.5
    return ti, signal

def muestreo_sennal(signal_continua, t_continuo, fs):
    t_muestro = np.arange(0, t_continuo[-1], 1/fs)
    signal_muestreada = np.interp(t_muestro, t_continuo, signal_continua)
    return t_muestro, signal_muestreada

def visualizar_proceso():
    """
    Visualiza el proceso de simulación de una señal analógica.
    """
    f0 = 500  # Frecuencia de la señal en Hz
    T = 0.01  # Duración de la señal en segundos
    fs = 100000  # Frecuencia de muestreo en Hz

    fs_muestro = 10000
    bits_cuantizacion = 3

    ti, signal = simular_sennal_analogica(f0, T, fs)
    t_muestreo, signal_muestreada = muestreo_sennal(signal, ti, fs_muestro)
    senal_cuantizada = cuantizar(signal_muestreada,bits_cuantizacion)

    plt.figure(figsize=(10, 8))

    plt.subplot(2, 1, 1)
    plt.plot(ti * 1000, signal, label="Señal continua")
    plt.stem(t_muestreo * 1000, signal_muestreada, "g.", label="Muestras", basefmt=" ")
    plt.plot(t_muestreo * 1000, senal_cuantizada, "r--", label="Cuantización")
    plt.title('Señal analógica y su muestreo')
    plt.xlabel('Tiempo [ms]')
    plt.ylabel('Amplitud [V]')
    plt.grid(True)
    plt.legend()

    plt.subplot(2, 1, 2)
    plt.hist(senal_cuantizada, bins=2**bits_cuantizacion, edgecolor='black')
    plt.title('Histograma de la señal cuantizada')
    plt.xlabel('Amplitud cuantizada')
    plt.ylabel('Frecuencia')
    plt.grid(True)

    plt.tight_layout()
    plt.savefig('senal_analogica.png')  # Guardar la figura en un archivo
    plt.show()

if __name__ == '__main__':
    visualizar_proceso()