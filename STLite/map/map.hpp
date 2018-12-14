#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu
{
enum RBColor
{
	RED,
	BLACK
};
template <
	class Key,
	class T,
	class Compare = std::less<Key> >
class map
{
  public:
	typedef pair<const Key, T> value_type;
	struct RBNode
	{
		value_type *data;
		RBNode *parent;
		RBNode *left;
		RBNode *right;
		RBColor color;
		RBNode(RBColor c = RED, RBNode *p = NULL, RBNode *lc = NULL, RBNode *rc = NULL)
			: data(NULL), color(c), parent(p), left(lc), right(rc) {}
		~RBNode()
		{
			delete data;
		}
	};
	RBNode *root;
	size_t currentSize;

	/********************************************************************************************/
	void changeColor(RBNode *t) //Color-changing Function for insert
	{
		t->color = BLACK;
		if (t->left)
			t->left->color = RED;
		if (t->right)
			t->right->color = RED;
	}
	void exchangeNode(RBNode *&cur, RBNode *&tmp) //Exchange Nodes for a node with two children in __erase in case that iterator may become invalid
	{
		bool isRoot = false;
		isRoot = (cur == root);
		RBNode *tmp_parent = tmp->parent, *tmp_right = tmp->right;
		RBColor tmp_color = tmp->color;

		if (tmp == cur->right)
		{
			tmp->left = cur->left;
			tmp->right = cur;
			tmp->parent = cur->parent;
			tmp->color = cur->color;
			if (cur->parent)
			{
				if (cur == cur->parent->left)
					cur->parent->left = tmp;
				else
					cur->parent->right = tmp;
			}
			if (tmp->left)
				tmp->left->parent = tmp;
			if (tmp->right)
				tmp->right->parent = tmp;
			cur->left = NULL;
			cur->right = tmp_right;
			if (tmp_right)
				tmp_right->parent = cur;
			cur->color = tmp_color;
		}
		else
		{
			tmp->left = cur->left;
			tmp->right = cur->right;
			tmp->parent = cur->parent;
			tmp->color = cur->color;
			if (cur->parent)
			{
				if (cur == cur->parent->left)
					cur->parent->left = tmp;
				else
					cur->parent->right = tmp;
			}
			if (tmp->left)
				tmp->left->parent = tmp;
			if (tmp->right)
				tmp->right->parent = tmp;
			cur->left = NULL;
			cur->right = tmp_right;
			if (tmp_right)
				tmp_right->parent = cur;
			cur->parent = tmp_parent;
			tmp_parent->left = cur;
			cur->color = tmp_color;
		}

		if (isRoot)
			root = tmp;
		cur = tmp;
	}
	RBNode *__insert(const value_type &x)
	{
		Compare cmp;
		RBNode *cur, *p, *gp;
		if (root == NULL)
		{
			root = new RBNode(BLACK);
			root->data = new value_type(x);
			return root;
		}
		p = gp = cur = root;
		while (true)
		{
			if (cur != NULL)
			{
				if (cur->left != NULL && cur->left->color == RED && cur->right != NULL && cur->right->color == RED)
				{
					cur->left->color = cur->right->color = BLACK;
					cur->color = RED;
					insertAdjust(gp, p, cur);
				}
				gp = p;
				p = cur;
				cur = (cmp(x.first, cur->data->first) ? cur->left : cur->right);
			}
			else
			{
				cur = new RBNode();
				cur->data = new value_type(x);
				if (cmp(x.first, p->data->first))
				{
					p->left = cur;
					cur->parent = p;
				}
				else
				{
					p->right = cur;
					cur->parent = p;
				}
				insertAdjust(gp, p, cur);
				root->color = BLACK;
				return cur;
			}
		}
	}
	void __erase(const Key &x)
	{
		Compare cmp;
		Key *del = new Key(x);
		RBNode *s, *p, *cur; //p for parent, cur for current, s for sibling
		if (root == NULL)
		{
			delete del;
			return;
		}
		if (!(cmp(root->data->first, *del) || cmp(*del, root->data->first)) && root->left == NULL && root->right == NULL) //use !(cmp(root->data.first, del) || cmp(del, root->data.first))?
		{
			delete root;
			root = NULL;
			delete del;
			return;
		}

		s = p = cur = root;
		while (true)
		{
			eraseAdjust(p, cur, s, del);
			if (!(cmp(cur->data->first, *del) || cmp(*del, cur->data->first)) && cur->left != NULL && cur->right != NULL) //find the del-node with two children
			{
				RBNode *tmp = cur->right;
				while (tmp->left) //find the right minimum
					tmp = tmp->left;

				delete cur->data;
				cur->data = new value_type(*(tmp->data));
				exchangeNode(cur, tmp);

				delete del;
				del = new Key(tmp->data->first);
				p = cur; //go to next layer
				cur = cur->right;
				s = p->left;
				continue; //go back and delete the node
			}
			if (!(cmp(cur->data->first, *del) || cmp(*del, cur->data->first)) && cur->left == NULL && cur->right == NULL) //find the del-node with no child
			{
				delete cur;
				if (p->left == cur)
					p->left = NULL;
				else
					p->right = NULL;
				root->color = BLACK;
				delete del;
				return; //delete finished
			}

			p = cur; //if not find, continue
			cur = (cmp(*del, p->data->first) ? p->left : p->right);
			s = (cur == p->left ? p->right : p->left);
		}
	}
	RBNode *search(const Key &key) const
	{
		RBNode *t = root;
		Compare cmp;
		while (t != NULL && (cmp(t->data->first, key) || cmp(key, t->data->first)))
		{
			if (cmp(key, t->data->first))
				t = t->left;
			else
				t = t->right;
		}
		return t;
	}
	void makeEmpty(RBNode *&t)
	{
		if (t != NULL)
		{
			makeEmpty(t->left);
			makeEmpty(t->right);
			delete t;
		}
		t = NULL;
	}
	RBNode *makeTree(RBNode *t)
	{
		if (t == NULL)
			return NULL;
		RBNode *tmp = new RBNode(t->color);
		tmp->data = new value_type(*(t->data));
		if (t->left != NULL)
		{
			tmp->left = makeTree(t->left);
			tmp->left->parent = tmp;
		}
		if (t->right != NULL)
		{
			tmp->right = makeTree(t->right);
			tmp->right->parent = tmp;
		}
		return tmp;
	}
	void insertAdjust(RBNode *gp, RBNode *p, RBNode *cur)
	{
		if (p->color == BLACK)
			return;
		if (p == root)
		{
			p->color = BLACK;
			return;
		}
		if (gp->left == p)
		{
			if (p->left == cur)
			{
				LL(gp);
				changeColor(gp);
			}
			else
			{
				LR(gp);
				changeColor(gp);
			}
		}
		else
		{
			if (p->right == cur)
			{
				RR(gp);
				changeColor(gp);
			}
			else
			{
				RL(gp);
				changeColor(gp);
			}
		}
	}
	void eraseAdjust(RBNode *&p, RBNode *&cur, RBNode *&s, Key *del) //p for parent, cur for current, s for sibling
	{
		Compare cmp;
		if (cur->color == RED) //no need to adjust
			return;
		if (cur == root)
		{
			if (cur->left != NULL && cur->right != NULL && cur->left->color == cur->right->color)
			{
				cur->left->color = cur->right->color = BLACK;
				cur->color = RED; //it should be changed to BLACK after delete the node
				return;
			}
		}

		/*cur has two BLACK son*/
		if (((cur->left != NULL && cur->left->color == BLACK) || cur->left == NULL) //if cur's left & right child is black(or NULL)
			&& ((cur->right != NULL && cur->right->color == BLACK) || cur->right == NULL))
		{
			if (((s->left != NULL && s->left->color == BLACK) || s->left == NULL) && ((s->right != NULL && s->right->color == BLACK) || s->right == NULL)) //State 1: sibling's left & right child is black(or NULL)
			{
				p->color = BLACK; //change their color and parent's color
				cur->color = s->color = RED;
			}
			else
			{
				if (p->left == s)
				{
					if (s->left != NULL && s->left->color == RED) //State 2: Outer RED son of sibling
					{
						s->left->color = BLACK;
						LL(p);
						p->color = RED;
						p->right->color = BLACK;
						p = p->right;
					}
					else //State 3: Inner RED son of sibling
					{
						LR(p);
						p = p->right;
						p->color = BLACK;
					}
				}
				else
				{
					if (s->right != NULL && s->right->color == RED) //State 2: Outer RED son of sibling
					{
						s->right->color = BLACK;
						RR(p);
						p->color = RED;
						p->left->color = BLACK;
						p = p->left;
					}
					else //State 3: Inner RED son of sibling
					{
						RL(p);
						p = p->left;
						p->color = BLACK;
					}
				}
				cur->color = RED;
			}
		}
		else //cur has at least one RED son
		{
			if (!(cmp(cur->data->first, *del) || cmp(*del, cur->data->first))) //cur is the del-node
			{
				if (cur->left != NULL && cur->right != NULL) //if cur has two son
				{
					if (cur->right->color == BLACK) //if cur->right is not RED
					{
						LL(cur);
						cur->color = BLACK;
						cur->right->color = RED;
						cur = cur->right;
					}
					return;
				}
				if (cur->left != NULL) //if cur has one son, it must be RED, just rotate
				{
					LL(cur);
					changeColor(cur);
					p = cur;
					cur = cur->right;
				}
				else //according to book
				{
					RR(cur);
					changeColor(cur);
					p = cur;
					cur = cur->left;
				}
			}
			else //cur is not the del-node, go to the next layer
			{
				p = cur;
				cur = (cmp(*del, p->data->first) ? p->left : p->right);
				s = (cur == p->left ? p->right : p->left);
				if (cur->color == BLACK) //if new node is BLACK, go on
				{
					if (s == p->left)
					{
						LL(p);
						p->color = BLACK;
						p->right->color = RED;
						p = p->right;
					}
					else
					{
						RR(p);
						p->color = BLACK;
						p->left->color = RED;
						p = p->left;
					}
					s = (cur == p->left ? p->right : p->left);
					eraseAdjust(p, cur, s, del);
				}
			}
		}
	}

	void LL(RBNode *&gp)
	{
		bool isRoot = false;
		bool isLeft = false;
		if (gp == root)
			isRoot = true;

		RBNode *tmp = gp->left;
		tmp->parent = gp->parent;
		if (gp->parent)
		{
			if (gp->parent->left == gp)
				isLeft = true;
			else
				isLeft = false;
		}

		gp->left = tmp->right;
		if (tmp->right != NULL)
			tmp->right->parent = gp;

		tmp->right = gp;
		gp->parent = tmp;

		gp = tmp;
		if (gp->parent)
		{
			if (isLeft)
				gp->parent->left = gp;
			else
				gp->parent->right = gp;
		}

		if (isRoot)
		{
			root = gp;
			root->parent = NULL;
		}
	}
	void RR(RBNode *&gp)
	{
		bool isRoot = false;
		bool isLeft = false;
		if (gp == root)
			isRoot = true;

		RBNode *tmp = gp->right;
		tmp->parent = gp->parent;
		if (gp->parent)
		{
			if (gp->parent->left == gp)
				isLeft = true;
			else
				isLeft = false;
		}

		gp->right = tmp->left;
		if (tmp->left != NULL)
			tmp->left->parent = gp;

		tmp->left = gp;
		gp->parent = tmp;

		gp = tmp;
		if (gp->parent)
		{
			if (isLeft)
				gp->parent->left = gp;
			else
				gp->parent->right = gp;
		}

		if (isRoot)
		{
			root = gp;
			root->parent = NULL;
		}
	}
	void LR(RBNode *&gp)
	{
		RR(gp->left);
		LL(gp);
	}
	void RL(RBNode *&gp)
	{
		LL(gp->right);
		RR(gp);
	}
	/********************************************************************************************/
	class const_iterator;
	class iterator
	{
	  public:
		RBNode *current;
		map<Key, T, Compare> *container;
		RBNode *findMin(RBNode *t)
		{
			if (t == NULL)
				return NULL;
			while (t->left != NULL)
				t = t->left;
			return t;
		}
		RBNode *findMax(RBNode *t)
		{
			if (t == NULL)
				return NULL;
			while (t->right != NULL)
				t = t->right;
			return t;
		}
		RBNode *increment(RBNode *t)
		{
			if (t->right != NULL)
				return findMin(t->right);
			RBNode *tmp = t->parent;
			while (tmp != NULL && t == tmp->right)
			{
				t = tmp;
				tmp = tmp->parent;
			}
			return tmp;
		}
		RBNode *decrement(RBNode *t)
		{
			if (t->left != NULL)
				return findMax(t->left);
			RBNode *tmp = t->parent;
			while (tmp != NULL && t == tmp->left)
			{
				t = tmp;
				tmp = tmp->parent;
			}
			return tmp;
		}
		iterator(RBNode *n = NULL, map *c = NULL) : current(n), container(c) {}
		iterator(const iterator &other) : current(other.current), container(other.container) {}

		iterator operator++(int)
		{
			iterator tmp = *this;
			++*this;
			return tmp;
		}

		iterator &operator++()
		{
			if (current != NULL && container != NULL)
			{
				current = increment(current);
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
			if (current == NULL && container != NULL)
			{
				current = findMax(container->getRoot());
				if (current == NULL)
					throw invalid_iterator();
				return *this;
			}
			else
			{
				if (container != NULL)
				{
					current = decrement(current);
					if (current == NULL)
						throw invalid_iterator();
					return *this;
				}
				else
					throw invalid_iterator();
			}
		}

		value_type &operator*() const
		{
			if (current != NULL)
				return *(current->data);
			else
				throw invalid_iterator();
		}
		bool operator==(const iterator &rhs) const
		{
			if (current == rhs.current && container == rhs.container)
				return true;
			else
				return false;
		}
		bool operator==(const const_iterator &rhs) const
		{
			if (current == rhs.current && container == rhs.container)
				return true;
			else
				return false;
		}

		bool operator!=(const iterator &rhs) const
		{
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return !(*this == rhs);
		}

		value_type *operator->() const noexcept
		{
			return current->data;
		}
	};
	class const_iterator
	{
	  public:
		RBNode *current;
		const map<Key, T, Compare> *container;
		RBNode *findMin(RBNode *t)
		{
			if (t == NULL)
				return NULL;
			while (t->left != NULL)
				t = t->left;
			return t;
		}
		RBNode *findMax(RBNode *t)
		{
			if (t == NULL)
				return NULL;
			while (t->right != NULL)
				t = t->right;
			return t;
		}
		RBNode *increment(RBNode *t)
		{
			if (t->right != NULL)
				return findMin(t->right);
			RBNode *tmp = t->parent;
			while (tmp != NULL && t == tmp->right)
			{
				t = tmp;
				tmp = tmp->parent;
			}
			return tmp;
		}
		RBNode *decrement(RBNode *t)
		{
			if (t->left != NULL)
				return findMax(t->left);
			RBNode *tmp = t->parent;
			while (tmp != NULL && t == tmp->left)
			{
				t = tmp;
				tmp = tmp->parent;
			}
			return tmp;
		}
		const_iterator(RBNode *n = NULL, const map *c = NULL) : current(n), container(c) {}
		const_iterator(const const_iterator &other) : current(other.current), container(other.container) {}
		const_iterator(const iterator &other) : current(other.current), container(other.container) {}
		const_iterator operator++(int)
		{
			const_iterator tmp = *this;
			++*this;
			return tmp;
		}

		const_iterator &operator++()
		{
			if (current != NULL && container != NULL)
			{
				current = increment(current);
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
			if (current == NULL && container != NULL)
			{
				current = findMax(container->getRoot());
				if (current == NULL)
					throw invalid_iterator();
				return *this;
			}
			else
			{
				if (container != NULL)
				{
					current = decrement(current);
					if (current == NULL)
						throw invalid_iterator();
					return *this;
				}
				else
					throw invalid_iterator();
			}
		}

		value_type &operator*() const
		{
			if (current != NULL)
				return *(current->data);
			else
				throw invalid_iterator();
		}

		bool operator==(const iterator &rhs) const
		{
			if (current == rhs.current && container == rhs.container)
				return true;
			else
				return false;
		}

		bool operator==(const const_iterator &rhs) const
		{
			if (current == rhs.current && container == rhs.container)
				return true;
			else
				return false;
		}

		bool operator!=(const iterator &rhs) const
		{
			return !(*this == rhs);
		}

		bool operator!=(const const_iterator &rhs) const
		{
			return !(*this == rhs);
		}

		value_type *operator->() const noexcept
		{
			return current->data;
		}
	};

	map()
	{
		root = NULL;
		currentSize = 0;
	}
	map(const map &other)
	{
		root = makeTree(other.root);
		currentSize = other.currentSize;
	}

	map &operator=(const map &other)
	{
		if (this == &other)
			return *this;
		clear();
		root = makeTree(other.root);
		currentSize = other.currentSize;
		return *this;
	}

	~map()
	{
		makeEmpty(root);
	}

	T &at(const Key &key)
	{
		RBNode *foundNode = search(key);
		if (foundNode != NULL)
			return foundNode->data->second;
		else
			throw index_out_of_bound();
	}
	const T &at(const Key &key) const
	{
		RBNode *foundNode = search(key);
		if (foundNode != NULL)
			return foundNode->data->second;
		else
			throw index_out_of_bound();
	}

	T &operator[](const Key &key)
	{
		RBNode *foundNode = search(key);
		if (foundNode != NULL)
			return foundNode->data->second;
		else
		{
			pair<iterator, bool> tmp = insert(pair<Key, T>(key, T()));
			return tmp.first.current->data->second;
		}
	}

	const T &operator[](const Key &key) const
	{
		RBNode *foundNode = search(key);
		if (foundNode != NULL)
			return foundNode->data->second;
		else
			throw index_out_of_bound();
	}

	iterator begin()
	{
		RBNode *tmp;
		tmp = root;
		if (tmp == NULL)
			return iterator(tmp, this);
		while (tmp->left != NULL)
			tmp = tmp->left;
		return iterator(tmp, this);
	}
	const_iterator cbegin() const
	{
		RBNode *tmp;
		tmp = root;
		if (tmp == NULL)
			return const_iterator(tmp, this);
		while (tmp->left != NULL)
			tmp = tmp->left;
		return const_iterator(tmp, this);
	}

	iterator end()
	{
		return iterator(NULL, this);
	}
	const_iterator cend() const
	{
		return const_iterator(NULL, this);
	}

	bool empty() const
	{
		return root == NULL;
	}

	size_t size() const
	{
		return currentSize;
	}

	void clear()
	{
		makeEmpty(root);
		currentSize = 0;
	}

	pair<iterator, bool> insert(const value_type &value)
	{
		RBNode *tmp = search(value.first);
		if (tmp == NULL)
		{
			tmp = __insert(value);
			++currentSize;
			return pair<iterator, bool>(iterator(tmp, this), true);
		}
		else
			return pair<iterator, bool>(iterator(tmp, this), false);
	}

	void erase(iterator pos)
	{
		if (pos.current != NULL && pos.container == this)
		{
			__erase(pos.current->data->first);
			--currentSize;
		}
		else
			throw invalid_iterator();
	}

	size_t count(const Key &key) const
	{
		RBNode *foundNode = search(key);
		if (foundNode != NULL)
			return 1;
		else
			return 0;
	}

	iterator find(const Key &key)
	{
		RBNode *foundNode = search(key);
		return iterator(foundNode, this);
	}
	const_iterator find(const Key &key) const
	{
		RBNode *foundNode = search(key);
		return const_iterator(foundNode, this);
	}
	RBNode *getRoot() const { return root; }
};
} // namespace sjtu

#endif
