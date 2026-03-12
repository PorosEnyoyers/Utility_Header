#pragma once
#include <vector>
#include <array>
#include <initializer_list>
#include <memory>
#include <stdexcept>

template <typename T>
class Linked_List
{
public:
	struct Node
	{
		T data;
		std::unique_ptr<Node> next;
	};
private:
	std::unique_ptr<Node> m_first;
	int m_length;
	Node* m_tail_ptr;
public:
	Linked_List() : m_length{ 0 }, m_first{ nullptr } {}
	Linked_List(int length)
		: m_length{ length }, m_first{ std::make_unique<Node>() }, m_tail_ptr{ nullptr }
	{
		if (length <= 0)
		{
			throw std::logic_error("Linked_List can't be empty!!!");
		}
		Node* temp{ m_first.get() };
		for (int i{ 1 }; i < m_length; i++)
		{
			temp->next = std::make_unique<Node>();
			temp = temp->next.get();
		}
	}
	/*
	Logic for the above constructor:
	1.We create a temp Node pointer that point to the same node as m_first using std::unique_ptr::get().
	2.We then create an empty(default) node in the heap using std::make_unique<Node>() and is being pointed by the Node pointer inside the current node.
	3.Visualized a second node is created and being pointed to by a Node pointer inside the first Node, so the next step is find a way to get temp to point to the second node so we can access the node pointer inside the second node to create another node and get it pointed by "next" of the second node.
	4.Repeat the loop til we reach the required length.
	*/
	template <typename Iter>
	Linked_List(Iter begin, Iter end, int length)
		: m_length{ length }, m_first{ std::make_unique<Node>() }, m_tail_ptr{ nullptr }
	{
		if (length <= 0)
		{
			throw std::logic_error("Linked List can't be empty!!!");
		}
		Node* temp{ m_first.get() };
		auto iter{ begin };
		temp->data = *iter;
		++iter;
		for (; iter != end; ++iter)
		{
			temp->next = std::make_unique<Node>();
			temp = temp->next.get();
			temp->data = *iter;
		}
	}
	Linked_List(std::initializer_list<T> list)
		: Linked_List(list.begin(), list.end(), static_cast<int>(list.size()))
	{
	}
	template <std::size_t N>
	Linked_List(const std::array<T, N>& arr)
		: Linked_List(arr.cbegin(), arr.cend(), static_cast<int>(arr.size()))
	{
	}
	Linked_List(const std::vector<T>& vect)
		: Linked_List(vect.cbegin(), vect.cend(), static_cast<int>(vect.size()))
	{
	}
	~Linked_List() = default;

	struct Iterator
	{
		Node* m_ptr;

		Iterator(Node* ptr) : m_ptr{ ptr } {}

		T& operator*() { return m_ptr->data; }
		Iterator& operator++() { m_ptr = m_ptr->next.get(); return *this; }
		bool operator!=(const Iterator& other) { return m_ptr != other.m_ptr; }
	};

	Iterator begin() { return Iterator( this->m_first.get() ); }
	Iterator end()
	{
		Node* ptr = this->m_first.get();
		while (ptr != nullptr)
		{
			ptr = ptr->next.get();
		}
		return Iterator( ptr );
	}

	//Only works with fundamental data type(intergral and strings) or data already overloaded std::cout and operator <<.
	friend std::ostream& operator<<(std::ostream& out, const Linked_List<T>& list)
	{
		auto iter{ list.m_first->next.get() };
		while (iter != nullptr)
		{
			out << iter->data << " ";
			iter = iter->next.get();
		}
		return out;
	}

	Node* find(const T& key)
	{
		auto iter{ this->begin() };
		if (m_first == nullptr)
		{
			return nullptr;
		}
		while (iter.m_ptr->next.get() != nullptr)
		{
			if (m_first.get() == nullptr || m_first.get()->data == key)
			{
				return m_first.get();
			}
			m_tail_ptr = iter.m_ptr;
			++iter;
			if (iter.m_ptr->data == key)
			{
				Node* temp = m_tail_ptr->next.release();//temp is a raw ptr holding iter;
				m_tail_ptr->next.reset(temp->next.release());//release the iter next node and assign to tail next pointer
				temp->next.reset(m_first.release());// release m_first and assign it to iter which is now a temp ptr
				m_first.reset(temp);// reassign m_first with a iter which is now a raw temp ptr.
				m_tail_ptr = nullptr;
				return iter.m_ptr;
			}
		}
		return nullptr;
	}
};