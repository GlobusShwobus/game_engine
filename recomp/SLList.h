#pragma once
#include <memory>
#include "badUtility.h"

namespace badEngine {
	template <typename T>
		requires IS_SLLIST_COMPATIBLE<T>
	class SLList {
	private:

		struct NodeBase {
			NodeBase() = default;
			NodeBase(NodeBase* next) :next(next) {}
			NodeBase* next = nullptr;
		};
		struct Node : NodeBase {
			template<typename... Args>
				requires std::constructible_from<T, Args&&...>
			Node(NodeBase* next, Args&&... args)
				: NodeBase{ next }, value(std::forward<Args>(args)...)
			{
			}

			T value;
		};

		class const_iterator;

		class iterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			iterator() = default;
			explicit iterator(NodeBase* pNode) :mPtr(pNode) {}

			reference operator*()const
			{
				return static_cast<Node*>(mPtr)->value;
			}
			pointer operator->()const
			{
				return &static_cast<Node*>(mPtr)->value;
			}
			iterator& operator++()
			{
				mPtr = mPtr->next;
				return *this;
			}
			iterator operator++(int)
			{
				iterator tmp = *this;
				++(*this);
				return tmp;
			}

			bool operator==(const iterator& rhs)const
			{
				return mPtr == rhs.mPtr;
			}
			bool operator!=(const iterator& rhs)const {
				return mPtr != rhs.mPtr;
			}

		private:
			friend class const_iterator;
			friend class SLList;
			NodeBase* mPtr = nullptr;
		};

		class const_iterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = const T*;
			using reference = const T&;

			const_iterator() = default;
			explicit const_iterator(NodeBase* pNode) :mPtr(pNode) {}
			const_iterator(const iterator& it) :mPtr(it.mPtr) {}

			reference operator*()const
			{
				return static_cast<const Node*>(mPtr)->value;
			}
			pointer operator->()const
			{
				return &static_cast<const Node*>(mPtr)->value;
			}
			const_iterator& operator++()
			{
				mPtr = mPtr->next;
				return *this;
			}
			const_iterator operator++(int)
			{
				const_iterator tmp = *this;
				++(*this);
				return tmp;
			}

			bool operator==(const const_iterator& rhs)const
			{
				return mPtr == rhs.mPtr;
			}
			bool operator!=(const const_iterator& rhs)const {
				return mPtr != rhs.mPtr;
			}

		private:
			friend class SLList;
			NodeBase* mPtr = nullptr;
		};


	public:

		using value_type = T;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;
		using iterator = SLList::iterator;
		using const_iterator = SLList::const_iterator;

	public:
		SLList() = default;
		explicit SLList(size_type count)
			requires std::default_initializable<value_type>
		{
			for (size_type i = 0; i < count; ++i) {
				emplace_after(before_begin(), value_type{});
			}
		}
		SLList(size_type count, const_reference value)
			requires std::constructible_from<value_type, const_reference>
		{
			for (size_type i = 0; i < count; ++i) {
				emplace_after(before_begin(), value);
			}
		}
		template<std::input_iterator InputIt>
			requires std::constructible_from<value_type, std::iter_reference_t<InputIt>>
		SLList(InputIt first, InputIt last)
		{
			insert_after(before_begin(), first, last);
		}
		template<std::ranges::input_range R>
			requires std::constructible_from<value_type, std::ranges::range_reference_t<R>>//NOTE, if range_ref_t can be used to construct T, being the same exact type is not required
		SLList(R&& range)
		{
			insert_range_after(before_begin(), std::move(range));
		}
		SLList(const SLList& other)
			requires std::constructible_from<value_type, const_reference>
		{
			auto curr = before_begin();
			for (const auto& v : other)
			{
				curr = emplace_after(curr, v);
			}
		}
		SLList(SLList&& other)noexcept
		{
			mSentinel.next = other.mSentinel.next;
			other.mSentinel.next = nullptr;
		}
		SLList(std::initializer_list<value_type> init)
			requires std::constructible_from<value_type, const_reference>
		{
			auto curr = before_begin();
			for (const auto& v : init)
			{
				curr = emplace_after(curr, v);
			}
		}
		SLList& operator=(SLList other) noexcept
		{
			this->swap(other);
			return *this;
		}
		SLList& operator=(std::initializer_list<value_type> list)
		{
			SLList temp(list);
			this->swap(temp);
			return *this;
		}
		void swap(SLList& other)noexcept
		{
			NodeBase* temp = mSentinel.next;
			mSentinel.next = other.mSentinel.next;
			other.mSentinel.next = temp;
		}


		~SLList()noexcept
		{
			clear();
		}

		//ELEMENT ACCESS
		iterator begin()noexcept
		{
			return iterator(mSentinel.next);
		}
		const_iterator begin()const noexcept
		{
			return const_iterator(mSentinel.next);
		}
		const_iterator cbegin()const noexcept
		{
			return const_iterator(mSentinel.next);
		}
		iterator end()noexcept
		{
			return iterator(nullptr);
		}
		const_iterator end()const noexcept
		{
			return const_iterator(nullptr);
		}
		const_iterator cend()const noexcept
		{
			return const_iterator(nullptr);
		}
		iterator before_begin()noexcept
		{
			return iterator(&mSentinel);
		}
		const_iterator before_begin()const noexcept
		{
			return const_iterator(&mSentinel);
		}
		const_iterator cbefore_begin()const noexcept
		{
			return const_iterator(&mSentinel);
		}

		reference front()
		{
			return *begin();
		}
		const_reference front()const
		{
			return *begin();
		}
		//MODIFY

		void clear()noexcept
		{
			NodeBase* curr = mSentinel.next;
			while (curr) {
				NodeBase* temp = curr->next;
				delete curr;
				curr = temp;
			}
			mSentinel.next = nullptr;
		}
		/*
		UNDEFIEND BEHAVIOR CONDITION:
			if pos is not in range of [begin -> end)
		*/
		template<typename... Args>
			requires std::constructible_from<value_type, Args...>
		iterator emplace_after(const_iterator pos, Args&&...args)
		{
			NodeBase* given = pos.mPtr;
			//new node that sits in the middle of nodes points to whatever given->next is, data is saved or nullptr
			given->next = new Node(given->next, std::forward<Args>(args)...);

			return iterator(given->next);
		}
		iterator insert_after(const_iterator pos, const_reference value)
			requires std::constructible_from<value_type, const_reference>
		{
			return emplace_after(pos, value);
		}
		iterator insert_after(const_iterator pos, value_type&& value)
			requires std::constructible_from<value_type, value_type&&>
		{
			return emplace_after(pos, std::move(value));
		}
		iterator insert_after(const_iterator pos, size_type count, const_reference value)
			requires std::constructible_from<value_type, const_reference>
		{
			iterator ret = iterator(pos.mPtr);
			while (count-- > 0) {
				ret = emplace_after(ret, value);
			}
			return ret;
		}
		template<std::input_iterator InputIt>
			requires std::constructible_from<value_type, std::iter_reference_t<InputIt>>
		iterator insert_after(const_iterator pos, InputIt first, InputIt last)
		{
			iterator ret = iterator(pos.mPtr);
			for (; first != last; ++first) {
				ret = emplace_after(ret, *first);
			}
			return ret;
		}
		iterator insert_after(const_iterator pos, std::initializer_list<value_type> ilist)
		{
			return insert_after(pos, ilist.begin(), ilist.end());
		}
		template<std::ranges::input_range R>
			requires std::constructible_from<value_type, std::ranges::range_reference_t<R>>
		iterator insert_range_after(const_iterator pos, R&& range)//ranges is gigachad wtf
		{
			return insert_after(pos, std::ranges::begin(range), std::ranges::end(range));
		}
		/*
		UNDEFIEND BEHAVIOR CONDITION:
			if pos is not in the range of [begin -> end)
			calling it with end() will cause double delete
		*/
		iterator erase_after(const_iterator pos)
		{
			NodeBase* given = pos.mPtr;
			NodeBase* removedNode = given->next;
			given->next = removedNode->next;
			delete removedNode;
			return iterator(given->next);
		}
		iterator erase_after(const_iterator first, const_iterator last)
		{
			NodeBase* b = first.mPtr;
			NodeBase* e = last.mPtr;
			//so close yet so far from perfect, is linear compelxity
			if (b && b != e) {

				NodeBase* curr = b->next;

				while (curr && curr != e) {
					NodeBase* temp = curr->next;
					delete curr;
					curr = temp;
				}

				b->next = curr;
			}
			return iterator(e);
		}

		void push_front(const_reference value)
			requires std::constructible_from<value_type, const_reference>
		{
			emplace_after(before_begin(), value);
		}
		void push_front(value_type&& value)
			requires std::constructible_from<value_type, value_type&&>
		{
			emplace_after(before_begin(), std::move(value));
		}
		template<std::ranges::input_range R>
		void push_front_range(R&& range)
		{
			insert_range_after(before_begin(), std::forward<R>(range));
		}
		void pop_front()
		{
			erase_after(before_begin());
		}
		/*
		UNDEFINED BEHAVIOR CONDITIONS:
			if list is empty
		*/
		value_type pop_front_return()
			requires std::constructible_from<value_type, value_type&&>
		{
			value_type value = std::move(front());
			pop_front();
			return value;
		}
		//INFO
		bool is_empty()const noexcept
		{
			return mSentinel.next == nullptr;
		}

		//OPERATIONS
		/*
		UNDEFINED BEHAVIOR CONDITIONS:
			if pos is not in the range of *this [before_begin - > end]
			if this == &other
		*/
		void splice_after(const_iterator pos, SLList& other)
		{
			splice_after(pos, other, other.before_begin(), other.end());
		}
		/*
		UNDEFINED BEHAVIOR CONDITIONS:
			if pos is not in the range of *this [before_begin - > end]
			if this == &other but pos is within [before_first - > end]
		*/
		void splice_after(const_iterator pos, SLList& other, const_iterator before_first)
		{
			splice_after(pos, other, before_first, other.end());
		}
		/*
		UNDEFINED BEHAVIOR CONDITIONS
			if pos is not in the range of *this [before_begin - > end]
			if [before_first - > last] is not in the range of [other.before_begin - > other.end]
			if any iterator in others range is not dereferenceable (concurrency outcomes)
			if this == &other but pos is within [before_first - > last]
		*/
		void splice_after(const_iterator pos, SLList& other, const_iterator before_first, const_iterator last)
		{
			NodeBase* posNode = pos.mPtr;
			NodeBase* beforeFirst = before_first.mPtr;
			NodeBase* endNode = last.mPtr;

			//EMPTY RANGE
			if (beforeFirst->next == endNode) {
				return;
			}
			//FIND ONE BEFORE LAST NODE
			NodeBase* oneBeforeEnd = beforeFirst->next;
			while (oneBeforeEnd->next != endNode) {
				oneBeforeEnd = oneBeforeEnd->next;
			}

			//detach this tail
			NodeBase* currentTail = posNode->next;
			//attach full range of other
			posNode->next = beforeFirst->next;
			//detach other tail
			NodeBase* otherTail = oneBeforeEnd->next;
			//reattach current tail
			oneBeforeEnd->next = currentTail;
			//reattach other tail
			beforeFirst->next = otherTail;
		}
		void merge(SLList& other)
			requires std::strict_weak_order<std::less<>, value_type, value_type>
		{
			merge(other, std::less<>{});
		}
		/*
		UNDEFINED BEHAVIOR CONDITIONS:
			assumes the containers are sorted with respect to comp as a precondition
			merge does not sort nor does it check if it is sorted and it WILL produce a result unless this == &other
		*/
		template<typename Compare>
			requires std::strict_weak_order<Compare&, value_type, value_type>// maybe also match bool comp(cosnt&, const&)?
		void merge(SLList& other, Compare comp)
		{
			if (this == &other)return;

			//use pointers to unique_ptr, this helps incrementing iteration
			NodeBase** dest = &mSentinel.next;
			NodeBase** mine = &mSentinel.next;
			NodeBase* his = other.mSentinel.next;

			while (*mine && his) {
				//cast from NodeBase to Node
				Node* myVal = static_cast<Node*>(*mine);
				Node* hisVal = static_cast<Node*>(his);
				//if his value gets truth condition
				if (comp(hisVal->value, myVal->value)) {
					//assign his chain to temp, then reasign the chain back except first element
					NodeBase* temp = his;
					his = temp->next;
					//assign the dest chain to tail of temp then reattach it back with the 1 element we had up front
					temp->next = *dest;
					*dest = temp;
				}//if my value gets truth condition
				else {
					//deref unique ptr and get the address of next, this is the kicker
					dest = &(*dest)->next;
					mine = &(*mine)->next;
				}
			}
			//if his still has elements left, move over everything to the end
			if (his) {
				*dest = his;
			}
			//other sentinel non owning
			other.mSentinel.next = nullptr;
		}

		size_type remove(const_reference value)
			requires std::equality_comparable_with<value_type, value_type>
		{
			return remove_if([&value](const_reference other) {
				return other == value;
				}
			);
		}
		template<typename UnaryPred>
			requires std::predicate<UnaryPred&, const_reference>
		size_type remove_if(UnaryPred p)
		{
			size_type count = 0;

			auto prev = before_begin();
			auto cur = begin();
			auto last = end();

			while (cur != last) {
				if (p(*cur)) {
					cur = erase_after(prev);
					++count;
				}
				else {
					prev = cur;
					++cur;
				}
			}

			return count;
		}
		void reverse()noexcept
		{
			NodeBase* prev = nullptr;
			NodeBase* curr = mSentinel.next;

			while (curr) {
				NodeBase* next = curr->next;
				curr->next = prev;
				prev = curr;
				curr = next;
			}

			mSentinel.next = prev;
		}
		size_type unique()
			requires std::equality_comparable_with<value_type, value_type>
		{
			return unique(std::equal_to<>{});
		}
		/*
		UNDEFIEND BEHAVIOR CONDITION:
			if p does not establih an equivalance relation, the behavior is undefined
		*/
		template<typename BinaryPred>
			requires std::predicate<BinaryPred&, const_reference, const_reference>
		size_type unique(BinaryPred p)
		{
			size_type count = 0;

			auto prev = begin();
			auto last = end();

			if (prev == last) {
				return count;
			}

			auto cur = std::next(prev);

			while (cur != last) {
				if (p(*prev, *cur)) {
					cur = erase_after(prev);
					++count;
				}
				else {
					prev = cur;
					++cur;
				}
			}
			return count;
		}
		void sort()
		{
			sort(std::less<>());
		}
		template<typename Compare>
			requires std::strict_weak_order<Compare&, value_type, value_type> // maybe also match bool comp(cosnt&, const&)?
		void sort(Compare comp)
		{
			mSentinel.next = setup_merge_sort(mSentinel.next, comp);
		}
	private:

		template<typename Compare>
		NodeBase* setup_merge_sort(NodeBase* head, Compare comp)
		{
			//base case
			if (!head || !head->next) {
				return head;
			}
			//find middle point by going +1 and +2
			NodeBase* slow = head;
			NodeBase* fast = head->next;

			while (fast && fast->next) {
				slow = slow->next;
				fast = fast->next->next;
			}

			NodeBase* second = slow->next;
			//assign end point of half
			slow->next = nullptr;

			head = setup_merge_sort(head, comp);
			second = setup_merge_sort(second, comp);

			return merge_sort(head, second, comp);
		}
		template<typename Compare>
		NodeBase* merge_sort(NodeBase* a, NodeBase* b, Compare comp)
		{
			NodeBase dummy;
			NodeBase* tail = &dummy;

			while (a && b)
			{
				Node* val1 = static_cast<Node*>(a);
				Node* val2 = static_cast<Node*>(b);

				if (comp(val1->value, val2->value))
				{
					tail->next = a;
					a = a->next;
				}
				else
				{
					tail->next = b;
					b = b->next;
				}
				tail = tail->next;
			}

			tail->next = a ? a : b;
			return dummy.next;
		}


	private:
		mutable NodeBase mSentinel;
	};
}