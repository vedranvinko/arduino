My attempt at a hardware random number generator in Arduino with no external components.

Only produces ~64 bit/s because of the minimum length of the watchdog timer. :(

Tested only on a Duemilanove.  May not work on other hardware.  Post a comment if you try it on other hardware or if you find a scenario where it doesn't work.

It uses the watchdog timer to sample (and reset) Timer 1.  Since the watchdog timer runs on its own RC oscillator, and Timer 1 is on the crystal oscillator, there is random variation in the value read.  Then the randomness is spread around to all 8 bits by reading 8 times and bit-shifting and XORing, to produce a random byte.  

The assumption is that at least one bit in each sample is truly random.  Though in reality, probably multiple bits have varying amounts of entropy?  The raw read from the timer sampling is estimated at 4.4 bits of entropy per byte.

It seems to work.  I think the main flaw would be if the two oscillators become correlated to each other in certain hardware configurations or at certain points in time, which I haven't noticed, despite running it continuously for days.

* [Entropy gathering for cryptographic applications in AVR - Qualification of WDT as entropy source](http://wap.taur.dk/engather.pdf) (125 bits per second)
* [True Random Number Generation on an AtmelAVR Microcontroller](http://www.scribd.com/doc/51705150/TRUERA-1) (8 bits per second)
* [Ardrand: The Arduino as a Hardware Random-Number Generator](http://benedikt.sudo.is/ardrand.pdf) "We explore various methods to extract true randomness from
the micro-controller and conclude that it should not be used to produce
randomness from its analog pins."

Disclaimer: I have no idea what I'm doing.

But it measures better than [TrueRandom](https://code.google.com/p/tinkerit/wiki/TrueRandom).  ["TrueRandom" is not truly random](https://gist.github.com/endolith/2472824):

    Entropy = 7.544390 bits per byte.
    
    Optimum compression would reduce the size
    of this 92810048 byte file by 5 percent.
    
    Chi square distribution for 92810048 samples is 131287892.21, and randomly
    would exceed this value 0.01 percent of the times.
    
    Arithmetic mean value of data bytes is 93.7178 (127.5 = random).
    Monte Carlo value for Pi is 3.682216212 (error 17.21 percent).
    Serial correlation coefficient is -0.008583 (totally uncorrelated = 0.0).

For comparison, with `probably_random_with_TimerOne.ino`, [ent](http://www.fourmilab.ch/random/) says:

    Entropy = 7.996943 bits per byte.
    
    Optimum compression would reduce the size
    of this 65536 byte file by 0 percent.
    
    Chi square distribution for 65536 samples is 277.59, and randomly
    would exceed this value 15.83 percent of the times.
    
    Arithmetic mean value of data bytes is 127.8158 (127.5 = random).
    Monte Carlo value for Pi is 3.126899835 (error 0.47 percent).
    Serial correlation coefficient is -0.007752 (totally uncorrelated = 0.0).

Generating the 13 GB required by [dieharder](http://www.phy.duke.edu/~rgb/General/dieharder.php) would take about 48 years.  :D

Output as an image:

![noise](https://farm8.staticflickr.com/7095/6985793444_74c6359a4b_o.png)

Output scatter plotted (`plot(a, 'bo', alpha=0.1)`):

![plot](https://farm8.staticflickr.com/7099/6985796316_e9d7e3e2a0.jpg)

Histogram of output:

![histogram](https://farm8.staticflickr.com/7258/6985796104_cbc8d4eb3f.jpg)

[Gallery of tests of both TrueRandom and ProbablyRandom](https://secure.flickr.com/photos/56868697@N00/sets/72157629934367149/)

I've also tested without the TimerOne library (`probably_random.ino`), just sampling the Arduino's constantly-cycling PWM timers instead of configuring and resetting Timer 1, and it doesn't seem to hurt the randomness:

    Entropy = 7.999969 bits per byte.
    
    Optimum compression would reduce the size
    of this 5489591 byte file by 0 percent.
    
    Chi square distribution for 5489591 samples is 233.95, and randomly
    would exceed this value 75.00 percent of the times.
    
    Arithmetic mean value of data bytes is 127.4536 (127.5 = random).
    Monte Carlo value for Pi is 3.145767276 (error 0.13 percent).
    Serial correlation coefficient is -0.001267 (totally uncorrelated = 0.0).

So you should be able to use all the inputs and outputs normally, and still generate random numbers. The LSBs will still be noisy even if the MSBs are periodic, and XORing the two will preserve only the randomness of the LSBs.

Obviously if your program turns off Timer 1 completely, this will no longer produce random numbers.  (But it doesn't use any obfuscation or whitening other than the XOR shifting, so if you feed it nothing but 0s, it will output nothing but 0s and it will be obvious.)

Using *Clear Timer on Compare* mode should be ok, as long as the compare isn't  correlated with the watchdog.  If you use the watchdog timer to reset Timer 1 before reading it, for instance, you won't get random numbers.  :)

Not sure about switching Timer 1 to other prescalers.  Is it possible to run Timer 1 so slowly that it doesn't change between samples?  That would be bad.