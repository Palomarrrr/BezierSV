
BezierSV.exe: BezierSV.c
	gcc BezierSV.c `pkg-config --cflags --libs gtk+-3.0` -lm -o BezierSV.exe
