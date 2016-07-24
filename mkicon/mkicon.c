#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "lodepng.h"

void chext(char *target, const char *path, const char *ext)
{
	int len = strlen(path);

	for(int i = len-1; i >= 0; i--) {
		char c = path[i];
		if(c == '.') {
			memcpy(target, path, i);
			target[i + 1] = 0;
			strcat(target, ext);
			return;
		}
		if(c == '/') {
			strcpy(target, path);
			return;
		}
	}
}

int main(int argc, char **argv)
{
	for(int i = 1; i < argc; i++) {
		unsigned error;
		unsigned char* image;
		unsigned width, height;

		error = lodepng_decode32_file(&image, &width, &height, argv[i]);
		if(error) {
			fprintf(stderr, "error %u: %s\n", error, lodepng_error_text(error));
			continue;
		}
		char buffer[PATH_MAX];
		chext(buffer, argv[1], ".bin");
		
		fprintf(stdout,
			"%s -> %s\n"
			"  w: %d\n"
			"  h: %d\n",
			argv[i], buffer,
			width, height);
		
		if(width != 64 || height != 64) {
			fprintf(stderr, "Input image must be 64*64 pixels large.\n");
		} else {
			FILE *f = fopen(buffer, "wb");
			fwrite(image, 4 * width, height, f);
			fclose(f);
		}
		free(image);
	}
	return 0;
}