#include <stdint.h>

#include <multiboot.hpp>
#include <io.hpp>

#include <driver/video.hpp>

// Prüft, ob man bereits schreiben kann
static uint8_t is_transmit_empty(uint16_t base) {
	return inb(base+5) & 0x20;
}
 
 // Byte senden
static void write_com(uint16_t base, uint8_t chr) {
	while (is_transmit_empty(base)==0);
	outb(base,chr);
}

extern "C" void init(multiboot::Structure const & data)
{
	write_com(0x3F8, 'H');
	write_com(0x3F8, 'i');
	write_com(0x3F8, '\n');
	
	VideoScreen video(*data.vbe.modeInfo);
	for(uint32_t y = 0; y < video.height(); y++)
	{
		for(uint32_t x = 0; x < video.width(); x++)
		{
			video(x,y) = COLOR(255, 0, 0);
		}
	}
	
	write_com(0x3F8, 'B');
	write_com(0x3F8, 'y');
	write_com(0x3F8, 'e');
	write_com(0x3F8, '\n');
	
	while(true);
}
