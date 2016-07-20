#include "console.hpp"
#include "asm.hpp"
#include "idt.hpp"


void timer_intr(CpuState * & cpu)
{
	Console::main << "tick!\n";
}

#include "enums.hpp"

/**
 * @brief Defines different flags for key hits.
 */
enum class KeyHitFlags
{
	None = 0,
	
	/**
	 * The key was pressed.
	 */
	KeyPress = (1 << 0),
	
	/**
	 * @brief The key was released
	 */
	KeyRelease = (1 << 1),
	
	/**
	 * @brief A character was typed with this event.
	 */
	CharInput = (1 << 2),
};
ENUM_CLASS_OPERATORS(KeyHitFlags)

/**
 * @brief A basic keyboard event
 */
struct KeyHit
{
	/**
	 * @brief The scan code of the key that produces this key hit.
	 */
	uint32_t scanCode;
	
	/**
	 * @brief The code point (character) value that is associated with
	 *        the pressed key.
	 */
	uint32_t codepoint;
	
	/**
	 * @brief Flags that define the specific type and behaviour of this key hit.
	 */
	KeyHitFlags flags;
};

void kbd_intr(CpuState * & cpu)
{
	Console::main << "tick!\n";
}

void dasos_demo()
{
	IDT::interrupt(0x20) = Interrupt(timer_intr);
	IDT::interrupt(0x21) = Interrupt(kbd_intr);


	ASM::sti();



	while(1);
}