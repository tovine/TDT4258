#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h> // Needed for RNG

// Define screen dimensions as constants
#define SCR_WIDTH 320
#define SCR_HEIGHT 240
#define SCR_BPP 2  //16 bits per pixel (in bytes)

// Define constants for game
#define MAX_LENGTH 20 // Maximum length of snake before game is over
#define BLOCK_PIXELS 10 // Number of pixels (in each direction) of one 'block' rendered on screen

#define BLOCKS_X (SCR_WIDTH / BLOCK_PIXELS)
#define BLOCKS_Y (SCR_HEIGHT / BLOCK_PIXELS)

struct fb_copyarea display;
int fbfd = 0, gpfd = 0; // File descriptor ids for framebuffer and gamepad

typedef union {
	struct {
		uint16_t B : 5;
		uint16_t G : 6;
		uint16_t R : 5;
	};
	uint16_t toint;
} Color;
// "Constructor"
Color color(uint8_t r, uint8_t g, uint8_t b) {
	Color col;
	if(r >= 0 && r < 32 && g >= 0 && g < 64 && b >= 0 && b < 32){
		col.R = r;
		col.G = g;
		col.B = b;
	} else {
		col.R = 0;
		col.G = 0;
		col.B = 0;
		printf("Invalid color value supplied: (R: %d, G: %d, B: %d), max (31, 63, 31)", r, g, b);
	}
	return col;
}

// Define data types that are used in the game
typedef union {
	struct {
		uint8_t x;
		uint8_t y;
	};
	uint16_t coords;
} Coord;
Coord coord(uint8_t x, uint8_t y) {
	Coord crd;
	if(x >= 0 && x < BLOCKS_X && y >= 0 && y < BLOCKS_Y) {
		crd.x = x;
		crd.y = y;
	} else {
		crd.x = 0;
		crd.y = 0;
		printf("Invalid coordinate value supplied: (x: %d, y: %d). Valid ranges are x:[0 - %d], y:[0 - %d])", x, y, BLOCKS_X, BLOCKS_Y);
	}
	return crd;
}

enum direction {
	NONE	= 0,
	UP	= 1,
	DOWN	= -1,
	RIGHT	= 2,
	LEFT	= -2
};

typedef struct {
	uint8_t length, id;
	Coord pos[MAX_LENGTH];
	Color color;
	enum direction dir;
} Snake;

typedef struct {
	Coord pos;
	Color color;
} Fruit;

Snake snake1, snake2;
Fruit dot;
// End of defined game data types

Color col_bg;
uint16_t *framebuffer;

// Function to set the color for a pixel in the framebuffer
void draw_pixel(uint16_t x, uint16_t y, Color color) {
	if(x < SCR_WIDTH && x >= 0 && y < SCR_HEIGHT && y >= 0) { // validate pixel position
		framebuffer[ x + y*SCR_WIDTH ] = color.toint;
	} else {
		printf("Invalid pixel position supplied: (%d,%d), max %d x %d", x, y, (SCR_WIDTH-1), (SCR_HEIGHT-1));
	}
}

// Function to output contents of framebuffer to display
void flush_framebuffer() {
	ioctl(fbfd, 0x4680, &display); //command driver to update display
}

// Function used to draw a 'block' (size defined by constant 'BLOCK_PIXELS')
void draw_block(uint8_t x, uint8_t y, Color color) {
	uint8_t i, j;
	for(i = 0;i < BLOCK_PIXELS;i++) {
		for(j = 0;j < BLOCK_PIXELS;j++) {
			draw_pixel(x*BLOCK_PIXELS + i, y*BLOCK_PIXELS + j, color);
		}
	}
}

// Helper function to check if a position is occupied by another object
int pos_ok(Coord point) {
	uint8_t i = 0;
	if(point.x < 0 || point.x >= BLOCKS_X || point.y < 0 || point.y >= BLOCKS_Y) return 0; // posistion is out of bounds
	// Iterate through all object positions and check for collisions
	for(i = 0;i < snake1.length;i++) {
		if(snake1.pos[i].coords == point.coords) return 0;
	}
	for(i = 0;i < snake2.length;i++) {
		if(snake2.pos[i].coords == point.coords) return 0;
	}
	return 1;
}

void gen_random_fruit() {
	srand(time(NULL));
	// Generate a random position, check for collisions (with snake elements) and return it if OK
	dot.pos.x = random() % BLOCKS_X;
	dot.pos.y = random() % BLOCKS_Y;
	if(!pos_ok(dot.pos)) {
		printf("Invalid/busy position (%d, %d), trying again...\n", dot.pos.x, dot.pos.y);
		gen_random_fruit();
	} else {
		// Draw the fruit on the display
		draw_block(dot.pos.x, dot.pos.y, dot.color);
//		flush_framebuffer();
	}
}

// Cleanup function that can be used to exit from anywhere in the code
void exit_clean(void) {
	close(fbfd);
	close(gpfd);
	exit(EXIT_SUCCESS);
}

void snake_move(Snake *snake, enum direction dir) {
	Coord tmp_pos; // Temporary placeholder for the snake head
	uint8_t i;
	if(dir == -snake->dir || dir == NONE) return;
	snake->dir = dir;
	tmp_pos = snake->pos[0];
	switch(dir) { // Move the snake head according to direction
		// (0,0) is top left corner
		case UP:
			tmp_pos.y--;
			break;
		case DOWN:
			tmp_pos.y++;
			break;
		case LEFT:
			tmp_pos.x--;
			break;
		case RIGHT:
			tmp_pos.x++;
			break;
		default:
			return;
	}
	if(!pos_ok(tmp_pos)) {
		// Snake either bit itself or hit the edges - you lost...
		printf("Ouch - you lost, player %d...\n", snake->id);
		memset(framebuffer, 0, SCR_WIDTH * SCR_HEIGHT * SCR_BPP); // Fill screen with black
		for(i = 0;i < snake->length;i++) {
			draw_block(snake->pos[i].x, snake->pos[i].y, snake->color);
		}
		flush_framebuffer();
		exit_clean();
	}

	// Iterate over all the snake elements and move them
	for (i = snake->length;i > 0;i--) {
		snake->pos[i] = snake->pos[i-1];
	}
	snake->pos[0] = tmp_pos;

	if(tmp_pos.coords == dot.pos.coords) {
		// Snake hit a fruit - increment length and update position
		snake->length++;
		if(snake->length >= MAX_LENGTH) {
			// You win!
			printf("Max length reached - Player %d wins!\n", snake->id);
			memset(framebuffer, 0xFFFF, SCR_WIDTH * SCR_HEIGHT * SCR_BPP); // Fill screen with white
			for (i = 0;i < snake->length;i++){
				draw_block(snake->pos[i].x, snake->pos[i].y, snake->color);
			}
			flush_framebuffer();
			exit_clean();
		}
		// Place a new fruit
		gen_random_fruit();
	} else {
		// Draw the background color to the square behind the snake
		draw_block(snake->pos[snake->length].x, snake->pos[snake->length].y, col_bg);
	}
	draw_block(tmp_pos.x, tmp_pos.y, snake->color);
//	flush_framebuffer(); Defer framebuffer update until both snakes have been updated
}

void game_start() {
	// Init snake 1
	snake1.id = 1;
	snake1.length = 1;
//	snake1.lives = 3; - not used yet
	snake1.dir = NONE;
	snake1.pos[0].x = BLOCKS_X / 3;
	snake1.pos[0].y = BLOCKS_Y / 3;
	snake1.color = color(31,0,0);
	// Init snake 2
	snake2.id = 2;
	snake2.length = 1;
//	snake2.lives = 3; - not used yet
	snake2.dir = NONE;
	snake2.pos[0].x = BLOCKS_X*2 / 3;
	snake2.pos[0].y = BLOCKS_Y*2 / 3;
	snake2.color = color(0,0,31);
	
	dot.color = color(0,63,0);
	int x = 0, y = 0;
	for (x = 0;x < SCR_WIDTH;x++){
		for (y = 0;y < SCR_HEIGHT;y++) {
			draw_pixel(x,y, color(x % 32, (x+y) % 64, y % 32));
		}
	}
	for(x = -1;x <= 1; x++) {
		for(y = -1;y <= 1; y++) {
			draw_block(snake1.pos[0].x + x, snake1.pos[0].y + y, col_bg);
			draw_block(snake2.pos[0].x + x, snake2.pos[0].y + y, col_bg);
		}
	}
	draw_block(snake1.pos[0].x, snake1.pos[0].y, snake1.color);
	draw_block(snake2.pos[0].x, snake2.pos[0].y, snake2.color);

	gen_random_fruit();
	flush_framebuffer();
}

// Interrupt signal handler to respond to keypresses
void keypress_handler(int signal) {
//	printf("Keypress detected, reading input file...\n"); - debug spam
	char input_buff, filtered_1, filtered_2;
	if(read(gpfd, &input_buff, 1) != -1) { // Read exactly one byte
		// Read was successful
//		printf("Successfully read the input, data: %d\n", input_buff); - debug spam
		// Filter input to let two players press buttons at once
		filtered_1 = input_buff & 0x0F;
		filtered_2 = input_buff & 0xF0;
		switch(filtered_1) {
			case 1:
				snake_move(&snake1, LEFT);
				break;
			case 2:
				snake_move(&snake1, UP);
				break;
			case 4:
				snake_move(&snake1, RIGHT);
				break;
			case 8:
				snake_move(&snake1, DOWN);
				break;
			default:
				snake_move(&snake1, NONE);
		}
		switch(filtered_2) {
			case 16:
				snake_move(&snake2, LEFT);
				break;
			case 32:
				snake_move(&snake2, UP);
				break;
			case 64:
				snake_move(&snake2, RIGHT);
				break;
			case 128:
				snake_move(&snake2, DOWN);
				break;
			default:
				snake_move(&snake2, NONE);
		}
		// Flush framebuffer after all updates have been made, and only if something actually was updated
		if(input_buff) flush_framebuffer();
		// Press three rightmost buttons to terminate game
		if(input_buff > 223) exit_clean();
	} else{
		printf("Failed to read input, contents of input_buff: %d\n", input_buff);
	}
}

void exit_error() {
	close(fbfd);
	close(gpfd);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	// Init display driver
	display.dx = 0;
	display.dy = 0;
	display.width = SCR_WIDTH;
	display.height = SCR_HEIGHT;
	fbfd = open("/dev/fb0", O_RDWR);
	if(fbfd == -1) {
		// Failed to open file descriptor
		printf("Error: unable to open framebuffer, exiting...\n)");
		exit(EXIT_FAILURE);
	}
	int screensize = SCR_WIDTH * SCR_HEIGHT * SCR_BPP; 	
	framebuffer = (uint16_t*) mmap(NULL,screensize,PROT_READ |PROT_WRITE, MAP_SHARED, fbfd, 0);
	if(framebuffer == MAP_FAILED) { // Error, return
		printf("Error: unable to map framebuffer, exiting...\n");
		close(fbfd);
		exit(EXIT_FAILURE);
	}

	// Open gamepad device driver
	gpfd = open("/dev/gamepad", O_RDWR);
	if(gpfd == -1) {
		// Failed to open file descriptor
		printf("Error: unable to open gamepad device, exiting...\n)");
		close(fbfd);
		exit(EXIT_FAILURE);
	}
	if(signal(SIGIO, &keypress_handler) == SIG_ERR) {
		printf("Error: failed to create signal handler\n");
		exit_error();
	}
	// Set correct ownership
	if(fcntl(gpfd, F_SETOWN, getpid()) == -1) {
		printf("Error: failed to set owner of gamepad device\n");
		exit_error();
	}
	// Set ASYNC flag to be able to receive notifications
	long flags = fcntl(gpfd, F_GETFL);
	if(fcntl(gpfd, F_SETFL, flags | FASYNC) == -1) {
		printf("Error: failed to set FASYNC flag\n");
		exit_error();
	}

	col_bg = color(0,0,0);

	printf("Hello, welcome to 2-player Snake Deathmatch!\nNormally you just control the snake around to pick up food, but this time it gets serious...\nPlayer 1 uses the 4 left buttons, and player 2 has the ones to the right.\nThere is no auto-move here, so you have to mash those buttons if you want to get somewhere.\nFirst player to eat %d dots wins, crash and you will die - good luck!\n", MAX_LENGTH);

	game_start();
	while(1) {
		// Loop and react to input
		pause();
	}

	munmap(framebuffer,screensize); // Not really necessary, as it is automatically unmapped when the process exits

	exit_clean();

	return 0; // Just to make the compiler happy, as main() must return int - even though it will never reach this far...
}
