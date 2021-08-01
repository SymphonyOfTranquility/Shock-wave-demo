# Shock wave demo
This is a program that shows realistic shock wave on images.
Used 2D library SFML to process image and ImGui to show user interface to change parameters.

## Pre-run instructions
The program should be run on Visual Studio. \
Platform configuration: ***Visual Studio 2017 (v141)***. (Required by SFML)\
Windows SDK version: ***(10.0.18362.0)***. (Required by SFML)\
Solution configuration should be Release, x64.

## Run instructions
At the start user will see 4 movable menus(click on the cap of the menu to move): 
* __Load image__
* __Algorithm type__ 
* __Shock wave parameters__
* __Benchmark__

Also by default loaded image is *Test Image Colored.jpg* that is included in the project. \
The demo of shock waves will start immediately from random points.

### Menu _Load image_
In this menu, user can find **Load image** button. On a click, user can load his image from the disk.

### Menu _Algorithm type_
In this menu, user can find radiobuttons:
* __Sequential__ - if is on - works sequential algorithm;
* __Parallel__       - if is on - works parallel algorith.

### Menu _Shock wave parameters_
In this menu user can configure shock wave and demo parameters:
* __Speed simulation__ (_speed\_simul_) - parameter to change time flow for **one current** shock wave:
	* `speed_simul < 0`		- Simulation works in reverse;
	* `speed_simul == 0`	- Time is stopped, nothing is changing; 
	* `0 < speed_simul < 1`	- Simulation is slow down of real time simulation;
	* `speed_simul == 1`	- Simulation works in **real time**;
	* `speed_simul > 1`		- Simulation is speed up real time simulation.\
    By default speed simulation is __1__ (min = -1, max = 2);

* __Velocity__ - parameter that changes the speed of shock wave.\
	By default velocity is __80__ (min = 1, max = 200);

* __Magnitude__ - parameter that changes the maximum deviation of pixel from a real image on shock wave.\
	By default magnitude is __15__ (min = -100, max = 100);

* __Mean__ - parameter that changes the mean of the cumulative distribution function for the normal distribution (details [here](https://en.wikipedia.org/wiki/Cumulative_distribution_function)).\
	By default mean value is __19.5__ (min = -20, max = 20);

* __Scale__ - parameter that changes the scale of the cumulative distribution function for the normal distribution (changes how smooth is distortion). \
	By default scale value is __1.1__ (min = 0, max = 8);

* __Ring size__ - parameter that changes ring of the distorted circle. \
	By default ring size value is __70__ (min = 1, max = 100).
	
### Menu _Benchmark_
In this menu user can start benchmark for sequential and parallel algorithms.

* __Steps number__ - number of steps to run each algo.\
    By default steps number value __100__ (min = 100, max = 10000);
* __Scale image__ - scale size of image only for benchmark. Size after scale is `((scale*N) x (scale*M))`, \
where 
    * `scale` - scale parameter;
    * `N` - width; 
    * `M = 600` - height.
    
By default results of benchmark are `-nan(ind)`.
To start benchmark click button __Start__. After that program will be frozen for a while. \
As results user will see time of work for each algorithm and program will continue working.

