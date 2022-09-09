linux: BezierSV.c
	cc BezierSV.c `pkg-config --cflags --libs gtk+-3.0` -lm -o BezierSVLinux

windows: BezierSV.c 
	i686-w64-mingw32-gcc BezierSV.c -o BezierSVWindows.exe `i686-w64-mingw32-pkg-config --cflags --libs gtk+-3.0`


