
BezierSV: BezierSV.c
	gcc BezierSV.c `pkg-config --cflags --libs gtk+-3.0` -lm -o BezierSV
	g++ OTFGraph.cpp -DWITHOUT_NUMPY -I/usr/include/python3.10 -lpython3.10 -I/usr/include/matplotlib-cpp -o OTFGraph

