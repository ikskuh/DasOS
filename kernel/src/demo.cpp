#include "console.hpp"
#include "asm.hpp"
#include "idt.hpp"
#include "io.hpp"


void timer_intr(CpuState * & cpu)
{
	// Console::main << "tick!\n";
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

enum class VKey
{
	Invalid,
	Escape,
	D0,	D1,	D2,	D3,	D4,	D5,	D6,	D7,	D8,	D9,	
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	
	// "\\",
	// "`",
	Backspace,
	Tab,
	// "ü",
	Plus,
	Enter,
	ControlLeft,
	// "ö",
	// "ä",
	// "^",
	ShiftLeft,
	Number, // #,
	Comma,
	Dot,
	Hyphen,
	ShiftRight,
	AltLeft,
	Space,
	CapsLock,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	ScrollLock,
	NumpadLock,
	NumpadPlus,
	NumpadMinus,
	NumpadMult,
	NumpadDot,
	Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
	
	LIMIT,
};

#define KBD_DEBUG


/**
 * @brief Key definition on a key map.
 */
struct Key
{
#if defined(KBD_DEBUG)
	/**
	 * @brief Name of the key, can be used for debugging
	 */
	const char *name;
#endif
	/**
	 * @brief Virtual key code of this scancode.
	 */
	VKey key;
	
	/**
	 * Lower case representation.
	 */
	char lower;
	
	
	/**
	 * Upper case representation.
	 */
	char upper;
	 
	 
	/**
	 * Variant (alt+graph) representation.
	 */
	char variant;
};

#if defined(KBD_DEBUG)
#define DEFKEY(name, vkey, lower, upper, variant) { name, VKey::vkey, lower, upper, variant }
#else
#define DEFKEY(name, vkey, lower, upper, variant) { VKey::vkey, lower, upper, variant }
#endif

const Key keymap_set0[] = {
	DEFKEY( "NUL/INVALID", Invalid, 0, 0, 0 ),
	DEFKEY( "ESCAPE", Escape, 0, 0, 0 ),
	DEFKEY( "1", D1, '1', '!', 0 ),
	DEFKEY( "2", D2, '2', '\"', 0 ),
	DEFKEY( "3", D3, '3', 0, 0 ),
	DEFKEY( "4", D4, '4', '$', 0 ),
	DEFKEY( "5", D5, '5', '%', 0 ),
	DEFKEY( "6", D6, '6', '&', 0 ),
	DEFKEY( "7", D7, '7', '/', '{' ),
	DEFKEY( "8", D8, '8', '(', '[' ),
	DEFKEY( "9", D9, '9', ')', ']' ),
	DEFKEY( "0", D0, '0', '=', '}' ),
	DEFKEY( "\\", Invalid, 0, '?', '\\' ), // TODO: Add virtual key code
	DEFKEY( "`", Invalid, '\'', '`', 0 ), // TODO: Add virtual key code
	DEFKEY( "BACKSPACE", Backspace, 8, 8, 8 ),
	DEFKEY( "TAB", Tab, 9, 9, 9 ),
	DEFKEY( "q", Q, 'q', 'Q', '@' ),
	DEFKEY( "w", W, 'w', 'W', 0 ),
	DEFKEY( "e", E, 'e', 'E', 0 ),
	DEFKEY( "r", R, 'r', 'R', 0 ),
	DEFKEY( "t", T, 't', 'T', 0 ),
	DEFKEY( "z", Z, 'z', 'Z', 0 ),
	DEFKEY( "u", U, 'u', 'U', 0 ),
	DEFKEY( "i", I, 'i', 'I', 0 ),
	DEFKEY( "o", O, 'o', 'O', 0 ),
	DEFKEY( "p", P, 'p', 'P', 0 ),
	DEFKEY( "ü", Invalid, 'q', 'Q', 0 ), // TODO: Add virtual key code
	DEFKEY( "+", Plus, 'q', 'Q', 0 ),
	DEFKEY( "Enter", Enter, '\n', '\n', '\r' ),
	DEFKEY( "CTRL-LEFT", ControlLeft, 0, 0, 0 ),
		
	/* 30 */ DEFKEY( "a", A, 'a', 'A', 0 ),
	/* 31 */ DEFKEY( "s", S, 's', 'S', 0 ),
	/* 32 */ DEFKEY( "d", D, 'd', 'D', 0 ),
	/* 33 */ DEFKEY( "f", F, 'f', 'F', 0 ),
	/* 34 */ DEFKEY( "g", G, 'g', 'G', 0 ),
	/* 35 */ DEFKEY( "h", H, 'h', 'H', 0 ),
	/* 36 */ DEFKEY( "j", J, 'j', 'J', 0 ),
	/* 37 */ DEFKEY( "k", K, 'k', 'K', 0 ),
	/* 38 */ DEFKEY( "l", L, 'l', 'L', 0 ),
	/* 39 */ DEFKEY( "ö", Invalid, 0, 0, 0 ),
	/* 40 */ DEFKEY( "ä", Invalid, 0, 0, 0 ),
	/* 41 */ DEFKEY( "^", Invalid, '^', 0, 0 ),
	/* 42 */ DEFKEY( "SHIFT-LEFT", ShiftLeft, 0, 0, 0 ),
	/* 43 */ DEFKEY( "#", Number, '#', '\'', 0 ),
	/* 44 */ DEFKEY( "y", Y, 'y', 'Y', 0 ),
	/* 45 */ DEFKEY( "x", X, 'x', 'X', 0 ),
	/* 46 */ DEFKEY( "c", C, 'c', 'C', 0 ),
	/* 47 */ DEFKEY( "v", V, 'v', 'V', 0 ),
	/* 48 */ DEFKEY( "b", B, 'b', 'B', 0 ),
	/* 49 */ DEFKEY( "n", N, 'n', 'N', 0 ),
	/* 50 */ DEFKEY( "m", M, 'm', 'M', 0 ),
	/* 51 */ DEFKEY( ",", Comma, ',', ';', 0 ),
	/* 52 */ DEFKEY( ".", Dot, '.', ':', 0 ),
	/* 53 */ DEFKEY( "-", Hyphen, '-', '_', 0 ),
	/* 54 */ DEFKEY( "SHIFT-RIGHT", ShiftRight, 0, 0, 0 ),
	/* 55 */ DEFKEY( "NUM-MULT", NumpadMult, '*', '*', 0 ),
	/* 56 */ DEFKEY( "ALT-LEFT", AltLeft, 0, 0, 0 ),
	/* 57 */ DEFKEY( "SPACE", Space, ' ', ' ', ' ' ),
	/* 58 */ DEFKEY( "CAPS-LOCK", CapsLock, 0, 0, 0 ),
	/* 59 */ DEFKEY( "F1", F1, 0, 0, 0 ),
	/* 60 */ DEFKEY( "F2", F2, 0, 0, 0 ),
	/* 61 */ DEFKEY( "F3", F3, 0, 0, 0 ),
	/* 62 */ DEFKEY( "F4", F4, 0, 0, 0 ),
	/* 63 */ DEFKEY( "F5", F5, 0, 0, 0 ),
	/* 64 */ DEFKEY( "F6", F6, 0, 0, 0 ),
	/* 65 */ DEFKEY( "F7", F7, 0, 0, 0 ),
	/* 66 */ DEFKEY( "F8", F8, 0, 0, 0 ),
	/* 67 */ DEFKEY( "F9", F9, 0, 0, 0 ),
	/* 68 */ DEFKEY( "F10", F10, 0, 0, 0 ),
	/* 69 */ DEFKEY( "NUM-LOCK", NumpadLock, 0, 0, 0 ),
	/* 70 */ DEFKEY( "SCROLL", ScrollLock, 0, 0, 0 ),
	/* 71 */ DEFKEY( "NUM-7", Numpad7, '7', '7', 0 ),
	/* 72 */ DEFKEY( "NUM-8", Numpad8, '8', '8', 0 ),
	/* 73 */ DEFKEY( "NUM-9", Numpad9, '9', '9', 0 ),
	/* 74 */ DEFKEY( "NUM-MINUS", NumpadMinus, '-', '-', 0 ),
	/* 75 */ DEFKEY( "NUM-4", Numpad4, '4', '4', 0 ),
	/* 76 */ DEFKEY( "NUM-5", Numpad5, '5', '5', 0 ),
	/* 77 */ DEFKEY( "NUM-6", Numpad6, '6', '6', 0 ),
	/* 78 */ DEFKEY( "NUM-PLUS", NumpadPlus, '+', '+', 0 ),
	/* 79 */ DEFKEY( "NUM-1", Numpad1, '1', '1', 0 ),
	/* 80 */ DEFKEY( "NUM-2", Numpad2, '2', '2', 0 ),
	/* 81 */ DEFKEY( "NUM-3", Numpad3, '3', '3', 0 ),
	/* 82 */ DEFKEY( "NUM-0", Numpad0, '0', '0', 0 ),
	/* 83 */ DEFKEY( "NUM-DOT", NumpadDot, '.', ',', 0 ),
	/* 84 */ DEFKEY( "84", Invalid, 0, 0, 0 ),
	/* 85 */ DEFKEY( "85", Invalid, 0, 0, 0 ),
	/* 86 */ DEFKEY( "<>", Invalid, '<', '>', '|' ),
	/* 87 */ DEFKEY( "F11", F11, 0, 0, 0 ),
	/* 88 */ DEFKEY( "F12", F12, 0, 0, 0 ),
	/* 89 */ DEFKEY( "89", Invalid, 0, 0, 0 ),
	/* 90 */ DEFKEY( "90", Invalid, 0, 0, 0 ),

	/* 91 */ DEFKEY( "91", Invalid, 0, 0, 0 ),
	/* 92 */ DEFKEY( "92", Invalid, 0, 0, 0 ),
	/* 93 */ DEFKEY( "93", Invalid, 0, 0, 0 ),
	/* 94 */ DEFKEY( "94", Invalid, 0, 0, 0 ),
	/* 95 */ DEFKEY( "95", Invalid, 0, 0, 0 ),
	/* 96 */ DEFKEY( "96", Invalid, 0, 0, 0 ),
	/* 97 */ DEFKEY( "97", Invalid, 0, 0, 0 ),
	/* 98 */ DEFKEY( "98", Invalid, 0, 0, 0 ),
	/* 99 */ DEFKEY( "99", Invalid, 0, 0, 0 ),
	/* 100 */ DEFKEY( "100", Invalid, 0, 0, 0 ),
	/* 101 */ DEFKEY( "101", Invalid, 0, 0, 0 ),
	/* 102 */ DEFKEY( "102", Invalid, 0, 0, 0 ),
	/* 103 */ DEFKEY( "103", Invalid, 0, 0, 0 ),
	/* 104 */ DEFKEY( "104", Invalid, 0, 0, 0 ),
	/* 105 */ DEFKEY( "105", Invalid, 0, 0, 0 ),
	/* 106 */ DEFKEY( "106", Invalid, 0, 0, 0 ),
	/* 107 */ DEFKEY( "107", Invalid, 0, 0, 0 ),
	/* 108 */ DEFKEY( "108", Invalid, 0, 0, 0 ),
	/* 109 */ DEFKEY( "109", Invalid, 0, 0, 0 ),
	/* 110 */ DEFKEY( "110", Invalid, 0, 0, 0 ),
	/* 111 */ DEFKEY( "111", Invalid, 0, 0, 0 ),
	/* 112 */ DEFKEY( "112", Invalid, 0, 0, 0 ),
	/* 113 */ DEFKEY( "113", Invalid, 0, 0, 0 ),
	/* 114 */ DEFKEY( "114", Invalid, 0, 0, 0 ),
	/* 115 */ DEFKEY( "115", Invalid, 0, 0, 0 ),
	/* 116 */ DEFKEY( "116", Invalid, 0, 0, 0 ),
	/* 117 */ DEFKEY( "117", Invalid, 0, 0, 0 ),
	/* 118 */ DEFKEY( "118", Invalid, 0, 0, 0 ),
	/* 119 */ DEFKEY( "119", Invalid, 0, 0, 0 ),
	/* 120 */ DEFKEY( "120", Invalid, 0, 0, 0 ),
	/* 121 */ DEFKEY( "121", Invalid, 0, 0, 0 ),
	/* 122 */ DEFKEY( "122", Invalid, 0, 0, 0 ),
	/* 123 */ DEFKEY( "123", Invalid, 0, 0, 0 ),
	/* 124 */ DEFKEY( "124", Invalid, 0, 0, 0 ),
	/* 125 */ DEFKEY( "125", Invalid, 0, 0, 0 ),
	/* 126 */ DEFKEY( "126", Invalid, 0, 0, 0 ),
	/* 127 */ DEFKEY( "127", Invalid, 0, 0, 0 ),
	/* 128 */ DEFKEY( "128", Invalid, 0, 0, 0 ),
	/* 129 */ DEFKEY( "129", Invalid, 0, 0, 0 ),
	/* 130 */ DEFKEY( "130", Invalid, 0, 0, 0 ),
	/* 131 */ DEFKEY( "131", Invalid, 0, 0, 0 ),
	/* 132 */ DEFKEY( "132", Invalid, 0, 0, 0 ),
	/* 133 */ DEFKEY( "133", Invalid, 0, 0, 0 ),
	/* 134 */ DEFKEY( "134", Invalid, 0, 0, 0 ),
	/* 135 */ DEFKEY( "135", Invalid, 0, 0, 0 ),
	/* 136 */ DEFKEY( "136", Invalid, 0, 0, 0 ),
	/* 137 */ DEFKEY( "137", Invalid, 0, 0, 0 ),
	/* 138 */ DEFKEY( "138", Invalid, 0, 0, 0 ),
	/* 139 */ DEFKEY( "139", Invalid, 0, 0, 0 ),
	/* 140 */ DEFKEY( "140", Invalid, 0, 0, 0 ),
	/* 141 */ DEFKEY( "141", Invalid, 0, 0, 0 ),
	/* 142 */ DEFKEY( "142", Invalid, 0, 0, 0 ),
	/* 143 */ DEFKEY( "143", Invalid, 0, 0, 0 ),
	/* 144 */ DEFKEY( "144", Invalid, 0, 0, 0 ),
	/* 145 */ DEFKEY( "145", Invalid, 0, 0, 0 ),
	/* 146 */ DEFKEY( "146", Invalid, 0, 0, 0 ),
	/* 147 */ DEFKEY( "147", Invalid, 0, 0, 0 ),
	/* 148 */ DEFKEY( "148", Invalid, 0, 0, 0 ),
	/* 149 */ DEFKEY( "149", Invalid, 0, 0, 0 ),
	/* 150 */ DEFKEY( "150", Invalid, 0, 0, 0 ),
	/* 151 */ DEFKEY( "151", Invalid, 0, 0, 0 ),
	/* 152 */ DEFKEY( "152", Invalid, 0, 0, 0 ),
	/* 153 */ DEFKEY( "153", Invalid, 0, 0, 0 ),
	/* 154 */ DEFKEY( "154", Invalid, 0, 0, 0 ),
	/* 155 */ DEFKEY( "155", Invalid, 0, 0, 0 ),
	/* 156 */ DEFKEY( "156", Invalid, 0, 0, 0 ),
	/* 157 */ DEFKEY( "157", Invalid, 0, 0, 0 ),
	/* 158 */ DEFKEY( "158", Invalid, 0, 0, 0 ),
	/* 159 */ DEFKEY( "159", Invalid, 0, 0, 0 ),
	/* 160 */ DEFKEY( "160", Invalid, 0, 0, 0 ),
	/* 161 */ DEFKEY( "161", Invalid, 0, 0, 0 ),
	/* 162 */ DEFKEY( "162", Invalid, 0, 0, 0 ),
	/* 163 */ DEFKEY( "163", Invalid, 0, 0, 0 ),
	/* 164 */ DEFKEY( "164", Invalid, 0, 0, 0 ),
	/* 165 */ DEFKEY( "165", Invalid, 0, 0, 0 ),
	/* 166 */ DEFKEY( "166", Invalid, 0, 0, 0 ),
	/* 167 */ DEFKEY( "167", Invalid, 0, 0, 0 ),
	/* 168 */ DEFKEY( "168", Invalid, 0, 0, 0 ),
	/* 169 */ DEFKEY( "169", Invalid, 0, 0, 0 ),
	/* 170 */ DEFKEY( "170", Invalid, 0, 0, 0 ),
	/* 171 */ DEFKEY( "171", Invalid, 0, 0, 0 ),
	/* 172 */ DEFKEY( "172", Invalid, 0, 0, 0 ),
	/* 173 */ DEFKEY( "173", Invalid, 0, 0, 0 ),
	/* 174 */ DEFKEY( "174", Invalid, 0, 0, 0 ),
	/* 175 */ DEFKEY( "175", Invalid, 0, 0, 0 ),
	/* 176 */ DEFKEY( "176", Invalid, 0, 0, 0 ),
	/* 177 */ DEFKEY( "177", Invalid, 0, 0, 0 ),
	/* 178 */ DEFKEY( "178", Invalid, 0, 0, 0 ),
	/* 179 */ DEFKEY( "179", Invalid, 0, 0, 0 ),
	/* 180 */ DEFKEY( "180", Invalid, 0, 0, 0 ),
	/* 181 */ DEFKEY( "181", Invalid, 0, 0, 0 ),
	/* 182 */ DEFKEY( "182", Invalid, 0, 0, 0 ),
	/* 183 */ DEFKEY( "183", Invalid, 0, 0, 0 ),
	/* 184 */ DEFKEY( "184", Invalid, 0, 0, 0 ),
	/* 185 */ DEFKEY( "185", Invalid, 0, 0, 0 ),
	/* 186 */ DEFKEY( "186", Invalid, 0, 0, 0 ),
	/* 187 */ DEFKEY( "187", Invalid, 0, 0, 0 ),
	/* 188 */ DEFKEY( "188", Invalid, 0, 0, 0 ),
	/* 189 */ DEFKEY( "189", Invalid, 0, 0, 0 ),
	/* 190 */ DEFKEY( "190", Invalid, 0, 0, 0 ),
	/* 191 */ DEFKEY( "191", Invalid, 0, 0, 0 ),
	/* 192 */ DEFKEY( "192", Invalid, 0, 0, 0 ),
	/* 193 */ DEFKEY( "193", Invalid, 0, 0, 0 ),
	/* 194 */ DEFKEY( "194", Invalid, 0, 0, 0 ),
	/* 195 */ DEFKEY( "195", Invalid, 0, 0, 0 ),
	/* 196 */ DEFKEY( "196", Invalid, 0, 0, 0 ),
	/* 197 */ DEFKEY( "197", Invalid, 0, 0, 0 ),
	/* 198 */ DEFKEY( "198", Invalid, 0, 0, 0 ),
	/* 199 */ DEFKEY( "199", Invalid, 0, 0, 0 ),
	/* 200 */ DEFKEY( "200", Invalid, 0, 0, 0 ),
	/* 201 */ DEFKEY( "201", Invalid, 0, 0, 0 ),
	/* 202 */ DEFKEY( "202", Invalid, 0, 0, 0 ),
	/* 203 */ DEFKEY( "203", Invalid, 0, 0, 0 ),
	/* 204 */ DEFKEY( "204", Invalid, 0, 0, 0 ),
	/* 205 */ DEFKEY( "205", Invalid, 0, 0, 0 ),
	/* 206 */ DEFKEY( "206", Invalid, 0, 0, 0 ),
	/* 207 */ DEFKEY( "207", Invalid, 0, 0, 0 ),
	/* 208 */ DEFKEY( "208", Invalid, 0, 0, 0 ),
	/* 209 */ DEFKEY( "209", Invalid, 0, 0, 0 ),
	/* 210 */ DEFKEY( "210", Invalid, 0, 0, 0 ),
	/* 211 */ DEFKEY( "211", Invalid, 0, 0, 0 ),
	/* 212 */ DEFKEY( "212", Invalid, 0, 0, 0 ),
	/* 213 */ DEFKEY( "213", Invalid, 0, 0, 0 ),
	/* 214 */ DEFKEY( "214", Invalid, 0, 0, 0 ),
	/* 215 */ DEFKEY( "215", Invalid, 0, 0, 0 ),
	/* 216 */ DEFKEY( "216", Invalid, 0, 0, 0 ),
	/* 217 */ DEFKEY( "217", Invalid, 0, 0, 0 ),
	/* 218 */ DEFKEY( "218", Invalid, 0, 0, 0 ),
	/* 219 */ DEFKEY( "219", Invalid, 0, 0, 0 ),
	/* 220 */ DEFKEY( "220", Invalid, 0, 0, 0 ),
	/* 221 */ DEFKEY( "221", Invalid, 0, 0, 0 ),
	/* 222 */ DEFKEY( "222", Invalid, 0, 0, 0 ),
	/* 223 */ DEFKEY( "223", Invalid, 0, 0, 0 ),
	/* 224 */ DEFKEY( "224", Invalid, 0, 0, 0 ),
	/* 225 */ DEFKEY( "225", Invalid, 0, 0, 0 ),
	/* 226 */ DEFKEY( "226", Invalid, 0, 0, 0 ),
	/* 227 */ DEFKEY( "227", Invalid, 0, 0, 0 ),
	/* 228 */ DEFKEY( "228", Invalid, 0, 0, 0 ),
	/* 229 */ DEFKEY( "229", Invalid, 0, 0, 0 ),
	/* 230 */ DEFKEY( "230", Invalid, 0, 0, 0 ),
	/* 231 */ DEFKEY( "231", Invalid, 0, 0, 0 ),
	/* 232 */ DEFKEY( "232", Invalid, 0, 0, 0 ),
	/* 233 */ DEFKEY( "233", Invalid, 0, 0, 0 ),
	/* 234 */ DEFKEY( "234", Invalid, 0, 0, 0 ),
	/* 235 */ DEFKEY( "235", Invalid, 0, 0, 0 ),
	/* 236 */ DEFKEY( "236", Invalid, 0, 0, 0 ),
	/* 237 */ DEFKEY( "237", Invalid, 0, 0, 0 ),
	/* 238 */ DEFKEY( "238", Invalid, 0, 0, 0 ),
	/* 239 */ DEFKEY( "239", Invalid, 0, 0, 0 ),
	/* 240 */ DEFKEY( "240", Invalid, 0, 0, 0 ),
	/* 241 */ DEFKEY( "241", Invalid, 0, 0, 0 ),
	/* 242 */ DEFKEY( "242", Invalid, 0, 0, 0 ),
	/* 243 */ DEFKEY( "243", Invalid, 0, 0, 0 ),
	/* 244 */ DEFKEY( "244", Invalid, 0, 0, 0 ),
	/* 245 */ DEFKEY( "245", Invalid, 0, 0, 0 ),
	/* 246 */ DEFKEY( "246", Invalid, 0, 0, 0 ),
	/* 247 */ DEFKEY( "247", Invalid, 0, 0, 0 ),
	/* 248 */ DEFKEY( "248", Invalid, 0, 0, 0 ),
	/* 249 */ DEFKEY( "249", Invalid, 0, 0, 0 ),
	/* 250 */ DEFKEY( "250", Invalid, 0, 0, 0 ),
	/* 251 */ DEFKEY( "251", Invalid, 0, 0, 0 ),
	/* 252 */ DEFKEY( "252", Invalid, 0, 0, 0 ),
	/* 253 */ DEFKEY( "253", Invalid, 0, 0, 0 ),
	/* 254 */ DEFKEY( "254", Invalid, 0, 0, 0 ),
	/* 255 */ DEFKEY( "255", Invalid, 0, 0, 0 ),
};

/**
 * @brief A basic keyboard event
 */
struct KeyHit
{
	/**
	 * @brief The scan code of the key that produces this key hit.
	 */
	struct {
		uint16_t code;
		int16_t set;
	} scanCode;
	
	/**
	 * @brief The key that was hit.
	 */
	Key key;
	
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

#define KBD_EVENT_QUEUE_SIZE 128

volatile uint32_t kbdEventQueueReadPtr = 0;
volatile uint32_t kbdEventQueueWritePtr = 0;
KeyHit kbdEventQueue[KBD_EVENT_QUEUE_SIZE];
bool kbdState[(int)VKey::LIMIT];

void kbd_intr(CpuState * & cpu)
{
	int break_code = 0;

	// Status-Variablen fuer das Behandeln von e0- und e1-Scancodes
	static int      e0_code = 0;
	// Wird auf 1 gesetzt, sobald e1 gelesen wurde, und auf 2, sobald das erste
	// Datenbyte gelesen wurde
	static int      e1_code = 0;
	static uint16_t e1_prev = 0;

	uint8_t kbpInput = inb(0x60);
	struct {
		uint16_t code;
		int16_t set;
	} scancode = {
		0, -1
	};

	// Um einen Breakcode handelt es sich, wenn das oberste Bit gesetzt ist und
	// es kein e0 oder e1 fuer einen Extended-scancode ist
	if ((kbpInput & 0x80) &&
			(e1_code || (kbpInput != 0xE1)) &&
			(e0_code || (kbpInput != 0xE0)))
	{
		break_code = 1;
		kbpInput &= ~0x80;
	}

	if (e0_code) {
		// Fake shift abfangen und ignorieren
		if ((kbpInput == 0x2A) || (kbpInput == 0x36)) {
			e0_code = 0;
			return;
		}
		scancode = { kbpInput, 1 };
		e0_code = 0;
	} else if (e1_code == 2) {
		// Fertiger e1-Scancode
		// Zweiten Scancode in hoeherwertiges Byte packen
		e1_prev |= ((uint16_t) kbpInput << 8);
		scancode = { e1_prev, 2 };
		e1_code = 0;
	} else if (e1_code == 1) {
		// Erstes Byte fuer e1-Scancode
		e1_prev = kbpInput;
		e1_code++;
	} else if (kbpInput == 0xE0) {
		// Anfang eines e0-Codes
		e0_code = 1;
	} else if (kbpInput == 0xE1) {
		// Anfang eines e1-Codes
		e1_code = 1;
	} else {
		// Normaler Scancode
		scancode = { kbpInput, 0 };
	}
	if(scancode.set >= 0) {
		if(scancode.set == 0 && scancode.code < sizeof(keymap_set0) / sizeof(keymap_set0[0])) {
			
			KeyHit event;
			event.scanCode.set = scancode.set;
			event.scanCode.code = scancode.code;
			event.flags = KeyHitFlags::None;
			event.key = keymap_set0[scancode.code];
			
			if(break_code) {
				event.flags |= KeyHitFlags::KeyRelease;
			} else {
				event.flags |= KeyHitFlags::KeyPress;
			}
			
			// TODO: Add variant support with alt-graph here...
			if(event.flags * KeyHitFlags::KeyPress) {
				if(kbdState[(int)VKey::ShiftLeft] || kbdState[(int)VKey::ShiftRight]) {
					event.codepoint = event.key.upper;
				} else {
					event.codepoint = event.key.lower;
				}
				if(event.codepoint != 0) {
					event.flags |= KeyHitFlags::CharInput;
				}
			}
			
			// TODO: Implement key press/release counter for more precision?
			kbdState[(int)event.key.key] = (break_code == 0);
			
			kbdEventQueue[(kbdEventQueueWritePtr++) % KBD_EVENT_QUEUE_SIZE] = event;
		}
	}
}

bool isPressed(VKey key) 
{
	return kbdState[(int)key];
}

bool getkey(KeyHit &hit, bool blocking)
{
	if(blocking) {
		while(kbdEventQueueReadPtr >= kbdEventQueueWritePtr); // Burn, baby, burn!
		hit = kbdEventQueue[(kbdEventQueueReadPtr++) % KBD_EVENT_QUEUE_SIZE];
		return true;
	} else {
		if(kbdEventQueueReadPtr >= kbdEventQueueWritePtr) {
			return false;
		} else {
			hit = kbdEventQueue[(kbdEventQueueReadPtr++) % KBD_EVENT_QUEUE_SIZE];
			return true;
		}
	}
}

char getchar()
{
	KeyHit key;
	do {
		getkey(key, true);
	} while(!(key.flags * KeyHitFlags::CharInput));
	return (char)key.codepoint;
}

void init_keyboard(void);

void dasos_demo()
{
	IDT::interrupt(0x20) = Interrupt(timer_intr);
	IDT::interrupt(0x21) = Interrupt(kbd_intr);

	init_keyboard();

	ASM::sti();

	/*
	while(true)
	{
		KeyHit key;
		getkey(key, true);
	
		if(key.flags * KeyHitFlags::KeyPress) {
			Console::main << "Out of interrupt: " << key.key.name << "\n";
			
			if(isPressed(VKey::Escape)) {
				Console::main << "with escape pressed :)\n";
			}
		}
		if(key.flags * KeyHitFlags::CharInput) {
			Console::main << "'" << (char)key.codepoint << "'\n";
		}
	}
	*/
	while(true)
	{
		Console::main << getchar(); 
	}
}



 
static void send_command(uint8_t command);
void init_keyboard(void)
{ 
	// Tastaturpuffer leeren
	while (inb(0x64) & 0x1) {
		inb(0x60);
	}   

	// Tastatur aktivieren
	send_command(0xF4);
}
 
/** Befehl an die Tastatur senden */
static void send_command(uint8_t command)
{
	// Warten bis die Tastatur bereit ist, und der Befehlspuffer leer ist
	while ((inb(0x64) & 0x2)) {}
	outb(0x60, command);
}
 