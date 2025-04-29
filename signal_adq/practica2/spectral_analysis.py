"""
Spectral Analysis of Data from Serial Port
This script reads data from a serial port, applies different window functions, and performs spectral analysis using FFT. The results are plotted and can be saved to a file.
The script includes functions for data acquisition, windowing, spectral analysis, and plotting. It also provides a simple command-line interface for user interaction.

Usage:
1. Connect the device to the specified serial port.
2. Run the script.
3. Follow the prompts to acquire data and analyze it.
4. The results will be displayed and can be saved to a file.
5. The script will create a 'results' directory if it doesn't exist.
6. The script will exit if the user chooses to do so.
7. The script will handle invalid data gracefully and skip over it.
8. The script will remove the DC component from the data before performing spectral analysis.
9. The script will apply different window functions to the data and plot the results.
10. The script will save the results to a PNG file in the 'results' directory.
11. The script will print messages to the console to inform the user of the progress and any errors encountered.
"""

import serial as pyserial  # Renombramos el módulo para evitar conflictos
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
from time import sleep
import os

# 128 256 512 1024

def daq(port='/dev/ttyACM0', buffer_size=1024):
    """
    Function to read data from a serial port and return it as a numpy array.
    Args:
        port (str): The serial port to read from.
        buffer_size (int): The number of data points to read.
    Returns:
        np.ndarray: Array of data read from the serial port.
    """
    try:
        # Port configuration
        ser = pyserial.Serial(port, baudrate=115200, timeout=1)  # Usamos pyserial.Serial
        print(f"Connected to {port}")
        sleep(2)  # Allow time for the connection to establish

        # Read data from the serial port
        data = []
        while len(data) < buffer_size:
            if ser.in_waiting > 0:  # Read available data
                try:
                    line = ser.readline()
                    valor = int(line.decode('utf-8').strip())
                    data.append(valor)
                except ValueError:
                    print("Invalid data received, skipping...")
        
        ser.close()
        return np.array(data)
    
    except Exception as e:
        print(f"Error connection to mcu: {e}")
        return None
    
def windos(data, window_type='rectangle'):
    """
    Function to apply a window function to the data.
    Args:
        data (np.ndarray): The data to apply the window to.
        window_type (str): Type of window to apply ('rectangle', 'hann', 'hamming', 'blackman').
    Returns:
        np.ndarray: Windowed data.
    """
    N = len(data)

    if window_type.lower() == 'rectangle':
        return data  # Rectangle window does not change the data
    elif window_type.lower() == 'hann':  # Updated from 'hanning' to 'hann'
        return data * signal.windows.hann(N)
    elif window_type.lower() == 'hamming':
        return data * signal.windows.hamming(N)
    elif window_type.lower() == 'blackman':
        return data * signal.windows.blackman(N)
    else:
        print(f"Unknown window type: {window_type}. Using rectangle window.")
        return data
    
def espectra_analysis(data, fs, window_type='rectangle'):
    """
    Function to perform spectral analysis on the given data.
    Args:
        data (np.ndarray): The data to analyze.
        fs (int): Sampling frequency.
        window_type (str): Type of window to apply ('rectangle', 'hanning', 'hamming', 'blackman').
    Returns:
        tuple: Frequencies and corresponding FFT magnitudes.
    """
    data = data - np.mean(data) # remove dc component

    # Apply window function
    windowed_data = windos(data, window_type)

    # Compute the FFT
    N = len(windowed_data)
    fft_result = np.fft.fft(windowed_data)
    fft_magnitude = np.abs(fft_result)/N  # Take the positive frequencies and normalize

    # Frequency axis
    freq = np.fft.fftfreq(N, 1/fs)

    return freq[:N//2], 2*fft_magnitude[:N//2]  # Return only the positive frequencies

def safe_rslt (data, fs, name_file = 'espectra_results'):
    if not os.path.exists('results'):
        os.makedirs('results')
    
    # Apply different window types and save results
    window_types = ['rectangle', 'hanning', 'hamming', 'blackman']

    plt.figure(figsize=(10, 6))

    for i, window in  enumerate(window_types,1):
        # spectral analysis for each window type
        freq, fft_magnitude = espectra_analysis(data, fs, window)

        # Plotting
        plt.subplot(2, 2, i)
        plt.plot(freq, fft_magnitude)
        plt.title(f'FFT with {window} window')
        plt.xlabel('Frequency (Hz)')
        plt.ylabel('Magnitude')
        plt.grid()
    
    plt.tight_layout()
    plt.savefig(f'results/{name_file}.png')
    plt.close()

    print(f"Results saved to results/{name_file}.png")

def plot_spectral_analysis(data, fs, window_types, save_path=None):
    """
    Helper function to perform spectral analysis and plot results for different window types.
    Args:
        data (np.ndarray): Input signal data.
        fs (int): Sampling frequency.
        window_types (list): List of window types to analyze.
        save_path (str): Path to save the plot (optional).
    """
    plt.figure(figsize=(10, 6))

    for i, window in enumerate(window_types, 1):
        freq, fft_magnitude = espectra_analysis(data, fs, window)

        # Plotting
        plt.subplot(2, 2, i)
        plt.plot(freq, fft_magnitude)
        plt.title(f'FFT with {window} window')
        plt.xlabel('Frequency (Hz)')
        plt.ylabel('Magnitude')
        plt.grid()

    plt.tight_layout()

    if save_path:
        plt.savefig(save_path)
        plt.close()
        print(f"Results saved to {save_path}")
    else:
        plt.show()   

if __name__ == "__main__":
     # Configuration
    port = '/dev/ttyACM0'  # Default port
    buffer_size = 1024
    fs = 5000  # Sampling frequency in Hz

    # Menu
    print("1. Acquire data and analyze")
    print("2. Exit")

    option = input("Select an option: ")

    if option == '1':
        # Select port
        port = input("Enter the port (default is /dev/ttyACM0): ") or port

        # Acquire data
        data = daq(port, buffer_size)

        if data is not None:
            print(f"{len(data)} samples acquired.")

            # Show and save signal in time domain
            plt.figure(figsize=(10, 4))
            t = np.arange(0, len(data) / fs, 1 / fs)
            plt.plot(t, data)
            plt.title('Signal in Time Domain')
            plt.xlabel('Time (s)')
            plt.ylabel('Amplitude')
            plt.grid()

            # Ask the user for the name of the time-domain plot
            time_plot_name = input("Enter the name for the time-domain plot (no extension, default: 'time_domain_signal'): ") or 'time_domain_signal'
            time_plot_path = f'results/{time_plot_name}.png'

            # Save the plot to the 'results' directory
            if not os.path.exists('results'):
                os.makedirs('results')
            plt.savefig(time_plot_path)
            print(f"Time-domain plot saved to {time_plot_path}")

            # Show the plot
            plt.show()

            # Analyze and show spectra for different windows
            window_types = ['rectangle', 'hanning', 'hamming', 'blackman']
            plot_spectral_analysis(data, fs, window_types)

            # Save results
            save = input("Do you want to save the results? (y/n): ")
            if save.lower() == 'y':
                name = input("Enter the name of the file (no extension): ") or 'espectra_results'
                save_path = f'results/{name}.png'
                if not os.path.exists('results'):
                    os.makedirs('results')
                plot_spectral_analysis(data, fs, window_types, save_path)
        else:
            print("Data acquisition failed. Exiting...")
    else:
        print("Exiting... see you soon!")