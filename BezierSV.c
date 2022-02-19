#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>

GtkWidget *mainWindow, *entryStartTime, *entryEndTime, *entryBezP1, *entryBezP4, *entryBezP2, *entryBezP3, *entryBPM, *entrySnap, *startButton, *label, *label2, *vbox1, *hbox1, *hbox2, *hbox3, *label3, *hbox4, *hbox5;




float bezSv(GtkWidget *widget, gpointer data) {
	float bezPy[4];

	 bezPy[0] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP1))); 
	 bezPy[1] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP2))); 
	 bezPy[2] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP3))); 
	 bezPy[3] = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBezP4))); 

	 
	 float startTk = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryStartTime)));
	 float endTk = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryEndTime)));
	 float BPM = atof((char *)gtk_entry_get_text(GTK_ENTRY(entryBPM)));
	 int snap = atoi((char *)gtk_entry_get_text(GTK_ENTRY(entrySnap)));

	 int incTk = 60000.0 / snap / BPM;

	 float svPoints = (endTk - startTk) / incTk;
	
	 long double divs = 1/svPoints;

	 double currentTk = startTk;

	 FILE *outputFile = fopen("SV.txt", "w");

	 for( double i = 0.0; i <= 1; i += divs) {

	 	double sv = (1 - i) * (1 - i) * (1 - i) * bezPy[0] + 3 * (1 -   i) * (1 - i) * i * bezPy[1] + 3 * (1 - i) * i * i * bezPy[2] + i * i * i * bezPy[3];	

		fprintf(outputFile, "%0.lf,%lf,4,1,0,100,0,0\n", floor(currentTk), -100 / sv);

		currentTk = currentTk + incTk;
		
	 }

	 fclose(outputFile);

}

int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv);

	mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	entryStartTime = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryStartTime), "Start Tick");
	entryEndTime = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryEndTime), "End Tick");

	entryBezP1 = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryBezP1), "Start");
	entryBezP2 = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryBezP2), "Ctrl 1");
	entryBezP3 = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryBezP3), "Ctrl 2");
	entryBezP4 = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryBezP4), "End");

	entryBPM = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entryBPM), "BPM");
	entrySnap = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entrySnap), "1/x (enter x)");

	label = gtk_label_new("--Ticks->");
	label2 = gtk_label_new("--General->");
	label3 = gtk_label_new("--Controls->");

	startButton = gtk_button_new_with_mnemonic("Start");


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

	gtk_box_pack_start(GTK_BOX(hbox1), entryStartTime, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), label, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), entryEndTime, 0, 0, 0);

	gtk_box_pack_start(GTK_BOX(hbox2), entryBezP1,0 ,0 ,0);
	gtk_box_pack_start(GTK_BOX(hbox2), label2, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(hbox2), entryBezP4, 0, 0, 0);

	gtk_box_pack_start(GTK_BOX(hbox3), entryBezP2, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(hbox3), label3, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(hbox3), entryBezP3, 0, 0, 0);

	gtk_box_pack_start(GTK_BOX(hbox4), entryBPM, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(hbox4), entrySnap, 0, 0, 0);

	gtk_box_pack_start(GTK_BOX(hbox5), startButton, 0, 0, 0);


	g_signal_connect(mainWindow, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(startButton, "clicked", G_CALLBACK(bezSv), NULL);

	gtk_container_add(GTK_CONTAINER(mainWindow), vbox1);

	gtk_widget_show_all(mainWindow);
	gtk_main();
	return 0;
}
