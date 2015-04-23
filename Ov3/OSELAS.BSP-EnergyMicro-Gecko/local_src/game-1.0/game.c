#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>

// Define screen dimensions as constants
#define SCR_WIDTH 320
#define SCR_HEIGHT 240
#define SCR_BPP 2  //16 bits per pixel (in bytes)
struct fb_copyarea display;

typedef struct Color {
	Color(r,g,b) : R(r), G(g), B(b){}
	uint16_t R : 5;
	uint16_t G : 6;
	uint16_t B : 5;
}
uint16_t *framebuffer;

// Function to set the color for a pixel in the framebuffer
void draw_pixel(uint16_t x, uint16_t y, Color color) {
	if(x < SCR_WIDTH && x > 0 && y < SCR_HEIGTH && y > 0) { // validate pixel position
		framebuffer[ x + y*SCR_WIDTH ] = color;
	} else {
		printf("Invalid pixel position supplied: (%d,%d), max %d x %d", x, y, (SCR_WIDTH-1), (SCR_HEIGHT-1));
	}
}

// Function to output contents of framebuffer to display
void flush_framebuffer() {
	ioctl(fbfd, 0x4680, &display); //command driver to update display
}

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");
	display.dx = x;
	display.dy = y;
	display.width = SCR_WIDTH;
	display.height = SCR_HEIGHT;
	int fbfd= open("/dev/fb0",O_RDWR); // TODO: sjekk etter feil og vis feilmelding/avslutt
	
	screensize = display.dx * display.dy * SCR_BPP; 	
	framebuffer = (uint16_t*)mmap(0,screensize,PROT_READ |PROT_WRITE, MAP_SHARED, fbfd, 0);
	if(framebuffer == MAP_FAILED) { // Error, return
		close(fbfd);
		printf("Error: unable to map framebuffer, exiting...");
		exit(EXIT_FAILURE);
	}

	// TODO: draw something fancy for demonstration
	Color pix_color(0,0,0);
	for (int x = 0;x < SCR_WIDTH;x++){
		for (int y = 0;y < SCR_HEIGHT;y++) {
			draw_pixel(x,y, new Color(x % 32, (x+y) % 64, y % 32));
		}
	}

	munmap(mfb,screensize);
	close(fbfd);
	exit(EXIT_SUCCESS);

}
