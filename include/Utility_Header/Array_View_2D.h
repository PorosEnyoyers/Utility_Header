#pragma once
#include <array>
#include <functional>
template<typename T, std::size_t Rows, std::size_t Cols>
using ArrayFlat2D = std::array<T, Rows * Cols>;

template<typename T, std::size_t Rows, std::size_t Cols>
class ArrayView2D
{
private:
	std::reference_wrapper<ArrayFlat2D<T, Rows, Cols>> m_arr{};

public:
	ArrayView2D(ArrayFlat2D<T, Rows, Cols>& arr)
		: m_arr{ arr }
	{
	}
	//Accessing as a 1d array
	T& operator()(int i) { return m_arr.get()[static_cast<std::size_t>(i)]; }
	const T& operator()(int i) const { return m_arr.get()[static_cast<std::size_t>(i)]; }
	//Accessing as a 2d array
	T& operator()(int row, int col) { return m_arr.get()[static_cast<std::size_t>(col + row * cols())]; }
	const T& operator()(int row, int col) const { return m_arr.get()[static_cast<std::size_t>(col + row * cols())]; }
	//Util member functions
	int rows() const { return static_cast<int>(Rows); }
	int cols() const { return static_cast<int>(Cols); }
	int length() const { return static_cast<int>(Rows * Cols); }
};

