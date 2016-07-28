#include <iostream>
#include <vector>
#include <map>

template <class K, class T>
struct Node
{
	K key_;
	T data_;
	Node* prev_;
	Node* next;
};

template <class K, class T>
class LRUCache
{
public:
	LRUCache(size_t size)
	{
		entries_ = new Node<K, T>[size];
		for (size_t i = 0; i < size; i++)
		{
			free_entries_.push_back(entries_ + i);
		}
		head_ = new Node<K, T>;
		tail_ = new Node<K, T>;
		head_->prev_ = NULL;
		head_->next_ = tail_;
		tail_->prev_ = head_;
		tail_->next_ = NULL;
	}
	
	virtual ~LRUCache()
	{
		if (head_)
		{
			delete head_;
			head_ = NULL;
		}
		if (tail_)
		{
			delete tail_;
			tail_ = NULL;
		}
		if (entries_)
		{
			delete[] entries_;
		}
	}
	
	void put(K key, T data)
	{
		Node<K, T>* node = map_[key];
		// �ýڵ��Ѿ�����
		if (node)
		{
			// ȡ���ýڵ�
			detach(node);
			// ��������
			node->data_ = data;
			// ���ýڵ����ͷ��
			attach(node);
		}
		else
		{
			// cache����
			if (free_entries_.empty())
			{
				// �����һ��node�Ƴ�
				node = tail_->prev_;
				detach(node);
				map_.erase(node->key_);
			}
			else
			{
				// ����һ���սڵ�
				node = free_entries_.back();
				free_entries_.pop_back();
			}
			
			node->key_ = key;
            node->data_ = data;
            map_[key] = node;
			// �ýڵ���ӵ�ͷ��
            attach(node);
		}
	}
	
	T get(K key)
	{
		Node<K, T>* node = map_[key];
		// ����
		if (node)
		{
			// ȡ���ýڵ㣬���ŵ�����ͷ��
			detach(node);
			attach(node);
			return node->data_;
		}
		else
		{
			// ���cache��û�У�����T��Ĭ��ֵ����map��Ϊһ��
			return T();
		}
	}
	
private:
	// �����node
	void detach(Node<K, T>* node)
	{
		node->prev_->next_ = node->next_;
		node->next_->prev_ = node->prev_;
	}
	
	// ��ӵ�����ͷ��
	void attach(Node<K, T>* node)
	{
		node->prev_ = head_;
		node->next_ = head_->next_;
		head_->next_ = node;
		node->next_->prev_ = node;
	}

private:
	std::map<K, Node<K, T>*> map_;
	// �洢���ýڵ��ַ
	std::vector<Node<K, T>*> free_entries_;
	Node<K, T>* head_;
	Node<K, T>* tail_;
	// ˫�������еĽڵ�
	Node<K, T>* entries_;
};