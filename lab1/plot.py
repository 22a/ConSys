import matplotlib.pyplot as pyplot
import os
import re
from subprocess import Popen, PIPE, STDOUT
import multiprocessing as mp
import math

x = []
y = []

#compile subprocess code
cmd = "gcc -pthread -o pi newPi.c"
p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
p.wait() #wait for compiling to complete

samples = 2
max_threads = 16
iterations = "100000000"			#100 million iterations
for i in range(1, max_threads):
	cmd = "time -p ./pi " + str(i) + " " + iterations
	avg_time = 0
	for j in range(samples):
		p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
		avg_time = avg_time + float(re.findall("\d+.\d+", p.stdout.readline())[0])
	avg_time = avg_time / samples
	x.append(i)
	y.append(avg_time)

pyplot.plot(x,y)
pyplot.ylabel('time (sec)')
pyplot.xlabel('threads used')
pyplot.axis([1, max_threads - 1, 0, max(y) + min(y)], 'o')
pyplot.axvline(mp.cpu_count(), color='r')
pyplot.title('A plot of time vs threads used to calculate pi')
pyplot.show()