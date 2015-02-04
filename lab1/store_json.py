import re, json
from subprocess import Popen, PIPE, STDOUT

thread_vals = []
time_vals = []

#compile C
cmd = 'gcc -pthread -o pi pi.c'
p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
p.wait() #wait for compiling to complete

max_threads = 64                #max number of threads to run C with for testing
iterations = ' 1000000000'        #1billion iterations, added the leading space to speed up concatenation
for i in range(1, max_threads):
  cmd = 'time -p ./pi ' + str(i) + iterations
  p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
  time = float(re.findall("\d+.\d+", p.stdout.readline())[0])
  thread_vals.append(i)
  time_vals.append(time)

data = json.dumps({'time_vals' :time_vals, 'thread_vals' :thread_vals})
output = open('times.json', 'w')
output.write(data)
output.flush()
output.close()