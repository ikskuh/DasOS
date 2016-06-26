#pragma once

#include "enums.hpp"

enum class Exception
{
#define EXCEPTION(num, shorthand, ident, desc, type) ident = num,
#include "lists/exceptions.lst"
#undef EXCEPTION
};