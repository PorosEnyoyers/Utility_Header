#pragma once
//base class to inherit from
class No_Copy
{
protected:
	//Set constructor and destructor to default
	No_Copy() = default;
	~No_Copy() = default;

	//Disable copy assignment and copy constructor.
	No_Copy(const No_Copy&) = delete;
	No_Copy& operator=(const No_Copy&) = delete;
};