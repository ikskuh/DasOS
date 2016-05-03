#pragma once

template<typename TIdent>
class pointer
{
private:
  void *ptr;
public:
  explicit pointer(void *ptr) : 
    ptr(ptr)
  {
    
  }
  
  explicit pointer(uint32_t value) : 
    ptr(reinterpret_cast<void*>(value))
  {
    
  }
  
  pointer(const pointer &) = default;
  pointer(pointer &&) = default;
  ~pointer() = default;
  
  void * data() const {
    return this->ptr;
  }
  
  template<typename T>
  T * data () const {
    return reinterpret_cast<T*>(this->ptr);
  }
  
  explicit operator void * () const {
    return this->ptr;
  }
};

struct physical_t_ident;
struct virtual_t_ident;

// Add different memory pointer types here....
using physical_t = pointer<physical_t_ident>;
using virtual_t = pointer<virtual_t_ident>;