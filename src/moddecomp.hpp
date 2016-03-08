//
// Created by Lauréline Nevin <Lauréline Nevin> on 29/02/16.
//

#ifndef CLIQUECLAC_MODULARDECOMPOSITION_HPP
#define CLIQUECLAC_MODULARDECOMPOSITION_HPP

#include <list>
#include <map>
#include <boost/dynamic_bitset.hpp>
#include <string>
#include <boost/pool/pool_alloc.hpp>

#include "fmdm/dm_types.h"

namespace moddecomp {
	typedef boost::dynamic_bitset<unsigned long> NodeMask;

	template <class Data>
	class LinkedListIterator;

	template <class Data>
	class LinkedList {
		friend class iterator_type;

	private:
		typedef LinkedList<Data> self_type;
		typedef LinkedListIterator<Data> iterator_type;
		LinkedList(Data *data) : next(nullptr), prev(nullptr), data(data) {}
		static self_type * const END_ITER = (self_type*) 0xFFFFDEAD;
		static boost::fast_pool_allocator<self_type> * alloc = nullptr;
	public:
		static self_type *create(Data data) {
			if (alloc == nullptr)
				alloc = new boost::fast_pool_allocator<self_type>();
			self_type *p = alloc->allocate();
			*p = self_type(data);
			return p;
		}
		~LinkedList() {
			alloc->deallocate(this);
		}
		Data data;
		self_type *next, *prev;
		self_type *seekFirst() { return (prev == nullptr) ? this : prev->seekFirst(); }
		self_type *seekLast() { return (next == nullptr) ? this : next->seekLast(); }
		inline void insertNext(self_type *nx) {
			self_type *n = next;
			next = nx;
			nx->next = n;
		}
		inline self_type *pop() {
			if (prev->next != nullptr)
				prev->next = next;
			if (next != nullptr)
				next->prev = prev;
			next = prev = nullptr;
			return this;
		}
		inline self_type *splitAfter() {
			if (next == nullptr)
				return nullptr;
			auto n = next;
			next->prev = nullptr;
			next = nullptr;
			return n;
		}
		inline iterator_type begin() { return iterator_type(this); }
		inline iterator_type end() { return iterator_type(END_ITER); }
	};

	template <class Data>
	class LinkedListIterator {
		LinkedList <Data> *data;
	private:
		typedef LinkedListIterator<Data> self_type;
		typedef LinkedList<Data> base_type;
	public:
		LinkedListIterator(base_type *data) : data(data) {}
		LinkedListIterator(self_type &copy) : data(copy.data) {}
		inline self_type &operator++() {
			data = (data->next != nullptr) ? data->next : base_type::END_ITER;
			return *this;
		}
		inline self_type operator++(int) {
			self_type temp = *this;
			++*this;
			return temp;
		}
		inline bool operator==(const self_type &it2) {
			return data == it2.data;
		}
		inline base_type &operator*() { return *data; }
		inline base_type *operator->() { return data; }
	};

	struct DecompTreeList {
		fmdm::VertexIdType pivot;
		DecompTreeList *neighbour;
		LinkedList<DecompTreeList> *nonneighbour;
	};

	std::shared_ptr<DecompTreeList> modularDecomposition(fmdm::Graph G, NodeMask bitset);

	bool compareDMTrees(const fmdm::Node &root1, const fmdm::Node &root2);

}

#endif //CLIQUECLAC_MODULARDECOMPOSITION_HPP
