#pragma once
#include <array>
#include <vector>
#include <utility>
#include <initializer_list>
#include <stack>

namespace custom
{
	template <typename T, size_t ORDER = 4>
	struct B_Node
	{
		using size_type = size_t;
		using value_type = T;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using NODE = B_Node<value_type, ORDER>;

		std::array<value_type, ORDER-1> n_data = {};
		std::array<B_Node<value_type, ORDER>*, ORDER> n_children_ptr = {};
		size_type n_num_data = 0;

		bool is_not_leaf()
		{
			return this->n_children_ptr[0];
		}
		bool is_leaf()
		{
			return !is_not_leaf();
		}
		bool is_full()
		{
			return this->n_num_data >= ORDER-1;
		}
	};

	template <typename T, size_t ORDER = 4>
	class B_Tree
	{
		static_assert(ORDER >= 4 && ORDER < 1025, "Order 3 or lower is not allowed!!!");
	public:
		using size_type = size_t;
		using value_type = T;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using NODE = B_Node<value_type, ORDER>;

		struct Iterator
		{
			NODE* i_ptr = nullptr;
			size_type i_arr_index = 0;
		};
		struct split_info //providing the split function the ability to push median up to parent
		{
			NODE* parent_ptr = nullptr; //parent of the node being split
			size_type index_of_the_full_node = 0;//the index of the node needed to split
		};
		void insert(value_type key)
		{
			NODE* parent_node = nullptr;
			NODE* curr_node = m_root_ptr;
			size_type index{ 0 };
			if (curr_node->is_full())//case for spliting full root and a leaf
			{
				split_info info;
				size_type median_index = this->split(info);
				parent_node = m_root_ptr;
				curr_node = (m_root_ptr->n_data[0] > key ? m_root_ptr->n_children_ptr[0] : m_root_ptr->n_children_ptr[1]);
			}
			while (true)//find node to insert and split full node if encounter one.
			{
				if (curr_node->is_leaf() && !curr_node->is_full())
				{
					break;
				}
				if (curr_node->is_full())
				{
					split_info info;
					info.index_of_the_full_node = index;
					info.parent_ptr = parent_node;
					size_type median_index = this->split(info);
					curr_node = (parent_node->n_data[median_index] > key ? parent_node->n_children_ptr[median_index] : parent_node->n_children_ptr[median_index + 1]);
				}
				else
				{
					index = 0;
					while (index < curr_node->n_num_data && key > curr_node->n_data[index])
					{
						++index;
					}
					parent_node = curr_node;
					curr_node = curr_node->n_children_ptr[index];
				}
			}
			insert_into_sorted_array(key, curr_node);
			++this->m_num_data;
		}

		B_Tree() : m_root_ptr{ new B_Node<value_type, ORDER>({}) }, m_num_data{ 0 }{}
		B_Tree(value_type data) : m_root_ptr{ new B_Node<value_type, ORDER>({}) }, m_num_data{ 1 }
		{
			m_root_ptr->n_data[0] = data;
			m_root_ptr->n_num_data = 1;
		}
		template <typename Iter>
		B_Tree(Iter begin, Iter end, size_type size) : B_Tree(*begin)
		{
			++begin;
			while (begin != end)
			{
				insert(*begin);
				++begin;
			}
			m_num_data = size;
		}
		B_Tree(std::vector<value_type> vect) : B_Tree(vect.begin(), vect.end(), vect.size()){}

		B_Tree(std::initializer_list<value_type> list) : B_Tree(list.begin(), list.end(), list.size()) {}

		template <size_type s>
		B_Tree(std::array<value_type, s> arr) : B_Tree(arr.begin(), arr.end(), s){}

		~B_Tree()
		{
			destroy_all_nodes(m_root_ptr);
		}

		void print_inorder()
		{
			if (m_num_data == 0)
			{
				std::cout << "Tree is empty!!!";
				return;
			}
			inorder_recursive(m_root_ptr);
		}
		void remove(const value_type& key)
		{
			remove_private(key, m_root_ptr);
		}
		void remove_private(const value_type& key, NODE* curr_node = m_root_ptr)
		{
			if (m_root_ptr == nullptr) return;
			std::stack<NODE*> parent_stack;
			size_type index = 0;
			while (true)
			{
				index = 0;
				while (index < curr_node->n_num_data && key > curr_node->n_data[index])
				{
					++index;
				}
				if (index < curr_node->n_num_data && key == curr_node->n_data[index])//found node and index to delete
				{
					break;
				}
				if (curr_node->is_leaf())//key doesn't exist return
				{
					return;
				}
				parent_stack.push(curr_node);
				curr_node = curr_node->n_children_ptr[index];
			}
			if (m_num_data == 1)
			{
				delete m_root_ptr;
				m_root_ptr = nullptr;
			}
			else if (curr_node->is_leaf())
			{
				remove_from_leaf(key, index, curr_node, parent_stack);
			}
			else
			{
				//find successor
				NODE* temp = curr_node->n_children_ptr[index + 1];
				if (temp->is_not_leaf())
				{
					parent_stack.push(curr_node);
					while (temp->is_not_leaf())
					{
						parent_stack.push(temp);
						temp = temp->n_children_ptr[0];
					}
				}
				size_type successor_index = temp->n_num_data - 1;
				value_type successor = temp->n_data[successor_index];
				curr_node->n_data[index] = successor;
				remove_from_leaf(successor, successor_index, temp, parent_stack);
			}
			--m_num_data;
		}
	private:
		B_Node<value_type, ORDER>* m_root_ptr;
		size_type m_num_data;

		size_type split(split_info info) //I chose right bias when ORDER is even because it will always push the highest possible median value up the parent
		{
			if (info.parent_ptr == nullptr) //splitting root case
			{
				NODE* left_node = m_root_ptr;
				NODE* right_node = new NODE({});
				NODE* new_root_node = new NODE({});
				size_type median_index = left_node->n_num_data / 2;
				size_type temp = 0;
				//Tranfer(copy) data from left node to root and right node
				new_root_node->n_data[0] = left_node->n_data[median_index];
				std::copy(left_node->n_data.begin() + median_index + 1, left_node->n_data.end(), right_node->n_data.begin());
				//Update number of data for each node
				if (left_node->n_num_data % 2)
				{
					left_node->n_num_data = left_node->n_num_data - median_index - 1;
					right_node->n_num_data = left_node->n_num_data;
				}
				else
				{
					left_node->n_num_data = left_node->n_num_data - median_index;
					right_node->n_num_data = left_node->n_num_data - 1;
				}
				new_root_node->n_num_data = 1;
				//Transfer pointer.
				if (left_node->is_not_leaf())//check if the node is leaf or not before transfering the pointer
				{
					auto right_child_iter = right_node->n_children_ptr.begin();
					for (size_type i{ median_index + 1 }; i < ORDER; ++i)
					{
						*right_child_iter = left_node->n_children_ptr[i];
						++right_child_iter;
						left_node->n_children_ptr[i] = nullptr;
					}
				}
				//Assign the new root ptr with the 2 child and update the class root
				new_root_node->n_children_ptr[0] = left_node;
				new_root_node->n_children_ptr[1] = right_node;
				m_root_ptr = new_root_node;
				return temp;
			}
			else // Non root case
			{
				NODE* left_node = info.parent_ptr->n_children_ptr[info.index_of_the_full_node];
				NODE* right_node = new NODE({});
				size_type median_index = left_node->n_num_data / 2;
				//Move median data into parent
				size_type temp = insert_into_sorted_array(left_node->n_data[median_index], info.parent_ptr);
				//Transfer(copy) data from left node to right node
				std::copy(left_node->n_data.begin() + median_index + 1, left_node->n_data.end(), right_node->n_data.begin());
				//Update number of data for each node
				if (left_node->n_num_data % 2)
				{
					left_node->n_num_data = left_node->n_num_data - median_index - 1;
					right_node->n_num_data = left_node->n_num_data;
				}
				else
				{
					left_node->n_num_data = left_node->n_num_data - median_index;
					right_node->n_num_data = left_node->n_num_data - 1;
				}
				//Transfer pointer
				if (left_node->is_not_leaf())//check if it a leaf or not before transfering the pointer
				{
					auto right_child_iter = right_node->n_children_ptr.begin();
					for (size_type i{ median_index + 1 }; i < ORDER; ++i)
					{
						*right_child_iter = left_node->n_children_ptr[i];
						++right_child_iter;
						left_node->n_children_ptr[i] = nullptr;
					}
				}
				//Assign the parent children pointer with newly created node
				for (size_type i{ ORDER - 1 }; i > temp + 1; --i)
				{
					info.parent_ptr->n_children_ptr[i] = info.parent_ptr->n_children_ptr[i - 1];
				}
				info.parent_ptr->n_children_ptr[temp + 1] = right_node;
				return temp;
			}
		}
		size_type insert_into_sorted_array(value_type key, NODE* node)
		{
			size_type index{ 0 };
			while (index < node->n_num_data && key > node->n_data[index])
			{
				++index;
			}
			for (size_type i{ node->n_num_data }; i > index; --i)
			{
				node->n_data[i] = node->n_data[i - 1];
			}
			node->n_data[index] = key;
			++node->n_num_data;
			return index;
		}
		void inorder_recursive(NODE* curr)
		{
			if (curr == nullptr) return;

			for (size_type i = 0; i < curr->n_num_data; ++i)
			{
				// Visit the child to the left of the current key
				if (curr->is_not_leaf())
				{
					inorder_recursive(curr->n_children_ptr[i]);
				}

				// Visit the key itself
				std::cout << curr->n_data[i] << " ";
			}

			// There is always one more child than there are keys.
			// After the loop, visit the right-most child.
			if (curr->is_not_leaf())
			{
				inorder_recursive(curr->n_children_ptr[curr->n_num_data]);
			}
		}
		void remove_from_leaf(const value_type& key, size_type index_of_the_key, NODE*& curr_node, std::stack<NODE*>& parent_stack)
		{
			if (is_underflow(curr_node))
			{
				size_type node_index_in_parent = get_node_index_in_parent(curr_node, parent_stack.top());
				int borrow_info = is_borrowable(node_index_in_parent, parent_stack.top());
				if (borrow_info == 1)
				{
					NODE* right_sib = parent_stack.top()->n_children_ptr[node_index_in_parent + 1];
					borrow_right(node_index_in_parent, curr_node, parent_stack.top(), right_sib);//now curr_node data num is > 1 so we can safely delete the key
					curr_node->n_data[0] = curr_node->n_data[1];//since the key to delete is left most after borrow 1 key from right we just assign the key to its next key and decreas num data
					--curr_node->n_num_data;
				}
				else if (borrow_info == -1)
				{
					NODE* left_sib = parent_stack.top()->n_children_ptr[node_index_in_parent - 1];
					borrow_left(node_index_in_parent, curr_node, parent_stack.top(), left_sib);//now curr_node data num is > 1 so we can safely delete the key
					--curr_node->n_num_data; //since the delete node will be the last node in the array we will just need to decrease the number of data
				}
				else//merge since not borrowable and underflow
				{
					NODE* parent_node = parent_stack.top();
					if (!parent_stack.empty() && is_underflow(parent_node))
					{
						parent_stack.pop();
						processing_parent(parent_node, parent_stack);
					}
					merge(node_index_in_parent ,curr_node, parent_node);
					size_type index{ 0 };
					while (key > curr_node->n_data[index])
					{
						++index;
					}
					for(size_type i{ index }; i < curr_node->n_num_data - 1; ++i)
					{
						curr_node->n_data[i] = curr_node->n_data[i + 1];
					}
					--curr_node->n_num_data;
				}
			}
			else
			{
				for (size_type i{ index_of_the_key }; i < curr_node->n_num_data - 1; ++i)
				{
					curr_node->n_data[i] = curr_node->n_data[i + 1];
				}
				--curr_node->n_num_data;
			}
		}
		bool is_underflow(NODE* node)
		{
			return node->n_num_data < 2;
		}
		size_type get_node_index_in_parent(NODE* curr_node, NODE* parent_node)
		{
			size_type index{ 0 };
			while (curr_node != parent_node->n_children_ptr[index])
			{
				++index;
			}
			return index;
		}
		int is_borrowable(size_type curr_node_index, NODE* parent_node)//return -1 if borrowable from left, 0 if not borrowalbe, 1 if borrowable on right
		{
			if (curr_node_index > 0 && parent_node->n_children_ptr[curr_node_index - 1] && parent_node->n_children_ptr[curr_node_index - 1]->n_num_data > 1)
			{
				return -1;
			}
			else if (curr_node_index < parent_node->n_num_data && parent_node->n_children_ptr[curr_node_index + 1] && parent_node->n_children_ptr[curr_node_index + 1]->n_num_data > 1)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		void borrow_left(size_type curr_node_index, NODE* curr_node, NODE* parent_node, NODE* left_sib)
		{
			value_type key_to_parent = std::move(left_sib->n_data[left_sib->n_num_data - 1]);
			value_type key_to_curr = std::move(parent_node->n_data[curr_node_index - 1]);
			if (left_sib->is_not_leaf())
			{
				NODE* ptr_to_curr = left_sib->n_children_ptr[left_sib->n_num_data];
				for (size_type i{ curr_node->n_num_data + 1}; i > 0; --i)
				{
					curr_node->n_children_ptr[i] = curr_node->n_children_ptr[i - 1];
				}
				curr_node->n_children_ptr[0] = ptr_to_curr;
			}
			for (size_type i{ curr_node->n_num_data }; i > 0; --i)
			{
				curr_node->n_data[i] = curr_node->n_data[i - 1];
			}
			curr_node->n_data[0] = std::move(key_to_curr);
			parent_node->n_data[curr_node_index - 1] = std::move(key_to_parent);
			--left_sib->n_num_data;
			++curr_node->n_num_data;
		}
		/*
		Procedure to borrow left:
		1. Get left_sib right most key(key to parent)
		2. If internal node, get left_sib right most ptr (ptr to transfer)
		3. Get parent key at index curr_node_index - 1(key to curr node)
		4. Shift all the element in curr_node to right by 1 to make space
		5. If interal node, shift all element in curr_node child ptr by 1 to make space
		6. Place key get from parent to the left most in curr_node arr
		7. If left_sib is internal node, place the child ptr to the left most of the curr_node child_ptr
		8. Place key get from left_sib to parent at index curr_node_index - 1
		9. Increment curr_node data num and decrement left_sib node data num
		*/
		void borrow_right(size_type curr_node_index, NODE* curr_node, NODE* parent_node, NODE* right_sib)
		{
			value_type key_to_parent = std::move(right_sib->n_data[0]);
			value_type key_to_curr = std::move(parent_node->n_data[curr_node_index]);
			curr_node->n_data[curr_node->n_num_data] = std::move(key_to_curr);
			parent_node->n_data[curr_node_index] = std::move(key_to_parent);
			for (size_type i{ 0 }; i < right_sib->n_num_data - 1; ++i)
			{
				right_sib->n_data[i] = right_sib->n_data[i + 1];
			}
			if (right_sib->is_not_leaf())
			{
				NODE* ptr_to_curr = right_sib->n_children_ptr[0];
				curr_node->n_children_ptr[curr_node->n_num_data + 1] = ptr_to_curr;
				for (size_type i{ 0 }; i < right_sib->n_num_data; ++i)
				{
					right_sib->n_children_ptr[i] = right_sib->n_children_ptr[i + 1];
				}
			}
			--right_sib->n_num_data;
			++curr_node->n_num_data;
		}
		/*
		Procedure to borrow right:
		1. Get right_sib left most key (key to parent)
		2. If interanal node, get right_sib left most child ptr (ptr to transfer)
		3. Get parent key at index curr_node_index (key to curr_node)
		4. Place key get from parent to the right most in the array.
		5. If right_sib is internal node, place the child ptr to the right most of the curr_node child_ptr
		6. Move key from right_sib to parent_node at curr_node_index.
		7. Shift all keys in right_sib left by one.
		8. If right_sib is internal node, shift all child ptr left by one.
		9. Increament curr_node data num and decrement right_sib node data num.
		*/
		void merge(size_type curr_node_index, NODE*& curr_node, NODE* parent_node)
		{
			NODE* right_sib = nullptr;
			if (curr_node_index == parent_node->n_num_data)
			{
				right_sib = parent_node->n_children_ptr[curr_node_index - 1];
				std::swap(curr_node, right_sib);
				--curr_node_index;
			}
			else
			{
				right_sib = parent_node->n_children_ptr[curr_node_index + 1];
			}
			value_type first_key_to_curr = std::move(parent_node->n_data[curr_node_index]);
			value_type second_key_to_curr = std::move(right_sib->n_data[0]);
			curr_node->n_data[1] = std::move(first_key_to_curr);
			curr_node->n_data[2] = std::move(second_key_to_curr);
			for (size_type i{ curr_node_index }; i < parent_node->n_num_data-1; ++i)
			{
				parent_node->n_data[i] = parent_node->n_data[i + 1];
			}
			for (size_type i{ curr_node_index + 1 }; i < parent_node->n_num_data; ++i)
			{
				parent_node->n_children_ptr[i] = parent_node->n_children_ptr[i + 1];
			}
			--parent_node->n_num_data;
			curr_node->n_num_data += 2;
			if (curr_node->is_not_leaf())
			{
				curr_node->n_children_ptr[2] = right_sib->n_children_ptr[0];
				curr_node->n_children_ptr[3] = right_sib->n_children_ptr[1];
			}
			delete right_sib;
			if (parent_node == m_root_ptr && parent_node->n_num_data == 0)
			{
				m_root_ptr = curr_node;
				delete parent_node;
			}
		}
		/*
		Procedure to merge_right
		1. Parent key to move down is at curr_node_index.
		2. Right_sib key to move is the only node there.
		3. Move the parent key into curr_node 1st then the right sib only key at the end.
		4. Shift all element at parent to the left by 1 node at curr_node_index.
		5. If internal node, transfer 2 child ptr from right_sib to curr_node right most.
		6. Delete the right_sib.
		7. Shift all element in parent node child ptr at curr_node_index + 1 to the left by 1.
		8. Increase n_data_num of curr_node by 2, decrease n_data_num of parent_node by 1.

		Procedure to merge_left
		1. Shift curr_node only key to right 2 times.
		2. Parent key tp move down is at curr_node_index - 1.
		3. Left_sib key to move is the only node there.
		4. Move left_sib key to 1st slot and parent_key to 2nd slot
		5. Shift all parent key at curr_node_index - 1 left by 1.
		6. Shift all parent child_ptr at curr_node_index-1 left by 1.
		7. Shift all curr_node child_ptr by 2.
		8. If internal node, insert 1st left_sib child at 0, 2nd left_sib child at 1.
		9. Increase n_data_num of curr_node by 2, decrease n_data num of parent node by 1.
		10. Delete the left_sib.

		But there's a trick, if we decrease the curr_node index and swap curr_node with left_sib, we can use merge right to get the same result.
		*/
		void processing_parent(NODE*& parent_node, std::stack<NODE*>& parent_stack)
		{
			if (parent_stack.empty()) return;

			size_type node_index = get_node_index_in_parent(parent_node, parent_stack.top());
			int borrow_info = is_borrowable(node_index, parent_stack.top());

			if (borrow_info == 1)
			{
				NODE* right_sib = parent_stack.top()->n_children_ptr[node_index + 1];
				borrow_right(node_index, parent_node, parent_stack.top(), right_sib);
			}
			else if (borrow_info == -1)
			{
				NODE* left_sib = parent_stack.top()->n_children_ptr[node_index - 1];
				borrow_left(node_index, parent_node, parent_stack.top(), left_sib);
			}
			else
			{
				NODE* grand_parent = parent_stack.top();
				merge(node_index, parent_node, grand_parent);
				if (is_underflow(grand_parent))
				{
					parent_stack.pop();
					processing_parent(grand_parent, parent_stack);
				}
			}
		}
		void destroy_all_nodes(NODE* node)
		{
			if (node == nullptr) return;
			if (node->is_not_leaf())
			{
				for (size_type i = 0; i <= node->n_num_data; ++i)
				{
					destroy_all_nodes(node->n_children_ptr[i]);
				}
			}
			delete node;
		}
	};
}