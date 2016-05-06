#pragma once

#include "base.hpp"
#include "cpustate.hpp"
#include "pointer.hpp"

namespace driver
{
	class Task
	{
	private:
		physical_t stackBottom;
		CpuState *cpu;
	public:
		Task();
		Task(const Task &) = delete;
		Task(Task &&) = delete;
		~Task();
	};

	class Scheduler : 
		public Driver
	{
	private:
		static Scheduler *current;
		static void dispatch(CpuState *cpu);
		
		void next(CpuState *cpu);
	public:
		Scheduler();
		
		void install() override;
	};
}