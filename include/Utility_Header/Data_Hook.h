#pragma once
#include <utility>
struct Hook
{
	Hook* prev;
	Hook* next;

	Hook()
	{
		prev = this;
		next = this;
	}
};

template<typename T>
struct Data
{
	T data;
	Hook hook;

	Data() : data{}, hook{} {}
	Data(T d) : data{ std::move(d) }, hook{} {}
};