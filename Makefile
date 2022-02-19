
BezierSV.exe: BezierSV.c
	gcc BezierSV.c `pkg-config --cflags --libs gtk+-2.0` -lm -o BezierSV.exe
