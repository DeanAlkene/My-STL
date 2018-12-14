#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cstring>
#include <cstddef>
#include <cmath>
namespace sjtu
{
const size_t MAX_BLOCK_SIZE = 2000;

template <class T>
class deque
{
  public:
	class const_iterator;
	struct Node
	{
		T *data[MAX_BLOCK_SIZE];
		int blockSize;
		Node *prev;
		Node *next;
		Node() : blockSize(0), prev(NULL), next(NULL)
		{
			memset(data, 0, sizeof(data));
		}
		Node(Node *p, Node *n) : blockSize(0), prev(p), next(n)
		{
			memset(data, 0, sizeof(data));
		}
		~Node()
		{
			for (int i = 0; i < MAX_BLOCK_SIZE; ++i)
			{
				if (data[i])
					delete data[i];
				data[i] = NULL;
			}
			blockSize = 0;
			prev = NULL;
			next = NULL;
		}
	};
	class iterator
	{
	  public:
		int index;
		Node *node;
		deque<T> *container;
		iterator() : index(0), node(NULL), container(NULL) {}
		iterator(int x, Node *n, deque<T> *id) : index(x), node(n), container(id) {}
		iterator(const iterator &rhs) : index(rhs.index), node(rhs.node), container(rhs.container) {}

	  public:
		iterator operator+(const int &n) const
		{
			iterator tmp = *this;
			return tmp += n;
		}
		iterator operator-(const int &n) const
		{
			iterator tmp = *this;
			return tmp -= n;
		}
		int operator-(const iterator &rhs) const
		{
			if (container == rhs.container)
			{
				Node *p_this = container->getHead()->next, *p_rhs = rhs.container->getHead()->next;
				ptrdiff_t offset_this = 0;
				ptrdiff_t offset_rhs = 0;
				while (p_this != node)
				{
					offset_this += p_this->blockSize;
					p_this = p_this->next;
				}
				offset_this += index;
				while (p_rhs != rhs.node)
				{
					offset_rhs += p_rhs->blockSize;
					p_rhs = p_rhs->next;
				}
				offset_rhs += rhs.index;
				ptrdiff_t offset = offset_this - offset_rhs;
				return offset;
			}
			else
				throw invalid_iterator();
		}
		iterator operator+=(const int &n)
		{
			if (n < 0)
				return operator-=(-n);
			if (n == 0)
				return *this;
			size_t offset = n;
			if (offset + index < node->blockSize)
				index += offset;
			else
			{
				int a = node->blockSize;
				int b = index;
				offset -= (node->blockSize - index);
				node = node->next;
				while (node->blockSize <= offset && node != container->getTail())
				{
					offset -= node->blockSize;
					node = node->next;
				}
				index = offset;
			}
			return *this;
		}
		iterator operator-=(const int &n)
		{
			if (n < 0)
				return operator+=(-n);
			if (n == 0)
				return *this;
			ptrdiff_t offset = n;
			if (offset <= index)
				index -= offset;
			else
			{
				offset -= (index + 1);
				node = node->prev;
				while (node->blockSize <= offset && node != container->getHead())
				{
					offset -= node->blockSize;
					node = node->prev;
				}
				index = node->blockSize - offset - 1;
			}
			return *this;
		}

		iterator operator++(int)
		{
			iterator tmp = *this;
			++*this;
			return tmp;
		}

		iterator &operator++()
		{
			if (*this != container->end())
			{
				if (index + 1 <= node->blockSize - 1)
					++index;
				else
				{
					node = node->next;
					index = 0;
				}
				return *this;
			}
			else
				throw invalid_iterator();
		}

		iterator operator--(int)
		{
			iterator tmp = *this;
			--*this;
			return tmp;
		}

		iterator &operator--()
		{
			if (*this != container->begin())
			{
				if (node == container->getTail())
				{
					node = node->prev;
					index = node->blockSize - 1;
				}
				else
				{
					if (index > 0)
					{
						--index;
					}
					else
					{
						node = node->prev;
						index = node->blockSize - 1;
					}
				}
				return *this;
			}
			else
				throw invalid_iterator();
		}

		T &operator*() const
		{
			if (*this != container->end())
				return *(node->data[index]);
			else
				throw invalid_iterator();
		}

		T *operator->() const noexcept
		{
			return &(operator*());
		}

		bool operator==(const iterator &rhs) const
		{
			if (rhs.container == container && rhs.index == index && rhs.node == node)
				return true;
			return false;
		}
		bool operator==(const const_iterator &rhs) const
		{
			if (rhs.container == container && rhs.index == index && rhs.node == node)
				return true;
			return false;
		}

		bool operator!=(const iterator &rhs) const
		{
			if (rhs.container != container || rhs.index != index || rhs.node != node)
				return true;
			return false;
		}
		bool operator!=(const const_iterator &rhs) const
		{
			if (rhs.container != container || rhs.index != index || rhs.node != node)
				return true;
			return false;
		}
	};
	class const_iterator
	{
	  public:
		int index;
		const Node *node;
		const deque<T> *container;
		const_iterator() : index(0), node(NULL), container(NULL) {}
		const_iterator(int x, const Node *n, const deque<T> *id) : index(x), node(n), container(id) {}
		const_iterator(const iterator &other) : index(other.index), node(other.node), container(other.container) {}
		const_iterator(const const_iterator &other) : index(other.index), node(other.node), container(other.container) {}

	  public:
		const_iterator operator+(const int &n) const
		{
			const_iterator tmp = *this;
			return tmp += n;
		}
		const_iterator operator-(const int &n) const
		{
			const_iterator tmp = *this;
			return tmp -= n;
		}

		int operator-(const const_iterator &rhs) const
		{
			if (container == rhs.container)
			{
				Node *p_this = container->getHead()->next, *p_rhs = rhs.container->getHead()->next;
				ptrdiff_t offset_this = 0;
				ptrdiff_t offset_rhs = 0;
				while (p_this != node)
				{
					offset_this += p_this->blockSize;
					p_this = p_this->next;
				}
				offset_this += index;
				while (p_rhs != rhs.node)
				{
					offset_rhs += p_rhs->blockSize;
					p_rhs = p_rhs->next;
				}
				offset_rhs += rhs.index;
				ptrdiff_t offset = offset_this - offset_rhs;
				return offset;
			}
			else
				throw invalid_iterator();
		}
		const_iterator operator+=(const int &n)
		{
			if (n < 0)
				return operator-=(-n);
			if (n == 0)
				return *this;
			ptrdiff_t offset = n;
			if (offset + index < node->blockSize)
				index += offset;
			else
			{
				offset -= (node->blockSize - index);
				node = node->next;
				while (node->blockSize <= offset && node != container->getTail())
				{
					offset -= node->blockSize;
					node = node->next;
				}
				index = offset;
			}
			return *this;
		}
		const_iterator operator-=(const int &n)
		{
			if (n < 0)
				return operator+=(-n);
			if (n == 0)
				return *this;
			ptrdiff_t offset = n;
			if (offset <= index)
				index -= offset;
			else
			{
				offset -= (index + 1);
				node = node->prev;
				while (node->blockSize <= offset && node != container->getHead())
				{
					offset -= node->blockSize;
					node = node->prev;
				}
				index = node->blockSize - offset - 1;
			}
			return *this;
		}

		const_iterator operator++(int)
		{
			iterator tmp = *this;
			++*this;
			return tmp;
		}

		const_iterator &operator++()
		{
			if (*this != container->cend())
			{
				if (index + 1 <= node->blockSize - 1)
					++index;
				else
				{
					node = node->next;
					index = 0;
				}
				return *this;
			}
			else
				throw invalid_iterator();
		}

		const_iterator operator--(int)
		{
			const_iterator tmp = *this;
			--*this;
			return tmp;
		}

		const_iterator &operator--()
		{
			if (*this != container->cbegin())
			{
				if (node == container->getTail())
				{
					node = node->prev;
					index = node->blockSize - 1;
				}
				else
				{
					if (index > 0)
					{
						--index;
					}
					else
					{
						node = node->prev;
						index = node->blockSize - 1;
					}
				}
				return *this;
			}
			else
				throw invalid_iterator();
		}

		T &operator*() const
		{
			if (*this != container->cend())
				return *(node->data[index]);
			else
				throw invalid_iterator();
		}

		T *operator->() const noexcept
		{
			return &(operator*());
		}

		bool operator==(const iterator &rhs) const
		{
			if (rhs.container == container && rhs.index == index && rhs.node == node)
				return true;
			return false;
		}
		bool operator==(const const_iterator &rhs) const
		{
			if (rhs.container == container && rhs.index == index && rhs.node == node)
				return true;
			return false;
		}

		bool operator!=(const iterator &rhs) const
		{
			if (rhs.container != container || rhs.index != index || rhs.node != node)
				return true;
			return false;
		}
		bool operator!=(const const_iterator &rhs) const
		{
			if (rhs.container != container || rhs.index != index || rhs.node != node)
				return true;
			return false;
		}
	};

	/*....................................................................................*/
  private:
	Node *head;
	Node *tail;
	int curLength;
	void addNode(Node *n, size_t pos, const T &value)
	{
		if (!empty())
		{
			if (n->blockSize < MAX_BLOCK_SIZE)
			{
				for (size_t i = n->blockSize - 1; i >= pos; --i)
				{
					if (i >= n->blockSize) //in case that unsigned long long overflow
						break;
					n->data[i + 1] = n->data[i];
				}
				n->data[pos] = new T(value);
				++n->blockSize;
			}
			else
			{
				if (n->blockSize == MAX_BLOCK_SIZE)
				{
					Node *cur_p = new Node(n, n->next);
					n->next->prev = cur_p;
					n->next = cur_p;
					cur_p->blockSize = n->blockSize / 2;
					for (size_t i = 0; i < cur_p->blockSize; ++i) //Copy MAX_BLOCK_SIZE / 2 elements into the new block
					{
						cur_p->data[i] = n->data[cur_p->blockSize + i];
						n->data[cur_p->blockSize + i] = NULL;
					}
					n->blockSize /= 2;

					if (pos + 1 <= cur_p->blockSize) //Insert the element in the previous node
					{
						for (size_t i = cur_p->blockSize - 1; i >= pos; --i)
						{
							if (i >= cur_p->blockSize)
								break;
							n->data[i + 1] = n->data[i];
						}
						n->data[pos] = new T(value);
						++n->blockSize;
					}
					else //Insert the element in the new node
					{
						n = cur_p;
						for (size_t i = cur_p->blockSize - 1; i >= pos - cur_p->blockSize; --i)
						{
							if (i >= cur_p->blockSize)
								break;
							n->data[i + 1] = n->data[i];
						}
						n->data[pos - cur_p->blockSize] = new T(value);
						++n->blockSize;
					}
				}
			}
		}
		else //the state that the container is empty
		{
			Node *cur_p = new Node(head, tail);
			head->next = cur_p;
			tail->prev = cur_p;
			n = cur_p;
			n->data[0] = new T(value);
			++n->blockSize;
		}
		++curLength;
	}
	void removeNode(Node *n, int pos)
	{
		delete n->data[pos];
		for (int i = pos; i < n->blockSize - 1; ++i)
		{
			n->data[i] = n->data[i + 1];
		}
		n->data[n->blockSize - 1] = NULL;
		--n->blockSize;
		if (n->blockSize == 0)
		{
			n->prev->next = n->next;
			n->next->prev = n->prev;
			delete n;
			n = NULL;
		}
		--curLength;
	}

  public:
	deque()
	{
		head = new Node();
		tail = new Node();
		head->next = tail;
		tail->prev = head;
		head->prev = NULL;
		tail->next = NULL;
		curLength = 0;
	}
	deque(const deque &other)
	{
		head = new Node();
		tail = new Node();
		head->next = tail;
		tail->prev = head;
		head->prev = NULL;
		tail->next = NULL;
		curLength = other.curLength;

		Node *p = other.head->next;
		Node *q = head;
		Node *r;
		while (p != other.tail)
		{
			r = new Node(q, q->next);
			q->next->prev = r;
			q->next = r;
			r->blockSize = p->blockSize;
			for (int i = 0; i < p->blockSize; ++i)
			{
				r->data[i] = new T(*(p->data[i]));
			}
			p = p->next;
			q = q->next;
		}
	}

	~deque()
	{
		clear();
		delete head;
		delete tail;
	}

	deque &operator=(const deque &other)
	{
		if (this == &other)
			return *this;
		clear();
		curLength = other.curLength;

		Node *p = other.head->next;
		Node *q = head;
		Node *r;
		while (p != other.tail)
		{
			r = new Node(q, q->next);
			q->next->prev = r;
			q->next = r;
			r->blockSize = p->blockSize;
			for (int i = 0; i < p->blockSize; ++i)
			{
				r->data[i] = new T(*(p->data[i]));
			}
			p = p->next;
			q = q->next;
		}
		return *this;
	}

	T &at(const size_t &pos)
	{
		if (pos < size() && pos >= 0)
		{
			size_t offset = pos;
			Node *cur_p = head->next;
			while (offset > cur_p->blockSize - 1)
			{
				offset -= cur_p->blockSize;
				cur_p = cur_p->next;
			}
			return *(cur_p->data[offset]);
		}
		else
			throw index_out_of_bound();
	}
	const T &at(const size_t &pos) const
	{
		if (pos < size() && pos >= 0)
		{
			size_t offset = pos;
			const Node *cur_p = head->next;
			while (offset > cur_p->blockSize - 1)
			{
				offset -= cur_p->blockSize;
				cur_p = cur_p->next;
			}
			return *(cur_p->data[offset]);
		}
		else
			throw index_out_of_bound();
	}
	T &operator[](const size_t &pos)
	{
		if (pos < size() && pos >= 0)
		{
			size_t offset = pos;
			Node *cur_p = head->next;
			while (offset > cur_p->blockSize - 1)
			{
				offset -= cur_p->blockSize;
				cur_p = cur_p->next;
			}
			return *(cur_p->data[offset]);
		}
		else
			throw index_out_of_bound();
	}
	const T &operator[](const size_t &pos) const
	{
		if (pos < size() && pos >= 0)
		{
			size_t offset = pos;
			const Node *cur_p = head->next;
			while (offset > cur_p->blockSize - 1)
			{
				offset -= cur_p->blockSize;
				cur_p = cur_p->next;
			}
			return *(cur_p->data[offset]);
		}
		else
			throw index_out_of_bound();
	}

	const T &front() const
	{
		if (!empty())
			return *(head->next->data[0]);
		else
			throw container_is_empty();
	}

	const T &back() const
	{
		if (!empty())
		{
			return *(tail->prev->data[tail->prev->blockSize - 1]);
		}
		else
			throw container_is_empty();
	}

	iterator begin()
	{
		return iterator(0, head->next, this);
	}
	const_iterator cbegin() const
	{
		return const_iterator(0, head->next, this);
	}

	iterator end()
	{
		return iterator(0, tail, this);
	}
	const_iterator cend() const
	{
		return const_iterator(0, tail, this);
	}

	bool empty() const
	{
		return curLength == 0;
	}

	size_t size() const
	{
		return curLength;
	}

	void clear()
	{
		Node *p = head->next, *q;
		head->next = tail;
		tail->prev = head;
		while (p != tail)
		{
			q = p->next;
			delete p;
			p = q;
		}
		curLength = 0;
	}

	iterator insert(iterator pos, const T &value)
	{
		if (pos.container != this || pos.node == NULL)
			throw invalid_iterator();
		if (pos.node == tail)
		{
			if (!empty())
			{
				addNode(tail->prev, tail->prev->blockSize, value);
				return iterator(tail->prev->blockSize - 1, tail->prev, this);
			}
			else
			{
				addNode(tail, pos.index, value);
				return iterator(tail->prev->blockSize - 1, tail->prev, this);
			}
		}
		else
		{
			if (pos.node->blockSize == MAX_BLOCK_SIZE)
			{
				if (pos.index < MAX_BLOCK_SIZE / 2)
				{
					addNode(pos.node, pos.index, value);
					return iterator(pos.index, pos.node, this);
				}
				else
				{
					addNode(pos.node, pos.index, value);
					return iterator(pos.index - MAX_BLOCK_SIZE / 2, pos.node->next, this);
				}
			}
			else
			{
				addNode(pos.node, pos.index, value);
				return iterator(pos.index, pos.node, this);
			}
		}
	}
	iterator erase(iterator pos)
	{
		if (curLength == 0 || pos == end() || pos.container != this || pos.node == NULL)
			throw invalid_iterator();
		if (pos.index == pos.node->blockSize - 1)
		{
			Node *tmp = pos.node->next;
			removeNode(pos.node, pos.index);
			return iterator(0, tmp, this);
		}
		else
		{
			removeNode(pos.node, pos.index);
			return iterator(pos.index, pos.node, this);
		}
	}

	void push_back(const T &value)
	{
		if (!empty())
		{
			addNode(tail->prev, tail->prev->blockSize, value);
		}
		else
		{
			addNode(tail, 0, value);
		}
	}

	void pop_back()
	{

		if (!empty())
		{
			removeNode(tail->prev, tail->prev->blockSize - 1);
		}
		else
			throw container_is_empty();
	}

	void push_front(const T &value)
	{
		addNode(head->next, 0, value);
	}

	void pop_front()
	{
		if (!empty())
		{
			removeNode(head->next, 0);
		}
		else
			throw container_is_empty();
	}
	Node *getHead() const { return head; }
	Node *getTail() const { return tail; }
};

} // namespace sjtu

#endif
