#pragma once
#include <memory>
#include <initializer_list>
#include <stdexcept>
template<typename T>
class Array
{
private:
	std::unique_ptr<T[]> m_arr;
	int m_size;
	int m_length;
public:
	enum Direction
	{
		Left,
		Right,
	};
	//constructors
	Array()
		: m_arr{ nullptr }, m_size{ 0 }, m_length{ 0 }
	{
	}
	Array(int size)
		: m_arr{ std::make_unique<T[]>(size) }, m_size{ size }, m_length{ 0 }
	{
	}
	Array(std::initializer_list<T> list)
		:Array(static_cast<int>(list.size()))
	{
		std::copy(list.begin(), list.end(), m_arr.get());
		m_length = static_cast<int>(list.size());
	}
	//destructor
	~Array() = default;
	//copy constructor
	Array(const Array& arr)
		:m_arr{ std::make_unique<T[]>(arr.m_size) }, m_size{ arr.m_size }, m_length{ arr.m_length }
	{
		for (int i{ 0 }; i < m_length - 1; ++i)
		{
			m_arr[i] = arr.m_arr[i];
		}
	}
	//copy assignment
	Array& operator=(const Array& arr)
	{
		if (this != &arr)
		{
			m_arr = std::make_unique<T[]>(arr.m_size);
			m_size = arr.m_size;
			m_length = arr.m_length;
			for (int i{ 0 }; i < m_length - 1; ++i)
			{
				m_arr[i] = arr.m_arr[i];
			}
		}
		return *this;
	}
	//Move constructor
	Array(Array&& arr) noexcept
		: m_arr{ std::move(arr.m_arr) }, m_length{ arr.m_length }, m_size{ arr.m_size }
	{
		arr.m_length = 0;
		arr.m_size = 0;
	}
	//Move Assignment
	Array& operator=(Array&& arr) noexcept
	{
		if (this != &arr)
		{
			m_arr = std::move(arr.m_arr);
			m_size = arr.m_size;
			m_length = arr.m_length;

			arr.m_size = 0;
			arr.m_length = 0;
		}
		return *this;
	}
	int get_length() const { return m_length; }
	T& operator[](int index) { return m_arr[index]; }
	const T& operator[](int index) const { return m_arr[index]; }
	//Overload operator<< for display
	friend std::ostream& operator<<(std::ostream& out, const Array& arr)
	{
		out << "Array{";
		for (int i{ 0 }; i < arr.get_length(); ++i)
		{
			out << arr[i];
			if (i != (arr.get_length() - 1))
			{
				out << ", ";
			}
		}
		out << "}";
		return out;
	}

	void resize()
	{
		m_size += (m_size / 2 + 1);
		std::unique_ptr<T[]> temp = std::make_unique<T[]>(m_size);
		for (int i{ 0 }; i < m_length; ++i)
		{
			temp[i] = std::move(m_arr[i]);
		}
		m_arr = std::move(temp);
	}

	void push_back(const T& element)
	{
		if (m_size == m_length)
		{
			resize();
		}
		m_arr[m_length] = element;
		++m_length;
	}
	void push_back(T&& element)
	{
		if (m_size == m_length)
		{
			resize();
		}
		m_arr[m_length] = std::move(element);
		++m_length;
	}

	void insert(int index, const T& element)
	{
		if (index >= m_length)
		{
			this->push_back(element);
		}
		else
		{
			if (m_length == m_size)
			{
				resize();
			}
			for (int i{ m_length }; i > index; --i)
			{
				m_arr[i] = std::move(m_arr[i - 1]);
			}
			m_arr[index] = element;
			++m_length;
		}
	}
	void insert(int index, T&& element)
	{
		if (index >= m_length)
		{
			this->push_back(std::move(element));
		}
		else
		{
			if (m_length == m_size)
			{
				resize();
			}
			for (int i{ m_length }; i > index; --i)
			{
				m_arr[i] = std::move(m_arr[i - 1]);
			}
			m_arr[index] = std::move(element);
			++m_length;
		}
	}

	void erase(int index)
	{
		for (int i{ index }; i < m_length; ++i)
		{
			swap(m_arr[i], m_arr[i + 1]);
		}
		--m_length;
	}

	void swap(T& i1, T& i2)
	{
		T temp{ std::move(i1) };
		i1 = std::move(i2);
		i2 = std::move(temp);
	}

	int TranspositionSearch(const T& value)
	{
		for (int i{ 0 }; i < m_length; ++i)
		{
			if (value == m_arr[i])
			{
				if (i != 0)
				{
					swap(m_arr[i], m_arr[i - 1]);
					return i - 1;
				}
				return i;
			}
		}
		return -1;
	}
	//The Array must be sorted.
	int Binary_Search(const T& value)
	{
		if (!(*this->is_sorted()))
		{
			throw std::logic_error("The array is not sorted!");
		}
		int low = 0;
		int high = m_length - 1;
		while (high >= low)
		{
			int middle = low + (high - low) / 2;
			if (m_arr[middle] == value)
				return middle;
			else
			{
				if (value > m_arr[middle])
					low = middle + 1;
				else
					high = middle - 1;
			}
		}
		return -1;
	}
	//Bound checking the index to accessing the element
	T& at(int index)
	{
		if (index < 0 || index >= m_length)
		{
			throw std::out_of_range("Index out of bounds");
		}
		return m_arr[index];
	}
	const T& at(int index) const
	{
		if (index < 0 || index >= m_length)
		{
			throw std::out_of_range("Index out of bounds");
		}
		return m_arr[index];
	}

	//Reverse array
	void reverse()
	{
		T temp{};
		for (int i{ 0 }, j{ m_length - 1 }; i < j; ++i, --j)
		{
			temp = std::move(m_arr[i]);
			m_arr[i] = std::move(m_arr[j]);
			m_arr[j] = std::move(temp);
		}
	}

	//Shift the array
	void shift(Array::Direction d, int num = 1)
	{
		if (num < 0 || num > m_length)
		{
			throw std::logic_error("Number of elements to shift bigger than array length");
		}
		if (num == m_length)
		{
			for (int i{ 0 }; i > (m_length - 1); ++i)
			{
				m_arr[i] = T{};
			}
		}
		switch (d)
		{
		case(Left):
		{
			for (int i{ 0 }; i < num; ++i)
			{
				erase(0);
			}
			m_length += num;
			int last_index = m_length - 1;
			for (int i{ last_index }; i > (last_index - num); --i)
			{
				m_arr[i] = T{};
			}
			return;
		}
		case(Right):
		{
			int last_index = m_length - 1;
			for (int i{ last_index }; i >= (num) ; --i )
			{
				swap(m_arr[i], m_arr[i - num]);
			}
			for (int i{ 0 }; i < num; ++i)
			{
				m_arr[i] = T{};
			}
			return;
		}
		default:
			return;
		}
	}
	//Check if an array is sorted(only works with integral values or user defined values that have overloaded <>= operators).
	bool is_sorted()
	{
		for (int i{ 0 }; i < (m_length - 2); ++i)
		{
			if (m_arr[i] > m_arr[i + 1])
				return false;
		}
		return true;
	}
	//Insert a value in a sorted array(only works with integral valuesor or user defined values that have overloaded <>= operators).
	void insert_sorted(const T& value)
	{
		for (int i{ 0 }; i < (m_length - 1); ++i)
		{
			if (m_arr[i] > value)
			{
				insert(i, value);
				return;
			}
		}
		insert(m_length - 1, value);
	}
	//Sort the values in the array using bubble sort(only works with integral values or user defined values that have overloaded <>= operators). By default will sort from smallest to largest.
	void sort()
	{
		sort([](const T& x, const T& y) { return x > y; });
	}
	template<typename Func>
	void sort(Func operation)
	{
		bool is_sorted{ false };
		while (!is_sorted)
		{
			is_sorted = true;
			int last_element = m_length - 1;
			for (int i{ 0 }; i < last_element; ++i)
			{
				if (operation(m_arr[i], m_arr[i + 1]))
				{
					swap(m_arr[i], m_arr[i + 1]);
					is_sorted = false;
				}
			}
			--last_element;
		}
	}
};