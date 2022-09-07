//THIS IS ONLY A TEMPORARY SOLUTION TILL I FIND A BETTER GRAPHING LIBRARY
#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <stdlib.h>

typedef struct dataSet{
	double *x;
	double *y;
	unsigned int len;
}dataSet;

dataSet xyVals;

void splitFile(FILE *file){

	dataSet set;
	double *xpoints = malloc(10000 * sizeof(double));
	double *ypoints = malloc(10000 * sizeof(double));
	char *eptr;

	int i = 0, currChar, iX = 0, iY = 0, len = 0;

	char *tmpBuff = malloc(1000 * sizeof(char));
	memset(tmpBuff, 0, sizeof(char) * 1000);
	
	do{
		if((char)currChar == ','){
			xpoints[iX] = strtod(tmpBuff, &eptr);
			iX++;
			i = 0;
			len++;
			memset(tmpBuff, 0, sizeof(char) * 1000);
		}else if((char)currChar == '\n'){
			ypoints[iY] = strtod(tmpBuff, &eptr);
			iY++;
			i = 0;
			memset(tmpBuff, 0, sizeof(char) * 1000);
		}else if(isdigit((char)currChar) || (char)currChar == '.'){
			tmpBuff[i] = (char)currChar;
			//printf("%s\n", tmpBuff);
			i++;
		}else{
			tmpBuff[i] = ' ';
		}


	}while((currChar = fgetc(file)) != EOF);

	xyVals.len = len;
	xyVals.x = xpoints;
	xyVals.y = ypoints;
}

static gboolean drawGraph(GtkWidget *widget, cairo_t *cr, gpointer user_data){
	
	GdkRectangle surface;
	GdkWindow *window = gtk_widget_get_window(widget);
	gdouble dx = 5.0, dy = 5.0, clipX1, clipY1, clipX2, clipY2, i;
	
	gdk_window_get_geometry(window, &surface.x, &surface.y, &surface.width, &surface.height);

	cairo_set_source_rgb(cr, 1.0, 1.0, 0.95);
	cairo_paint(cr);

	cairo_translate(cr, surface.width / 2, surface.height / 2);
	cairo_scale(cr, 100.0, -100.0);

	cairo_device_to_user_distance(cr, &dx, &dy);
	cairo_clip_extents(cr, &clipX1, &clipY1, &clipX2, &clipY2);
	cairo_set_line_width(cr, dx);



	//drawing axes
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_move_to(cr, -3.18,2.4);
	cairo_line_to(cr, -3.18,-2.4);
	cairo_move_to(cr, -3.2,-2.39);
	cairo_line_to(cr, 3.2,-2.39);
	cairo_stroke(cr);
	
	dx = 1.0;
	dy = 1.0;

	cairo_device_to_user_distance(cr, &dx, &dy);
	cairo_set_line_width(cr, dx);
	for(double i = -3.16; i < 3.2; i+=0.2){
		cairo_move_to(cr, i, 2.4);
		cairo_line_to(cr, i, -2.4);
	}

	for(double i = -2.36; i < 2.4; i+=0.2){
		cairo_move_to(cr, -3.2, i);
		cairo_line_to(cr, 3.2, i);
	}
	cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.3);
	cairo_stroke(cr);
	

	//drawing the line
	dx = 1.0;
	dy = 1.0;

	cairo_device_to_user_distance(cr, &dx, &dy);
	cairo_set_line_width(cr, dx);

	int itr = 0;
	double expander = (640.0/xyVals.len);

	for(i = clipX1; i < clipX2; i+=dx * expander){
		cairo_line_to(cr, i, xyVals.y[itr] - 1);
		itr++;
	}

	cairo_set_source_rgba(cr, 0, 0, 1, 0.8);
	cairo_stroke(cr);

	return FALSE;
}

int main(int argc, char **argv){
	
	FILE *svfile;
	GtkWidget *window;
	GtkWidget *surface;

	//reading the SVPlot file
	svfile = fopen("SVPlot.txt", "r");
	splitFile(svfile);

	//setting up gtk
	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
	gtk_window_set_title(GTK_WINDOW(window), "SV Graph");

	g_signal_connect(G_OBJECT(window), "destroy", gtk_main_quit, NULL);

	surface = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), surface);

	g_signal_connect(G_OBJECT(surface), "draw", G_CALLBACK(drawGraph), NULL);

	gtk_widget_show_all(window);
	gtk_main();

	fclose(svfile);
	return 0;
}



