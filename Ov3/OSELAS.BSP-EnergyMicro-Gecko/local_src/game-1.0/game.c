#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

// Define screen dimensions as constants
#define SCR_WIDTH 320
#define SCR_HEIGHT 240
#define SCR_BPP 2  //16 bits per pixel (in bytes)
struct fb_copyarea display;
int fbfd = 0, gpfd = 0; // File descriptor ids for framebuffer and gamepad
int quit = 0;

typedef union {
	struct {
		uint16_t R : 5;
		uint16_t G : 6;
		uint16_t B : 5;
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

// Cleanup function that can be used to exit from anywhere in the code
void exit_clean(void) {
	close(fbfd);
	close(gpfd);
	exit(EXIT_SUCCESS);
}

// Interrupt signal handler to respond to keypresses
void keypress_handler(int signal) {
	printf("Keypress detected, reading input file...\n");
	char input_buff;
	if(read(gpfd, &input_buff, 1) != -1) { // Read exactly one byte
		// Read was successful
		printf("Successfully read the input, data: %d\n", input_buff);
		//decode_input(input_buf); // TODO:write function
		if(input_buff > 127) exit_clean();
	} else{
		printf("Failed to read input, contents of input_buff: %d\n", input_buff);
	}
}

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");

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
		// TODO: cleanup and exit
		close(fbfd);
		close(gpfd);
		exit(EXIT_FAILURE);
	}
	// Set correct ownership
	if(fcntl(gpfd, F_SETOWN, getpid()) == -1) {
		printf("Error: failed to set owner of gamepad device\n");
		// TODO: cleanup and exit
		close(fbfd);
		close(gpfd);
		exit(EXIT_FAILURE);
	}
	// Set ASYNC flag to be able to receive notifications
	long flags = fcntl(gpfd, F_GETFL);
	if(fcntl(gpfd, F_SETFL, flags | FASYNC) == -1) {
		printf("Error: failed to set FASYNC flag\n");
		// TODO: cleanup and exit
		close(fbfd);
		close(gpfd);
		exit(EXIT_FAILURE);
	}

	int x = 0, y = 0;
	for (x = 0;x < SCR_WIDTH;x++){
		for (y = 0;y < SCR_HEIGHT;y++) {
			draw_pixel(x,y, color(x % 32, (x+y) % 64, y % 32));
		}
	}
	flush_framebuffer();

	while(quit != 1) {
		// Loop and react to input
	}

	munmap(framebuffer,screensize); // Not really necessary, as it is automatically unmapped when the process exits

	exit_clean();

	return 0; // Just to make the compiler happy, as main() must return int - even though it will never reach this far...
}
