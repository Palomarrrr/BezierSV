#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <gtk/gtk.h>
#include <strings.h>

GtkWidget *mainWindow, *outptWindow, *outptBox, *entryStartTime, *entryEndTime, *entryBezP1, *entryBezP4, *entryBezP2, *entryBezP2x, *entryBezP3, *entryBezP3x, *entryBPM, *entrySnap, *startButton, *label, *label2, *vbox1, *hbox1, *hbox2, *hbox3, *label3, *hbox4, *hbox5;

int getDenom(char *snapping){
	float snap; 
	char *snapChar[2];
	char *token;
	int i = 0;

	while((token = strtok_r(snapping, "/", &snapping))) { 
		snapChar[i] = token;
		i++;
	}
	
	snap = atof(snapChar[1]);

	return snap;
}

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

	 
	 //float startTk = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryStartTime)));
	 float startTk = timeToTick((char *)gtk_entry_get_text(GTK_ENTRY(entryStartTime)));
	 //float endTk = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryEndTime)));
	 float endTk = timeToTick((char *)gtk_entry_get_text(GTK_ENTRY(entryEndTime)));
	 float BPM = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBPM)));
	 float snap = getDenom((char *)gtk_entry_get_text(GTK_ENTRY(entrySnap)));

	 double incTk = 60000.0 / snap / BPM;

	 float svPoints = (endTk - startTk) / incTk;
	
	 long double divs = 1/svPoints;

	 FILE *outputFile = fopen("SV.txt", "w");

	 for( long double i = 0.0; i <= 1 + divs; i += divs) {

	 	long double sv = (1 - i) * (1 - i) * (1 - i) * bezPy[0] + 3 * (1 -   i) * (1 - i) * i * bezPy[1] + 3 * (1 - i) * i * i * bezPy[2] + i * i * i * bezPy[3];	
	 	long double currentPt = (1 - i) * (1 - i) * (1 - i) * 0 + 3 * (1 -   i) * (1 - i) * i * bezPx[0] + 3 * (1 - i) * i * i * bezPx[1] + i * i * i * 1;	

		
		double slope = endTk - startTk;

		long double currentTk = slope * currentPt + startTk;
		long double actTk = slope * i + startTk;

		currentTk = currentTk + (actTk - currentTk);

		fprintf(outputFile, "%0.lf,%Lf,4,1,0,100,0,0\n", floor(currentTk), -100.0 / sv);

	 }

	 fclose(outputFile);

	#ifdef _WIN32
	 system("notepad SV.txt");
	#elif _WIN64
	 system("notepad SV.txt");
	#elif __linux__
	 system("gedit SV.txt");
	#elif __APPLE__
	 printf("Get a real pc");
	#else
	 printf("wtf are you using???????");
	#endif

}



int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv);

	mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

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
	entrySnap = gtk_entry_new(); gtk_entry_set_text(GTK_ENTRY(entrySnap), "1/x");

	label = gtk_label_new("--Time->");
	label2 = gtk_label_new("--SV->");
	label3 = gtk_label_new("--Ctrls->");

	startButton = gtk_button_new_with_label("Start");


	vbox1 = gtk_vbox_new(1, 0);
	hbox1 = gtk_hbox_new(1, 0);
	hbox2 = gtk_hbox_new(1, 0);
	hbox3 = gtk_hbox_new(1, 0);
	hbox4 = gtk_hbox_new(1, 0);
	hbox5 = gtk_hbox_new(1, 0);
	

	gtk_box_pack_start(GTK_BOX(vbox1), hbox1, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox2, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox3, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox4, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox5, 0, 0, 0);

	gtk_box_pack_start(GTK_BOX(hbox1), entryStartTime, 0, 1, 5);
	gtk_box_pack_start(GTK_BOX(hbox1), label, 0, 1, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), entryEndTime, 0, 1, 5);

	gtk_box_pack_start(GTK_BOX(hbox2), entryBezP1, 0, 1, 5);
	gtk_box_pack_start(GTK_BOX(hbox2), label2, 0, 1, 0);
	gtk_box_pack_start(GTK_BOX(hbox2), entryBezP4, 0, 1, 5);

	gtk_box_pack_start(GTK_BOX(hbox3), entryBezP2, 1, 1, 5);
	gtk_box_pack_start(GTK_BOX(hbox3), entryBezP2x, 1, 1, 5);
	gtk_box_pack_start(GTK_BOX(hbox3), label3, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(hbox3), entryBezP3, 1, 1, 5);
	gtk_box_pack_start(GTK_BOX(hbox3), entryBezP3x, 1, 1, 5);

	gtk_box_pack_start(GTK_BOX(hbox4), entryBPM, 0, 0, 5);
	gtk_box_pack_start(GTK_BOX(hbox4), entrySnap, 0, 0, 5);

	gtk_box_pack_start(GTK_BOX(hbox5), startButton, 0, 0, 5);

	g_signal_connect(mainWindow, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(startButton, "clicked", G_CALLBACK(bezSv), NULL);

	gtk_container_add(GTK_CONTAINER(mainWindow), vbox1);
	

	gtk_widget_show_all(mainWindow);
	gtk_window_resize(GTK_WINDOW(mainWindow), 200, 200);

	gtk_main();
	return 0;
}
