#pragma once

/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include <config/env.hh>

namespace Input
{

namespace PS3
{
	static const uint CROSS = 1,
	CIRCLE = 2,
	SQUARE = 3,
	TRIANGLE = 4,
	L1 = 5,
	L2 = 6,
	L3 = 7,
	R1 = 8,
	R2 = 9,
	R3 = 10,
	SELECT = 11,
	START = 12,
	UP = 13, RIGHT = 14, DOWN = 15, LEFT = 16,
	PS = 17,
	LSTICK_RIGHT = 18, LSTICK_LEFT = 19, LSTICK_DOWN = 20, LSTICK_UP = 21,
	RSTICK_RIGHT = 22, RSTICK_LEFT = 23, RSTICK_DOWN = 24, RSTICK_UP = 25
	;

	static const uint COUNT = 26;
}

namespace Wiimote
{
	static const uint PLUS = 1,
	MINUS = 2,
	HOME = 3,
	LEFT = 4, RIGHT = 5, UP = 6, DOWN = 7,
	_1 = 8,
	_2 = 9,
	A = 10,
	B = 11,
	// Nunchuk
	NUN_C = 12, NUN_Z = 13,
	NUN_STICK_LEFT = 14, NUN_STICK_RIGHT = 15, NUN_STICK_UP = 16, NUN_STICK_DOWN = 17
	;

	static const uint COUNT = 18;
}

namespace WiiCC
{
	static const uint PLUS = 1,
	MINUS = 2,
	HOME = 3,
	LEFT = 4, RIGHT = 5, UP = 6, DOWN = 7,
	A = 8, B = 9,
	X = 10, Y = 11,
	L = 12, R = 13,
	ZL = 14, ZR = 15,
	LSTICK_LEFT = 16, LSTICK_RIGHT = 17, LSTICK_UP = 18, LSTICK_DOWN = 19,
	RSTICK_LEFT = 20, RSTICK_RIGHT = 21, RSTICK_UP = 22, RSTICK_DOWN = 23,
	LH = 24, RH = 25
	;

	static const uint COUNT = 26;
}

namespace iControlPad
{
	static const uint A = 1,
	B = 2,
	X = 3,
	Y = 4,
	L = 5,
	R = 6,
	START = 7,
	SELECT = 8,
	LNUB_LEFT = 9, LNUB_RIGHT = 10, LNUB_UP = 11, LNUB_DOWN = 12,
	RNUB_LEFT = 13, RNUB_RIGHT = 14, RNUB_UP = 15, RNUB_DOWN = 16,
	LEFT = 17, RIGHT = 18, UP = 19, DOWN = 20
	;

	static const uint COUNT = 21;
}

namespace Zeemote
{
	static const uint A = 1,
	B = 2,
	C = 3,
	POWER = 4,
	// Directions (from analog stick)
	LEFT = 5, RIGHT = 6, UP = 7, DOWN = 8
	;

	static const uint COUNT = 9;
}

namespace ICade
{
#if defined INPUT_SUPPORTS_KEYBOARD

// mapping overlaps system/keyboard so the same "device" can send iCade
// events as well as other key events that don't conflict with its mapping.
// Here we just use all the On-States since they won't be sent as regular
// key events.
static const uint UP = Keycode::asciiKey('w'),
	RIGHT = Keycode::asciiKey('d'),
	DOWN = Keycode::asciiKey('x'),
	LEFT = Keycode::asciiKey('a'),
	A = Keycode::asciiKey('y'),
	B = Keycode::asciiKey('h'),
	C = Keycode::asciiKey('u'),
	D = Keycode::asciiKey('j'),
	E = Keycode::asciiKey('i'),
	F = Keycode::asciiKey('k'),
	G = Keycode::asciiKey('o'),
	H = Keycode::asciiKey('l');

static const uint COUNT = Keycode::COUNT;

#else

// dedicated mapping
static const uint UP = 1,
RIGHT = 2,
DOWN = 3,
LEFT = 4,
A = 5,
B = 6,
C = 7,
D = 8,
E = 9,
F = 10,
G = 11,
H = 12
;

static const uint COUNT = 13;

#endif
}

}
