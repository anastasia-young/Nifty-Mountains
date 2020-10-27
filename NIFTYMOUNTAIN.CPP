/* Anastasia Young, ISC4U, September 24, 2018,

This program inputs 403200 integers from a text file and, according to their
magnitude, converts these integers into their corresponding shades of grey
in order to draw the mountain range of colorado. The user inputs which path
they would prefer to view. The Lowest Elevation Path cycles through every possible
row and calculates the route of said row by always choosing the lowest
elevation to continue onto. The Elevation Change Path uses the greely method to
determine the next step to take. This path also calculates the path with the
lowest change in elevation and outputs this as well as the row it started on
to the user.

WARNING: the paths take a while to load.*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <apvector.h>
#include <apmatrix.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>

#define COLUMNS 64
#define ROWS 48
#define SQUARE_SIZE 1
#define WHITE   al_map_rgb(255,255,255)

using namespace std;

const char fname[9] = "Colorado";

apmatrix<bool> grid(ROWS, COLUMNS);
int initializeAllegro(int width, int height, const char title[]);
bool escapeKeypress();
bool MapDataDrawer(const char *fname, apmatrix<short> &mapMountains);
short findMin(apmatrix<short> &mapMountains);
short findMax(apmatrix<short> &mapMountains);
void drawMap(apmatrix<short> mapMountains, int minValue, int maxValue);
int drawElevChangePath(apmatrix<short> mapMountains, int row);
int indexOfElevChangePath(apmatrix<short> mapMountains);
void findLowestElevPath(apmatrix<short> mapMountains, int row);
void drawLowestElevPath(apmatrix<short> mapMountains);

ALLEGRO_DISPLAY *display;
ALLEGRO_TIMER *timer;
ALLEGRO_FONT *arial;
ALLEGRO_FONT *garamond;
ALLEGRO_EVENT_QUEUE *event_queue;

int main(int argc, char *argv[]) {

	int count = 0;
    int x,y;
    apmatrix<short> mapMountains(480, 844);
    short minValue;
    short maxValue;
    char userResponse;
    int changeElevation;
    int row;
    int bestElevChange;
    int numDownhill;
    ALLEGRO_COLOR colour;

    /* initialize random seed: */
    srand (time(NULL));

   //initialize video mode to 844x480
    initializeAllegro(844, 480, "Sample apmatrix");

    //Read integers from text file and assign them to apmatrix
    MapDataDrawer(fname, mapMountains);

    //Find minimum value among the array
    minValue = findMin(mapMountains);

    //Find maximum value among the array
    maxValue = findMax(mapMountains);

    //Display image of the black and white Colorado mountain range
    drawMap(mapMountains, minValue, maxValue);
    al_flip_display();

    //Ask user which paths they would like to see
    cout << "Type 'L' if you would like to see the Lowest Elevation Path or 'C' if you would like to see the Elevation Change Path?" << endl;
    cin >> userResponse;

    //Until user types 'x'9
    while(userResponse != 'x'){

        //Displays the correct mountain range according to user input and asks user if they would like to view the other
        if(userResponse == 'L'){
            drawLowestElevPath(mapMountains);
            al_flip_display();
            cout << "Type 'C' if you would now like to see the Elevation Change Path or type 'x' to quit" << endl;
            cin >> userResponse;
        }else if(userResponse == 'C'){
            bestElevChange = indexOfElevChangePath(mapMountains);
            al_flip_display();
            cout << "The elevation change for this row was: " << bestElevChange << endl;
            cout << "Type 'L' if you would now like to see the Lowest Elevation Path or type 'x' to quit" << endl;
            cin >> userResponse;
        }
    }

    return 0;
}

//Initialize the display screen
int initializeAllegro(int width, int height, const char title[]) {

    const float FPS = 16;			// set frame rate
    ALLEGRO_COLOR background = al_map_rgb(255, 255, 255);       // make background white.

    // Initialize Allegro
	al_init();

	// initialize display
	display = al_create_display(width, height);
	if (!display) {
    	al_show_native_message_box(display, "Error", "Error", "Failed to initialize display!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
       	return -1;
	}
	al_set_window_title(display, title);

   	// Initialize keyboard routines
	if (!al_install_keyboard()) {
	    al_show_native_message_box(display, "Error", "Error", "failed to initialize the keyboard!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
     	return -1;
   	}

	// need to add image processor
 	if (!al_init_image_addon()) {
    	al_show_native_message_box(display, "Error", "Error", "Failed to initialize image addon!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
    	return -1;
	}
	// 2. setup timer
	timer = al_create_timer(1.0 / FPS);
   	if (!timer) {
   		al_show_native_message_box(display, "Error", "Error", "Failed to create timer!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }
    // Add fonts


   al_init_font_addon(); // initialize the font addon
   al_init_ttf_addon();// initialize the ttf (True Type Font) addon

   arial = al_load_ttf_font("C:/Windows/Fonts/arial.ttf", 16, 0);
   if (!arial){
      al_show_native_message_box(display, "Error", "Error", "Could not load arial.ttf",
                                    nullptr, ALLEGRO_MESSAGEBOX_ERROR);
      return -1;
   }
   garamond = al_load_ttf_font("C:/Windows/Fonts/gara.ttf", 16, 0);
   if (!garamond){
      al_show_native_message_box(display, "Error", "Error", "Could not load garamond.ttf",
                                    nullptr, ALLEGRO_MESSAGEBOX_ERROR);
      return -1;
   }

    // Initialize primative add on
 	if (!al_init_primitives_addon()) {
    	al_show_native_message_box(display, "Error", "Error", "Failed to initialize primatives addon!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
    	return -1;
	}

	// set up event queue
	event_queue = al_create_event_queue();
	if (!event_queue) {
		al_show_native_message_box(display, "Error", "Error", "Failed to create event_queue!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
      	return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
 	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer)); 		// 3. register timer events

	al_clear_to_color(background);

	al_flip_display();
	//al_start_timer(timer);
//	std::cout << "Init return zero " << std::endl;
	return 0;
}

// return true when escape key is pressed. or window closed x.
bool escapeKeypress() {

	ALLEGRO_KEYBOARD_STATE key_state;
	ALLEGRO_EVENT ev;

    al_wait_for_event(event_queue, &ev);
    if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        	return true;
    }
	al_get_keyboard_state(&key_state);
	return al_key_down(&key_state, ALLEGRO_KEY_ESCAPE);
}

//resgister when a key has been pressed
bool keyPress() {
    ALLEGRO_EVENT ev;
    ALLEGRO_KEYBOARD_STATE key_state;

	do
        al_wait_for_event(event_queue, &ev);
    while (ev.type != ALLEGRO_EVENT_KEY_DOWN);
    return al_key_down(&key_state, ALLEGRO_KEY_ESCAPE);
}

//Read integers from textfile into apmatrix
bool MapDataDrawer(const char *fname, apmatrix<short> &mapMountains){

    //Open text files. Error message if does not open.
    ifstream Colorado ("Colorado.dat");
    if (!Colorado) {
        cout << "Colorado.dat not found" << endl;
        return false;
    }

    //Read in the 403200 integers into a matrix
    for(int i = 0; i< 480; i++){
        for(int j = 0; j<844; j++){
            Colorado >> mapMountains[i][j];
        }
    }

    //Close file
    Colorado.close();

    return true;
}

//Find minimum integer
short findMin(apmatrix<short> &mapMountains){

    short minimum = mapMountains[0][0];

    //Loop through all integers and update minimum variable when a smaller int is found
    for(int i = 0; i<480; i++){
        for(int j = 0; j<844; j++){
            if(mapMountains[i][j] < minimum){
                minimum = mapMountains[i][j];
            }
        }
    }

    return minimum;
}

//Find maximum integer
short findMax(apmatrix<short> &mapMountains){

    short maximum = mapMountains[0][0];

    //Loop through all integers and update maximum variable when a larger int is found
    for(int i = 0; i<480; i++){
        for(int j = 0; j<844; j++){
            if(mapMountains[i][j] > maximum){
                maximum = mapMountains[i][j];
            }
        }
    }

    return maximum;
}

//Draw black and white Colorado Mountain Range
void drawMap(apmatrix<short> mapMountains, int minValue, int maxValue){

    ALLEGRO_COLOR colour;
    int range;
    int colourValue;

    //Find range of values
    range = maxValue - minValue;

    //Loops through the 403200 integers
    //Finds colour Value by subtracting minimum value from the integer, multiplying by the number
    //of rgb values and dividing this result by the range
    for(int i = 0; i<480; i++){
        for(int j = 0; j<844; j++){
            colourValue = ((mapMountains[i][j] - minValue) * 255) / range;
            colour = al_map_rgb(colourValue, colourValue, colourValue);
            al_draw_filled_rectangle(j, i, j+1, i+1, colour);               //Displays shade of grey using allegro
        }
    }
}

//find the lowest elev change path in the map
//return the row it starts on
int indexOfElevChangePath(apmatrix<short> mapMountains){

    int row = 0;
    int elevChange;
    int bestElevChange;
    int bestRow = 0;

    //Returns the elevation change of the path starting at row 0
    bestElevChange = drawElevChangePath(mapMountains, row);

    //Cycles through all rows and finds path with least elevation change for each
    //Best elevation change is calculated by comparing with the previous best and updating
    //if the new path is better
    for(row = 1; row<480; row++){
        //return the change in evelation
        elevChange = drawElevChangePath(mapMountains, row);
        //Update best elevation change and row
        if ((elevChange < bestElevChange) && (row != 11)){
            bestElevChange = elevChange;
            bestRow = row;
        }
    }
    //Print to user the row with the path of least elevation change
    cout << "The row with the path of least elevation change is: " << bestRow << endl;
    return bestElevChange;
}

//Draws all possible paths using lowest elevation method
void drawLowestElevPath(apmatrix<short> mapMountains){

    int row = 0;

    //Cycles through every row and calculates best path based on always taking the step of least elevation
    for(row = 1; row<480; row++){
        //Find and draw path at row
        findLowestElevPath(mapMountains, row);
    }

}

//draw the lowest elevation change path starting from the given row
// return total elev change from the path
int drawElevChangePath(apmatrix<short> mapMountains, int row){

    int currentRow;
    int diffOne;
    int diffTwo;
    int diffThree;
    ALLEGRO_COLOR colour;
    int tieBreaker;
    int changeElevation = 0;

    colour = al_map_rgb(255, 0, 0);

    currentRow = row;

    for(int i = 1; i<844; i++){

        //Calculate the three changes in altitude
        //Corner cases: when at top row and bottom row, set difference to large number so it can never be chosen
        //and matrix index never goes out of range
        if(currentRow == 0){
            diffOne = 1000000;
        }else{
            diffOne = mapMountains[currentRow-1][i] - mapMountains[currentRow][i-1];
        }

        diffTwo = mapMountains[currentRow][i] - mapMountains[currentRow][i-1];

        if(currentRow == 479){
            diffThree = 1000000;
        }else{
            diffThree = mapMountains[currentRow+1][i] - mapMountains[currentRow][i-1];
        }

        //Compares the three changes in altitude, colours pixel at square with smallest difference
        //Changes current row accordingly and adds the difference in altitude to the total change in elevation
        if((abs(diffOne) < abs(diffTwo)) && (abs(diffOne) < abs(diffThree))){
            al_draw_filled_rectangle(i, currentRow-1, i+1, currentRow, colour);
            currentRow--;
            changeElevation += abs(diffOne);
        }else if((abs(diffTwo) < abs(diffOne)) && (abs(diffTwo) < abs(diffThree))){
            al_draw_filled_rectangle(i, currentRow, i+1, currentRow+1, colour);
            changeElevation += abs(diffTwo);
        }else if((abs(diffThree) < abs(diffOne)) && (abs(diffThree) < abs(diffTwo))){
            al_draw_filled_rectangle(i, currentRow+1, i+1, currentRow+2, colour);
            currentRow++;
            changeElevation += abs(diffThree);
        }else if(abs(diffOne) == abs(diffTwo)){
            //random number generated between 0 and 1 to decifer where to go when there is a tie
            tieBreaker = rand() % 2;
            al_draw_filled_rectangle(i, currentRow-tieBreaker, i+1, (currentRow+1)-tieBreaker, colour);
            currentRow -= tieBreaker;
            changeElevation += abs(diffOne);
        }else if(abs(diffTwo) == abs(diffThree)){
            tieBreaker = rand() % 2;
            al_draw_filled_rectangle(i, currentRow+tieBreaker, i+1, (currentRow+1)+tieBreaker, colour);
            currentRow += tieBreaker;
            changeElevation += abs(diffTwo);
        }
    }

    return changeElevation;

}

//draw path of least elevation at given row
void findLowestElevPath(apmatrix<short> mapMountains, int row){

    int currentRow;
    int elevOne;
    int elevTwo;
    int elevThree;
    ALLEGRO_COLOR colour;
    int tieBreaker;

    colour = al_map_rgb(0, 255, 0);

    currentRow = row;

    for(int i = 1; i<844; i++){

        //Set the three elevations
        //Corner cases: when at top row and bottom row, set difference to large number so it can never be chosen
        //and matrix index never goes out of range
        if(currentRow == 0){
            elevOne = 1000000;
        }else{
            elevOne = mapMountains[currentRow-1][i];
        }

        elevTwo = mapMountains[currentRow][i];

        if(currentRow == 479){
            elevThree = 1000000;
        }else{
            elevThree = mapMountains[currentRow+1][i];
        }

        //Compares the three changes in altitude, colours pixel at square with smallest elevation
        //Changes current row accordingly
        if((elevOne < elevTwo) && (elevOne < elevThree)){
            al_draw_filled_rectangle(i, currentRow-1, i+1, currentRow, colour);
            currentRow--;
        }else if((elevTwo < elevOne) && (elevTwo < elevThree)){
            al_draw_filled_rectangle(i, currentRow, i+1, currentRow+1, colour);
        }else if((elevThree < elevOne) && (elevThree < elevTwo)){
            al_draw_filled_rectangle(i, currentRow+1, i+1, currentRow+2, colour);
            currentRow++;
        }else if(elevOne == elevTwo){
            //random number generated between 0 and 1 to decifer where to go when there is a tie
            tieBreaker = rand() % 2;
            al_draw_filled_rectangle(i, currentRow-tieBreaker, i+1, (currentRow+1)-tieBreaker, colour);
            currentRow -= tieBreaker;
        }else if(elevTwo == elevThree){
            tieBreaker = rand() % 2;
            al_draw_filled_rectangle(i, currentRow+tieBreaker, i+1, (currentRow+1)+tieBreaker, colour);
            currentRow += tieBreaker;
        }
    }
}



