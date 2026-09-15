#pragma once
#include "Queue.h"
#include<utility>
#include<vector>
#include<iostream>
#include<stdexcept>
#include<bitset>
#include<bit>
namespace custom
{
	template<typename T, typename Queue = custom::Queue<T>, std::size_t max_prio = 10>
	class Prio_Queue
	{
	public:
		using value_type = typename Queue::value_type;
		using reference = value_type&;
		using const_reference = const value_type&;
		using iterator = typename Queue::iterator;
		using const_iterator = typename Queue::const_iterator;
		using size_type = typename Queue::size_type;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		static constexpr size_type default_prio = max_prio / 2;
		//Default constructor
		Prio_Queue() : m_size{}, m_array(max_prio,Queue()), m_flag{}
		{
		}

		//This is put on hold right now for later implementation.
		//constructor for passing in containers of data to queue and process, to get correct prio, the user must pass in the data with the most improtant prio list to least important prio list.
		//template<typename Iter>
		//Prio_Queue(Iter begin, Iter end, size_type size) : m_size{}, m_array(max_prio, {}), m_flag{}
		//{
		//	size_type least_important_prio_possible = { max_prio - size };
		//	while (begin != end)
		//	{
		//		m_size += *begin.get_size();
		//		if (*begin.get_size() != 0)
		//		{
		//			m_flag.set(least_important_prio_possible);
		//		}
		//		std::swap(m_array[least_important_prio_possible], *begin);
		//		++begin;
		//	}
		//}
		//Prio_Queue(std::initializer_list<Queue> ilist)
		//	: Prio_Queue(ilist.begin(), ilist.end(), ilist.size())
		//{
		//	static_assert(ilist.size() <= max_prio, "List passed in must has size 10 or smaller!!!");
		//}
		//Prio_Queue(std::vector<Queue> vect)
		//	: Prio_Queue(vect.begin(), vect.end(), vect.size())
		//{
		//	static_assert(vect.size() <= max_prio, "Vector passed in must has size 10 or smaller!!!");
		//}
		//template<size_type S>
		//Prio_Queue(std::array<Queue, S> arr)
		//	: Prio_Queue(arr.begin(), arr.end(), arr.size())
		//{
		//	static_assert(S <= max_prio, "Array passed in must has size 10 or smaller!!!");
		//}

		//Since we built on top of C++ stl, we use default destructor
		~Prio_Queue() = default;
		//Move constructor
		Prio_Queue(Prio_Queue&& prio_queue) noexcept
			: m_array(std::move(prio_queue.m_array)), m_size(prio_queue.m_size), m_flag(prio_queue.m_flag)
		{
			prio_queue.m_size = 0;
			prio_queue.m_flag.reset();
		}
		//Copy constructor
		Prio_Queue(const Prio_Queue& prio_queue)
			: m_array(prio_queue.m_array), m_size(prio_queue.m_size), m_flag(prio_queue.m_flag)
		{
		}
		//Move assignment
		[[nodiscard]] Prio_Queue& operator=(Prio_Queue&& prio_queue) & noexcept
		{
			std::swap(m_array, prio_queue.m_array);
			std::swap(m_size, prio_queue.m_size);
			std::swap(m_flag, prio_queue.m_flag);
			return *this;
		}
		//function to get how many data is queued to process regardless of prio
		[[nodiscard]] size_type get_size()
		{
			return m_size;
		}
		//function to check if the prio_queue is empty
		constexpr bool is_empty() const
		{
			return m_size == 0;
		}
		//function to see the top of the queue, you can alter the data
		[[nodiscard]] constexpr reference front()&
		{
			if (this->is_empty())
			{
				throw std::logic_error("All queues are empty!!! Can't call front()!!!");
			}
			return m_array[get_highest_prio_non_empty()].front();
		}
		//function to see the last data to be processed in the queue, you can alter the data
		[[nodiscard]] constexpr reference last()&
		{
			if (this->is_empty())
			{
				throw std::logic_error("All queues are empty!!! Can't call last()!!!");
			}
			return m_array[get_lowest_prio_non_empty()].last();
		}
		//function to see the top of the queue, you can't alter the data
		[[nodiscard]] constexpr const_reference front() const &
		{
			if (this->is_empty())
			{
				throw std::logic_error("All queues are empty!!! Can't call const front()!!!");
			}
			return m_array[get_highest_prio_non_empty()].front();
		}
		//function to see the last data to be processed in the queue, you can't alter the data
		[[nodiscard]] constexpr const_reference last() const &
		{
			if (this->is_empty())
			{
				throw std::logic_error("All queues are empty!!! Can't call const last()!!!");
			}
			return m_array[get_lowest_prio_non_empty()].last();
		}
		//Function to tell numbers of data in queue based on priority given
		[[nodiscard]] size_type numbers_of_data_with_prio(size_type prio)
		{
			return m_array[prio].get_size();
		}
		//Function to tell if any data in queue based on prio
		[[nodiscard]] bool is_prio_empty(size_type prio)
		{
			return (m_flag.test(prio) ? false : true);
		}
		//Function to queue up a data and provide no prio so it will send to the middle.
		void enqueue(value_type data)
		{
			enqueue(std::move(data),default_prio);
		}
		void enqueue(value_type data, size_type prio)
		{
			m_array[prio].enqueue(std::move(data));
			++m_size;
			m_flag.set(prio); //set the flag regardless since a condition check is more expensive and set an already set bit.
		}
		//Function to get the 1st data queued with the highest possible priority as default but we allow user to dequeue a pick prio incase of emergency
		[[nodiscard]] value_type dequeue()&
		{
			return dequeue(this->get_highest_prio_non_empty());
		}
		[[nodiscard]] value_type dequeue(size_type prio)&
		{
			if (m_size == 0)
			{
				throw std::logic_error("All queues is empty, can't call dequeue()!!!");
			}
			if (!m_flag.test(prio))
			{
				throw std::logic_error("The requested queue to dequeue is empty, can't call dequeue(prio)!!!");
			}
			value_type temp = m_array[prio].dequeue();
			--m_size;
			if (m_array[prio].is_empty())
			{
				m_flag.set(prio, 0);
			}
			return temp;
		}
	private:
		std::vector<Queue> m_array;
		size_type m_size;
		std::bitset<max_prio> m_flag;
		//Get the queue that has the highest prio that is non-empty
		size_type get_highest_prio_non_empty()
		{
			return static_cast<size_type>(std::countr_zero(m_flag.to_ullong()));
		}
		size_type get_lowest_prio_non_empty()
		{
			return static_cast<size_type>(std::bit_width(m_flag.to_ullong())) - 1;
		}
	};
}