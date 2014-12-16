/* 
 * File:   Heap.h
 * Author: staszek
 *
 * Created on August 21, 2012, 3:40 AM
 */

#ifndef HEAP_H
#define	HEAP_H

#include "utils/utils.h"
#include <list>

namespace gmum {

template<class T>
class Heap {
public:

	struct HeapNode {
		int key;
		T val;
		HeapNode() {
		}
		HeapNode(const T& val, int key) :
				key(key), val(val) {
		}
		bool operator>(const HeapNode & rhs) {
			return val > rhs.val;
		}
		bool operator<(const HeapNode & rhs) {
			return val < rhs.val;
		}
		bool operator>=(const HeapNode & rhs) {
			return val >= rhs.val;
		}
		bool operator<=(const HeapNode & rhs) {
			return val <= rhs.val;
		}
		friend std::ostream & operator<<(std::ostream & out,
				const HeapNode & rhs) {
			out << rhs.val;
			return out;
		}
	};
	std::vector<HeapNode*> m_heap;
	int m_size;

	void checkHeapSize() {
		if (m_size == SIZE(m_heap))
			m_heap.resize(2 * (int) m_heap.size());
	}

	int parent(int i) {
		return (i - 1) / 2; //shift
	}
	int leftChild(int i) {
		return 2 * i + 1; //shift
	}
	int rightChild(int i) {
		return 2 * i + 2; //shift
	}

	bool operator=(const Heap & rhs) {
		return true;
	}
public:

	void print() {
		REP(i, m_size)
			cout << m_heap[i]->val << ",";
		cout << endl;
	}
	bool isEmpty() const {
		return (int) m_heap.size() == 0;
	}
	int getSize() const {
		return m_size;
	}

	~Heap() {
		for (int i = 0; i < m_size; ++i) {
			delete m_heap[i];
		}
	}
	Heap(int size = 100) :
			m_size(0) {
		m_heap.resize(size);
	}

	int moveUp(int i, HeapNode * node) {
		if (m_size == 0)
			return -1;

		while (i > 0 && *node > *(m_heap[parent(i)])) {
			m_heap[i] = m_heap[parent(i)];

			m_heap[i]->key = i;

			i = parent(i);
		}

		m_heap[i] = node;
		m_heap[i]->key = i;
		return i;
	}
	int moveDown(int i) {
		if (m_size == 0)
			return -1;

		HeapNode * tmp;
		int j = 0;

		if (m_size > 1)
			while (i <= parent(m_size - 1)) {

				j = leftChild(i);
				if (j != (m_size - 1) && *(m_heap[j + 1]) > *(m_heap[j]))
					++j; //right child

				if (*(m_heap[j]) > *(m_heap[i])) {
					tmp = m_heap[i];
					m_heap[i] = m_heap[j];
					m_heap[j] = tmp;
					m_heap[i]->key = i;
					m_heap[j]->key = j;
					i = j;
				} else
					break;
			}

		m_heap[i]->key = i;

		return i;
	}

	//zwraca wskaznik do struktury w heap - uzywane do remove w O(1) co jest konieczne tutaj
	void* insert(const T& val) {
		checkHeapSize();
		HeapNode * node = new HeapNode(val, SIZE(m_heap));
		m_heap[m_size++] = node;

		int i = moveUp(m_size - 1, node);

		return reinterpret_cast<void*>(m_heap[i] = node);
	}

	bool check(int i) const {
		if (m_size == 0)
			return false;

		bool r = true, l = true;

		if (leftChild(i) < m_size) {
			l = *m_heap[leftChild(i)] <= *m_heap[i] && check(leftChild(i));
		}
		if (rightChild(i) < m_size) {
			r = *m_heap[rightChild(i)] <= *m_heap[i] && check(rightChild(i));
		}
		return l && r;
	}

	void remove(void * ptr) {
		HeapNode * node = reinterpret_cast<HeapNode *>(ptr);
		//delete node;

		int i = node->key;

		//memory leak

		m_heap[i] = m_heap[m_size - 1];
		m_heap[i]->key = i;
		--m_size;

		node = m_heap[i];

		//REPORT(i);
		//REPORT(node->val);
		//if(leftChild(i)<m_size) REPORT(m_heap[leftChild(i)]->val)

		if (i != 0 && *node > *m_heap[parent(i)])
			moveUp(i, node);
		else {
			if ((leftChild(i) < m_size && *m_heap[leftChild(i)] > *node)
					|| (rightChild(i) < m_size && *m_heap[rightChild(i)] > *node)) {
				moveDown(i);
				//cout<<"moveDown("<<i<<")\n";
			}
		}

		delete reinterpret_cast<HeapNode *>(ptr);
	}

	T extractMax() {
		if (m_size == 0)
			throw "Zero sized Heap max extraction?";

		HeapNode * tmp = m_heap[0];
		T extracted = tmp->val;

		delete tmp;

		m_heap[0] = m_heap[m_size - 1];
		--m_size;

		moveDown(0);

		return extracted;
	}

};
}
#endif	/* HEAP_H */

