#include <iostream>
#include "Utility_Header/no_copy.h"
#include <string>
#include <string_view>
#include "Utility_Header/Random.h"
#include "Utility_Header/Array_View_2D.h"
#include "Utility_Header/Array.h"
#include <vector>
#include "Utility_Header/Linked_List.h"

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
int main()
{
	//static_assert(!std::is_copy_constructible_v<FullName>);
	//static_assert(!std::is_copy_assignable_v<FullName>);
	//static_assert(std::is_move_constructible_v<FullName>);
	//static_assert(std::is_move_assignable_v<FullName>);

	//FullName one{ "Khoi", "Pham", "Le Anh" };
	//FullName two;
	//FullName three{ "Khoi", "Pham" };

	//two = std::move(one);
	//std::cout << two;
	//std::cout << three;

	//int randomNumber1{ generateRandomNum<int>(1, 100) };
	//long randomNumber2{ generateRandomNum<long>(1, 100000) };
	//long long randomNumber3{ generateRandomNum<long long>(1, 1000000000) };

	//std::cout << randomNumber1 << '\n';
	//std::cout << randomNumber2 << '\n';
	//std::cout << randomNumber3 << '\n';

	//// Define a one-dimensional std::array of int (with 3 rows and 4 columns)
	//ArrayFlat2D<int, 3, 4> arr{
	//	1, 2, 3, 4,
	//	5, 6, 7, 8,
	//	9, 10, 11, 12 };

	//// Define a two-dimensional view into our one-dimensional array
	//ArrayView2D<int, 3, 4> arrView{ arr };

	//// print array dimensions
	//std::cout << "Rows: " << arrView.rows() << '\n';
	//std::cout << "Cols: " << arrView.cols() << '\n';

	//// print array using a single dimension
	//for (int i = 0; i < arrView.length(); ++i)
	//	std::cout << arrView(i) << ' ';

	//std::cout << '\n';

	//// print array using two dimensions
	//for (int row = 0; row < arrView.rows(); ++row)
	//{
	//	for (int col = 0; col < arrView.cols(); ++col)
	//		std::cout << arrView(row, col) << ' ';
	//	std::cout << '\n';
	//}

	//std::cout << arr[0];
	//std::cout << "\n";
	//std::cout << arrView(0, 0);
	//std::cout << "\n";
	//try
	//{
	//	Array<int> int_arr{ 20,12,52,6,41,54,36,27,68,19,2 };
	//	int_arr.insert(4, 15);
	//	int_arr.push_back(30);
	//	for (int i{ 0 }; i < int_arr.get_length(); ++i)
	//	{
	//		std::cout << int_arr[i] << " ";
	//	}
	//	std::cout << std::boolalpha << int_arr.is_sorted() <<'\n';
	//	int_arr.sort();
	//	std::cout << int_arr;
	//}
	//catch (const std::out_of_range& err)
	//{
	//	std::cerr << "\nError: " << err.what() << std::endl;
	//}
	//catch (const std::logic_error& err)
	//{
	//	std::cerr << "\nErro: " << err.what() << std::endl;
	//}
	//std::cout << "\n";

	//std::vector<int> arraa{0,5};
	//print_missing_element(arraa);
	//fill_consecutive_array(arraa);
	//for (auto i : arraa)
	//{
	//	std::cout << i <<' ';
	//}
	Linked_List<int> list{ std::vector{3,43,5,63,7,86,12,13} };
	Linked_List<double> list1{ std::array{3.3,21.13,23.1,54.23,32.13,44.2, 34.13, 7834.2} };
	Linked_List<float> list2{ 2.1,2,3.4,54,5,23,24.6,68,47,89,45 };
	Linked_List<std::string> list3{ "James", "Rust", "Nick", "Witherspoon", "Sam", "Kennth" };
	Linked_List<int> list4{};

	if (auto* node = list.find(63))
	{
		node->data = 4;
	}

	if (auto* node = list1.find(23.1))
	{
		node->data = 33.2;
	}

	if (auto* node = list2.find(24.6))
	{
		node->data = 43.2;
	}

	if (auto* node = list3.find("James"))
	{
		node->data = "Khoi";
	}

	if (auto* node = list4.find(2))
	{
		node->data = 3;
	}

	std::cout << list.begin().m_ptr->data << "\n\n";
	std::cout << list1.begin().m_ptr->data << "\n\n";
	std::cout << list2.begin().m_ptr->data << "\n\n";
	std::cout << list3.begin().m_ptr->data << "\n\n";

	return 0;
}