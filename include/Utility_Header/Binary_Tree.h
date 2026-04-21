#pragma once
#include <utility>
#include "Queue.h"
#include <stack>
#include <array>
#include <vector>
#include <initializer_list>
#include <iostream>
#include <cassert>
namespace custom
{
	struct Binary_Hook
	{
		Binary_Hook* left_ptr;
		Binary_Hook* right_ptr;
		Binary_Hook* parent_ptr;

		Binary_Hook() : left_ptr(nullptr), right_ptr(nullptr), parent_ptr(nullptr) {}
	};

	template<typename T>
	struct Binary_Node
	{
		T data;
		Binary_Hook hook;

		Binary_Node() : data{}, hook{} {}
		Binary_Node(T d) : data{ std::move(d) }, hook{} {}
	};
	//This is just a simple binary tree that take a collection of data and store it on the heap. There's no logic to the arrangement of data except it being store somewhere on the heap in a binary tree data structure.
	template<typename T, typename Stack = std::stack<Binary_Node<T>*>, typename Queue = custom::Queue<Binary_Node<T>*>>
	class Binary_Tree
	{
	public:
		using value_type = T;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = typename Stack::size_type;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		enum class Direction 
		{
			LEFT,
			RIGHT,
		};

		struct Iterator
		{
			Binary_Hook* m_ptr;

			Iterator(Binary_Hook* ptr) : m_ptr(ptr) {}

			bool operator== (const Iterator& other) const { return m_ptr == other.m_ptr; }
			bool operator!= (const Iterator& other) const { return m_ptr != other.m_ptr; }
			[[nodiscard]] constexpr reference operator*() &
			{
				auto* temp = reinterpret_cast<char*>(this->m_ptr) - offsetof(Binary_Node<value_type>, hook);
				return *(reinterpret_cast<pointer>(temp));
			}
			[[nodiscard]] constexpr const_reference operator*() const &
			{
				auto* temp = reinterpret_cast<char*>(this->m_ptr) - offsetof(Binary_Node<value_type>, hook);
				return *(reinterpret_cast<const_pointer>(temp));
			}
			//Logic to implement operator++: we are advancing the tree using inorder method.
			constexpr Iterator& operator++() &
			{
				if (m_ptr->right_ptr != nullptr)
				{
					m_ptr = m_ptr->right_ptr;
					while (m_ptr->left_ptr != nullptr)
					{
						m_ptr = m_ptr->left_ptr;
					}
				}
				else
				{
					Binary_Hook* temp = m_ptr;
					while (temp->parent_ptr != nullptr && temp != temp->parent_ptr->left_ptr)
					{
						temp = temp->parent_ptr;
					}
					m_ptr = temp->parent_ptr;
				}
				return *this;
			}
			constexpr Iterator& operator--() &
			{
				if (m_ptr->left_ptr != nullptr)
				{
					m_ptr = m_ptr->left_ptr;
					while (m_ptr->right_ptr != nullptr)
					{
						m_ptr = m_ptr->right_ptr;
					}
				}
				else
				{
					Binary_Hook* temp = m_ptr;
					while (temp->parent_ptr != nullptr && temp != temp->parent_ptr->right_ptr)
					{
						temp = temp->parent_ptr;
					}
					m_ptr = temp->parent_ptr;
				}
				return *this;
			}
			//level will start at 1.
			size_type get_level() const
			{
				size_type level{ 0 };
				Binary_Hook* temp = this->m_ptr;
				while (temp != nullptr)
				{
					temp = temp->parent_ptr;
					++level;
				}
				return level;
			}
			bool is_root() const
			{
				if (m_ptr == nullptr)
				{
					return false;
				}
				return m_ptr->parent_ptr == nullptr;
			}
			bool is_l_child() const
			{
				if (m_ptr == nullptr || this->is_root())
				{
					return false;
				}
				return m_ptr == (m_ptr->parent_ptr->left_ptr);
			}
			bool is_r_child() const
			{
				if (m_ptr == nullptr || this->is_root())
				{
					return false;
				}
				return m_ptr == (m_ptr->parent_ptr->right_ptr);
			}

		};
		[[nodiscard]] constexpr Iterator begin() const &
		{
			Binary_Hook* temp = &(m_root_ptr->hook);
			while (temp->left_ptr != nullptr)
			{
				temp = temp->left_ptr;
			}
			return Iterator(temp);
		}
		[[nodiscard]] constexpr Iterator rbegin() const &
		{
			Binary_Hook* temp = &(m_root_ptr->hook);
			while (temp->right_ptr != nullptr)
			{
				temp = temp->right_ptr;
			}
			return Iterator(temp);
		}
		[[nodiscard]] constexpr Iterator end() const &
		{
			return Iterator(nullptr);
		}
		[[nodiscard]] constexpr Iterator rend() const &
		{
			return Iterator(nullptr);
		}
		[[nodiscard]] constexpr Iterator root() & //Return an iterator that point to root of the tree.
		{
			return Iterator(&(m_root_ptr->hook));
		}
		[[nodiscard]] constexpr const Iterator croot() const& //Return a const iterator that point to root.
		{
			return Iterator(&(m_root_ptr->hook));
		}
		//delete default constructor to follow RAII since there's no sentinel/dummy node on stack.
		Binary_Tree() = delete;
		//destructor
		~Binary_Tree()
		{
			destroy_all_nodes();
		}
		template <typename Iter>
		Binary_Tree(Iter begin, Iter end, size_type size) 
			: m_root_ptr{ nullptr }, m_num_node{ 0 }
		{
			if (size == 0)
			{
				throw std::logic_error("Can't form a binary tree on an empty container!!!");
			}
			size_type root_index = size / 2;
			m_root_ptr = new Binary_Node<value_type>{ std::move(*(begin + root_index)) };
			Binary_Hook* curr_hook = &(m_root_ptr->hook);
			std::stack<Task> s;
			s.push({ curr_hook,0,root_index - 1, Direction::LEFT });
			s.push({ curr_hook,root_index + 1,size - 1, Direction::RIGHT });
			while (!s.empty())
			{
				Task curr_task = s.top();
				s.pop();
				if (curr_task.l_child_index > curr_task.r_child_index || curr_task.r_child_index >= size)
				{
					continue;
				}
				size_type mid = curr_task.l_child_index + (curr_task.r_child_index - curr_task.l_child_index) / 2;
				Binary_Node<value_type>* new_node = new Binary_Node<value_type>{ std::move(*(begin + mid)) };
				if (curr_task.direction == Direction::LEFT)
				{
					curr_task.parent_hook->left_ptr = &(new_node->hook);
					new_node->hook.parent_ptr = curr_task.parent_hook;
				}
				else
				{
					curr_task.parent_hook->right_ptr = &(new_node->hook);
					new_node->hook.parent_ptr = curr_task.parent_hook;
				}
				Binary_Hook* new_node_hook = &(new_node->hook);
				s.push({ new_node_hook,curr_task.l_child_index, mid - 1, Direction::LEFT });
				s.push({ new_node_hook,mid + 1,curr_task.r_child_index,Direction::RIGHT });
			}
			m_num_node = size;
		}
		Binary_Tree(std::vector<value_type> vect)
			: Binary_Tree(vect.begin(), vect.end(), vect.size())
		{
		}
		Binary_Tree(std::initializer_list<value_type> list)
			: Binary_Tree(list.begin(), list.end(), list.size())
		{
		}
		template<size_type s>
		Binary_Tree(std::array<value_type, s> arr)
			: Binary_Tree(arr.begin(), arr.end(), s)
		{
		}
		template<typename Iter>
		Binary_Tree(Iter in_begin, Iter in_end, Iter pre_begin, Iter pre_end, size_type size)
		{
			std::stack<Task>s;
			Iter in_temp = in_begin;
			size_type in_root_index{ 0 };
			for (; *in_temp != *pre_begin; ++in_temp)
			{
				++in_root_index;
			}
			m_root_ptr = new Binary_Node<value_type>{ std::move(*in_temp)};
			Binary_Hook* curr_hook = &(m_root_ptr->hook);
			s.push({ curr_hook, in_root_index + 1, size - 1, Direction::RIGHT });
			s.push({ curr_hook, 0, in_root_index - 1, Direction::LEFT });
			size_type pre_index{ 1 };
			while (!s.empty())
			{
				Task curr_task = s.top();
				s.pop();
				if (curr_task.l_child_index > curr_task.r_child_index || curr_task.r_child_index >= size)
				{
					continue;
				}
				size_type new_node_index = curr_task.l_child_index;
				in_temp = in_begin + curr_task.l_child_index;
				while (*in_temp != *(pre_begin + pre_index))
				{
					++in_temp;
					++new_node_index;
				}
				++pre_index;
				Binary_Node<value_type>* new_node = new Binary_Node<value_type>{ std::move(*(in_begin + new_node_index)) };
				if (curr_task.direction == Direction::LEFT)
				{
					curr_task.parent_hook->left_ptr = &(new_node->hook);
					new_node->hook.parent_ptr = curr_task.parent_hook;
				}
				else
				{
					curr_task.parent_hook->right_ptr = &(new_node->hook);
					new_node->hook.parent_ptr = curr_task.parent_hook;
				}
				Binary_Hook* new_node_hook = &(new_node->hook);
				s.push({ new_node_hook,new_node_index + 1,curr_task.r_child_index,Direction::RIGHT });
				s.push({ new_node_hook,curr_task.l_child_index, new_node_index - 1, Direction::LEFT });
			}
			m_num_node = size;
		}
		Binary_Tree(std::vector<value_type> inorder, std::vector<value_type>preorder)
			: Binary_Tree(inorder.begin(), inorder.end(), preorder.begin(), preorder.end(), preorder.size())
		{
		}
	private:
		Binary_Node<value_type>* m_root_ptr;
		size_type m_num_node;

		[[nodiscard]]Binary_Node<value_type>* get_node_ptr(Iterator& iter) &
		{
			return reinterpret_cast<Binary_Node<value_type>*>(&(*iter));
		}
		// To traverse the tree we use Morris Traversal algorithm. Overdesigned since the pro of morris method is to avoid using stack to traverse a tree but in this we still use stack to delete the node anyways so this is mosly use as practice for Morris algo.
		void destroy_all_nodes()
		{
			if (m_num_node == 0) return;
			Stack stack; //stack will store the address of nodes in preorder to delete in postorder
			Iterator curr = this->root(); //curr will start at root.
			Iterator temp{ nullptr };
			while (curr.m_ptr != nullptr)
			{
				if (curr.m_ptr->right_ptr != nullptr)
				{
					temp.m_ptr = curr.m_ptr->right_ptr;
					while (temp.m_ptr->left_ptr != curr.m_ptr && temp.m_ptr->left_ptr != nullptr)
					{
						temp.m_ptr = temp.m_ptr->left_ptr;
					}
					if (temp.m_ptr->left_ptr == curr.m_ptr)
					{
						temp.m_ptr->left_ptr = nullptr;
						curr.m_ptr = curr.m_ptr->left_ptr;
						continue;
					}
					temp.m_ptr->left_ptr = curr.m_ptr;
					stack.push(get_node_ptr(curr));
					curr.m_ptr = curr.m_ptr->right_ptr;
				}
				else
				{
					stack.push(get_node_ptr(curr));
					curr.m_ptr = curr.m_ptr->left_ptr;
				}
			}
			while (!stack.empty())
			{
				delete stack.top();
				stack.pop();
			}
			m_num_node = 0;
			m_root_ptr = nullptr;
		}
		struct Task
		{
			Binary_Hook* parent_hook;
			size_type l_child_index;
			size_type r_child_index;
			Direction direction;

			Task() = delete;
			Task(Binary_Hook* m_ptr, size_type l, size_type r, Direction dir) : parent_hook{ m_ptr }, l_child_index{ l }, r_child_index{ r }, direction{ dir } {}
		};
		friend std::ostream& operator<<(std::ostream& out, const Binary_Tree<T,Stack,Queue>& tree)
		{
			Iterator iter{ tree.rbegin() };
			while (iter.m_ptr != nullptr)
			{
				std::string prefix = "";
				for (size_type i{ 0 }; i < iter.get_level(); ++i)
				{
					prefix = prefix + "    ";
				}
				if (iter.is_root())
				{
					prefix += "[Root: ";
				}
				else
				{ 
					(iter.is_l_child() ? prefix += "L___[" : prefix += "TTTT[");
				}
				out << prefix << *iter << "]\n";
				--iter;
			}
			return out;
		}
	};
}
//curr = &root
//while (curr != nullptr)
//{
//	if (curr->right != nullptr)
//	{
//		temp = curr->right;
//		while (temp->left != curr && temp->left != nullptr)
//		{
//			temp = temp->left;
//		}
//		if (temp->left == curr)
//		{
//			temp->left = nullptr;
//			curr = curr->left;
//			continue;
//		}
//		temp->left = curr;
//		push curr;
//		curr = curr->right;
//	}
//	else
//	{
//		push curr;
//		curr = curr->left;
//	}
//};
//
//
//
//curr = &root
//while(curr != nullptr)
//{
//	if (curr->left != nullptr)
//	{
//		temp = curr->left;
//		while (temp->right != curr && temp->right != nullptr)
//		{
//			temp = temp->right;
//		}
//		if (temp->right == curr)
//		{
//			temp->right = nullptr;
//			curr = curr->right;
//			continue;
//		}
//		temp->right = curr;
//		print curr;
//		curr = curr->left;
//	}
//
//	else
//	{
//		print curr;
//		curr = curr->right;
//	}
//};
