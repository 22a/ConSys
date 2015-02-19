# Lab 2: Threaded Producer/Consumer Trade

### What the hell is this?
Simple multi threaded C solution for for simulating marketplace transactions where producers and consumers need access to shared resources. The whole point of the lab is to avoid constant polling and unnecessary mutex locks. the producers don't actually produce anything, and the consumers don't actually do anything with the resource they adamantly wait in line for. (I have some commented out code in the threads that could mess around with the producer constructing strings and then the consumer processing them on arrival but that's not what was asked of us.

I have the exec set up to take the number of consumers as a command line arg, so the program run with:
```
./pc <num-of-consumers>
```

### Sexy Output:
You're probably going to want some "sexy" output like this:

![alt tag](http://22a.me/git/conSys/after0.png)

as opposed to this:

![alt tag](http://22a.me/git/conSys/before0.png)

So, instead of running the above, run:
```
./sexy_pc <num-of-consumers>
```

### Why is this on github?
* To save me time scp'ing to remote test servers with hella cores.
* Because I put the last one up here too, might as well make a habit of it.