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
#include <cmath>
#include "Queue.h"
#include "Utility_Header/Prio_Queue.h"
#include "Utility_Header/Binary_Tree.h"
#include "Utility_Header/AVL_Tree.h"
#include "Utility_Header/B_Tree.h"
#include "Utility_Header/Red_Black_Tree_Ver2.h"
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

int char_to_int(char c)
{
	return static_cast<int>(c - '0');
}

char int_to_char(int i)
{
	return static_cast<char>(i + 48);
}

int get_expr_result(std::string_view expression)//single digit number for now.
{
	std::string postfix{ std::move(infix_to_postfix(expression)) };
	auto iter = postfix.begin();
	Stack<int> int_stack;
	int x1{};
	int x2{};
	auto get_and_push_result_to_stack
	{
		[&x1,&x2,&int_stack](char operation) {
			switch (operation)
			{
			case'-':
				int_stack.push(x1 - x2);
				break;
			case'+':
				int_stack.push(x1 + x2);
				break;
			case'/':
				int_stack.push(x1 / x2);
				break;
			case'*':
				int_stack.push(x1 * x2);
				break;
			case'^':
				int_stack.push(std::pow(x1,x2));
				break;
			default:
				break;
			}
		}
	};

	while (iter != postfix.end())
	{
		if(!is_operation(*iter))
		{
			int_stack.push(char_to_int(*(iter++)));
		}
		else
		{
			x2 = int_stack.pop();
			x1 = int_stack.pop();
			get_and_push_result_to_stack(*(iter++));
		}
	}
	return int_stack.pop();
}

int main()
{
	//Demo for stack
	//std::string expression{ "( a + b ) * ( c - d ))" };
	//Stack<char> char_stack{};
	//std::cout << std::boolalpha << is_parenthesis_matched(expression, char_stack);
	//std::string expression1{ "3+4*3+2^2^2" };
	//std::cout << get_expr_result(expression1);
	//std::cout << "\n\n";

	//Demo for queue
	//custom::Queue<double> double_queue{};
	//double_queue.enqueue(3.1);
	//double_queue.enqueue(67.4);
	//double_queue.enqueue(93.2);
	//double_queue.enqueue(8.5);
	//double_queue.enqueue(6.2);
	//double_queue.enqueue(2.4);
	//double_queue.enqueue(34.3);
	//custom::Queue<double> double_queue1 = std::move(double_queue);
	//while (!double_queue1.is_empty())
	//{
	//	std::cout << double_queue1.dequeue() << "\n\n";
	//}
	//std::cout << std::boolalpha << double_queue.is_empty() << "\n\n";
	//custom::Queue<std::string> string_queue{ std::vector<std::string>{"James","Rust","Northernlion","Kate","Nhu","Ly Dam"} };
	//std::cout << string_queue.front();
	//std::cout << "\n\n" << string_queue.last();

	//Demo for prio queue
	//custom::Prio_Queue<std::string> string_prio_queue{};
	//string_prio_queue.enqueue("Rust",0);
	//string_prio_queue.enqueue("James",1);
	//string_prio_queue.enqueue("Northernlion",9);
	//string_prio_queue.enqueue("Khoa",3);
	//string_prio_queue.enqueue("Khue",4);
	//string_prio_queue.enqueue("Nhu");
	//string_prio_queue.enqueue("Klaus");
	//string_prio_queue.enqueue("Ly Dam",2);
	//string_prio_queue.enqueue("Anh",6);
	//string_prio_queue.enqueue("Puer Aeternus",7);
	//std::cout << string_prio_queue.front() << "\n\n";
	//std::cout << string_prio_queue.last() << "\n\n";
	//while (!string_prio_queue.is_empty())
	//{
	//	std::cout << string_prio_queue.dequeue() << "\n\n";
	//}

	////Demo for Binary_tree
	//std::vector<int> preorder{ 4,7,9,6,3,2,5,8,1 };
	//std::vector<int> inorder{ 7,6,9,3,4,5,8,2,1 };
	//custom::Binary_Tree<int> tree{ inorder,preorder };
	//std::cout << tree;

	//Demo for AVL_Tree
	//std::vector<int> vect{ 102,54,77,2,18,32,64,27,84,24 };
	//std::array<double,8>arr{-3.6,-8.7,6.7,8.8,-3.4,83.5,-14.6,4.5 };
	//custom::AVL_Tree<int> tree1{ vect };
	////custom::AVL_Tree<double> tree2{ arr };
	////custom::AVL_Tree<std::string> tree3{ {"Witherspoon", "JSN", "Sam Darnold", "DLaw","Mike Macdonald", "Grey Zabel", "Nick Emmanwori","James", "Rasheed Shaheed","Byron Murphy", "Nwosu"} };
	//std::cout << "Tree 1: \n" << tree1 << "\n\n";
	////std::cout << "Tree 2: \n" << tree2 << "\n\n";
	////std::cout << "Tree 3: \n" << tree3 << "\n\n";
	//while (true)
	//{
	//	std::cout << "Enter data to find and delete: ";
	//	int input{};
	//	std::cin >> input;
	//	tree1.remove(input);
	//	std::cout << "\n\n" << tree1 << "\n\n";
	//	if (input == 9999)
	//	{
	//		break;
	//	}
	//}
	//std::vector<int> vect{ 1,2,3,4,5,6,7,8,9,10 };
	//custom::B_Tree<int, 9> tree{ vect };
	//tree.remove(1);
	//tree.remove(2);
	//tree.remove(3);
	//tree.remove(4);
	//tree.print_inorder();
	std::vector<int> vect{ 5,6,34,64,434,436,75,4436,533,465,64,34,64,23,54,62 };
	custom::RB_Tree<int> tree{ vect };
	std::cout << tree;
	std::cout << "\n\n\n";
	int input{};
	while (true)
	{
		if (input == 9999)
		{
			break;
		}
		std::cout << "\n\nEnter number to delete from tree: ";
		std::cin >> input;
		tree.remove(input);
		std::cout << "\n\n" << tree;
	}
	return 0;
	//custom::AVL_Tree<int> tree{ 102 };
	//tree.insert(54);
	//tree.insert(77);
	//std::cout << tree;
	//return 0;
}

//Iterator& operator++()
//{
//	Node* temp = this->m_ptr
//		if (temp == temp->parent->left)
//		{
//			temp = temp->patrent;
//			while (temp->right)
//			{
//				temp = temp->right;
//				while (temp->left)
//				{
//					temp = temp->left;
//				}
//			}
//		}
//		else
//		{
//			temp = temp->parent;
//		}
//	this->m_ptr = temp;
//	return *this;
//}

//int func(Node* p)
//{
//	int x, y;
//	if (p != nullptr)
//	{
//		x = func(p->l_child);
//		y = func(p->r_child);
//		if (x > y)
//			return x + 1;
//		else
//			return y + 1;
//	}
//	return 0;
//}