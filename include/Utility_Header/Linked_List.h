#pragma once
#include <vector>
#include <array>
#include <initializer_list>
#include <memory>
#include <stdexcept>

//I like to manually set pointer to null pointer even if some functions already set the pointer back to null like .release or pointer go out of scope. It's more like defensive coding and a habit.
template <typename T>
class Linked_List
{
public:
	struct Node
	{
		T data;
		std::unique_ptr<Node> next;

		Node() : data{ {} }, next{ nullptr } {}
		Node(T d, std::unique_ptr<Node> n) : data{ std::move(d) }, next{ std::move(n) } {}
	};
private:
	std::unique_ptr<Node> m_first;
	int m_length;
	Node* m_tail_ptr;
public:
	Linked_List() = default;
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
	~Linked_List()
	{
		if (m_first)
		{
			m_first = std::move(m_first->next);
		}
	}

	struct Iterator
	{
		Node* m_ptr;

		Iterator(Node* ptr) : m_ptr{ ptr } {}

		T& operator*() { return m_ptr->data; }
		Iterator& operator++() { m_ptr = m_ptr->next.get(); return *this; }
		bool operator!=(const Iterator& other) { return m_ptr != other.m_ptr; }
	};

	Iterator begin() const { return Iterator(this->m_first.get()); }
	Iterator end() const
	{
		Node* ptr = this->m_first.get();
		while (ptr->next != nullptr)
		{
			ptr = ptr->next.get();
		}
		return Iterator(ptr);
	}

	T& operator[](int index) const
	{
		if (index >= m_length)
		{
			throw std::logic_error("Index out of bound!!!");
		}
		Iterator iter{ this->begin() };
		for (int i = 0; i < index; ++i)
		{
			++iter;
		}
		return iter.m_ptr->data;
	}

	//Only works with fundamental data type(intergral and strings) or data already overloaded std::cout and operator <<.
	friend std::ostream& operator<<(std::ostream& out, const Linked_List<T>& list)
	{
		auto iter{ list.m_first.get() };
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

	void insert(T data, int position)
	{
		std::unique_ptr<Node> n_node{ std::make_unique<Node>(std::move(data),nullptr) };
		if (position < 1)
		{
			n_node->next.swap(m_first);
			m_first.reset(n_node.release());
		}
		else
		{
			m_tail_ptr = m_first.get();
			for (int i{ 0 }; i < position - 1; ++i)
			{
				m_tail_ptr = m_tail_ptr->next.get();
			}
			n_node->next.swap(m_tail_ptr->next);
			m_tail_ptr->next.reset(n_node.release());
			m_tail_ptr = nullptr;
		}
		m_length++;
	}

	void push_back(T data)
	{
		std::unique_ptr<Node> n_node{ std::make_unique<Node>(std::move(data),nullptr) };
		if (m_length == 0)
		{
			m_first = std::move(n_node);
			++m_length;
			return;
		}
		Node* end_note{ this->end().m_ptr };
		end_note->next.reset(n_node.release());
		++m_length;
	}

	void remove(int index)
	{
		if (index < 0)
		{
			throw std::logic_error("Indexes can't be negative!!!");
		}
		if (index == 0)
		{
			Node* temp{ m_first->next.release() };
			m_first.reset(temp);
		}
		else
		{
			auto iter{ this->begin() };
			for (int i{ 0 }; i < index; ++i)
			{
				m_tail_ptr = iter.m_ptr;
				++iter;
			}
			Node* temp{ iter.m_ptr->next.release() };
			m_tail_ptr->next.reset(temp);
			m_tail_ptr = nullptr;
		}
		m_length--;
	}

	void reverse()
	{
		if (m_length <= 1)
		{
			throw std::logic_error("Linked list too small to reverse!!!");
		}
		else
		{
			std::unique_ptr<Node> tail = nullptr;
			std::unique_ptr<Node> body = std::move(m_first);
			std::unique_ptr<Node> head = nullptr;

			while (body != nullptr)
			{
				head = std::move(body->next);
				body->next = std::move(tail);
				tail = std::move(body);
				body = std::move(head);
			}
			m_first = std::move(tail);
		}
	}
	
	int get_m_length() const{return m_length;}

	void set_m_length(int i){m_length = i;}

	//Copy constructor
	Linked_List(const Linked_List& list)
		: m_first{nullptr}, m_length{0}, m_tail_ptr{nullptr}
	{
		auto iter{ list.begin()};
		while (iter.m_ptr != nullptr)
		{
			this->push_back(iter.m_ptr->data);
			iter = iter.m_ptr->next.get();
		}
	}

	//Move constructor
	Linked_List(Linked_List&& list) noexcept
		:m_first{ std::move(list.m_first) }, m_length{list.m_length}, m_tail_ptr{nullptr}
	{
		list.m_length = 0;
	}

	//Move and copy assignment
	Linked_List& operator=(Linked_List list) noexcept
	{
		m_first = std::move(list.m_first);
		m_length = list.m_length;
		return *this;
	}

	//Overloading operator+ and concatenate function
	Linked_List operator+(Linked_List list) noexcept
	{
		Linked_List<T> temp{*this};
		temp.end().m_ptr->next = std::move(list.m_first);
		temp.m_length = m_length + list.m_length;
		return temp;
	}
	Linked_List& concatenate(Linked_List list) noexcept
	{
		this->end().m_ptr->next = std::move(list.m_first);
		m_length += list.m_length;
		return *this;
	}

	//Merge 2 sorted lists only use for fundamental types, from smallest to largest as default. Modify the caller by default, copy the called list unless use std::move().
	//Linked_List& merge_sorted(Linked_List list)
	//{
	//	return this->merge_sorted(std::move(list), [](const T& data1,const T& data2) {return data1 <= data2; });
	//}
	//template <typename Func>
	//Linked_List& merge_sorted(Linked_List list, Func operation)
	//{
	//	int new_length{ list.m_length + m_length };
	//	std::unique_ptr<Node> temp{ nullptr };
	//	Node* curr{ nullptr };

	//	if (operation(m_first->data, list.m_first->data))
	//	{
	//		temp = std::move(m_first);
	//		m_first = std::move(temp->next);
	//		curr = temp.get();
	//	}
	//	else
	//	{
	//		temp = std::move(list.m_first);
	//		list.m_first = std::move(temp->next);
	//		curr = temp.get();
	//	}
	//	while ((m_first != nullptr) && (list.m_first != nullptr))
	//	{
	//		if (operation(m_first->data, list.m_first->data))
	//		{
	//			std::unique_ptr<Node> transition_node = std::move(m_first);
	//			m_first = std::move(transition_node->next);
	//			curr->next = std::move(transition_node);
	//			curr = curr->next.get();
	//		}
	//		else
	//		{
	//			std::unique_ptr<Node> transition_node = std::move(list.m_first);
	//			list.m_first = std::move(transition_node->next);
	//			curr->next = std::move(transition_node);
	//			curr = curr->next.get();
	//		}
	//	}
	//	if (m_first == nullptr)
	//	{
	//		curr->next = std::move(list.m_first);
	//		curr = nullptr;
	//	}
	//	else
	//	{
	//		curr->next = std::move(m_first);
	//		curr = nullptr;
	//	}
	//	m_first = std::move(temp);
	//	m_length = new_length;
	//	return *this;
	//}


	//Improved merge sorted using dummy/sentinel node method and juggling unique_ptr using std::move() and a helper unique pointer.
	Linked_List& merge_sorted(Linked_List list)
	{
		return this->merge_sorted(std::move(list), [](const T& data1, const T& data2) {return data1 <= data2; });
	}
	template <typename Func>
	Linked_List& merge_sorted(Linked_List list, Func operation)
	{
		int new_length{ m_length + list.m_length };
		Node dummy{ {},nullptr };
		Node* curr{ &dummy };
		while (m_first!=nullptr && list.m_first!=nullptr)
		{
			std::unique_ptr<Node>& sorted_node = (operation(m_first->data, list.m_first->data) ? m_first : list.m_first);
			std::unique_ptr<Node> transition_node = std::move(sorted_node);
			sorted_node = std::move(transition_node->next);
			curr->next = std::move(transition_node);
			curr = curr->next.get();
		}
		curr->next = std::move((m_first == nullptr) ? list.m_first : m_first);
		m_length = new_length;
		m_first = std::move(dummy.next);
		return *this;
	}

	//Use to keep both list the same.copy by default, return an rvalue linked_list need to catch it or use it immediately, use std::move() to modify list1 or list2.
	friend Linked_List merge_linked_list(Linked_List list1, Linked_List list2)
	{
		return merge_linked_list(std::move(list1), std::move(list2), [](const T& a, const T& b) {return a <= b; });
	}
	template <typename Func>
	friend Linked_List merge_linked_list(Linked_List list1, Linked_List list2, Func operation)
	{
		Linked_List temp{ std::move(list1.merge_sorted(std::move(list2),operation)) };
		return temp;
	}
	
	T pop_first()
	{
		T temp = std::move(m_first->data);
		this->remove(0);
		return temp;
	}

	void insert_first(T data)
	{
		insert(std::move(data), 0);
	}

	//Sort Linked_List by swapping data and bubble sort.
	Linked_List& sort()
	{
		return this->sort([]( T& a, T& b) { return a < b; });
	}
	template <typename Func>
	Linked_List& sort(Func operation)
	{
		int last_node_position{ m_length - 1 };
		bool not_sorted{ true };
		while(not_sorted)
		{
			not_sorted = false;
			Iterator iter{ this->begin() };
			for (int i{ 0 }; i < last_node_position; ++i)
			{
				m_tail_ptr = iter.m_ptr;
				++iter;
				if (iter.m_ptr != nullptr && operation(iter.m_ptr->data, m_tail_ptr->data))
				{
					std::swap(iter.m_ptr->data, m_tail_ptr->data);
					not_sorted = true;
				}
			}
			--last_node_position;
		}
		m_tail_ptr = nullptr;
		return *this;
	}
	
	Linked_List split_off(int num)
	{
		Linked_List temp;
		Iterator iter{ this->begin() };
		for (int i = 0; i < num - 1; ++i)
		{
			++iter;
		}
		temp.m_first = std::move(iter.m_ptr->next);
		temp.m_length = m_length - num;
		m_length = num;
		return temp;
	}

	//Sort Linked_List with recursive by spliting a linked list by half until a linked list have one note and merge them with merge_sort
	Linked_List& sort_recursive()
	{
		return sort_recursive(*this);
	}
	Linked_List& sort_recursive(Linked_List& list)
	{
		if (list.m_length > 1)
		{
			Linked_List temp{ list.split_off(list.m_length / 2) };
			sort_recursive(list);
			sort_recursive(temp);
			list.merge_sorted(temp);
		}
		return list;
	}
};