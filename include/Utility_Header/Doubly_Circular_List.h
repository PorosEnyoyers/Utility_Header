#pragma once
#include "Data_Hook.h"
#include <algorithm>
#include <array>
#include <vector>
#include <initializer_list>
#include <stdexcept>
#include <iostream>
#include <utility>

//This doubly list will be implemented as a circular (sentinel/dummy head) intrusive linked list as an excercise for intrusive linked list and circular linked list.
template<typename T>
class Doubly_Circular_List
{
private:
	Data<T> m_data;
	int m_length;

	void destroy_all_nodes()
	{
		if (m_length == 0) return;
		Iterator iter = this->begin();
		--iter;//set iter on tail node.
		iter.m_ptr->next = nullptr; //set tail hook next to nullptr to get condition for a loop.
		iter = this->begin();//set iter back on first node.
		while (iter.m_ptr->next != nullptr) //stop the loop at last node
		{
			++iter;
			delete& get_node(iter.m_ptr->prev);//delete the data on the heap.
			iter.m_ptr->prev = nullptr; //set hook prev of the node to nullptr.
		}
		delete& get_node(iter.m_ptr); //delete the last node.
		m_length = 0;
		this->m_data.hook.next = nullptr;
	}
public:
	//Iterator struct for easy traversing the list
	struct Iterator
	{
		Hook* m_ptr;

		Iterator(Hook* ptr) : m_ptr{ ptr } {}

		Iterator& operator++() { m_ptr = m_ptr->next; return *this; }
		Iterator& operator--() { m_ptr = m_ptr->prev; return *this; }
		bool operator!=(const Iterator& other) { return m_ptr != other.m_ptr; }
		bool operator==(const Iterator& other) { return m_ptr == other.m_ptr; }
		T& get_data()
		{
			auto* temp = reinterpret_cast<char*>(this->m_ptr) - offsetof(Data<T>, hook);
			return *(reinterpret_cast<T*>(temp));
		}
	};
	Iterator begin() { return Iterator(m_data.hook.next); }
	Iterator end() { return Iterator(&m_data.hook); }
	Iterator begin() const { return Iterator(const_cast<Hook*>(m_data.hook.next)); }
	Iterator end() const { return Iterator(const_cast<Hook*>(&m_data.hook)); }

	T& front()
	{ 
		if (m_length == 0) throw std::logic_error("List is empty!!!");
		return this->begin().get_data();
	}
	T& back() 
	{ 
		if (m_length == 0) throw std::logic_error("List is empty!!!");
		Iterator temp = this->begin();
		--temp;
		return temp.get_data();
	}

	const T& front() const
	{
		if (m_length == 0) throw std::logic_error("List is empty!!!");
		return this->begin().get_data();

	}

	const T& back() const
	{
		if (m_length == 0) throw std::logic_error("List is empty!!!");
		Iterator temp = this->begin();
		--temp;
		return temp.get_data();
	}
	Data<T>& get_node(Hook* hook_ptr)
	{
		char* temp = reinterpret_cast<char*>(hook_ptr) - offsetof(Data<T>, hook);
		return *(reinterpret_cast<Data<T>*>(temp));
	}

	//Default constructor will create a sentinel/dummy node on stack.
	Doubly_Circular_List() : m_data{ {} }, m_length{ 0 } { m_data.hook.prev = nullptr; }
	Doubly_Circular_List(int length) : m_length{ length }
	{
		this->m_data = {};
		this->m_data.hook = {};
		Hook* temp = &(this->m_data.hook);
		for (int i{ 0 }; i < length; i++)
		{
			Data<T>* new_node = new Data<T>(T{});//Create a new node on heap. Default initialized the data inside the newly created node.
			new_node->hook.prev = temp;//Point the new_node hook.prev to temp;
			temp->next = &(new_node->hook);// Point the temp->next to the address of the new_node->m_hook;
			temp = &(new_node->hook);// Advance them to point to the new node hook.
		}
		temp->next = this->m_data.hook.next; //after the loop, temp is on the last node so we set the last node hook next to the 1st node hook which this the sentinel hook.next;
		this->m_data.hook.prev = nullptr; //Sentinel prev either point to itself or nullptr, I choose nullptr so we can check if m_hook.prev == nullptr its a sentinel node.
		this->m_data.hook.next->prev = temp;//Set the 1st node hook prev to the last node hook.
	}

	template <typename Iter>
	Doubly_Circular_List(Iter begin, Iter end, int length)
		: Doubly_Circular_List(length)
	{
		if (length <= 0)
		{
			throw std::logic_error("The list you given is empty!!! Please use default intialization if you want an empty list with only a sentinel.");
		}
		auto iter = this->begin();
		auto list_iter{ begin };
		for (; list_iter != end; ++list_iter)
		{
			iter.get_data() = std::move(*list_iter);
			++iter;
		}
	}

	Doubly_Circular_List(std::initializer_list<T> list)
		: Doubly_Circular_List(list.begin(), list.end(), static_cast<int>(list.size()))
	{
	}

	Doubly_Circular_List(std::vector<T> vect)
		: Doubly_Circular_List(vect.begin(), vect.end(), static_cast<int>(vect.size()))
	{
	}

	template<std::size_t N>
	Doubly_Circular_List(std::array<T, N> arr)
		: Doubly_Circular_List(arr.begin(), arr.end(), static_cast<int>(arr.size()))
	{
	}

	~Doubly_Circular_List()
	{
		destroy_all_nodes();
	}

	void push_back(T data)
	{
		Data<T>* new_node = new Data<T>(std::move(data));
		if (m_length == 0)
		{
			this->m_data.hook.next = &(new_node->hook);//set the sentinel next on new node hook
			new_node->hook.prev = &(new_node->hook);//set the new node hook.prev on its hook cause it's the only element.
			new_node->hook.next = &(new_node->hook);//set the noew node hook.next on its hook cause it's the only element.
			++m_length;
		}
		else
		{
			Hook* first_node_hook = (this->begin().m_ptr);
			Hook* last_node_hook = (this->begin().m_ptr->prev);
			last_node_hook->next = &(new_node->hook); //set the last node hook.next on the new node hook
			new_node->hook.prev = last_node_hook; //set the new node hook.prev to the last node hook
			new_node->hook.next = first_node_hook; //set he new node hook.next to the 1st node hook
			first_node_hook->prev = &(new_node->hook); //set the first node hook.prev on the new node hook
			++m_length;
		}
	}

	void insert_first(T data)
	{
		push_back(std::move(data));
		Hook* last_node_hook = (this->begin().m_ptr->prev); //get the hook of the last node which is the new node inserted
		this->m_data.hook.next = last_node_hook; // move the sentinel hook.next to the last node which will make it the 1st node in the list.
	}

	void insert(T data, int index)
	{
		if (index >= m_length)
		{
			throw std::logic_error("The index given is bigger than list length!!!");
		}
		if (index == 0)
		{
			insert_first(std::move(data));
		}
		else
		{
			Data<T>* new_node = new Data<T>(std::move(data));
			Iterator iter{ this->begin() };
			for (int i = 0; i < index; ++i)
			{
				++iter;
			}
			Hook* prev_node_hook = iter.m_ptr->prev;
			prev_node_hook->next = &(new_node->hook);
			iter.m_ptr->prev = &(new_node->hook);
			new_node->hook.next = iter.m_ptr;
			new_node->hook.prev = prev_node_hook;
			++m_length;
		}
	}

	//Copy constructor
	Doubly_Circular_List(const Doubly_Circular_List& list) : m_data{}, m_length{ 0 }
	{
		this->destroy_all_nodes();
		Iterator list_iter{ list.begin() };
		do
		{
			this->push_back(list_iter.get_data());
			++list_iter;
		} while (list_iter != list.begin());
		m_length = list.m_length;
	}
	//Move constructor
	Doubly_Circular_List(Doubly_Circular_List&& list) noexcept : m_data{}, m_length{ 0 }
	{
		std::swap(this->m_data.hook.next, list.m_data.hook.next);
		std::swap(this->m_length, list.m_length);
	}

	//Move and copy assignment. Will use move if user use std::move().
	Doubly_Circular_List& operator=(Doubly_Circular_List list)
	{
		std::swap(this->m_data.hook.next, list.m_data.hook.next);
		std::swap(this->m_length, list.m_length);
		return *this;
	}

	int get_length() const { return m_length; }

	friend std::ostream& operator<<(std::ostream& out, const Doubly_Circular_List& list)
	{
		if (list.m_length == 0)
		{
			out << "List is empty!!! Nothing to print.";
			return out;
		}
		auto iter{ list.begin() };
		do
		{
			out << iter.get_data() << "* ";
			++iter;
		} while (iter != list.begin());
		return out;
	}
	//If not found return an iterator that point to the sentinel(dummy) hook
	Iterator find(const T& key)
	{
		if (m_length == 0) { return this->end(); }
		Iterator iter = this->begin();
		do
		{
			if (iter.get_data() == key)
			{
				return iter;
			}
			++iter;
		} while (iter.m_ptr != this->m_data.hook.next);//Terminate the loop if iterator reach the end and loop back to the 1st node.
		return this->end();
	}
	//To use this, the user need to find and provide 2 iterator from the list. Risk are high since if the user provide 2 nodes from a different list, the function will destroy both list.
	void swap_position(Iterator node1, Iterator node2)
	{
		if (node1.m_ptr == node2.m_ptr || node1.m_ptr == nullptr || node2.m_ptr == nullptr || node1 == this->end() || node2 == this->end()) //check if node1 and node2 is the same node, node1 or node2 point to nullptr, node1 or node2 pointing to the sentinel hook.
		{
			throw std::logic_error("Can't swap the same node or illegitimate iterators!!!");
		}
		if (node1.m_ptr == this->begin().m_ptr || node2.m_ptr == this->begin().m_ptr)
		{
			this->m_data.hook.next = (node1.m_ptr == this->begin().m_ptr ? node2.m_ptr : node1.m_ptr);//if user swap included 1st node, we update the sentinel to point to node that will become 1st node.
		}
		if (node1.m_ptr->next == node2.m_ptr)//check if they are adjacent because the implementation will be different.
		{
			std::swap(node1.m_ptr->next, node2.m_ptr->next);
			std::swap(node1.m_ptr->prev, node2.m_ptr->prev);
			node1.m_ptr->prev = node2.m_ptr;
			node2.m_ptr->next = node1.m_ptr;
			std::swap(node1.m_ptr->next->prev, node2.m_ptr->prev->next);
		}
		else if (node2.m_ptr->next == node1.m_ptr)//check if they are adjacent and node2 comes first so the implementation will be different
		{
			std::swap(node1.m_ptr->next, node2.m_ptr->next);
			std::swap(node1.m_ptr->prev, node2.m_ptr->prev);
			node2.m_ptr->prev = node1.m_ptr;
			node1.m_ptr->next = node2.m_ptr;
			std::swap(node2.m_ptr->next->prev, node1.m_ptr->prev->next);
		}
		else
		{
			std::swap(node1.m_ptr->next, node2.m_ptr->next);
			std::swap(node1.m_ptr->prev, node2.m_ptr->prev);
			//Now we update the adjacent nodes after swapping.
			node1.m_ptr->next->prev = node1.m_ptr;
			node1.m_ptr->prev->next = node1.m_ptr;
			node2.m_ptr->next->prev = node2.m_ptr;
			node2.m_ptr->prev->next = node2.m_ptr;
		}
	}

	//Provide an iterator and move it to 1st node.
	void swap_to_first(Iterator node)
	{
		if (node == this->begin() || node.m_ptr == nullptr || node == this->end())
		{
			throw std::logic_error("Already 1st node or illegitimate iterators!!!");
		}
		swap_position(this->begin(), node);
	}

	//Find and move to first node
	Iterator find_and_swap_to_first(const T& key)
	{
		Iterator get_node = find(key);
		if (get_node == this->end())
		{
			return this->end();
		}
		if (get_node == this->begin())
		{
			return this->begin();
		}
		else
		{
			swap_position(this->begin(), get_node);
			return this->begin();
		}
	}

	//Remove a node given an iterator.
	void remove(Iterator node)
	{
		if (node.m_ptr == nullptr)
		{
			throw std::logic_error("Invalid node!!!");
		}
		if (node == this->end())
		{
			return;
		}
		if (node == this->begin())
		{
			if (m_length == 1)
			{
				this->m_data.hook.next = nullptr;
				delete& (get_node(node.m_ptr));
				--m_length;
				return;
			}
			this->m_data.hook.next = node.m_ptr->next;
		}
		node.m_ptr->next->prev = node.m_ptr->prev;
		node.m_ptr->prev->next = node.m_ptr->next;
		node.m_ptr->next = nullptr;
		node.m_ptr->prev = nullptr;
		delete& (get_node(node.m_ptr));
		node.m_ptr = nullptr;
		--m_length;
	}
	//Combine find and remove to find a node and remove
	void find_and_remove(const T& data)
	{
		Iterator node = this->find(data);
		this->remove(node);
	}
	//Pop first will remove the 1st node in the list and return the data
	T pop_first()
	{
		if (m_length == 0)
		{
			throw std::logic_error("List is empty!!!");
		}
		T data = std::move(this->begin().get_data());
		remove(this->begin());
		return data;
	}
	T pop_last()
	{
		if (m_length == 0)
		{
			throw std::logic_error("List is empty!!!");
		}
		Iterator iter = this->begin();
		--iter;
		T data = std::move(iter.get_data());
		remove(iter);
		return data;
	}
};

