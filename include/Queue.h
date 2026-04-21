#pragma once
#include<list>
#include<initializer_list>
#include<array>
#include<vector>
#include<utility>
#include<stdexcept>
#include<iostream>

namespace custom
{
	template<typename T, typename Container = std::list<T>>
	class Queue
	{
	public:
		using value_type = typename Container::value_type;
		using reference = value_type&;
		using const_reference = const value_type&;
		using iterator = typename Container::iterator; 
		using size_type = typename Container::size_type;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using const_iterator = typename Container::const_iterator;

		//Default constructor
		Queue() : m_size{ 0 }, m_list{}
		{
			m_front = m_rear = m_list.begin();
		}
		//constructor for queuing 1 item
		Queue(value_type data) : m_size{ 1 }, m_list { {} }
		{
			m_list.push_back(std::move(data));
			m_front = m_list.begin();
			m_rear = --(m_list.end());
		}
		//constructor for passing in a container of data to queue and process
		template<typename Iter>
		Queue(Iter begin, Iter end, size_type size) : m_list{ begin, end }, m_size{ size }
		{
			if (m_size == 0)
			{
				m_front = m_rear = m_list.begin();
			}
			else
			{
				m_front = m_list.begin();
				m_rear = --(m_list.end());
			}
		}
		Queue(std::initializer_list <value_type> ilist)
			: Queue(ilist.begin(), ilist.end(), ilist.size())
		{
		}
		Queue(std::vector<value_type> vect)
			: Queue(vect.begin(), vect.end(), vect.size())
		{
		}
		template<size_type S>
		Queue(std::array<value_type, S> arr)
			: Queue(arr.begin(), arr.end(), arr.size())
		{
		}
		//Since we built on top of list we use default destructor
		~Queue() = default;
		//Move constructor
		Queue(Queue&& queue) noexcept : m_list(std::move(queue.m_list)), m_size(m_list.size())
		{
			queue.m_size = 0;
			if (m_size == 0)
			{
				m_front = m_rear = m_list.begin();
			}
			else
			{
				m_front = m_list.begin();
				m_rear = --(m_list.end());
			}
		}
		//Copy constructor
		Queue(const Queue& queue) : m_list(queue.m_list), m_size(queue.m_size)
		{
			if (m_size == 0)
			{
				m_front = m_rear = m_list.begin();
			}
			else
			{
				m_front = m_list.begin();
				m_rear = --(m_list.end());
			}
		}
		//Move and copy assignment
		[[nodiscard]] Queue& operator=(Queue queue) & noexcept
		{
			std::swap(m_list, queue.m_list);
			std::swap(m_size, queue.m_size);
			if (m_size == 0)
			{
				m_front = m_rear = m_list.begin();
			}
			else
			{
				m_front = m_list.begin();
				m_rear = --(m_list.end());
			}
			return *this;
		}
		//function to check if its empty
		constexpr bool is_empty() const
		{
			return (m_size == 0 ? true : false);
		}
		//function to see the top of the queue, you can alter the data
		[[nodiscard]]constexpr value_type& front()&
		{
			if (this->is_empty())
			{
				throw std::logic_error("List is empty!!! Can't call front()!!!");
			}
			return *m_front;
		}
		//function to see the end of the queue, you can alter the data
		[[nodiscard]] constexpr value_type& last()&
		{
			if (this->is_empty())
			{
				throw std::logic_error("List is empty!!! Can't call last()!!!");
			}
			return *m_rear;
		}
		//function to peek at the top of the queue, can't alter the data
		[[nodiscard]] constexpr const value_type& front() const &
		{
			if (this->is_empty())
			{
				throw std::logic_error("List is empty!!! Can't call const front()!!!");
			}
			return *m_front;
		}
		//function to peek at the end of the queue, can't alter the data
		[[nodiscard]] constexpr const value_type& last() const &
		{
			if (this->is_empty())
			{
				throw std::logic_error("List is empty!!! Can't call const last()!!!");
			}
			return *m_rear;
		}
		void enqueue(value_type data) &
		{
			try
			{
				m_list.push_back(std::move(data));
			}
			catch (std::bad_alloc)
			{
				throw std::overflow_error("Encounter memory allocation error!!!");
			}
			++m_size;
			if (m_size == 1)
			{
				m_front = m_list.begin();
				m_rear = m_list.begin();
			}
			else
			{
				++m_rear;
			}
		}
		[[nodiscard]] value_type dequeue()&
		{
			if (m_size == 0)
			{
				throw std::logic_error("Queue is empty, can't call dequeue()!!!");
			}
			value_type temp = std::move(*m_front);
			m_list.pop_front();
			m_front = m_list.begin();
			--m_size;
			return temp;
		}
		[[nodiscard]] size_type get_size()
		{
			return m_size;
		}
	private:
		Container m_list;
		iterator m_front;
		iterator m_rear;
		size_type m_size;
	};
}