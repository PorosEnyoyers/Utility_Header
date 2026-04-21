#pragma once
#include <initializer_list>
#include <array>
#include <vector>
#include <utility>
#include <stdexcept>
#include <iostream>
#include <cassert>

namespace custom
{
	struct AVL_Hook
	{
		AVL_Hook* l_ptr;
		AVL_Hook* r_ptr;
		AVL_Hook* p_ptr;
		int height;

		AVL_Hook() = delete;
		AVL_Hook(AVL_Hook* parent_ptr) : l_ptr{ nullptr }, r_ptr{ nullptr }, p_ptr{ parent_ptr }, height{ 1 } {}
		constexpr void update_height()& 
		{
			int l_height = (l_ptr ? l_ptr->height : 0);
			int r_height = (r_ptr ? r_ptr->height : 0);
			height = (l_height > r_height ? l_height : r_height) + 1;
		}
		constexpr int get_balance_factor()const&
		{
			int l_height = (l_ptr ? l_ptr->height : 0);
			int r_height = (r_ptr ? r_ptr->height : 0);
			return l_height - r_height;
		}
	};
	template<typename T>
	struct AVL_Node
	{
		T n_data;
		AVL_Hook n_hook;

		AVL_Node() = delete;
		AVL_Node(T data, AVL_Hook* parent_ptr = nullptr) : n_data{ std::move(data) }, n_hook{ parent_ptr } {}
	};

	template<typename T>
	class AVL_Tree
	{
	public:
		using value_type = T;
		using size_type = size_t;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;

		struct Iterator
		{
			AVL_Hook* m_ptr;

			Iterator(AVL_Hook* ptr) : m_ptr(ptr) {}

			bool operator==(const Iterator& other) const { return m_ptr == other.m_ptr; }
			bool operator!=(const Iterator& other) const { return m_ptr != other.m_ptr; }
			[[nodiscard]] constexpr AVL_Node<value_type>* get_node_ptr()
			{
				auto* temp = reinterpret_cast<char*>(this->m_ptr) - offsetof(AVL_Node<value_type>, n_hook);
				return reinterpret_cast<AVL_Node<value_type>*>(temp);
			}
			[[nodiscard]] constexpr reference operator*()&
			{
				return this->get_node_ptr()->n_data;
			}
			constexpr void update_height()&
			{
				this->m_ptr->update_height();
			}
			constexpr int get_balance_factor()&
			{
				return this->m_ptr->get_balance_factor();
			}
			bool is_root()&
			{
				if (m_ptr == nullptr)
				{
					return false;
				}
				return (m_ptr->p_ptr == nullptr);
			}
			bool is_l_child()&
			{
				if (m_ptr == nullptr || this->is_root())
				{
					return false;
				}
				return m_ptr == (m_ptr->p_ptr->l_ptr);
			}
			bool is_r_child()&
			{
				if (m_ptr == nullptr || this->is_root())
				{
					return false;
				}
				return m_ptr == (m_ptr->p_ptr->r_ptr);
			}
			Iterator& go_left()&
			{
				m_ptr = m_ptr->l_ptr;
				return *this;
			}
			Iterator& go_right()&
			{
				m_ptr = m_ptr->r_ptr;
				return *this;
			}
			Iterator& go_to_parent()&
			{
				m_ptr = m_ptr->p_ptr;
				return *this;
			}
			Iterator& operator++() &
			{
				if (this->m_ptr == nullptr)
				{
					throw std::logic_error("Call operator++ past the end of tree!!!");
				}
				Iterator temp = *this;
				if (temp.m_ptr->r_ptr)
				{
					temp.go_right();
					while (temp.m_ptr->l_ptr)
					{
						temp.go_left();
					}
				}
				else
				{
					while(temp.is_r_child())
					{
						temp.go_to_parent();
					}
					temp.go_to_parent();
				}
				this->m_ptr = temp.m_ptr;
				return *this;
			}
			Iterator& operator--() &
			{
				if (this->m_ptr == nullptr)
				{
					throw std::logic_error("Call operator-- past the start of tree!!!");
				}
				Iterator temp = *this;
				if (temp.m_ptr->l_ptr)
				{
					temp.go_left();
					while (temp.m_ptr->r_ptr)
					{
						temp.go_right();
					}
				}
				else
				{
					while (temp.is_l_child())
					{
						temp.go_to_parent();
					}
					temp.go_to_parent();
				}
				this->m_ptr = temp.m_ptr;
				return *this;
			}
			size_type get_level() &
			{
				size_type level{ 0 };
				AVL_Hook* temp = this->m_ptr;
				while (temp != nullptr)
				{
					temp = temp->p_ptr;
					++level;
				}
				return level;
			}
			bool is_leaf()const&
			{
				if (this->m_ptr->l_ptr || this->m_ptr->r_ptr)
				{
					return false;
				}
				return true;
			}
			bool is_not_leaf()const&
			{
				return !is_leaf();
			}
		};
		[[nodiscard]] constexpr Iterator root() const&
		{
			return Iterator(&(m_root_ptr->n_hook));
		}
		[[nodiscard]] constexpr Iterator begin() const&
		{
			AVL_Hook* temp = &(m_root_ptr->n_hook);
			while (temp->l_ptr)
			{
				temp = temp->l_ptr;
			}
			return Iterator(temp);
		}
		[[nodiscard]] constexpr Iterator rbegin() const&
		{
			AVL_Hook* temp = &(m_root_ptr->n_hook);
			while (temp->r_ptr)
			{
				temp = temp->r_ptr;
			}
			return Iterator(temp);
		}
		[[nodiscard]] constexpr Iterator end() const&
		{
			return Iterator(nullptr);
		}
		[[nodiscard]] constexpr Iterator rend() const&
		{
			return Iterator(nullptr);
		}
		void ll_rotation(Iterator& node)
		{
			AVL_Hook* p_hook = node.m_ptr->p_ptr;
			AVL_Hook* l_hook = node.m_ptr->l_ptr;
			AVL_Hook* l_r_hook = l_hook->r_ptr;
			if (!p_hook)//check if this a root
			{
				m_root_ptr = Iterator(l_hook).get_node_ptr();//change root
			}
			else if (node.is_l_child())//check if node is left or right child
			{
				p_hook->l_ptr = l_hook;//update parent node left child
			}
			else
			{
				p_hook->r_ptr = l_hook;//update parent node right child
			}
			l_hook->p_ptr = p_hook;//update left hook parent to node parent
			l_hook->r_ptr = node.m_ptr;//update left hook right child to node
			node.m_ptr->l_ptr = l_r_hook;//update left child of node to right child of left hook
			node.m_ptr->p_ptr = l_hook;//update node parent
			if(l_r_hook)
			{
				l_r_hook->p_ptr = node.m_ptr;//update right child of left hook parent to point to node
			}
			node.update_height();//update node height
			l_hook->update_height();//update left node heighy
		}
		void lr_rotation(Iterator& node)
		{
			AVL_Hook* p_hook = node.m_ptr->p_ptr;
			AVL_Hook* l_hook = node.m_ptr->l_ptr;
			AVL_Hook* l_r_hook = l_hook->r_ptr;
			AVL_Hook* l_r_hook_left = l_r_hook->l_ptr;
			AVL_Hook* l_r_hook_right = l_r_hook->r_ptr;
			if (!p_hook)//check if this is root
			{
				m_root_ptr = Iterator(l_r_hook).get_node_ptr();//change root
			}
			else if (node.is_l_child())//check if node is from left or right pointer
			{
				p_hook->l_ptr = l_r_hook;//assign the parent pointer left pointer to l_r_hook
			}
			else
			{
				p_hook->r_ptr = l_r_hook;//assign the parent pointer right pointer to l_r_hook
			}
			l_r_hook->p_ptr = p_hook;//assign new l_r parent
			l_r_hook->l_ptr = l_hook;//assign l_r left child
			l_r_hook->r_ptr = node.m_ptr;//assign l_r right child
			l_hook->p_ptr = l_r_hook;//assign l_hook parent
			l_hook->r_ptr = l_r_hook_left;//assign l_hook right child to old l_r left child;
			node.m_ptr->p_ptr = l_r_hook;//assign node parent to l_r
			node.m_ptr->l_ptr = l_r_hook_right;//assign node left child to old l_r right chidl;
			if (l_r_hook_left)
			{
				l_r_hook_left->p_ptr = l_hook;//assign old l_r left child new parent
			}
			if (l_r_hook_right)
			{
				l_r_hook_right->p_ptr = node.m_ptr;//assign old l_r right child new parent
			}
			node.update_height();//update height of moved node cause it receive old l_r right
			l_hook->update_height();//update height of left node cause it just receive old l_r left
			l_r_hook->update_height();//update height of l_r cause it lost it old left and right child
		}
		void rr_rotation(Iterator& node)
		{
			AVL_Hook* p_hook = node.m_ptr->p_ptr;
			AVL_Hook* r_hook = node.m_ptr->r_ptr;
			AVL_Hook* r_l_hook = r_hook->l_ptr;
			if (!p_hook)
			{
				m_root_ptr = Iterator(r_hook).get_node_ptr();
			}
			else if (node.is_r_child())
			{
				p_hook->r_ptr = r_hook;
			}
			else
			{
				p_hook->l_ptr = r_hook;
			}
			r_hook->p_ptr = p_hook;
			r_hook->l_ptr = node.m_ptr;
			node.m_ptr->p_ptr = r_hook;
			node.m_ptr->r_ptr = r_l_hook;
			if (r_l_hook)
			{
				r_l_hook->p_ptr = node.m_ptr;
			}
			node.update_height();
			r_hook->update_height();
		}
		void rl_rotation(Iterator& node)
		{
			AVL_Hook* p_hook = node.m_ptr->p_ptr;
			AVL_Hook* r_hook = node.m_ptr->r_ptr;
			AVL_Hook* r_l_hook = r_hook->l_ptr;
			AVL_Hook* r_l_hook_left = r_l_hook->l_ptr;
			AVL_Hook* r_l_hook_right = r_l_hook->r_ptr;
			if (!p_hook)
			{
				m_root_ptr = Iterator(r_l_hook).get_node_ptr();
			}
			else if (node.is_r_child())
			{
				p_hook->r_ptr = r_l_hook;
			}
			else
			{
				p_hook->l_ptr = r_l_hook;
			}
			r_l_hook->p_ptr = p_hook;
			r_l_hook->l_ptr = node.m_ptr;
			r_l_hook->r_ptr = r_hook;
			node.m_ptr->p_ptr = r_l_hook;
			node.m_ptr->r_ptr = r_l_hook_left;
			r_hook->p_ptr = r_l_hook;
			r_hook->l_ptr = r_l_hook_right;
			if (r_l_hook_left)
			{
				r_l_hook_left->p_ptr = node.m_ptr;
			}
			if (r_l_hook_right)
			{
				r_l_hook_right->p_ptr = r_hook;
			}
			node.update_height();
			r_hook->update_height();
			r_l_hook->update_height();
		}
		void rotate(Iterator& node)
		{
			int b_f = node.get_balance_factor();
			switch (b_f)
			{
			case(2):
			{
				Iterator l_node = node.m_ptr->l_ptr;
				if (l_node.get_balance_factor() >= 0)
				{
					ll_rotation(node);
				}
				else
				{
					lr_rotation(node);
				}
				break;
			}
			case(-2):
			{
				Iterator r_node = node.m_ptr->r_ptr;
				if (r_node.get_balance_factor() <= 0)
				{
					rr_rotation(node);
				}
				else
				{
					rl_rotation(node);
				}
				break;
			}
			default:
				break;
			}
		}
		void insert(value_type key)
		{
			Iterator curr_node = this->root();
			if (m_size == 0)
			{
				m_root_ptr = new AVL_Node<value_type>({ std::move(key), nullptr });
				m_size = 1;
				return;
			}
			bool inserted(false);
			while (!inserted)
			{
				if (*curr_node == key)
				{
					throw std::logic_error("List passed in contain duplicates. AVL is strictly no dups!!!");
				}
				if (key < *curr_node)
				{
					if (curr_node.m_ptr->l_ptr)
					{
						curr_node.go_left();
					}
					else
					{
						AVL_Node<value_type>* new_node = new AVL_Node<value_type>({std::move(key), curr_node.m_ptr});
						curr_node.m_ptr->l_ptr = &(new_node->n_hook);
						inserted = true;
					}
				}
				else
				{
					if (curr_node.m_ptr->r_ptr)
					{
						curr_node.go_right();
					}
					else
					{
						AVL_Node<value_type>* new_node = new AVL_Node<value_type>({ std::move(key), curr_node.m_ptr });
						curr_node.m_ptr->r_ptr = &(new_node->n_hook);
						inserted = true;
					}
				}
			}
			while (curr_node.m_ptr && curr_node.get_balance_factor() != 0)
			{
				curr_node.update_height();
				rotate(curr_node);
				curr_node.m_ptr = curr_node.m_ptr->p_ptr;
			}
			++m_size;
		}
		// will return iterator = this.end() if value not found 
		Iterator find(const value_type& key)&
		{
			Iterator curr_node = this->root();
			while (curr_node.m_ptr)
			{
				if (key == *curr_node)
				{
					break;
				}
				(key < *curr_node ? curr_node.go_left() : curr_node.go_right());
			}
			return curr_node;
		}

		void remove(value_type key)
		{
			Iterator curr_node = find(key);
			if (curr_node.m_ptr)
			{
				curr_node.is_leaf() ? this->remove_a_leaf_node(curr_node) : this->remove_non_leaf_node(curr_node);
				while (curr_node.m_ptr)
				{
					curr_node.update_height();
					rotate(curr_node);
					curr_node.go_to_parent();
				}
				--m_size;
			}
		}

		AVL_Tree() : m_root_ptr{ nullptr }, m_size{0} {}
		AVL_Tree(value_type data) : m_root_ptr{ new AVL_Node<value_type>({std::move(data),nullptr}) }, m_size{ 1 } {}
		template<typename Iter>
		AVL_Tree(Iter begin, Iter end, size_type size) : AVL_Tree(std::move(*begin))
		{
			++begin;
			while (begin != end)
			{
				insert(std::move(*begin));
				++begin;
			}
			m_size = size;
		}
		AVL_Tree(std::vector<value_type> vect) : AVL_Tree(vect.begin(), vect.end(), vect.size()) {}
		AVL_Tree(std::initializer_list<value_type> list) : AVL_Tree(list.begin(), list.end(), list.size()) {}
		template <size_type s>
		AVL_Tree(std::array<value_type, s> arr) : AVL_Tree(arr.begin(), arr.end(), s) {}
		friend std::ostream& operator<< (std::ostream& out, AVL_Tree<value_type>& tree)
		{
			if (tree.m_size <= 0)
			{
				out << "Tree is empty!!!";
				return out;
			}
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
		~AVL_Tree()
		{
			this->destroy_all_node();
		}
	private:
		AVL_Node<value_type>* m_root_ptr;
		size_type m_size;
		void destroy_all_node()
		{
			Iterator curr_node{ this->root() };
			if (m_size == 0) return;
			//Get the 1st node for traversing postorder
			while (!curr_node.is_leaf())
			{
				while (curr_node.m_ptr->l_ptr)
				{
					curr_node.go_left();
				}
				if (curr_node.m_ptr->r_ptr)
				{
					curr_node.go_right();
				}
			}
			while (curr_node.m_ptr)
			{
				Iterator temp = curr_node;
				if (curr_node.m_ptr->p_ptr && curr_node.is_l_child())
				{
					curr_node.go_to_parent();
					while (curr_node.m_ptr->r_ptr)
					{
						curr_node.go_right();
						while (curr_node.m_ptr->l_ptr)
						{
							curr_node.go_left();
						}
					}
				}
				else
				{
					curr_node.go_to_parent();
				}
				delete temp.get_node_ptr();
			}
			m_size = 0;
		}
		void remove_a_leaf_node(Iterator& node)
		{
			if (node.is_root())
			{
				m_root_ptr = nullptr;
				delete node.get_node_ptr();
				node.m_ptr = nullptr;
			}
			else
			{
				Iterator temp = node;
				node.go_to_parent();
				(temp.is_l_child() ? node.m_ptr->l_ptr = nullptr : node.m_ptr->r_ptr = nullptr);
				delete temp.get_node_ptr();
			}
		}
		void remove_1_child_node(Iterator& node)
		{
			if (node.is_root())
			{
				node.m_ptr->l_ptr ? node.go_left() : node.go_right();
				node.m_ptr->p_ptr = nullptr;
				delete m_root_ptr;
				m_root_ptr = node.get_node_ptr();
			}
			else
			{
				AVL_Hook* parent = node.m_ptr->p_ptr;
				AVL_Hook* child = (node.m_ptr->l_ptr ? node.m_ptr->l_ptr : node.m_ptr->r_ptr);
				(node.is_l_child() ? parent->l_ptr = child : parent->r_ptr = child);
				child->p_ptr = parent;
				delete node.get_node_ptr();
				node.m_ptr = parent;
			}
		}
		void remove_2_child_node(Iterator& node)
		{
			AVL_Hook* n_parent = node.m_ptr->p_ptr;
			AVL_Hook* n_left = node.m_ptr->l_ptr;
			AVL_Hook* n_right = node.m_ptr->r_ptr;
			Iterator postdecessor = node;
			++postdecessor;
			if (postdecessor.m_ptr->p_ptr == node.m_ptr)
			{
				postdecessor.m_ptr->p_ptr = n_parent;
				if (n_parent)
				{
					node.is_l_child() ? n_parent->l_ptr = postdecessor.m_ptr : n_parent->r_ptr = postdecessor.m_ptr;
				}
				else
				{
					m_root_ptr = postdecessor.get_node_ptr();
				}
				postdecessor.m_ptr->l_ptr = n_left;
				n_left->p_ptr = postdecessor.m_ptr;
				delete node.get_node_ptr();
				node.m_ptr = postdecessor.m_ptr;
			}
			else
			{
				AVL_Hook* post_parent = postdecessor.m_ptr->p_ptr;
				AVL_Hook* post_right = postdecessor.m_ptr->r_ptr;
				postdecessor.is_l_child() ? post_parent->l_ptr = post_right : post_parent->r_ptr = post_right;
				if (post_right)
				{
					post_right->p_ptr = post_parent;
				}
				postdecessor.m_ptr->p_ptr = n_parent;
				if (n_parent)
				{
					node.is_l_child() ? n_parent->l_ptr = postdecessor.m_ptr : n_parent->r_ptr = postdecessor.m_ptr;
				}
				else
				{
					m_root_ptr = postdecessor.get_node_ptr();
				}
				postdecessor.m_ptr->l_ptr = n_left;
				n_left->p_ptr = postdecessor.m_ptr;
				postdecessor.m_ptr->r_ptr = n_right;
				n_right->p_ptr = postdecessor.m_ptr;
				delete node.get_node_ptr();
				node.m_ptr = post_parent;
			}
		}
		void remove_non_leaf_node(Iterator& node)
		{
			(node.m_ptr->l_ptr && node.m_ptr->r_ptr ? this->remove_2_child_node(node) : this->remove_1_child_node(node));
		}
	};
}