#pragma once
#include "Linked_List.h"
#include <iostream>
#include <stdexcept>

template<typename T>
class Stack
{
private:
	Linked_List<T> m_list;
public:
	Stack() = default;
	Stack(T data) : m_list{ std::move(data) } {}
	~Stack() = default;
	Stack(const Stack& other) { this->m_list = other.m_list; }
	Stack(Stack&& other) { this->m_list = std::move(other.m_list); }
	Stack& operator=(Stack other) { this->m_list = std::move(other.m_list); return *this; }

	bool is_empty()
	{
		return m_list.get_m_length() == 0;
	}

	void push(T data)
	{
		try
		{
			m_list.insert_first(std::move(data));
		}
		catch (const std::bad_alloc&)
		{
			std::cerr << "Heap is full!!! Stack overflow!!!";
		}
	}

	T pop()
	{
		if (is_empty())
		{
			throw std::logic_error("Stack is empty!!! Stack underflow!!!");
		}
		return m_list.pop_first();
	}

	const T& peek(int index)
	{
		if (is_empty())
		{
			throw std::logic_error("Stack is empty!!!");
		}
		return m_list[index];
	}

	const T& stack_top()
	{
		if (is_empty())
		{
			throw std::logic_error("Stack is empty!!!");
		}
		return m_list[0];
	}
};