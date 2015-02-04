# Lab 1: Concurrent Pi

### What the hell is this?
Simple multi threaded C solution for calculating pi using the Gregory–Leibniz series that allows the user to specify the number of threads and number of iterations(elements in the series) to run. I've decided on a really weird way to delegate work to threads. Buyer beware.

There's also a python script (using matplotlib to plot number of execution time against number of threads) written by [Conor Brennan](https://github.com/c-brenn) that runs the C to show the point of diminishing returns regarding number of threads created.

There's another pair of scripts for storing the C execution time by thread number values as json and then plotting that json. They're separated for testing compatibility on machines that can't install the plotting lib.

### Plotting lib: [MatPlotLib](http://matplotlib.org/)
You're probably going to want to pip install inside a virtual env
```
pip install python-matplotlib
```

### Why is this on github?
To save me time scp'ing to remote test servers with hella cores.