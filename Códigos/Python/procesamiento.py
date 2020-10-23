#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Sep 27 01:21:53 2020

@author: Franco Chiesa Docampo 

Función de procesamiento para calcular el z-score dF/F de la senal adquirida mediante el sistema de Fiber Photometry.

Para el desarrollo de esta función se adaptó código de la siguiente referencia:
    
(1) Martianova, E., Aronson, S., Proulx, C.D. Multi-Fiber Photometry 
      to Record Neural Activity in Freely Moving Animal. J. Vis. Exp. 
      (152), e60278, doi:10.3791/60278 (2019)
      https://www.jove.com/video/60278/multi-fiber-photometry-to-record-neural-activity-freely-moving

"""

# Procesamiento
import scipy
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

from get_zdFF import get_zdFF

# Cargar datos | Ajustar directorio de forma acorde al usuario
folder = '/Users/marcelochiesa/Desktop/Photometry_data_processing-master/Python/' 
# Nombre del archivo 
file_name = '100fps.csv'   
        
# Lectura
df = pd.read_csv(folder+file_name,index_col=False) 
            
time = df['time'][0:] # Se extrae el vector tiempo del archivo "df".
intensity = df['intensity'][0:] # Se extrae el vector intensidad del archivo "df".
    
time[:] = (time[:] - time[0])/1000; #  Convierte el tiempo a tiempo relativo (s).

dataOdd = intensity[1::2] # Separar por índice impar | L470    
dataEven = intensity[2::2] # Separar por índice par | L410
    
raw_signal = dataEven
raw_reference = dataOdd
    
"""
    
1) Calcular la SNR de la señal de fluorescencia correspondiente a la de 410 nm y 470 nm.

""" 

avg_signal= np.mean(raw_signal)
stdev_signal = np.std(raw_signal)

SNR_signal = avg_signal/stdev_signal

print(f'La SNR de la señal correspondiente al LED de 470 nm es {SNR_signal}.')

avg_ref = np.mean(raw_reference)
stdev_ref = np.std(raw_reference)

SNR_ref = avg_ref/stdev_ref

print(f'La SNR de la señal correspondiente al LED de 410 nm es {SNR_ref}.')

"""
# example.csv BLOCK --------------------------------------------------------------------------------------------
   
raw_reference = df['MeanInt_410nm'][1:]
raw_signal = df['MeanInt_470nm'][1:]
    
"""
    
# --------------------------------------------------------------------------------------------------------------
    
# Grafica la data sin procesar.
fig = plt.figure(figsize=(16, 10))
    
ax1 = fig.add_subplot(211)
ax1.plot(raw_signal,'blue',linewidth=1.5)
    
ax2 = fig.add_subplot(212)
ax2.plot(raw_reference,'purple',linewidth=1.5)
    
# ----------------------------------------------------------------------------------------------------
    
""" 
    
2) Revisa que los intervalos entre muestras del vector, que contiene la fluorescencia correspondiente 
   a ambos haces de luz, sea consistente con los cuadros por segundo especificados. Si se observan 
   picos en la resta entre un elemento del vector tiempo y su consecutivo, significa que los FPS 
   de la cámara sufrieron una caída.
 
   Si se adquiere a 40 FPS, se buscan los elementos que hayan sido adquiridos por fuera 50 milisegundos
   respecto del dato anterior y se los elimina. 
    
"""
    
# Encuentra el tiempo que efectivamente transcurre entre cada muestra.
IFI = np.diff(time[:])

# Revisar visualmente los picos que figuren al graficar IFI. Los picos abruptos significan que los FPS de la cámara sufrieron una caida.
fig = plt.figure(figsize=(16, 10))
ax1 = fig.add_subplot(211)
ax1.plot(IFI,'red',linewidth=1.5)
        
fig = plt.figure(figsize=(30, 20))
plt.hist(IFI*1000, bins = 10000)
plt.show()
    
# Se determina el umbral adecuado para IFI, el cual es 1/[FPS*0.5] (s)
    
from scipy import signal
from scipy.signal import find_peaks
        
locs, peaks = find_peaks(IFI, height=0.05) # 1/[40*0.5] = 0.05
            
df.iloc[locs,:] = 0.0000000001 #  Los cuadros malos pasan a valer 0.0000000001 para luego poder identificarlos facilmente y eliminarlos.
        
data_corr = df[df != 0.0000000001]
data_corr = data_corr.dropna() #  drop NaN.
        
intensity_corr = data_corr['intensity'][0:]
    
"""
    
3) Se extraen los valores de fluorescencia registrados correspondientes al haz de 470 nm y 410 nm.
    
"""
   
# Una vez corregidos los datos se pasa a la fase de procesamiento.
raw_signal = intensity_corr[2::2] # Separar por datos con índice par.
raw_reference = intensity_corr[1::2] # Separar por datos con índice impar.
    
raw_reference = raw_reference.iloc[:-1] # Se lo usa para emparejar el largo de "raw_reference" respecto de "raw_signal".

# ----------------------------------------------------------------------------------------------------
    
"""
    
4) Cálculo directo de la zdFF (z-score dF/F signal based on fiber photometry calcium-idependent and calcium-dependent signals).
    
"""
    
zdFF = get_zdFF(raw_reference,raw_signal) 

# Grafica el resultado.
fig = plt.figure(figsize=(16, 8))
ax1 = fig.add_subplot(111)
ax1.plot(zdFF,'black',linewidth=1.5)
# ----------------------------------------------------------------------------------------------------
    
"""
    
Cálculo de la zdFF de forma gradual, pudiendo observar los gráficos paso a paso. 
    
5) Suaviza cada señal usando un algoritmo de media móvil.

"""
    
from smooth_signal import smooth_signal
    
smooth_win = 10
smooth_reference = smooth_signal(raw_reference, smooth_win)
smooth_signal = smooth_signal(raw_signal, smooth_win)
    
fig = plt.figure(figsize=(16, 10))
ax1 = fig.add_subplot(211)
ax1.plot(smooth_signal,'blue',linewidth=1.5)
ax2 = fig.add_subplot(212)
ax2.plot(smooth_reference,'purple',linewidth=1.5)
    
"""
    
6) Realiza una corrección de línea de base para cada señal usando un algoritmo denominado Adaptative 
   Reweigthed Penalized Least Squares (AirPLS).
    
    """
    
from airPLS import airPLS
    
lambd = 5e4 # Lambda es ajustable para mejorar la estimación de la línea de base.
porder = 1
itermax = 50
r_base=airPLS(smooth_reference.T,lambda_=lambd,porder=porder,itermax=itermax)
s_base=airPLS(smooth_signal,lambda_=lambd,porder=porder,itermax=itermax)
    
fig = plt.figure(figsize=(16, 10))
ax1 = fig.add_subplot(211)
ax1.plot(smooth_signal,'blue',linewidth=1.5)
ax1.plot(s_base,'black',linewidth=1.5)
ax2 = fig.add_subplot(212)
ax2.plot(smooth_reference,'purple',linewidth=1.5)
ax2.plot(r_base,'black',linewidth=1.5)
    
# Se remueve la linea de base y el comienzo de las mediciones.
remove=200
reference_corrected = (smooth_reference[remove:] - r_base[remove:])
signal_corrected = (smooth_signal[remove:] - s_base[remove:])  
    
fig = plt.figure(figsize=(16, 10))
ax1 = fig.add_subplot(211)
ax1.plot(signal_corrected,'blue',linewidth=1.5)
ax2 = fig.add_subplot(212)
ax2.plot(reference_corrected,'purple',linewidth=1.5)

"""
    
7) Estandarizar las señales utilizando sus respectivos valores medios y desviaciones estándar. 
   Dando lugar a 2 señales denominadas zInt410 (z_reference) y zInt470 (z_signal).

"""
    
z_reference = (reference_corrected - np.median(reference_corrected)) / np.std(reference_corrected)
z_signal = (signal_corrected - np.median(signal_corrected)) / np.std(signal_corrected)

fig = plt.figure(figsize=(16, 10))
ax1 = fig.add_subplot(211)
ax1.plot(z_signal,'blue',linewidth=1.5)
ax2 = fig.add_subplot(212)
ax2.plot(z_reference,'purple',linewidth=1.5)
    
"""
    
8) Ajusta las señales estandarizadas de 410 y 470 nm a la función de regresión: y = a*x + b
    
"""
# Ajuste de la señal de calcio usando regresión lineal.
from sklearn.linear_model import Lasso
    
lin = Lasso(alpha=0.0001,precompute=True,max_iter=1000,positive=True, random_state=9999, selection='random')
n = len(z_reference)
lin.fit(z_reference.reshape(n,1), z_signal.reshape(n,1))
    
fig = plt.figure(figsize=(16, 8))
ax1 = fig.add_subplot(111)
ax1.plot(z_reference,z_signal,'b.')
    
# Alineamiento de la referencia con la señal de la señal de calcio usando regresión lineal.
z_reference_fitted = lin.predict(z_reference.reshape(n,1)).reshape(n,)
ax1.plot(z_reference,z_reference_fitted, 'r--',linewidth=1.5)
    
fig = plt.figure(figsize=(16, 8))
ax1 = fig.add_subplot(111)
ax1.plot(z_signal,'blue')
ax1.plot(z_reference_fitted,'purple')
    
"""
    
9) Calcular la expresión dF/F normalizada.

""" 
    
# Cálculo de la zdFF.
zdFF = (z_signal - z_reference_fitted)
    
# Gráfico de zdFF. 
fig = plt.figure(figsize=(16, 8))
ax1 = fig.add_subplot(111)
ax1.plot(zdFF,'black')
    
