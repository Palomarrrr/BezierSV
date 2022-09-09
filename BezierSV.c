#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#define MAX_BUFF 200

#ifdef _WIN32
	#include <windows.h>
#elif _WIN64
	#include <windows.h>
#else
#endif

/* 
WARNING THIS PROGRAM DEPENDS ON GEDIT IF YOU ARE ON LINUX
PLEASE INSTALL THIS PROGRAM IN ORDER FOR THE APP TO WORK PROPERLY
*/

typedef struct dataSet{
        double *x;
        double *y;
        unsigned int len;
}dataSet;


GtkWidget *mainWindow, *outptWindow, *outptBox, *entryStartTime, *entryEndTime, *entryBezP1, *entryBezP4, *entryBezP2, *entryBezP2x, *entryBezP3, *entryBezP3x, *entryBPM, *entrySnap, *startButton, *label, *label2, *label3, *label4, *vbox1, *grid1, *graphButton, *showButton, *graphSurface;
char path[MAX_BUFF];
dataSet xyVals;



int timeToTick(char *time){
	int tick;
	char *token;

	char *ticks[3];
	int n = 0;
	while((token = strtok_r(time, ":;", &time))) { 
		ticks[n] = token;
		n++;
	}

	tick = atoi(ticks[2]);
	int tickS = atoi(ticks[1]);
	int tickM = atoi (ticks[0]);

	tick = tick + (tickS * 1000);
	tick = tick + (tickM * 60000);

	return tick;
}

float bezSv(GtkWidget *widget, gpointer data) {
	float bezPy[4], bezPx[2];

	 bezPy[0] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP1))); 
	 bezPy[1] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP2))); 
	 bezPy[2] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP3))); 
	 bezPy[3] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP4))); 
	 bezPx[0] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP2x))); 
	 bezPx[1] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP3x))); 

	 
	 float startTk = timeToTick((char *)gtk_entry_get_text(GTK_ENTRY(entryStartTime)));
	 float endTk = timeToTick((char *)gtk_entry_get_text(GTK_ENTRY(entryEndTime)));
	 float BPM = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBPM)));
	 float snap = atof((char *)gtk_entry_get_text(GTK_ENTRY(entrySnap)));

	 double incTk = 60000.0 / snap / BPM;

	 float svPoints = (endTk - startTk) / incTk;
	
	 long double divs = 1/svPoints;

	 FILE *outputFileForGraph = fopen("SVPlot.txt", "w"); // This is the output file for OTFGraph because im lazy and dont want to do something smarter
	 FILE *outputFileForNP = fopen("SV.txt", "w"); // This is the txt file that will open in notepad

	 for( long double i = 0.0; i <= 1 + divs; i += divs) {

	 	long double sv = (1 - i) * (1 - i) * (1 - i) * bezPy[0] + 3 * (1 -   i) * (1 - i) * i * bezPy[1] + 3 * (1 - i) * i * i * bezPy[2] + i * i * i * bezPy[3];	
	 	long double currentPt = (1 - i) * (1 - i) * (1 - i) * 0 + 3 * (1 -   i) * (1 - i) * i * bezPx[0] + 3 * (1 - i) * i * i * bezPx[1] + i * i * i * 1;	

		
		double slope = endTk - startTk;
		long double currentTk = slope * currentPt + startTk;
		long double actTk = slope * i + startTk;

		currentTk = currentTk + (actTk - currentTk);

		fprintf(outputFileForGraph, "%0.lf,%Lf\n", floor(currentTk), sv);
		fprintf(outputFileForNP, "%0.lf,%Lf,4,1,0,100,0,0\n", floor(currentTk), -100.0 / sv);

	 }

	fclose(outputFileForGraph);
	fclose(outputFileForNP);
}

int splitFile(){

	FILE *svfile;
	svfile = fopen("SVPlot.txt", "r");

	if(svfile == NULL)
		return 1;

        dataSet set;
        double *xpoints = malloc(50000 * sizeof(double));
        double *ypoints = malloc(50000 * sizeof(double));
        char *eptr;

        int i = 0, currChar, iX = 0, iY = 0, len = 0;

        char *tmpBuff = malloc(50000* sizeof(char));
        memset(tmpBuff, 0, sizeof(char) * 50000);

        do{
                if((char)currChar == ','){
                        xpoints[iX] = strtod(tmpBuff, &eptr);
                        iX++;
                        i = 0;
                        len++;
                        memset(tmpBuff, 0, sizeof(char) * 50000);
                }else if((char)currChar == '\n'){
                        ypoints[iY] = strtod(tmpBuff, &eptr);
                        iY++;
                        i = 0;
                        memset(tmpBuff, 0, sizeof(char) * 50000);
                }else if(isdigit((char)currChar) || (char)currChar == '.'){
                        tmpBuff[i] = (char)currChar;
                        i++;
                }else{
                        tmpBuff[i] = ' ';
                }


        }while((currChar = fgetc(svfile)) != EOF);

        xyVals.len = len;
        xyVals.x = xpoints;
        xyVals.y = ypoints;                                                                                                              

	fclose(svfile);

	return 0;
}                                           

void openFile() {
	#ifdef _WIN32
	 system("notepad SV.txt");
	#elif _WIN64
	 system("notepad SV.txt");
	#elif __linux__
	 system("gedit SV.txt");
	#elif __APPLE__
	 printf("applerss");
	#else
	 printf("wtf are you using???????");
	#endif
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

	if(xyVals.x == NULL)
		return FALSE;
	

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

void displayGraph(GtkWidget *widget, gpointer data) {

	if(xyVals.x != NULL){
        	memset(xyVals.x, 0, sizeof(double) * 50000);
        	memset(xyVals.y, 0, sizeof(double) * 50000);
	}

	splitFile();
	gtk_widget_queue_draw(graphSurface);
}

int main(int argc, char *argv[]) {

	splitFile();

	gtk_init(&argc, &argv);

	mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainWindow), "SV Graph");

	entryStartTime = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryStartTime), "x:xx:xxx");
	entryEndTime = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryEndTime), "x:xx:xxx");

	entryBezP1 = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryBezP1), "Start");
	entryBezP2 = gtk_entry_new();
	entryBezP2x = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryBezP2), "Ctrl 1");
	gtk_entry_set_text(GTK_ENTRY(entryBezP2x), "Ctrl 1 X");
	entryBezP3 = gtk_entry_new();
	entryBezP3x = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryBezP3), "Ctrl 2");
	gtk_entry_set_text(GTK_ENTRY(entryBezP3x), "Ctrl 2 X");
	entryBezP4 = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryBezP4), "End");

	entryBPM = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryBPM), "BPM");
	entrySnap = gtk_entry_new(); gtk_entry_set_text(GTK_ENTRY(entrySnap), "Snap");

	label = gtk_label_new("--Time->");
	label2 = gtk_label_new("--SV->");
	label3 = gtk_label_new("--Ctrls->");
	label4 = gtk_label_new("--Ctrls->");

	graphButton = gtk_button_new_with_label("Show Graph");
	showButton = gtk_button_new_with_label("Open File");
	startButton = gtk_button_new_with_label("Run Function");

	graphSurface = gtk_drawing_area_new();
	gtk_widget_set_size_request(graphSurface, 640, 480);
	

	gtk_container_set_border_width(GTK_CONTAINER(mainWindow), 3);

	g_signal_connect(mainWindow, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(showButton, "clicked", G_CALLBACK(openFile), NULL);
	g_signal_connect(startButton, "clicked", G_CALLBACK(bezSv), NULL);
	g_signal_connect(graphButton, "clicked", G_CALLBACK(displayGraph), NULL);
	g_signal_connect(G_OBJECT(graphSurface), "draw", G_CALLBACK(drawGraph), NULL);

	vbox1 = gtk_vbox_new(0, 0);
	grid1 = gtk_grid_new();

	gtk_grid_set_row_homogeneous(GTK_GRID(grid1), TRUE);

	gtk_box_pack_start(GTK_BOX(vbox1), graphSurface, 0, 1, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), grid1, 1, 1, 0);

	gtk_grid_attach(GTK_GRID(grid1), entryStartTime, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), label, 2, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), entryEndTime, 3, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), graphButton, 4, 0, 1, 1);

	gtk_grid_attach(GTK_GRID(grid1), entryBezP1, 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), label2, 2, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), entryBezP4, 3, 1, 1, 1);

	gtk_grid_attach(GTK_GRID(grid1), entryBezP2, 1, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), label3, 2, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), entryBezP3, 3, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), startButton, 4, 2, 1, 1);

	gtk_grid_attach(GTK_GRID(grid1), entryBezP2x, 1, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), label4, 2, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), entryBezP3x, 3, 3, 1, 1);

	gtk_grid_attach(GTK_GRID(grid1), entryBPM, 1, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), entrySnap, 3, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), showButton, 4, 4, 1, 1);


	gtk_container_add(GTK_CONTAINER(mainWindow), vbox1);

	gtk_widget_show_all(mainWindow);

	gtk_main();

	return 0;
}
