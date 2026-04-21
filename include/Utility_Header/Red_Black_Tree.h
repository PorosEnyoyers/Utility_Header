#pragma once
#include <array>
#include <utility>
#include <vector>
#include <initializer_list>
#include <iostream>
#include <string>
namespace custom_RB
{
	struct Hook
	{
		Hook* h_left = nullptr;
		Hook* h_right = nullptr;
		
		bool is_red() const
		{
			return h_parent & GET_COLOR;
		}
		bool is_black() const
		{
			return !is_red();
		}
		[[nodiscard]] Hook* get_parent_ptr()
		{
			return reinterpret_cast<Hook*>(h_parent & GET_PTR);
		}
		[[nodiscard]] void set_parent_ptr(Hook* new_par)
		{
			uintptr_t color_tag = h_parent & GET_COLOR;
			h_parent = reinterpret_cast<uintptr_t>(new_par) | color_tag;
		}
		void tag_red()
		{
			h_parent = h_parent | GET_COLOR;
		}
		void tag_black()
		{
			h_parent = h_parent & GET_PTR;
		}

		Hook() = default;
	private:
		uintptr_t h_parent = 0;
		static constexpr uintptr_t GET_COLOR = 0x1;
		static constexpr uintptr_t GET_PTR = ~0x1;
	};

	template <typename T>
	struct RB_Node
	{
		T n_data;
		Hook n_hook;

		RB_Node() = delete;
		RB_Node(T data) : n_data{ std::move(data) }, n_hook{} {}
	};

	template <typename T>
	class RB_Tree
	{
	public:
		using size_type = size_t;
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		struct Iterator
		{
			Hook* i_ptr;
			Hook* i_sentinel;

			Iterator(Hook* ptr, Hook* sentinel) : i_ptr{ ptr }, i_sentinel{ sentinel } {}

			bool operator==(const Iterator& other) const { return i_ptr == other.i_ptr; }
			bool operator!=(const Iterator& other) const { return i_ptr != other.i_ptr; }
			[[nodiscard]] constexpr RB_Node<value_type>* get_node_ptr()
			{
				auto* temp = reinterpret_cast<char*>(this->i_ptr) - offsetof(RB_Node<value_type>, n_hook);
				return reinterpret_cast<RB_Node<value_type>*>(temp);
			}
			[[nodiscard]] constexpr reference operator*()
			{
				return this->get_node_ptr()->n_data;
			}
			bool is_root()const&
			{
				return this->i_ptr->get_parent_ptr() == i_sentinel;
			}
			bool is_left()const&
			{
				return !(this->is_root()) && !(this->is_sentinel()) && this->i_ptr == this->i_ptr->get_parent_ptr()->h_left;
			}
			bool is_right()const&
			{
				return !(this->is_root()) && !(this->is_sentinel()) && this->i_ptr == this->i_ptr->get_parent_ptr()->h_right;
			}
			Iterator& go_left()&
			{
				this->i_ptr = this->i_ptr->h_left;
				return *this;
			}
			Iterator& go_right()&
			{
				this->i_ptr = this->i_ptr->h_right;
				return *this;
			}
			Iterator& go_parent()&
			{
				this->i_ptr = this->i_ptr->get_parent_ptr();
				return *this;
			}
			bool is_leaf()const&
			{
				if (this->i_ptr->h_left || this->i_ptr->h_right)
				{
					return false;
				}
				return true;
			}
			bool is_internal()const&
			{
				return !is_leaf();
			}
			bool is_sentinel()const&
			{
				return this->i_ptr == i_sentinel;
			}
			Iterator& operator++()&
			{
				//if ++ is call on iterator end(), it will loop back to the smallest element.
				if (this->is_sentinel())
				{
					this->i_ptr = i_sentinel->h_left;
				}
				else
				{
					Iterator temp = *this;
					if (temp.i_ptr->h_right)
					{
						temp.go_right();
						while (temp.i_ptr->h_left)
						{
							temp.go_left();
						}
					}
					else
					{
						while ((!temp.is_root()) && temp.is_right())
						{
							temp.go_parent();
						}
						temp.go_parent();
					}
					this->i_ptr = temp.i_ptr;
				}
				return *this;
			}
			Iterator& operator--()&
			{
				//if -- is call on iterator end(), it will go to the largest element.
				if (this->is_sentinel())
				{
					this->i_ptr = i_sentinel->h_right;
				}
				else
				{
					Iterator temp = *this;
					if (temp.i_ptr->h_left)
					{
						temp.go_left();
						while (temp.i_ptr->h_right)
						{
							temp.go_right();
						}
					}
					else
					{
						while ((!temp.is_root()) && temp.is_left())
						{
							temp.go_parent();
						}
						temp.go_parent();
					}
					this->i_ptr = temp.i_ptr;
				}
				return *this;
			}
			Iterator get_sib()
			{
				Iterator temp{ nullptr, i_sentinel };
				temp.i_ptr = (this->i_ptr == this->i_ptr->get_parent_ptr()->h_left ? this->i_ptr->get_parent_ptr()->h_right : this->i_ptr->get_parent_ptr()->h_left);
				return temp;
			}
			bool sib_is_red()
			{
				Iterator sib{ this->get_sib() };
				if (sib.i_ptr)
				{
					return sib.i_ptr->is_red();
				}
				return false;
			}
			size_type get_level()&
			{
				size_type level{ 0 };
				Hook* temp = this->i_ptr;
				while (temp != i_sentinel)
				{
					temp = temp->get_parent_ptr();
					++level;
				}
				return level;
			}
		};
		[[nodiscard]] constexpr Iterator root()&
		{
			return Iterator(m_sentinel.get_parent_ptr(), &m_sentinel);
		}
		[[nodiscard]] constexpr Iterator begin()&
		{
			return Iterator(m_sentinel.h_left, &m_sentinel);
		}
		[[nodiscard]] constexpr Iterator end()&
		{
			return Iterator(&m_sentinel, &m_sentinel);
		}
		[[nodiscard]] constexpr Iterator rbegin()&
		{
			return Iterator(m_sentinel.h_right, &m_sentinel);
		}
		[[nodiscard]] constexpr Iterator rend()&
		{
			return Iterator(&m_sentinel, &m_sentinel);
		}
		[[nodiscard]] constexpr Iterator min()&
		{
			return Iterator(m_sentinel.h_left, &m_sentinel);
		}
		[[nodiscard]] constexpr Iterator max()&
		{
			return Iterator(m_sentinel.h_right, &m_sentinel);
		}
		//Return an iterator that hold the sentinel if value not found
		Iterator find(const value_type& key)&
		{
			if (m_size == 0)
			{
				return Iterator(&m_sentinel, &m_sentinel);
			}
			Iterator curr_node = this->root();
			while (curr_node.i_ptr)
			{
				if (key == *curr_node)
				{
					break;
				}
				(key < *curr_node ? curr_node.go_left() : curr_node.go_right());
			}
			if (curr_node.i_ptr)
			{
				return curr_node;
			}
			else
			{
				curr_node.i_ptr = &m_sentinel;
				return curr_node;
			}
		}
		void ll_rotation(Iterator& curr_node, Iterator& parent_node)
		{
			parent_node.go_parent();
			curr_node.go_parent();
			Hook* grand_par_hook = parent_node.i_ptr->get_parent_ptr();
			if (grand_par_hook == &m_sentinel)
			{
				grand_par_hook->set_parent_ptr(curr_node.i_ptr);
			}
			else
			{
				(parent_node.is_left() ? grand_par_hook->h_left = curr_node.i_ptr : grand_par_hook->h_right = curr_node.i_ptr);
			}
			curr_node.i_ptr->set_parent_ptr(grand_par_hook);
			curr_node.i_ptr->tag_black();
			Hook* curr_r_child = curr_node.i_ptr->h_right;
			parent_node.i_ptr->h_left = curr_r_child;
			if (curr_r_child)
			{
				curr_r_child->set_parent_ptr(parent_node.i_ptr);
			}
			curr_node.i_ptr->h_right = parent_node.i_ptr;
			parent_node.i_ptr->set_parent_ptr(curr_node.i_ptr);
			parent_node.i_ptr->tag_red();
			parent_node.i_ptr = grand_par_hook;//now curr node is at parent node and parent node go to its parent.
		}
		/*
		Method for ll rotation:
		1. (A parent) l_ptr or r_ptr -> B
		2. B parent pointer -> A parent
		3. Tag B black.
		4. A l_ptr -> (B right child)
		5. If B has right child, (B right child) parent ptr -> A
		6. B r_child -> A
		7. A parent_ptr -> B
		8. Tag A red.
		*/
		void lr_rotation(Iterator& curr_node, Iterator& parent_node)
		{
			Hook* grand_par_hook = parent_node.i_ptr->get_parent_ptr();
			grand_par_hook->h_left = curr_node.i_ptr;
			curr_node.i_ptr->set_parent_ptr(grand_par_hook);
			Hook* curr_l_child = curr_node.i_ptr->h_left;
			parent_node.i_ptr->h_right = curr_l_child;
			if (curr_l_child)
			{
				curr_l_child->set_parent_ptr(parent_node.i_ptr);
			}
			curr_node.i_ptr->h_left = parent_node.i_ptr;
			parent_node.i_ptr->set_parent_ptr(curr_node.i_ptr);
			std::swap(curr_node.i_ptr, parent_node.i_ptr);
			ll_rotation(curr_node, parent_node);
		}
		/*
		We will just realign the curr node and parent node to make it a ll imbalance and call ll rotaion
		*/
		void rr_rotation(Iterator& curr_node, Iterator& parent_node)
		{
			parent_node.go_parent();
			curr_node.go_parent();
			Hook* grand_par_hook = parent_node.i_ptr->get_parent_ptr();
			if (grand_par_hook == &m_sentinel)
			{
				grand_par_hook->set_parent_ptr(curr_node.i_ptr);
			}
			else
			{
				(parent_node.is_right() ? grand_par_hook->h_right = curr_node.i_ptr : grand_par_hook->h_left = curr_node.i_ptr);
			}
			curr_node.i_ptr->set_parent_ptr(grand_par_hook);
			curr_node.i_ptr->tag_black();
			Hook* curr_l_child = curr_node.i_ptr->h_left;
			parent_node.i_ptr->h_right = curr_l_child;
			if (curr_l_child)
			{
				curr_l_child->set_parent_ptr(parent_node.i_ptr);
			}
			curr_node.i_ptr->h_left = parent_node.i_ptr;
			parent_node.i_ptr->set_parent_ptr(curr_node.i_ptr);
			parent_node.i_ptr->tag_red();
			parent_node.i_ptr = grand_par_hook;
		}
		/*
		Method for rr rotation:
		1. (A parent) l_ptr or r_ptr -> B
		2. B parent pointer -> A parent
		3. Tag B black.
		4. A r_ptr -> (B left child)
		5. If B has left child, (B left chidl) parent ptr -> A
		6. B l_child -> A
		7. A parent_ptr -> B
		8. Tag A red.
		*/
		void rl_rotation(Iterator& curr_node, Iterator& parent_node)
		{
			Hook* grand_par_hook = parent_node.i_ptr->get_parent_ptr();
			grand_par_hook->h_right = curr_node.i_ptr;
			curr_node.i_ptr->set_parent_ptr(grand_par_hook);
			Hook* curr_r_child = curr_node.i_ptr->h_right;
			parent_node.i_ptr->h_left = curr_r_child;
			if (curr_r_child)
			{
				curr_r_child->set_parent_ptr(parent_node.i_ptr);
			}
			curr_node.i_ptr->h_right = parent_node.i_ptr;
			parent_node.i_ptr->set_parent_ptr(curr_node.i_ptr);
			std::swap(curr_node.i_ptr, parent_node.i_ptr);
			rr_rotation(curr_node, parent_node);
		}
		/*
		We will just realign the curr node and parent node to make it a rr imbalance and call rr rotaion
		*/
		void rotate(Iterator& curr_node, Iterator& parent_node)
		{
			if (parent_node.is_left())
			{
				if (curr_node.is_left())
				{
					ll_rotation(curr_node, parent_node);
				}
				else
				{
					lr_rotation(curr_node, parent_node);
				}
			}
			else
			{
				if (curr_node.is_right())
				{
					rr_rotation(curr_node, parent_node);
				}
				else
				{
					rl_rotation(curr_node, parent_node);
				}
			}
		}
		void recolor(Iterator curr_node, Iterator parent_node)
		{
			Iterator sib_node = curr_node.get_sib();
			if (!parent_node.is_root())
			{
				parent_node.i_ptr->tag_red();
			}
			curr_node.i_ptr->tag_black();
			sib_node.i_ptr->tag_black();
		}
		void process_violation(Iterator curr_node, Iterator parent_node)
		{
			while (!parent_node.is_root() && curr_node.i_ptr->is_red() && parent_node.i_ptr->is_red())
			{
				if (parent_node.sib_is_red()) // if uncle of current node is red we recolor
				{
					parent_node.go_parent();
					curr_node.go_parent();
					recolor(curr_node, parent_node);
					parent_node.go_parent();
					curr_node.go_parent();
				}
				else //if unclr of current node is null or black we rotate
				{
					rotate(curr_node, parent_node);
					break;
				}
			}

		}
		void insert(value_type key)
		{
			Iterator curr_node = this->root();
			Iterator parent_node{ nullptr, &m_sentinel };
			if (m_size == 0)
			{
				RB_Node<value_type>* new_node_ptr = new RB_Node<value_type>(std::move(key));
				m_sentinel.h_left = m_sentinel.h_right = (&(new_node_ptr->n_hook));
				new_node_ptr->n_hook.set_parent_ptr(&m_sentinel);
				m_sentinel.set_parent_ptr(&(new_node_ptr->n_hook));
				m_size = 1;
				return;
			}
			bool inserted(false);
			while (!inserted)
			{
				if (*curr_node == key)
				{
					return;
				}
				if (key < *curr_node)
				{
					if (curr_node.i_ptr->h_left)
					{
						curr_node.go_left();
					}
					else
					{
						RB_Node<value_type>* new_node_ptr = new RB_Node<value_type>(std::move(key));
						new_node_ptr->n_hook.set_parent_ptr(curr_node.i_ptr);
						new_node_ptr->n_hook.tag_red();
						curr_node.i_ptr->h_left = &(new_node_ptr->n_hook);
						inserted = true;
						parent_node = curr_node;
						curr_node.go_left();
						if (*curr_node < *(this->min()))
						{
							m_sentinel.h_left = curr_node.i_ptr;
						}
					}
				}
				else
				{
					if (curr_node.i_ptr->h_right)
					{
						curr_node.go_right();
					}
					else
					{
						RB_Node<value_type>* new_node_ptr = new RB_Node<value_type>(std::move(key));
						new_node_ptr->n_hook.set_parent_ptr(curr_node.i_ptr);
						new_node_ptr->n_hook.tag_red();
						curr_node.i_ptr->h_right = &(new_node_ptr->n_hook);
						inserted = true;
						parent_node = curr_node;
						curr_node.go_right();
						if (*curr_node > *(this->max()))
						{
							m_sentinel.h_right = curr_node.i_ptr;
						}
					}
				}
			}
			if (parent_node.i_ptr->is_red())
			{
				process_violation(curr_node, parent_node);
			}
			++m_size;

		}
		//Default constructor will create a hook on the stack and act as a sentinel
		RB_Tree() : m_sentinel{}, m_size{ 0 } {}
		template<typename Iter>
		RB_Tree(Iter begin, Iter end) : m_sentinel{}, m_size{ 0 }
		{
			while (begin != end)
			{
				this->insert(std::move(*begin));
				++begin;
			}
		}
		RB_Tree(std::vector<value_type> vect) : RB_Tree(vect.begin(), vect.end()) {}
		template<size_type s>
		RB_Tree(std::array<value_type, s> arr) : RB_Tree(arr.begin(), arr.end()) {}
		RB_Tree(std::initializer_list<value_type> list) : RB_Tree(list.begin(), list.end()) {}
		~RB_Tree() { destroy_all_node(); std::cout << "\nDestructor called!!!"; }

		friend std::ostream& operator<< (std::ostream& out, RB_Tree<value_type>& tree)
		{
			if (tree.m_size <= 0)
			{
				out << "Tree is empty!!!";
			}
			else
			{
				Iterator iter{ tree.rbegin() };
				while (!iter.is_sentinel())
				{
					std::string prefix = "";
					for (size_type i{ 0 }; i < iter.get_level(); ++i)
					{
						prefix = prefix + "      ";
					}
					if (iter.is_root())
					{
						prefix += "[Root: ";
					}
					else
					{
						(iter.is_left() ? prefix += "L___[" : prefix += "TTTT[");
					}
					out << prefix << (iter.i_ptr->is_red() ? "red:" : "blk:") << *iter << "]\n";
					--iter;
				}
			}
			return out;
		}
	private:
		Hook m_sentinel;
		size_type m_size;
		void destroy_all_node()
		{
			Iterator curr_node{ this->root() };
			if (m_size == 0) { return; }
			//Get a leaf node by traversing most left then if its not a leaf node, move right and repeat
			while (curr_node.is_internal())
			{
				while (curr_node.i_ptr->h_left)
				{
					curr_node.go_left();
				}
				if (curr_node.is_internal())
				{
					curr_node.go_right();
				}
			}
			while (!curr_node.is_sentinel())
			{
				Iterator temp = curr_node;
				if (curr_node.is_left())
				{
					curr_node.go_parent();
					while (curr_node.i_ptr->h_right)
					{
						curr_node.go_right();
						while (curr_node.i_ptr->h_left)
						{
							curr_node.go_left();
						}
					}
				}
				else
				{
					curr_node.go_parent();
				}
				delete temp.get_node_ptr();
			}
		}
		void remove_leaf_node(Iterator curr_node)
		{
			if (curr_node.i_ptr->is_red())
			{
				if (curr_node.is_left())
				{
					curr_node.go_parent();
					delete curr_node.i_ptr->h_left;
					curr_node.i_ptr->h_left = nullptr;
				}
				else
				{
					cur__node.go_parent();
					delete curr_node.i_ptr->h_right;
					curr_node.i_ptr->h_right = nullptr;
				}
			}
			else
			{
				process_double_black(curr_node);
				if (curr_node.is_left())
				{
					curr_node.go_parent();
					delete curr_node.i_ptr->h_left;
					curr_node.i_ptr->h_left = nullptr;
				}
				else
				{
					cur__node.go_parent();
					delete curr_node.i_ptr->h_right;
					curr_node.i_ptr->h_right = nullptr;
				}
			}
		}
		void remove_internal_node_with_one_child(Iterator curr_node)
		{
			Iterator temp = curr_node;
			(temp.i_ptr->h_left ? temp.go_left() : temp.go_right);
			*curr_node = std::move(*temp);
			if (temp.is_left())
			{
				delete curr_node.i_ptr->h_left;
				curr_node.i_ptr->h_left = nullptr;
			}
			else
			{
				delete curr_node.i_ptr->h_right;
				curr_node.i_ptr->h_right = nullptr;
			}
		}
		void remove_internal_node(Iterator curr_node)
		{
			if (curr_node.i_ptr->h_left == nullptr || curr_node.i_ptr->h_right == nullptr)
			{
				remove_internal_node_with_one_child(curr_node);
			}
			else
			{
				//Find inorder successor
				Iterator successor = curr_node;
				++successor;
				if (successor.i_ptr->h_right) //successor has one child
				{
					*curr_node = std::move(*successor); //curr_node take successor data
					remove_internal_node_with_one_child(successor); //successor take it child node data and delete the child
				}
				else //successor has no child
				{
					*curr_node = std::move(*successor);
					process_double_black(successor);
					if (successor.is_left())
					{
						successor.go_parent();
						delete successor.i_ptr->h_left;
						successor.i_ptr->h_left = nullptr;
					}
					else
					{
						successor.go_parent();
						delete successor.i_ptr->h_right;
						successor.i_ptr->h_right = nullptr;
					}
				}
			}
		}
		void remove(const value_type& key)
		{
			Iterator curr_node = this->find(key);
			if (curr_node = this->end())
			{
				return;
			}
			if (curr_node.is_leaf())
			{
				remove_leaf_node(curr_node);
			}
			else
			{
				remove_internal_node(curr_node);
			}
			--m_size;
		}
	};
}