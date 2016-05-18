#include "driver/scheduler.hpp"
#include "bsod.hpp"
#include "pmm.hpp"
#include "vmm.hpp"
#include "idt.hpp"
#include "console.hpp"
#include <new>

extern VMMContext * kernelContext;

namespace driver
{
	Scheduler * Scheduler::current = nullptr;

	Scheduler::Scheduler() : 
		currentTask(nullptr),
		firstTask(nullptr),
		lastTask(nullptr)
	{
		
	}

	void Scheduler::install()
	{
		if(Scheduler::current != nullptr) {
			BSOD::die(Error::DriverAlreadyInstalled, "The scheduler is already installed.");
		}
		Scheduler::current = this;
		
		IDT::interrupt(0x20) = Interrupt(Scheduler::dispatch);
	}

	CpuState * Scheduler::next(CpuState *cpu)
	{
		if (this->currentTask != nullptr) {
			this->currentTask->cpu = cpu;
		}

		this->currentTask = this->currentTask->next;

		/* Prozessorzustand des neuen Tasks aktivieren */
		return this->currentTask->cpu;
	}
	
	void Scheduler::dispatch(CpuState *& cpu)
	{
		if(Scheduler::current != nullptr) {
			cpu = Scheduler::current->next(cpu);
		}
	}
	
	static uint32_t memoryPointer = 0x20000000; // start at 512 MB
	
	static virtual_t alloc()
	{
		virtual_t ptr(memoryPointer);
		kernelContext->provide(ptr, VMMFlags::Writable);
		memoryPointer += 0x1000;
		Console::main << "Providing " << ptr << "\n";
		return ptr;
	}
	
	Task *Scheduler::spawn(EntryPoint ep)
	{
		void *memory = alloc().data();
		Task *task = new (memory) Task(ep);
		
		asm volatile("cli");
		if(this->firstTask != nullptr) {
			task->next = this->firstTask;
			this->lastTask->next = task;
			this->firstTask = task;
		} else {
			this->lastTask = task;
			this->firstTask = task;
			task->next = task;
		}
		asm volatile("sti");
		return task;
	}
	
	Task::Task(EntryPoint ep) :
		previous(nullptr), next(nullptr), 
		cpu(nullptr),
		stackBottom(alloc().data())
	{
		this->cpu = (CpuState*)(this->stackBottom.numeric() + 4096 - sizeof(CpuState));
		this->cpu->eax = 0;
		this->cpu->ebx = 0;
		this->cpu->ecx = 0;
		this->cpu->edx = 0;
		this->cpu->esi = 0;
		this->cpu->edi = 0;
		this->cpu->ebp = 0;
		//this->cpu->.esp = unbenutzt (kein Ring-Wechsel)
		this->cpu->eip = reinterpret_cast<uint32_t>(ep);

		/* Ring-0-Segmentregister */
		this->cpu->cs  = 0x08;
		//this->cpu->.ss  = unbenutzt (kein Ring-Wechsel)

		/* IRQs einschalten (IF = 1) */
		this->cpu->eflags = 0x202;
	}
	
	Task::~Task()
	{
		PMM::free(this->stackBottom);
	}
}