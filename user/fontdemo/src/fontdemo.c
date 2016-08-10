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

char screenText[4096] = { 0 };
int textCursor = 0;

void load_font(const char *path);

int x, y;

void putc(char c)
{
	switch(c)
	{
		case 8:
			if(textCursor > 0) {
				screenText[--textCursor] = 0;
			}
			break;
		default:
			screenText[textCursor++] = c;
			screenText[textCursor] = 0;
			break;
	}
}

char const *getGlyph(int cp)
{
	if(cp > 127) {
		return NULL;
	}
	return font[cp].code;
}

void main()
{
	x = 8;
	y = 8 + tfont_getSize();
	
	video_getmode(&videomode);

	color_t background = {32, 32, 32};
	
	load_font("C:/test.tfn");
	
	tfont_setPainter(&tput, NULL);
	tfont_setFont(&getGlyph);
	tfont_setSize(16);
	tfont_setDotSize(2);
	tfont_setStroke(2);
	
	while(true)
	{
		char c = getchar();
		if(c == 0x1B) {
			return;
		}
		putc(c);
		
		video_clear(background);
		tfont_render_string(
			8, 8 + tfont_getSize(),
			screenText,
			videomode.width - 16,
			tfNone);
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