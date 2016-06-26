#include "compat.h"

typedef void (*constructor)();
 
extern constructor start_ctors;
extern constructor end_ctors;

void compat_call_ctors()
{
  for (constructor* i = &start_ctors;i != &end_ctors;++i)
    (*i)();
}

void compat_call_dtors()
{

}