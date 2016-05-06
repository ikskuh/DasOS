#pragma once

#include <inttypes.h>

/**
 * Provides a strong pointer wrapper which can be used to address
 * different memory types (physical, virtual, ...) and preventing
 * a wrong assignment.
 * @remarks The pointer size is fixed to 32 bits.
 */
template<typename TIdent>
class pointer
{
public:
  /**
   * A value that declares the pointer invalid.
   */
  static pointer invalid;
private:
  uint32_t ptr;
public:
  /**
   * Creates the pointer by giving a raw pointer.
   */
  explicit pointer(void *ptr) : 
    ptr(reinterpret_cast<uint32_t>(ptr))
  {
    
  }
  
  /**
   * Creates the pointer by giving an integer value.
   */
  explicit pointer(uint32_t value) : 
    ptr(value)
  {
    
  }
  
  pointer(const pointer &) = default;
  pointer(pointer &&) = default;
  ~pointer() = default;
  
  pointer & operator = (const pointer & other) {
    this->ptr = other.ptr;
    return *this;
  }
  
  /**
   * Returns the numeric integer value of the pointer.
   */
  uint32_t numeric() const {
    return this->ptr;
  }
  
  /**
   * Returns the pointer as a raw pointer.
   */
  void * data() const {
    return reinterpret_cast<void*>(this->ptr);
  }
  
  /**
   * Returns the pointer as a raw typed pointer.
   */
  template<typename T>
  T * data () const {
    return reinterpret_cast<T*>(this->ptr);
  }
  
  /**
   * Allow explicit conversion to a raw pointer.
   */
  explicit operator void * () const {
    return this->data();
  }
  
  /**
   * Returns an aligned version of the pointer.
   * Rounds the pointer to the memory bottom.
   */
  pointer alignLower(uint32_t alignment) {
    if(alignment == 0) return pointer::invalid;
    return pointer(this->ptr & ~(alignment - 1));
  }
  
  /**
   * Returns an aligned version of the pointer.
   * Rounds the pointer to the memory top.
   */
  pointer alignUpper(uint32_t alignment) {
    if(alignment == 0) return pointer::invalid;
    return pointer((this->ptr + (alignment - 1)) & ~(alignment - 1));
  }
} __attribute__((packed));

template <class T>
pointer<T> pointer<T>::invalid(uint32_t(0xFFFFFFFF));

struct physical_t_ident;
struct virtual_t_ident;

// Add different memory pointer types here....

/**
 * A pointer pointing to physical memory.
 */
using physical_t = pointer<physical_t_ident>;

/**
 * A pointer pointing to virtual memory.
 */
using virtual_t = pointer<virtual_t_ident>;


static_assert(sizeof(physical_t) == 4, "pointer is not 4 byte wide.");