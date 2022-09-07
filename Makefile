linux: BezierSV.c SVGraph.c
	cc BezierSV.c `pkg-config --cflags --libs gtk+-2.0` -lm -o BezierSV
	cc SVGraph.c -lm `pkg-config --cflags --libs gtk+-3.0` -o SVGraph

windows: BezierSV.c SVGraph.c
	i686-w64-mingw32-gcc BezierSV.c -o BezierSV.exe `i686-w64-mingw32-pkg-config --cflags --libs gtk+-3.0`
	i686-w64-mingw32-gcc SVGraph.c -o SVGraph.exe -lm `i686-w64-mingw32-pkg-config --cflags --libs gtk+-3.0`


