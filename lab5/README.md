the task was to make lab3 be threadsafe.

I've done that using mutexes.

Also, there was a task to make a really specific test. I've done it.

**Q:** how to run the test?

**A:** : run sequentially:

1.`cmake -S. -Bbuild`  ( yes, it will download boost::log through CPM, it can take 10 minutes. )

2.`cmake --build ./build` ( yes, it will compile some parts of boost. It's another time-comsuming event )

Then:

a.

`./build/test/test_lab5` ( it will be doing a printing of some stuff and creating files like "analysis_02.txt". It's going to be completed in 5 minutes. )

b.

`cd build`

`ctest --verbose` ( same. also, the command does unit tests from lab3 )


possible output :
```
[2023-05-04 02:27:19.178491] [0x00007f997412f6c0] [info]    Pushed an element  priority:    -4 exp.time: 2023-05-04 02:27:38 value: 0.734848
[2023-05-04 02:27:19.297842] [0x00007f99749306c0] [info]    Pushed an element  priority:    -1 exp.time: 2023-05-04 02:27:35 value: 0.657477
[2023-05-04 02:27:19.554503] [0x00007f997392e6c0] [info]    Pushed an element  priority:     1 exp.time: 2023-05-04 02:27:37 value: 0.528633
[2023-05-04 02:27:19.699197] [0x00007f997312d6c0] [info]    Popped an element
[2023-05-04 02:27:19.869816] [0x00007f997412f6c0] [info]    Pushed an element  priority:    -4 exp.time: 2023-05-04 02:27:26 value: 0.643514
[2023-05-04 02:27:20.417732] [0x00007f997392e6c0] [info]    Pushed an element  priority:    -1 exp.time: 2023-05-04 02:27:30 value: 0.194445
[2023-05-04 02:27:20.699322] [0x00007f997312d6c0] [info]    Popped an element
[2023-05-04 02:27:20.775033] [0x00007f99749306c0] [info]    Pushed an element  priority:     4 exp.time: 2023-05-04 02:27:34 value: 0.580465
[2023-05-04 02:27:21.297147] [0x00007f997412f6c0] [info]    Pushed an element  priority:     0 exp.time: 2023-05-04 02:27:34 value: 0.495006
[2023-05-04 02:27:21.699429] [0x00007f997312d6c0] [info]    Popped an element
[2023-05-04 02:27:22.089026] [0x00007f997392e6c0] [info]    Pushed an element  priority:     2 exp.time: 2023-05-04 02:27:38 value: 0.495542
[2023-05-04 02:27:22.699568] [0x00007f997312d6c0] [info]    Popped an element
[2023-05-04 02:27:22.702207] [0x00007f997392e6c0] [info]    Pushed an element  priority:    -4 exp.time: 2023-05-04 02:27:40 value: 0.812732
[2023-05-04 02:27:22.724188] [0x00007f99749306c0] [info]    Pushed an element  priority:    -1 exp.time: 2023-05-04 02:27:34 value: 0.100311
[2023-05-04 02:27:23.226440] [0x00007f997412f6c0] [error]   Queue is full
[2023-05-04 02:27:23.226613] [0x00007f997292c6c0] [info]    Analyzing...
[2023-05-04 02:27:23.227073] [0x00007f997292c6c0] [info]    Will be analyzing at most at 2023-05-04 02:27:56
[2023-05-04 02:27:23.464403] [0x00007f997392e6c0] [error]   Queue is full
[2023-05-04 02:27:23.464635] [0x00007f997292c6c0] [info]    Analyzing...
[2023-05-04 02:27:23.465188] [0x00007f997292c6c0] [info]    Will be analyzing at most at 2023-05-04 02:27:56
```
In files `analysis_*.txt` can be something like this:
```
Current time: Thu May  4 03:05:51 2023
Queue size: 15
Queue size (KiB): 0
Priority distribution:
Priority -5: 13.3333%
Priority -4: 40%
Priority -3: 6.66667%
Priority -2: 13.3333%
Priority 0: 6.66667%
Priority -1: 13.3333%
Priority 2: 6.66667%
Max time difference of messages (s): 14
```

