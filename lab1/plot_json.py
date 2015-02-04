import matplotlib.pyplot as pyplot
import json

data = open('times.json', 'r')
data = data.read()
results = json.loads(data)

thread_vals = results['thread_vals']
time_vals = results['time_vals']

pyplot.plot(thread_vals,time_vals)
pyplot.ylabel('time (sec)')
pyplot.xlabel('threads used')
pyplot.axis([1, len(thread_vals)-1, 0, max(time_vals) + min(time_vals)], 'o')
pyplot.title('A plot of time vs threads used to calculate pi')
pyplot.show()