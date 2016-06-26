#include <stdint.h>

#include <multiboot.hpp>

#include <driver/video.hpp>
#include <driver/serial.hpp>

extern "C" void init(multiboot::Structure const & data)
{
	SerialPort serial(SERIAL_COM1);

	serial.write('H');
	serial.write('i');
	serial.write('\n');
	
	VideoScreen video(*data.vbe.modeInfo);
	for(uint32_t y = 0; y < video.height(); y++)
	{
		for(uint32_t x = 0; x < video.width(); x++)
		{
			video(x,y) = COLOR(255, 0, 0);
		}
	}
	
	while(true);
}
