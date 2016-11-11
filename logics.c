typedef enum {
	water, single, top, bot, mid, left, right, waterU, shipU, unknown
} shiptype;

typedef struct {
	GtkWidget *img;
	shiptype type;
	shiptype hiddenType;
} shippart;

shippart map[MAP_SIZE][MAP_SIZE];
int colcount[MAP_SIZE], rowcount[MAP_SIZE];
int visibleparts[4];

//checks if there's a ship at (x,y) or at this point's surroundings
int hasShip(int x, int y) {
	int i, j;

	for(i = -1; i<2; i++) {
		for(j = -1; j<2; j++) {
			// has to be from <1,6> range to be sure it's ship (outside of array (map[-1][-1] = weird numbers)
			if(map[x+i][y+j].hiddenType >= 1 && map[x+i][y+j].hiddenType <= 6) return 1;
		}
	}

	return 0;
}

//places a single ship with given size on the map
int placeShip(int size) {
	int margin = MAP_SIZE - (size - 1); //need this when randomizing position of the ship
	int i, counter = 0; //i for for loop, counter for while loop
	int flag = 1, overlap; //checks whether to search for another position or not
	int orientation, column, verse; //parameters of the ship

	//randomizing coordinates of the ship and checking if they're correct
	while(flag) {
		overlap = 0;
		orientation = rand()%2;

		if(orientation == S_VERTICAL) {
			//Randomizing position of the ship, specifically its 1st part
			column = rand()%margin;
			verse = rand()%MAP_SIZE;

			//checking every part of the ship and its surroundings for other ships
			for(i = 0; i<size; i++) {
				overlap = hasShip(column+i, verse);
				if(overlap == 1) break; //if points overlap, skip the for loop and start searching again
			}
		}
		else {
			column = rand()%MAP_SIZE;
			verse = rand()%margin;

			for(i = 0; i<size; i++) {
				overlap = hasShip(column, verse+i);
				if(overlap == 1) break;
			}
		}

		if(overlap == 0) { //if there are no overlaps break the loop
			flag = 0;
		}

		counter++;
		if(counter == 100) return 0; //if exceeds 100, it's probably impossible to place it
	}

	//placing ship on the map
	for(i = 0; i<size; i++) {
		if(size == 1) map[column][verse].hiddenType = single;
		else {
			if(orientation == S_VERTICAL) {
				switch(size) {
					case 2:
						if(i == 0) map[column+i][verse].hiddenType = top;
						else map[column+i][verse].hiddenType = bot;
					break;
					default:
						if(i == 0) map[column+i][verse].hiddenType = top;
						else if(i == size-1) map[column+i][verse].hiddenType = bot;
						else map[column+i][verse].hiddenType = mid;
				}
			}
			else {
				switch(size) {
					case 2:
						if(i == 0) map[column][verse+i].hiddenType = left;
						else map[column][verse+i].hiddenType = right;
					break;
					default:
						if(i == 0) map[column][verse+i].hiddenType = left;
						else if(i == size-1) map[column][verse+i].hiddenType = right;
						else map[column][verse+i].hiddenType = mid;
				}
			}
		}
	}

	return 1;
}

void countRowsCols() {
	int i, j;

	//filling counter arrays with zeroes
	for(i = 0; i<MAP_SIZE; i++) {
		colcount[i] = 0;
		rowcount[i] = 0;
	}

	for(i = 0; i<MAP_SIZE; i++) {
		for(j = 0; j<MAP_SIZE; j++) {
			colcount[i] += map[j][i].hiddenType > 0 ? 1 : 0; //so it will always add +1, regardless of number
			rowcount[i] += map[i][j].hiddenType > 0 ? 1 : 0;
		}
	}
}

//fills map with multiple ships (1x4s, 2x3s, 3x2s, 4x1s)
void fillMap() {
	int i, j;
	int statusTemp, status = 1;

	//filling with zeroes
	for(i = 0; i<MAP_SIZE; i++) {
		for(j = 0; j<MAP_SIZE; j++) {
			map[i][j].hiddenType = water;
		}
	}

	for(i = 4; i > 0; i--) {
		for(j = 0; j<5-i; j++) {
			statusTemp = placeShip(i);
			if(!statusTemp) status = 0; //if there was at least one error...
		}
	}

	if(!status) fillMap(map); //...fill again
}


//makes map for user - ~100 hidden parts, 2-4 randomly generated visible parts
void makeUserMap() {
	int freeFields, column, row, i,j;

	for(i = 0; i<MAP_SIZE; i++) {
		for(j = 0; j<MAP_SIZE; j++) {
			map[i][j].type = unknown;
		}
	}

    for(i = 0; i<4; i++) {
        visibleparts[i] = -1;
    }

	freeFields = rand()%3 + 2;
	for(i=0; i<freeFields; i++) {
		column = rand()%MAP_SIZE;
		row = rand()%MAP_SIZE;

		//ensures that at least 1 visible part is ship part
		while(i == 0 && map[column][row].hiddenType == water) {
			column = rand()%MAP_SIZE;
			row = rand()%MAP_SIZE;
		}

		map[column][row].type = map[column][row].hiddenType;
		visibleparts[i] = column*10 + row;
	}
}
