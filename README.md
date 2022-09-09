# BezierSV

A dumb tool to make SV from a Bezier Curve.

![demo image](https://imgur.com/Fs02W13.png)

I do not personally use Windows so please let me know of any instabilities that it may have on there.

## Compiling 

If you are compiling the tool from source code use the provided Makefile.
I'm not sure how to compile this on windows so help with that would be great.
For Linux, make sure you have GTK+3.0 and Gedit installed then type `make linux` 

## How to use

Using the tool should be simple 

If you are not familiar with Bezier Curves, it is a mathematical way to describe a curve based on a number of points. [This desmos page](https://www.desmos.com/calculator/ebdtbxgbq0) is a good way to understand how they function.

To start, enter your start and end time in the first two boxes.
Then add your starting and ending SV. These will be the two endpoints of your curve and are represented by the blue dots in desmos.
Next will be your two control points. In the desmos page, these are represented by the black dots.
Below them is the control points X values. Currently this is measured by a number between 0 and 1 with 0 being the beginning of the SV and 1 being the end. I plan on making this more intuitive later.
After that, enter your BPM and snapping denominator (so if 1/4, just enter 4)
Then press "Run Function". This will automatically refresh the graph to show the function you just made.
Now you have the option to "Open File" which will open the SV file in either notepad or gedit.





