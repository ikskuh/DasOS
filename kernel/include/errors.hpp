#pragma once

enum class Error
{
#define ERROR(num, ident, desc) ident = num,
#include "lists/errors.lst"
#undef ERROR
};