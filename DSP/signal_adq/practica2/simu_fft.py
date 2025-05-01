"""
# simu_fft.py

This script demonstrates the generation, analysis, and visualization of signals in the frequency domain using the Fast Fourier Transform (FFT). 
It also explores the effects of applying different window functions to signals.

## Features:
1. **Signal Generation**:
    - Creates two signals with close frequencies (100 Hz and 110 Hz).
    - Case 1: Signals with equal amplitudes.
    - Case 2: Signals with different amplitudes.

2. **Spectrum Analysis**:
    - Computes and plots the spectrum of the signals using FFT.
    - Visualizes the frequency components of the signals.

3. **Window Functions**:
    - Defines and visualizes common window functions (Rectangular, Hamming, Hanning, Blackman) in the time domain.
    - Applies these windows to the signals and analyzes their impact on the frequency spectrum.

4. **Visualization**:
    - Plots the spectra of the original and windowed signals.
    - Compares the effects of different window functions on the frequency domain representation.

## Dependencies:
- `numpy`: For numerical computations.
- `matplotlib`: For plotting and visualization.
- `scipy.signal`: For generating window functions.

## Usage:
- Run the script to generate and analyze the signals.
- Observe the impact of windowing on the frequency spectrum.
"""

import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

# Parameters
fs = 1000  # Sampling frequency (Hz)
T = 1      # Signal duration (seconds)
N = fs * T  # Total number of samples
t = np.linspace(0, T, N, endpoint=False)  # Time vector

# Signal generation
f1 = 100  # Frequency of the first sine wave (Hz)
f2 = 105  # Frequency of the second sine wave (Hz)

# Case 1: Two signals with close frequencies and equal amplitudes
x1 = np.cos(2 * np.pi * f1 * t) + np.cos(2 * np.pi * f2 * t)

# Case 2: Two signals with close frequencies and different amplitudes
x2 = np.cos(2 * np.pi * f1 * t) + 0.1 * np.cos(2 * np.pi * f2 * t)

# Function to plot the spectrum of a signal
def plot_spectrum(x, title, N_fft=None):
    """
    Plot the spectrum of a signal using FFT.

    Parameters:
    x (array): Input signal
    title (str): Title of the plot
    N_fft (int): Number of FFT points (optional)
    """
    if N_fft is None:
        N_fft = len(x)  # Default to the length of the signal

    # Compute FFT and normalize
    X = np.fft.fft(x, N_fft)
    X_mag = np.abs(X) / N_fft

    # Generate frequency axis
    f = np.fft.fftfreq(N_fft, 1 / fs)

    # Plot the positive half of the spectrum
    plt.figure(figsize=(10, 6))
    plt.plot(f[:N_fft // 2], 2 * X_mag[:N_fft // 2])  # Multiply by 2 for amplitude
    plt.title(title)
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude')
    plt.grid()
    plt.xlim(0, 200)  # Limit x-axis to 200 Hz
    plt.ylim(0, 1)    # Limit y-axis to 1
    plt.axhline(0, color='black', lw=0.5)
    plt.axvline(0, color='black', lw=0.5)
    plt.show()

# Plot spectra of the signals without windowing
plot_spectrum(x1, "Spectrum: Close Frequencies")
plot_spectrum(x2, "Spectrum: Different Amplitudes")

# Define window functions
windows = {
    'Rectangular': np.ones(N),
    'Hamming': signal.windows.hamming(N),
    'Hanning': signal.windows.hann(N),
    'Blackman': signal.windows.blackman(N),
}

# Plot windows in the time domain
plt.figure(figsize=(10, 6))
for name, window in windows.items():
    plt.plot(t, window, label=name)
plt.title("Window Functions in Time Domain")
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.legend()
plt.grid()
plt.xlim(0, T)
plt.ylim(-0.5, 1.5)
plt.axhline(0, color='black', lw=0.5)
plt.axvline(0, color='black', lw=0.5)
plt.show()

# Function to apply windows and plot their spectra
def apply_and_plot_windows(signal, signal_name, windows, fs, N):
    """
    Apply different windows to a signal and plot their spectra.

    Parameters:
    signal (array): Input signal
    signal_name (str): Name of the signal (for plot titles)
    windows (dict): Dictionary of window functions
    fs (int): Sampling frequency
    N (int): Number of samples
    """
    plt.figure(figsize=(10, 8))
    for i, (name, window) in enumerate(windows.items(), 1):
        plt.subplot(2, 2, i)

        # Apply the window to the signal
        x_windowed = signal * window

        # Compute FFT and normalize
        X = np.fft.fft(x_windowed)
        X_mag = np.abs(X) / N

        # Generate frequency axis
        f = np.fft.fftfreq(len(X), 1 / fs)

        # Plot the spectrum
        plt.plot(f[:N // 2], 2 * X_mag[:N // 2])  # Multiply by 2 for amplitude
        plt.title(f"{name} Window - {signal_name}", fontsize=10)
        plt.xlabel('Frequency (Hz)', fontsize=9)
        plt.ylabel('Magnitude', fontsize=9)
        plt.grid()
        plt.xlim(0, 200)  # Limit x-axis to 200 Hz
        plt.ylim(0, 1)    # Limit y-axis to 1
        plt.axhline(0, color='black', lw=0.5)
        plt.axvline(0, color='black', lw=0.5)
        plt.tick_params(axis='both', which='major', labelsize=8)

    # Adjust layout and add a title
    plt.tight_layout(rect=[0, 0, 1, 0.95])
    plt.suptitle(f"Spectra with Different Windows - {signal_name}", fontsize=12)
    plt.show()

# Apply windows to both signals and plot their spectra
apply_and_plot_windows(x1, "Signal 1 (Close Frequencies)", windows, fs, N)
apply_and_plot_windows(x2, "Signal 2 (Different Amplitudes)", windows, fs, N)