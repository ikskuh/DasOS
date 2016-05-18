#pragma once

#include "driver.hpp"
#include "cpustate.hpp"
#include "pointer.hpp"

namespace driver
{
	using EntryPoint = void (*)();

	class Task
	{
		friend class Scheduler;
	private:
		Task *previous, *next;
		CpuState *cpu;
		physical_t stackBottom;
		
		Task(EntryPoint ep);
		Task(const Task &) = delete;
		Task(Task &&) = delete;
		~Task();
	public:
	};

	class Scheduler : 
		public Driver
	{
	private:
		static Scheduler *current;
		static void dispatch(CpuState *& cpu);
		
		Task *currentTask;
		Task *firstTask;
		Task *lastTask;
		CpuState * next(CpuState *cpu);
	public:
		Scheduler();
		
		void install() override;
		
		Task *spawn(EntryPoint ep);
	};
}