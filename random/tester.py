# -*- coding: utf-8 -*-

from __future__ import division
import serial
from pylab import *
from time import time
import Image

size = 2**16

with serial.Serial('COM6', 115200) as port:
    start_time = time()
    data += port.read(size)
    elapsed_time = time() - start_time

print 'Read ' + str(size) + ' bytes in ' + str(int(round(elapsed_time))) + ' s'
print 'Data rate: %.1f bit/s' % (size*8 / elapsed_time)

# Binary dump
with open(str(int(time())) + 'out.bin','wb') as f:
    f.write(data)

a = numpy.fromstring(data, dtype = 'uint8')

# Plot
figure()
plot(a, 'bo', alpha=0.1) # Transparent to show stackups

# Histogram
figure()
hist(a, bins=64, range=[0,255])

# Image
repeat = int(sqrt(size))
b = reshape(a[:len(a) - len(a)%repeat], (-1, repeat))
im = Image.fromarray(b)
im.save(str(int(time())) + 'out.png')