#pragma once

#include "base.hpp"
#include "cpustate.hpp"

namespace driver
{
	class Keyboard : 
		public Driver
	{
	private:
		static void dispatchIRQ(CpuState *cpu);
	public:
		Keyboard();
	
		void install() override;
	};
}