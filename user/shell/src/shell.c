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

extern char const *font[128];

char const * getGlyph(int codepoint)
{
	if(codepoint >= 128) {
		return NULL;
	}
	return font[codepoint];
}

color_t fontColor = { 255, 0, 255 };

void setPixel(int x, int y, void *arg)
{
	video_setpixel(x, y, fontColor);
}

struct entry
{
	char name[64];
	char path[64];
	char icon[64 * 64 * 4];
	struct entry * prev;
	struct entry * next;
};

struct entry *files = NULL;
struct entry *filesEnd = NULL;
struct entry *selection = NULL;

void gather();

void render();

void main()
{
	puts("Setup font rendering...\n");
	tfont_setFont(getGlyph);
	tfont_setPainter(setPixel, NULL);
	tfont_setSize(8);
	
	puts("Gather the files...\n");
	gather();
	
	puts("Enter main loop...\n");
	while(true)
	{
		puts("Mainloop.Render\n");
		render();
	
		keyhit_t hit;
		getkey(&hit, true);
		
		puts("Mainloop.EvaluateKeypress\n");
		if(hit.flags & khfKeyPress)
		{
			switch(hit.key.key)
			{
				case vkA:
					if(selection != NULL && selection->prev != NULL)
						selection = selection->prev;
					break;
				case vkD:
					if(selection != NULL && selection->next != NULL)
						selection = selection->next;
					break;
				case vkEnter:
					if(selection != NULL)
						exec(selection->path);
					break;
				default: break;
			}
		}
	}
}

void render()
{
	video_clear((color_t) { 64, 64, 64 });
	if(files == NULL) {
		puts("No files found.\n");
		return;
	}
	int i;
	struct entry *e;
	for(e = files, i = 0; e != NULL; e = e->next, i++)
	{
		blitIcon(
			e->icon, 
			32 + 96 * i, 
			32);
		int width = tfont_measure_string(e->name, 96, tfNone);
		
		if(e == selection) {
			fontColor = (color_t){ 255, 0, 0 };
		} else {
			fontColor = (color_t){ 255, 255, 255 };
		}
		
		tfont_render_string(
			32 + 96 * i + (64 - width) / 2,
			100 + tfont_getSize(),
			e->name,
			96,
			tfNone);
	}
	
	video_swap();
}

bool check_entry(struct fsnode * node)
{
	if(node->type != ftFile) {
		return false;
	}
	char buffer[64];
	strcpy(buffer, "C:/");
	strcat(buffer, node->name);
	int fd = fs_open(buffer);
	if(fd == 0) {
		return false;
	}
	
	struct elf_header header;
	file_read(fd, &header, 0, sizeof(struct elf_header));
	
	if(header.magic != ELF_MAGIC)
	{
		fs_close(fd);
		return false;
	}
	
	uint32_t pointer = header.ph_offset;
	for(uint32_t i = 0; i < header.ph_entry_count; i++, pointer += sizeof(struct elf_pheader))
	{
		struct elf_pheader programHeader;
		file_read(fd, &programHeader, pointer, sizeof(struct elf_pheader));
		if(programHeader.type != 0x10) {
			continue;
		}
		struct entry *entry = malloc(sizeof(struct entry));
		
		entry->prev = filesEnd;
		entry->next = NULL;
		strcpy(entry->name, node->name);
		strcpy(entry->path, buffer);
		
		file_read(fd, entry->icon, programHeader.offset, 64*64*4);
		
		if(files != NULL) {
			filesEnd->next = entry;
			filesEnd = entry;
		} else {
			files = entry;
			filesEnd = entry;
		}
		if(selection == NULL) {
			selection = entry;
		}
		
		fs_close(fd);
		return true;
	}
	
	fs_close(fd);
	return false;
}

void gather()
{
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
			puts("Scan node: ");
			puts(node.name);
			puts("\n");
			check_entry(&node);
		}
	}
	
	puts("done.\n");
}