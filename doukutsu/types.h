#pragma once

// Common types used across several structures in vanilla code

namespace csvanilla
{

using BOOL = int;

struct RECT
{
	long left;
	long top;
	long right;
	long bottom;
};

struct RANGE
{
	int front;
	int top;
	int back;
	int bottom;
};

}