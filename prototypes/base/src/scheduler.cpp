#include "driver/scheduler.hpp"
#include "bsod.hpp"
#include "pmm.hpp"

namespace driver
{
	Scheduler * Scheduler::current = nullptr;

	Scheduler::Scheduler()
	{
		
	}

	void Scheduler::install()
	{
		if(Scheduler::current != nullptr) {
			BSOD::die(Error::DriverAlreadyInstalled, "The scheduler is already installed.");
		}
		Scheduler::current = this;
		
		
	}

	void Scheduler::next(CpuState *cpu)
	{
	
	}
	
	void Scheduler::dispatch(CpuState *cpu)
	{
		if(Scheduler::current != nullptr) {
			Scheduler::current->next(cpu);
		} else {
			
		}
	}
	
	
	
	Task::Task() :
		stackBottom(PMM::alloc()), cpu(nullptr)
	{
		
	}
	
	Task::~Task()
	{
		PMM::free(this->stackBottom);
	}
}