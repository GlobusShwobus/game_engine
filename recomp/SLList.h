#pragma once
#include <memory>

namespace badEngine {
	//struct T {
		//int a;
	//};
	template <typename T>//basic restriction should be basically just deletable, and probably can't be a const/volatile obj?
	class SLList {
	private:

		struct NodeBase {
			NodeBase() = default;
			explicit NodeBase(std::unique_ptr<NodeBase> n) :next(std::move(n)) {}
			std::unique_ptr<NodeBase> next = nullptr;
			virtual ~NodeBase() = default;
		};
		struct Node : NodeBase {
			Node(std::unique_ptr<NodeBase> next, const T& val)
				:NodeBase{ std::move(next) }, value(val)
			{
			}
			Node(std::unique_ptr<NodeBase> next, T&& val)
				:NodeBase{ std::move(next) }, value(std::move(val))
			{
			}
			template<typename... Args>
				requires std::constructible_from<T, Args&&...>
			Node(std::unique_ptr<NodeBase> next, Args&&... args)
				: NodeBase{ std::move(next) }, value(std::forward<Args>(args)...)
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
				mPtr = mPtr->next.get();
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
				mPtr = mPtr->next.get();
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

		//ELEMENT ACCESS
		iterator begin()noexcept
		{
			return iterator(mSentinel.next.get());
		}
		const_iterator begin()const noexcept
		{
			return const_iterator(mSentinel.next.get());
		}
		const_iterator cbegin()const noexcept
		{
			return const_iterator(mSentinel.next.get());
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
			mSentinel.next.reset();
		}
		template<typename... Args>
		iterator emplace_after(const_iterator pos, Args&&...args)
			requires std::constructible_from<value_type, Args&&...>
		{
			//TODO::check for range [before_begin -> end] validity
			//TODO:: if an exception is thrown (likely make_unique, structure should remain unchanged)

			NodeBase* given = pos.mPtr;
			//new node that sits in the middle of nodes points to whatever given->next is, data is saved or nullptr
			given->next = std::make_unique<Node>(std::move(given->next), std::forward<Args>(args)...);
			//return val
			NodeBase* ret = given->next.get();

			//incr
			return iterator(ret);
		}
		iterator insert_after(const_iterator pos, const_reference value)
			requires std::copyable<value_type>
		{
			return emplace_after(pos, value);
		}
		iterator insert_after(const_iterator pos, value_type&& value)
			requires std::movable<value_type>
		{
			return emplace_after(pos, value);
		}
		iterator insert_after(const_iterator pos, size_type count, const_reference value)
			requires std::copyable<value_type>
		{
			iterator ret = iterator(pos.mPtr);
			while (count-- > 0) {
				ret = emplace_after(ret, value);
			}
			return iterator(ret);
		}
		template<std::input_iterator InputIt>
		iterator insert_after(const_iterator pos, InputIt first, InputIt last)
		{
			iterator ret = iterator(pos.mPtr);
			for (; first != last; ++first) {
				ret = emplace_after(ret, *first);
			}
			return iterator(ret);
		}
		iterator insert_after(const_iterator pos, std::initializer_list<value_type> ilist)
		{
			return insert_after(pos, ilist.begin(), ilist.end());
		}
		template<std::ranges::input_range R>
		iterator insert_range_after(const_iterator pos, R&& range)//ranges is gigachad wtf
		{
			return insert_after(pos, std::ranges::begin(range), std::ranges::end(range));
		}

		iterator erase_after(const_iterator pos)//UB if not deref it
		{
			NodeBase* given = pos.mPtr;
			//basically just a swap
			auto removedNode = std::move(given->next);
			given->next = std::move(removedNode->next);
			return iterator(given->next.get());
		}
		iterator erase_after(const_iterator first, const_iterator last)
		{
			NodeBase* b = first.mPtr;
			NodeBase* e = last.mPtr;
			//so close yet so far from perfect, is linear compelxity
			if (b && b != e) {

				auto curr = std::move(b->next);

				while (curr && curr.get() != e) {
					curr = std::move(curr->next);
				}

				b->next = std::move(curr);

			}
			return iterator(e);
		}

		void push_front(const_reference value)
			requires std::copyable<value_type>
		{
			emplace_after(before_begin(), value);
		}
		void push_front(value_type&& value)
			requires std::movable<value_type>
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
		void swap(SLList& other)noexcept
		{
			auto temp = std::move(mSentinel.next);
			mSentinel.next = std::move(other.mSentinel.next);
			other.mSentinel.next = std::move(temp);
		}

		//INFO
		bool is_empty()const noexcept
		{
			return mSentinel.next.get();
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
			if (beforeFirst->next.get() == endNode) {
				return;
			}
			//FIND ONE BEFORE LAST NODE
			NodeBase* oneBeforeEnd = beforeFirst->next.get();
			while (oneBeforeEnd->next.get() != endNode) {
				oneBeforeEnd = oneBeforeEnd->next.get();
			}

			//detach this tail
			auto currentTail = std::move(posNode->next);
			//attach full range of other
			posNode->next = std::move(beforeFirst->next);
			//detach other tail
			auto otherTail = std::move(oneBeforeEnd->next);
			//reattach current tail
			oneBeforeEnd->next = std::move(currentTail);
			//reattach other tail
			beforeFirst->next = std::move(otherTail);
		}
		template<typename Compare>
		void merge(SLList& other, Compare comp)//default uses std::less<T>
		{
			if (this == &other)return;

			//use pointers to unique_ptr, this helps incrementing iteration
			std::unique_ptr<NodeBase>* dest = &mSentinel.next;
			std::unique_ptr<NodeBase>* mine = &mSentinel.next;
			std::unique_ptr<NodeBase>* his = &other.mSentinel.next;

			while (*mine && *his) {
				//cast from NodeBase to Node
				Node* myVal = static_cast<Node*>(mine->get());
				Node* hisVal = static_cast<Node*>(his->get());
				//if his value gets truth condition
				if (comp(hisVal->value, myVal->value)) {
					//assign his chain to temp, then reasign the chain back except first element
					auto temp = std::move(*his);
					*his = std::move(temp->next);
					//assign the dest chain to tail of temp then reattach it back with the 1 element we had up front
					temp->next = std::move(*dest);
					*dest = std::move(temp);
				}
				else {
					//deref unique ptr and get the address of next, this is the kicker
					dest = &(*dest)->next;
					mine = &(*mine)->next;
				}
			}
			//if his still has elements left, move over everything to the end
			if (*his) {
				*dest = std::move(*his);
			}
		}

	private:
		mutable NodeBase mSentinel;
	};
}



	/*
	HASH TABLE STUFF, BUT LATER...
	template<typename T>
	void hash_combine(std::size_t& seed, const T& v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e37779b9 + (seed << 6) + (seed >> 2);
	}


	template<typename Key, typename T, typename Hash=std::hash<Key>>
	class HashTable {

		struct Node {
			Key key;
			T value;
			Node* next;
		};

	public:

		HashTable() = default;


		std::vector<Node*> mHashHeads;
	};
	*/