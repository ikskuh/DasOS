#pragma once

#include <inttypes.h>
#include "driver.hpp"

namespace driver
{
	class Timer : 
		public Driver
	{
	public:
		Timer();
		
		void install() override;
		
		void reset();
		
		uint32_t count();
	};
}