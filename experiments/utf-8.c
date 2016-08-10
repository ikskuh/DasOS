#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>




int main(int argc, char** argv)
{
	int state = 0;
	int codepoint = 0;
	while(true)
	{
		int c = getc(stdin);
		
		if(c == EOF)
			break;
		
		if(c & 0x80) {
			// utf8 char
			if(c & 0xC0 == 0x80) {
				// folgecharacter, ignore, but: WHY IS IT HEEEEEERE?
				continue;
			}
			char c0 = c;
			if((c & 0xE0) == 0xC0) {
				// two byte
				printf("Two byte.\n");
				char c1 = getc(stdin);
				if((c1 & 0xC0) != 0x80) {
					printf("2B:Weird unicode\n");
					continue;
				}
				codepoint = (c1 & 0x3F) | ((c0 & 0x1F) << 6);
			}
			else if((c & 0xF0) == 0xE0) {
				// three byte
				printf("Three byte.\n");
				char c1 = getc(stdin);
				char c2 = getc(stdin);
				if((c1 & 0xC0) != 0x80) {
					printf("3B:Weird unicode\n");
					continue;
				}
				if((c2 & 0xC0) != 0x80) {
					printf("3B:Weird unicode\n");
					continue;
				}
				codepoint = (c2 & 0x3F) | (((c1 & 0x3F) | ((c0 & 0x0F) << 6)) << 6);
			}
			else if((c & 0xF8) == 0xF0) {
				// four byte
				printf("Four byte.\n");
				char c1 = getc(stdin);
				char c2 = getc(stdin);
				char c3 = getc(stdin);
				if((c1 & 0xC0) != 0x80) {
					printf("4B:Weird unicode\n");
					continue;
				}
				if((c2 & 0xC0) != 0x80) {
					printf("4B:Weird unicode\n");
					continue;
				}
				if((c3 & 0xC0) != 0x80) {
					printf("4B:Weird unicode\n");
					continue;
				}
				codepoint = (c3 & 0x3F) | (((c2 & 0x3F) | (((c1 & 0x3F) | ((c0 & 0x07) << 6)) << 6)) << 6);
			}
			else {
				printf("Weird unicode.\n");
			}
		} else {
			codepoint = c;
		}
		if(codepoint != '\n')
				printf("u:%c -> %d, 0x%X\n", codepoint, codepoint, codepoint);
	}
	return 0;
}

