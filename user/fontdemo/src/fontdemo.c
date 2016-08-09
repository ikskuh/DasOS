#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "syscalls.h"
#include "tfont.h"

void tput(int x, int y, void *arg)
{
	video_setpixel(x, y, (color_t){ 255, 0, 0 });
}

struct glyph
{
	char code[128];
};

struct glyph font[128];

videomode_t videomode;

void load_font(const char *path);

int x, y;

void putc(char c)
{
	if(c == '\n') {
		x = 8;
		y += tfont_getLineHeight();
	} else {
		if(x + tfont_width(font[(int)c].code) >= videomode.width) {
			x = 8;
			y += tfont_getLineHeight();
		}	
		x += tfont_render(x, y, font[(int)c].code);
	}
}

void main()
{
	x = 8;
	y = 8 + tfont_getSize();
	
	video_getmode(&videomode);

	color_t background = {32, 32, 32};
	video_clear(background);
	
	load_font("C:/test.tfn");
	
	tfont_setPainter(&tput, NULL);
	tfont_setSize(16);
	tfont_setDotSize(2);
	
	while(true)
	{
		char c = getchar();
		
		putc(c);
		
		video_swap(); // Required for back buffering
	}
}

void load_font_next(char c)
{
	static int state = 0;
	static char current;
	static int cursor = 0;
	switch(state)
	{
		case 0:
			current = c;
			state = 1;
			break;
		case 1:
			if(c != ':')
				puts("warning: invalid font file.");
			state = 2;
			cursor = 0;
			break;
		case 2:
			if(c == '\n') {
				state = 0;
			} else {
				font[(int)current].code[cursor++] = c;
				font[(int)current].code[cursor] = '\0';
			}
			break;
	}
};

void load_font(const char *path)
{
	int fd = fs_open(path);
	if(fd == 0)
		return;
	
	char buffer[256];
	int len = file_size(fd);
	int cursor = 0;
	while(cursor < len)
	{
		int l = file_read(fd, buffer, cursor, 256);
		
		for(int i = 0; i < l; i++)
		{
			load_font_next(buffer[i]);
		}
		
		cursor += l;
	}
	
	fs_close(fd);
}

void die(const char *msg)
{
	puts("User Panic: ");
	puts(msg);
	puts("\n");
	while(1);
}

void die_extra(const char *msg, const char *extra)
{
	puts("User Panic: ");
	puts(msg);
	puts(": ");
	puts(extra);
	puts("\n");
	while(1);
}