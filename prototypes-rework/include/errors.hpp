#pragma once

enum class Error
{
#define ERROR(num, ident, desc) ident = num,
#include "errors.lst"
#undef ERROR
};