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

/* WARNING THIS PROGRAM DEPENDS ON GEDIT IF YOU ARE ON LINUX
PLEASE INSTALL THIS PROGRAM IN ORDER FOR THE APP TO WORK PROPERLY
*/

typedef struct dataSet{
        long double *x;
        long double *y;
        unsigned int len;
}dataSet;


GtkWidget *mainWindow, *entryStartTime, *entryEndTime, *entryBezP1, *entryBezP4, *entryBezP2, *entryBezP2x, *entryBezP3, *entryBezP3x, *entryBPM, *entrySnap, *startButton, *label, *label2, *label3, *label4, *vbox1, *grid1, *graphButton, *showButton, *graphSurface;
char path[MAX_BUFF];
dataSet xyVals;

int timeToTick(char *timeIn){

	//I cant use timeIn directly because that messes with pango for some reason so i need to create another var for it
	char *time = malloc(sizeof(char) * 1000);
	strcpy(time, timeIn);

	int tick, isTick = 1;
	char *token;

	char *ticks[3];
	int n = 2;

	//Checking if the input time is in ticks already or in MM:ss:mmm
	//probably the worst way to check if the string has a : or ; in it but it works
	for(int i = 0; i < strlen(time); i++){
		if(time[i] == ':' || time[i] == ';'){
			isTick = 0;
		}
	}

	//if not in tick already, convert it to one. else just return the ticks
	if(isTick == 0){
		while((token = strtok_r(time, ":;", &time))) {
			ticks[n] = token;
			n--;
		}
		if(!strcmp(ticks[2], "xxx")){
			return 0;
		}

		tick = atoi(ticks[0]);
		int tickS = atoi(ticks[1]);
		int tickM = atoi (ticks[2]);

		tick = tick + (tickS * 1000);
		tick = tick + (tickM * 60000);
	}else{
		tick = atoi(time);
	}

	return tick;
}

//opening the file with a system call
void openFile() {
	#ifdef _WIN32
	 system("notepad SV.txt &");
	#elif _WIN64
	 system("notepad SV.txt &");
	#elif __linux__
	 system("gedit SV.txt &");
	#elif __APPLE__
	 system("TextEdit SV.txt &");
	#else
	 printf("wtf are you using???????");
	#endif
}

//graph drawing function
static gboolean drawGraph(GtkWidget *widget, cairo_t *cr, gpointer user_data){

	//setting up cairo
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

	//drawing grid
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

//function that actually calculates the curve
int bezSv(GtkWidget *widget, gpointer data) {

	float bezPy[4], bezPx[2];

	//if xyVals has stuff in it already, wipe it clean. if it doesnt have anything in it set its size
	if(xyVals.x != NULL){
        	memset(xyVals.x, 0, sizeof(double) * 50000);
        	memset(xyVals.y, 0, sizeof(double) * 50000);
	}else{
        	xyVals.x = malloc(50000 * sizeof(double));
        	xyVals.y = malloc(50000 * sizeof(double));
	}

	//getting all the inputs converted
	bezPy[0] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP1)));
	bezPy[1] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP2)));
	bezPy[2] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP3)));
	bezPy[3] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP4)));
	bezPx[0] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP2x)));
	bezPx[1] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP3x)));

	//getting all other needed parameters
	float startTk = timeToTick((char *)gtk_entry_get_text(GTK_ENTRY(entryStartTime)));
	float endTk = timeToTick((char *)gtk_entry_get_text(GTK_ENTRY(entryEndTime)));
	float BPM = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBPM)));
	float snap = atof((char *)gtk_entry_get_text(GTK_ENTRY(entrySnap)));

	//checking if no time was inputted
	if(startTk == 0 || endTk == 0){
		return 1;
	}

	//checking how much i will increment each point by
	double incTk = 60000.0 / snap / BPM;

	//calculating how many points i need
	float svPoints = (endTk - startTk) / incTk;

	//scaling the points down since bezier functions only really work between 0 and 1
	long double divs = 1/svPoints;

	FILE *outputFileForNP = fopen("SV.txt", "w"); // This is the txt file that will open in notepad
	int itr = 0; //i am lazy and will use this to increment the xyVals arrays instead of just scaling i up

	for( long double i = 0.0; i <= 1 + divs; i += divs) {

		//calculating the y point
		long double sv = (1 - i) * (1 - i) * (1 - i) * bezPy[0] + 3 * (1 -   i) * (1 - i) * i * bezPy[1] + 3 * (1 - i) * i * i * bezPy[2] + i * i * i * bezPy[3];
		//calculating the x point
	 	long double currentPt = (1 - i) * (1 - i) * (1 - i) * 0 + 3 * (1 -   i) * (1 - i) * i * bezPx[0] + 3 * (1 - i) * i * i * bezPx[1] + i * i * i * 1;

		//scaling these values up to normal scale
		double slope = endTk - startTk;
		long double currentTk = slope * currentPt + startTk;
		long double actTk = slope * i + startTk;
		currentTk = currentTk + (actTk - currentTk);

		//putting the values into the xyVals struct to be graphed later
		xyVals.x[itr] = floor(currentTk);
		xyVals.y[itr] = sv;

		//outputting the sv info to the file
		fprintf(outputFileForNP, "%0.lf,%Lf,4,1,0,100,0,0\n", floor(currentTk), -100.0 / sv);

		//incrementing what position xyVals is at
		itr++;
	}

	//close the file
	fclose(outputFileForNP);

	//putting how long the arrays are in .len
	xyVals.len = itr;

	//creating the option to now show the button
	gtk_widget_show(showButton);

	//update the graph
	gtk_widget_queue_draw(graphSurface);

	//return 0 for success
	return 0;
}

int main(int argc, char *argv[]) {

	//init gtk
	gtk_init(&argc, &argv);

	//setting some bigger attributes of the window
	mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainWindow), "SV Graph");
	gtk_window_set_resizable(GTK_WINDOW(mainWindow), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(mainWindow), 3);

	//assigning variables
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
	label3 = gtk_label_new("    ,    ");
	label4 = gtk_label_new("    ,    ");

	startButton = gtk_button_new_with_label("Run Function");
	showButton = gtk_button_new_with_label("Open File");

	//setting up stuff with the graph
	graphSurface = gtk_drawing_area_new();
	gtk_widget_set_size_request(graphSurface, 640, 480);

	//connecting signals so that things actually do things when called for
	g_signal_connect(mainWindow, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(showButton, "clicked", G_CALLBACK(openFile), NULL);
	g_signal_connect(startButton, "clicked", G_CALLBACK(bezSv), NULL);
	g_signal_connect(G_OBJECT(graphSurface), "draw", G_CALLBACK(drawGraph), NULL);

	//placing things in the right spot
	vbox1 = gtk_vbox_new(0, 0);
	grid1 = gtk_grid_new();

	gtk_grid_set_row_homogeneous(GTK_GRID(grid1), TRUE);

	//add the subcontainers to the main vbox
	gtk_box_pack_start(GTK_BOX(vbox1), graphSurface, 0, 1, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), grid1, 1, 1, 0);

	//adding all the elements to their place in the grid
	gtk_grid_attach(GTK_GRID(grid1), entryStartTime, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), label, 2, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), entryEndTime, 3, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), startButton, 4, 0, 1, 1);

	gtk_grid_attach(GTK_GRID(grid1), entryBezP1, 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), label2, 2, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), entryBezP4, 3, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), showButton, 4, 1, 1, 1);

	gtk_grid_attach(GTK_GRID(grid1), entryBezP2, 3, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), label3, 2, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), entryBezP2x, 1, 2, 1, 1);

	gtk_grid_attach(GTK_GRID(grid1), entryBezP3, 3, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), label4, 2, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), entryBezP3x, 1, 3, 1, 1);

	gtk_grid_attach(GTK_GRID(grid1), entryBPM, 1, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid1), entrySnap, 3, 4, 1, 1);

	//adding main vbox to the window
	gtk_container_add(GTK_CONTAINER(mainWindow), vbox1);

	//show everything but the open file button
	gtk_widget_show_all(mainWindow);
	gtk_widget_hide(showButton);

	//now we loop
	gtk_main();

	return 0;
}
