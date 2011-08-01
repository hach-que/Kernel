/* KBDUS means US Keyboard Layout.  This is a scancode table
 * used to layout a standard US keyboard */

unsigned char kbd_us[256] =
{
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
	KB_CTRL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', KB_SHIFT,
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KB_SHIFT,
	'*',
	KB_ALT,	// Alt        -56
	' ',	// Space
	0,	// Caps Lock
	0,	// F1
	0,	// F2
	0,	// F3
	0,	// F4
	0,	// F5
	0,	// F6
	0,	// F7
	0,	// F8
	0,	// F9
	0,	// F10
	0,	// Num Lock
	0,	// Scroll Lock
	0,	// Home
	0,	// Up Arrow
	0,	// Page Up
	'-',
	0,	// Left Arrow
	0,
	0,	// Right Arrow
	'+',
	0,	// End
	0,	// Down Arrow
	0,	// Page Down
	0,	// Insert
	0,	// Delete
	0, 0, 0,
	0,	// F11
	0,	// F12
	0,	// Undefined    -89
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	// Upper keyboard layout (>128)
	0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
	KB_CTRL, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', KB_SHIFT,
	'|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', KB_SHIFT,
	'*',
	KB_ALT,
	' ',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0
};
