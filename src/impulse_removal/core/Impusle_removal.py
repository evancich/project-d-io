''' Remove spikes from a .wav file '''

#import pandas as pd 
import matplotlib.pyplot as plt
import numpy as np
from scipy.io import wavfile


# Memory for recursive power calculation
beta = .1

# Read Sound File
sample_rate, data = wavfile.read('AK 47 off.wav')


# Define and initialize array for power and output data values
power = np.zeros((len(data),1))
data_out = np.zeros((len(data), 1), dtype='int16')
power[0] = 0
data_out[0] = 0

# Set threshold for blanking
power_threshold = 1e6

for i in range(1, len(data)) :
    power[i] = beta*power[i-1] + (1 - beta) * data[i]**2
    if power[i] > power_threshold :
        data_out[i] = 0
    else :
        data_out[i] = data[i]

print('Max Power: ',max(power))
print('Max input value: ', max(abs(data)))
print('Max input value: ', max(abs(data_out)))

#Write out result as a .wav file
wavfile.write('AK_47_limited.wav', sample_rate, data_out)

#plot results
plt.figure(1)
plt.clf()
plt.plot(data)


plt.figure(2)
plt.clf()
plt.plot(data_out)
plt.show()





