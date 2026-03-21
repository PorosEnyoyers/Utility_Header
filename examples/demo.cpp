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

int main()
{
	//Doubly_List<int> list(10);
	//auto iter = list.begin();
	//int i = 0;
	//do
	//{
	//	iter.get_data() = i++;
	//	++iter;
	//} while (iter != list.begin());
	//do
	//{
	//	std::cout << iter.get_data() << " ";
	//	++iter;
	//} while (iter != list.begin());
	Doubly_Circular_List<int> list{ std::vector<int> {1,2,43,4,56,3,2,4,5} };
	std::cout << list << "\n\n";
	Doubly_Circular_List<double> list1{ 3.4,4.3,24.3,22.1,323.1,23.1,34.5 };
	std::cout << list1 << "\n\n";
	Doubly_Circular_List<std::string> list2{ std::array<std::string,5>{"Northernlion", "Cory", "Rust", "James", "Alan"} };
	std::cout << list2 << "\n\n";
	Doubly_Circular_List<std::string> list3{ std::move(list2) };
	std::cout << "Moved list length: " << list2.get_length();
	list3.insert("Khoi", 2);
	list3.insert_first("Khoa");
	list3.push_back("Khue");
	list2.push_back("Lissan Al Gaib");
	std::cout << "\n\n" << list3;
	std::cout << "\n\n" << list2;

	Doubly_Circular_List<int> list4(10);
	std::cout << "\n\n" << list4;
	if (list3.find("James") != list3.end())
	{
		std::cout << "\n\nFound";
	}
	else
	{
		std::cout << "\n\nNot Found";
	}
	return 0;
}