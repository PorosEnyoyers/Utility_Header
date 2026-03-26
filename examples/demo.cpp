#include <iostream>
#include "Utility_Header/no_copy.h"
#include <string>
#include <string_view>
#include "Utility_Header/Random.h"
#include "Utility_Header/Array_View_2D.h"
#include "Utility_Header/Array.h"
#include <vector>
#include "Utility_Header/Linked_List.h"
#include <array>
#include "Utility_Header/Data_Hook.h"
#include "Utility_Header/Doubly_Circular_List.h"
#include "Utility_Header/Stack.h"

//Testing No_Copy header
class FullName : private No_Copy
{
private:
	std::string m_first;
	std::string m_last;
	std::string m_middle;
public:
	FullName() = default;

	FullName(std::string first, std::string last)
		: m_first{ std::move(first) }, m_last{std::move(last)}
	{
	}

	FullName(std::string first, std::string last, std::string middle)
		: m_first{ std::move(first) }, m_last{ std::move(last) }, m_middle{ std::move(middle) }
	{
	}
	
	FullName(FullName&& name) noexcept
		: m_first(std::move(name.m_first)), m_last(std::move(name.m_last)), m_middle(std::move(name.m_middle))
	{
	}

	FullName& operator=(FullName&& name) noexcept
	{
		m_first = std::move(name.m_first);
		m_last = std::move(name.m_last);
		m_middle = std::move(name.m_middle);
		return *this;
	}

	const std::string& getFirst() const
	{
		return m_first;
	}
	const std::string& getLast() const
	{
		return m_last;
	}
	const std::string& getMiddle() const
	{
		return m_middle;
	}

	friend std::ostream& operator<<(std::ostream& out, const FullName& name)
	{
		out << "Full Name: " << name.getFirst();
		if (!name.getMiddle().empty())
		{
			out << " " << name.getMiddle();
		}
		out << " " << name.getLast() << "\n";
		return out;
	}
};

//Testimg Ramdom Header
template<typename T>
T generateRandomNum(T min = 0, T max = 100)
{
	return Random::get(min, max);
}

void fill_consecutive_array(std::vector<int>& arr)
{
	int diff = arr[0] - 0;
	int i = 0;
	while (i < arr.size())
	{
		if (arr[i] - i != diff)
		{
		arr.insert(arr.begin() + i, i + diff);//insert the missing number to the array.
		}
		++i;
	}
}

void print_missing_element(std::vector<int>& arr)
{
	int diff = arr[0] - 0;
	for (int i = 0; i < arr.size(); ++i)
	{
		if (arr[i] - i != diff)
		{
			while (diff < arr[i] - i)
			{
				std::cout << i + diff << '\n';
				++diff;
			}
		}
	}
}

struct ChaosInt {
	int value;
	static inline int count = 0;
	static inline int trigger = 5; // Explode after 5 comparisons

	bool operator<=(const ChaosInt& other) const {
		if (++count >= trigger) {
			throw std::runtime_error("BOOM! Comparison failed!");
		}
		return value <= other.value;
	}
};

bool is_parenthesis_matched(std::string& string, Stack<char>& stack)
{
	auto i = string.begin();
	while (i != string.end())
	{
		std::cout << "Loop running \n\n";
		if (*i == '(')
			stack.push(*i);
		else if (*i == ')')
		{
			if (stack.is_empty())
				return false;
			stack.pop();
		}
		++i;
	}
	if (stack.is_empty())
		return true;
	return false;
}

bool is_operation(char c)
{
	if (c == '+' || c == '-' || c == '/' || c == '*' || c == '^')
		return true;
	return false;
}

bool is_brackets(char c)
{
	if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}')
		return true;
	return false;
}

int get_prio_out_stack(char c)
{
	switch (c)
	{
	case'-':
		return 1;
	case'+':
		return 1;
	case'*':
		return 3;
	case'/':
		return 3;
	case'^':
		return 6;
	case'(':
		return 7;
	case')':
		return 0;
	case'[':
		return 7;
	case']':
		return 0;
	case'{':
		return 7;
	case'}':
		return 0;
	default:
		return 8;
	}
}

int get_prio_in_stack(char c)
{
	switch (c)
	{
	case'-':
		return 2;
	case'+':
		return 2;
	case'*':
		return 4;
	case'/':
		return 4;
	case'^':
		return 5;
	case'(':
		return 0;
	case')':
		return 0;
	case'{':
		return 0;
	case'}':
		return 0;
	case'[':
		return 0;
	case']':
		return 0;
	default:
		return 8;
	}
}

std::string infix_to_postfix(std::string_view string)
{
	std::string result{};
	result.reserve(string.length() + 1);
	Stack<char> char_stack{};
	auto iter{ string.begin() };
	while (iter != string.end())
	{
		if ((*iter == ')' || *iter == ']' || *iter == '}') && char_stack.is_empty())
		{
			throw std::logic_error("Invalid Expression!!! Can't convert to postfix!!!");
		}
		if (*iter == ' ')
		{
			iter++;
			continue;
		}
		if (is_operation(*iter) || is_brackets(*iter))
		{
			//If stack is empty or current prio is higher than stack top prio, we push it in on top of the stack.
			if (char_stack.is_empty() || get_prio_out_stack(*iter) > get_prio_in_stack(char_stack.stack_top()))
			{
				char_stack.push(*(iter++));
				continue;
			}
			else
			{
				//When we encounter a closing bracket, we pop everything then pop the openning bracket then move on.
				if (*iter == ')' || *iter == ']' || *iter == '}')
				{
					while (!char_stack.is_empty() && !is_brackets(char_stack.stack_top()))
					{
						result.push_back(char_stack.pop());
					}
					if (char_stack.is_empty())
					{
						throw std::logic_error("Invalid Expression!!! Can't convert to postfix!!!");
					}
					char_stack.pop();
					++iter;
				}
				else
				{
					while ((!char_stack.is_empty()) && (get_prio_out_stack(*iter) <= get_prio_in_stack(char_stack.stack_top())))
					{
						result.push_back(char_stack.pop());
					}
					char_stack.push(*(iter++));
					continue;
				}
			}
		}
		else
		{
			result.push_back(*(iter++));
			continue;
		}
	}
	while (!char_stack.is_empty())
	{
		if (is_brackets(char_stack.stack_top()))
		{
			char_stack.pop();
			continue;
		}
		result.push_back(char_stack.pop());
	}
	return result;
}

int main()
{
	//Demo for stack
	//std::string expression{ "( a + b ) * ( c - d ))" };
	//Stack<char> char_stack{};
	//std::cout << std::boolalpha << is_parenthesis_matched(expression, char_stack);
	std::string expression1{ "a*b+(c+d)*e" };
	std::string result{ infix_to_postfix(expression1) };
	std::cout << result;
	std::cout << "\n\n";
	return 0;
}