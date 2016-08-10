#include <stdint.h>
#include <stdbool.h>
#include "malloc.h"
#include "stdlib.h"
#include "syscalls.h"
#include "elf.h"
#include "tfont.h"

static void blitIcon(void *ptr, int px, int py)
{
	uint8_t *image = (uint8_t*)ptr;
	for(uint32_t y = 0; y < 64; y++) {
		for(uint32_t x = 0; x < 64; x++) {
			if(image[4 * (64 * y + x) + 3] < 0x80) {
				// Simple Alpha Testing
				continue;
			}
			color_t c = {
				image[4 * (64 * y + x) + 0],
				image[4 * (64 * y + x) + 1],
				image[4 * (64 * y + x) + 2]
			};
			video_setpixel(px + x, py + y, c);
		}
	}
}

void ls();

extern char const *font[128];

char const * getGlyph(int codepoint)
{
	if(codepoint >= 128) {
		return NULL;
	}
	return font[codepoint];
}

void setPixel(int x, int y, void *arg)
{
	video_setpixel(x, y, (color_t) { 255, 0, 0 });
}

void main()
{
	tfont_setFont(getGlyph);
	tfont_setPainter(setPixel, NULL);
	tfont_setSize(8);
	ls();
	while(true)
	{
		keyhit_t hit;
		getkey(&hit, true);
		
		if(hit.flags & khfKeyPress)
		{
			switch(hit.key.key)
			{
				case vkNumpad1: exec("C:/filedemo");
				case vkNumpad2: exec("C:/fontdemo");
				case vkNumpad3: exec("C:/game");
				default: break;
			}
		}
	}
}

int left = 0;

void check_entry(struct fsnode * node)
{
	if(node->type != ftFile) {
		return;
	}
	char buffer[64];
	strcpy(buffer, "C:/");
	strcat(buffer, node->name);
	puts(buffer);
	puts("\n");
	int fd = fs_open(buffer);
	if(fd == 0) {
		return;
	}
	
	struct elf_header header;
	file_read(fd, &header, 0, sizeof(struct elf_header));
	
	if(header.magic != ELF_MAGIC)
	{
		fs_close(fd);
		return;
	}
	
	uint32_t index = header.ph_offset;
	
	for(uint32_t i = 0; i < header.ph_entry_count; i++, index += sizeof(struct elf_pheader))
	{
		struct elf_pheader programHeader;
		file_read(fd, &programHeader, index, sizeof(struct elf_pheader));
		if(programHeader.type != 0x10) {
			continue;
		}
		
		static char icon[64*64*4];
		
		file_read(fd, icon, programHeader.offset, 64*64*4);
		
		blitIcon(
			icon, 
			32 + 96 * left, 
			32);
		
		tfont_render_string(
			32 + 96 * left,
			100 + tfont_getSize(),
			node->name,
			96,
			tfNone);
		
		left += 1;
		
		puts("found icon.\n");
		break;
	}
	
	
	fs_close(fd);
}

void ls()
{
	video_clear((color_t) { 64, 64, 64 });
	puts("Listing root directory:\n");
	
	int fd = fs_open("C:/");
	if(fd == 0) {
		puts("Failed to list root directory.\n");
		return;
	}
	if(fs_type(fd) != ftDirectory) {
		puts("C:/ is not a directory?!\n");
		return;
	}
	
	int len = dir_length(fd);
	for(int i = 0; i < len; i++)
	{
		struct fsnode node;
		if(dir_get(fd, i, &node))
		{
			puts(node.name);
			puts("\n");
			check_entry(&node);
		}
	}
	
	puts("done.\n");
	
	video_swap();
}