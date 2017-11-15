#pragma once

#include <iostream>

namespace Log
{
	inline std::ostream& Err() { return std::cerr; }
	inline std::ostream& Dbg() { return std::cout; }
}
