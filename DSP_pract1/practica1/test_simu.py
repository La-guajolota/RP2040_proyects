import matplotlib
matplotlib.use('Agg')  # Cambiar el backend de matplotlib
import numpy as np
import matplotlib.pyplot as plt

f0 = 500 # Frecuencia de la señal en Hz
Xa = lambda t: 2 * np.sin(2 * np.pi * f0 * t)

Ts = 0.000180  # Periodo de muestreo en segundos
# Vector de tiempo y de muestras
ti = np.linspace(0, 2/f0, 100) 
ni = np.arange(0, 2/f0, Ts)

signal_a = Xa(ti)
signal_x = Xa(ni)

plt.figure(figsize=(10, 8))
plt.plot(ti, signal_a, 'b', label='Señal analógica')
plt.stem(ni, signal_x, linefmt='r', markerfmt='ro', basefmt='r', label='Señal muestreada')
plt.plot(ni, signal_x, 'g', label='Señal reconstruida')
plt.title('Señal analógica')
plt.xlabel('Tiempo [s]')
plt.ylabel('Amplitud [V]')
plt.grid(True)
plt.savefig('senal_analogica_CASO1.png')  # Guardar la figura en un archivo