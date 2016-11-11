GdkPixbuf *shiptypes[10];
GtkWidget *colCountCont[MAP_SIZE], *rowCountCont[MAP_SIZE];

gboolean clickHandler(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
	shippart *info = user_data;

	if(info->type == unknown) {
        gtk_image_set_from_pixbuf(GTK_IMAGE(info->img), shiptypes[7]);
        info->type = waterU;
	} else if(info->type == waterU) {
        gtk_image_set_from_pixbuf(GTK_IMAGE(info->img), shiptypes[8]);
        info->type = shipU;
	} else if(info->type == shipU) {
        gtk_image_set_from_pixbuf(GTK_IMAGE(info->img), shiptypes[9]);
        info->type = unknown;
	}

	return TRUE;
}

void buttonCheckHandler(GtkButton *button, gpointer user_data) {
	int i, j, isDone = 1;
	GtkWidget *youWon;

    for(i = 0; i<MAP_SIZE; i++) {
    	for(j = 0; j<MAP_SIZE; j++) {
    		if(map[i][j].type == waterU) {
    			if(map[i][j].hiddenType == water) {
    				gtk_image_set_from_pixbuf(GTK_IMAGE(map[i][j].img), shiptypes[0]);
    				map[i][j].type = water;
    			}
    			else {
    				gtk_image_set_from_pixbuf(GTK_IMAGE(map[i][j].img), shiptypes[9]);
    				map[i][j].type = unknown;
    			}
    			continue;
    		}

    		if(map[i][j].type == shipU) {
    			if(map[i][j].hiddenType >= single && map[i][j].hiddenType <= right) {
    				gtk_image_set_from_pixbuf(GTK_IMAGE(map[i][j].img), shiptypes[map[i][j].hiddenType]);
    				map[i][j].type = map[i][j].hiddenType;
    			}
    			else {
    				gtk_image_set_from_pixbuf(GTK_IMAGE(map[i][j].img), shiptypes[9]);
    				map[i][j].type = unknown;
    			}
    			continue;
    		}
    	}
    }
    for(i = 0; i<MAP_SIZE; i++)
    	for(j = 0; j<MAP_SIZE; j++)
            if(map[i][j].type != map[i][j].hiddenType) isDone = 0;

    if(isDone == 1) {
        youWon = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "You won!");
        g_signal_connect_swapped(youWon, "response",G_CALLBACK(gtk_widget_destroy), youWon);
        gtk_widget_show_all(youWon);
    }
}

void buttonRestartHandler(GtkButton *button, gpointer user_data) {
    int i, j;
    for(i = 0; i<MAP_SIZE; i++) {
    	for(j = 0; j<MAP_SIZE; j++) {
            if(!(i*10+j == visibleparts[0] ||
               i*10+j == visibleparts[1] ||
               i*10+j == visibleparts[2] ||
               i*10+j == visibleparts[3])) {
                gtk_image_set_from_pixbuf(GTK_IMAGE(map[i][j].img), shiptypes[9]);
                map[i][j].type = unknown;
            }
        }
    }
}

void buttonNewGameHandler(GtkButton *button, gpointer user_data) {
    fillMap(); //fills map with .hiddenType
	countRowsCols(); //fills col- and rowcount
    makeUserMap();
    char collabel[3], rowlabel[3];
    int i, j;
    for(i = 0; i<MAP_SIZE; i++) {
    	for(j = 0; j<MAP_SIZE; j++) {
            gtk_image_set_from_pixbuf(GTK_IMAGE(map[i][j].img), shiptypes[map[i][j].type]);
        }
    }
    for(i = 0; i<MAP_SIZE; i++) {
		snprintf(collabel, sizeof collabel, " %d", colcount[i]); //converting to string
		gtk_label_set_text(colCountCont[i], collabel);
		snprintf(rowlabel, sizeof rowlabel, " %d", rowcount[i]);
		gtk_label_set_text(rowCountCont[i], rowlabel);
	}
}

void makePixbufs() {
	//order as in shiptype enum
	shiptypes[0] = gdk_pixbuf_new_from_file("img/0.png", NULL);
	shiptypes[1] = gdk_pixbuf_new_from_file("img/1.png", NULL);
	shiptypes[2] = gdk_pixbuf_new_from_file("img/2.png", NULL);
	shiptypes[3] = gdk_pixbuf_new_from_file("img/3.png", NULL);
	shiptypes[4] = gdk_pixbuf_new_from_file("img/4.png", NULL);
	shiptypes[5] = gdk_pixbuf_new_from_file("img/5.png", NULL);
	shiptypes[6] = gdk_pixbuf_new_from_file("img/6.png", NULL);
	shiptypes[7] = gdk_pixbuf_new_from_file("img/7.png", NULL);
	shiptypes[8] = gdk_pixbuf_new_from_file("img/8.png", NULL);
	shiptypes[9] = gdk_pixbuf_new_from_file("img/9.png", NULL);
}

//fill map with images
void makeImageMap() {
	int i, j;

	for(i = 0; i<MAP_SIZE; i++) {
		for(j = 0; j<MAP_SIZE; j++) {
			map[i][j].img = gtk_image_new();
			gtk_image_set_from_pixbuf(GTK_IMAGE(map[i][j].img), shiptypes[map[i][j].type]);
		}
	}
}

void makeOverlay() {
	int i, j;
    char collabel[3], rowlabel[3];
	GtkWidget *window, *layout, *tableCont, *btnCont;
	GtkWidget *evbox;
	GtkWidget *btnCheck, *btnRestart, *btnNew;

	makePixbufs();

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_title(GTK_WINDOW(window), "Battleships Solitaire"); //name
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 400); //size

	tableCont = gtk_table_new(MAP_SIZE_E, MAP_SIZE_E, TRUE);
	gtk_table_set_row_spacings(GTK_TABLE(tableCont), 1);
	gtk_table_set_col_spacings(GTK_TABLE(tableCont), 1);

	makeImageMap(map);

	//rows and columns counters
	for(i = 0; i<MAP_SIZE; i++) {
		snprintf(collabel, sizeof collabel, " %d", colcount[i]); //converting to string
		colCountCont[i] = gtk_label_new(collabel);
		snprintf(rowlabel, sizeof rowlabel, " %d", rowcount[i]);
		rowCountCont[i] = gtk_label_new(rowlabel);
	}

	btnCont = gtk_hbox_new(FALSE, 1);

    btnNew = gtk_button_new_with_label("New game");
    btnRestart = gtk_button_new_with_label("Restart");
    btnCheck = gtk_button_new_with_label("Check");
    //btnAbout = gtk_button_new_with_label("About");

    gtk_box_pack_start(GTK_BOX(btnCont), btnNew, TRUE, TRUE, 3);
    gtk_box_pack_start(GTK_BOX(btnCont), btnRestart, TRUE, TRUE, 3);
    gtk_box_pack_start(GTK_BOX(btnCont), btnCheck, TRUE, TRUE, 3);
    //gtk_box_pack_start(GTK_BOX(btnCont), btnAbout, TRUE, TRUE, 3);

	layout = gtk_layout_new(NULL, NULL);

	for(i = 0; i<MAP_SIZE; i++) {
		for(j = 0; j<MAP_SIZE; j++) {
			evbox = gtk_event_box_new();
			g_signal_connect(G_OBJECT(evbox), "button_press_event", G_CALLBACK(clickHandler), &map[i][j]);
			gtk_container_add(GTK_CONTAINER(evbox), map[i][j].img);
			gtk_table_attach_defaults(GTK_TABLE(tableCont), evbox, j, j+1, i, i+1);
		}
	}
	for(i = 0; i<MAP_SIZE; i++) {
		gtk_table_attach_defaults(GTK_TABLE(tableCont), colCountCont[i], i, i+1, MAP_SIZE, MAP_SIZE+1);
		gtk_table_attach_defaults(GTK_TABLE(tableCont), rowCountCont[i], MAP_SIZE, MAP_SIZE+1, i, i+1);
	}

	g_signal_connect(G_OBJECT(btnCheck), "clicked", G_CALLBACK(buttonCheckHandler), &map);
	g_signal_connect(G_OBJECT(btnNew), "clicked", G_CALLBACK(buttonNewGameHandler), NULL);
	g_signal_connect(G_OBJECT(btnRestart), "clicked", G_CALLBACK(buttonRestartHandler), NULL);

	gtk_container_add(GTK_CONTAINER(window), layout);
	gtk_layout_put(GTK_LAYOUT(layout), tableCont, 5, 5);
	gtk_layout_put(GTK_LAYOUT(layout), btnCont, 5, 365);
	gtk_widget_show_all(window);
}
