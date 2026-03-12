# Utility_Header
Making A Util Library.

Components:
No copy wrapper class.
Timer class to out put time in easy to read code. (looking at you <chrono>, very verbose library)
Basic Logging/Error Exception Handler.
A file handler to help output strings, interal type, error message into a file. Or read from a file and store it into strings.
A memster twister random number generator to help generate random numbers.
An optional wrapper that return a nullptr if initalization fail.


Date 1.8.2026
///Plan for No_Copy warpper class:
- Purpose: Have warpper class guarantee compile error for copies of data it wraps.
- Implementation:
	- Using template.
	- Copy constructor, copy assignment disabled.
	- Move constructor, move assignment.
	- Provide a swap helper function that use a temp to move object around.
	- Helper function to check for validity of an object.
- Usage: Wrap this around an expensive object to produce compile error when trying to copy, must pass everything by reference or move the object.///

The whole above idea was an attemp to immplement my own unique_ptr not in a No_Copy API for utility. Divert to the easy option of just creating a base class and disable copy constructor, copy assignment and add a default constructor and destructor. I wasted my time trying to implement a raw pointer that prevent copying and then realize "wait I'm making an inefficient, dangerous unique_ptr from <memory> not making a Non_Copy utility API.

Date 1.9.2026
Plan for Random header:
- Purpose: generate random number
- Implementation:
	- Using <random>, <chrono>
	- Make a function that create a seed_seq object and take in time and 7 random number and return a mersenne twister that is seeded with the seed_seq.
	- Create a static mersenne twister random number to get a pseudo random number.
	- Create a get function that use std::uniform_int_distribution that take a min and max throught function parameters and return a random number in that range.
- Usage: get a random number of interger type and can set min max.

Plan for Array_View_2D:
- Purpose: wrap around a linear array and view it as a 2D array by using mathematic calculation.
- Implementation:
	- Using template with 3 parameters: T for type, std::size_t row, std::size_t col.
	- Using std::reference_wrapper in <functional> to wrap around an array to allow non-copy-assignment
	- Overload operator[] to index the array fast
	- Overload operator() to do slow index checking array but less performant than [].
- Usage: Any linear std::array can be view as a 2d array but interally they are still linear.

To do list for Array.h:
- An append operation. Implement as member function.
- A merge operation for sorted arrays. Implement as static member function of the class.
- A concat operation/Overload operator+. Implement as static member function of the class.
- A union operation. Implement as static member function of the class as it return a new array.
- A intersection operation. Implement as static member function of the class as it return a new array.
- A difference operation/Overloading operator-. Implement as static member function of the class.
