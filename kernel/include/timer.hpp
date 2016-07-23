#pragma once

#include "syscalls.h"



void timer_init();

extern "C" uint32_t timer_get();

extern "C" void timer_reset();