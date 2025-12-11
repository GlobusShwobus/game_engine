#pragma once
#include <memory>

namespace badEngine {
	struct T {
		int a;
	};
	//template <typename T>//basic restriction should be basically just deletable
	class SLList {
	private:

		struct Element {
			Element(const T& val, std::unique_ptr<Element> next)
				:value(val), next(std::move(next)) {}
			Element(T&& val, std::unique_ptr<Element> next)
				:value(std::move(val)), next(std::move(next)) {
			}
			template<typename... Args>
				requires std::constructible_from<T, Args&&...>
			Element(std::unique_ptr<Element> next, Args&&... args)
				:value(std::forward<Args>(args)...), next(std::move(next)) {
			}
			T value;
			std::unique_ptr<Element> next = nullptr;
		};


		class iterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			iterator() = default;
			iterator(Element* pElem) :mPtr(pElem) {}

			reference operator*()const
			{
				return mPtr->value;
			}
			pointer operator->()const
			{
				return &mPtr->value;
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
			Element* mPtr = nullptr;
		};

		class const_iterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = const T*;
			using reference = const T&;

			const_iterator() = default;
			const_iterator(Element* pElem) :mPtr(pElem) {}

			reference operator*()const
			{
				return mPtr->value;
			}
			pointer operator->()const
			{
				return &mPtr->value;
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
			Element* mPtr = nullptr;
		};

	public:
		SLList() = default;
		//MODIFIERS
		void push_front(const T& val)
		{
			mFront = std::make_unique<Element>(val, std::move(mFront));
			++mCount;
		}
		void push_front(T&& value)
		{
			mFront = std::make_unique<Element>(std::move(value), std::move(mFront));
			++mCount;
		}
		template <typename... Args>
		T& emplace_front(Args&&... args)		
			requires std::constructible_from<T, Args&&...>
		{
			mFront = std::make_unique<Element>(std::move(mFront), std::forward<Args>(args)...);
			++mCount;
			return mFront->value;
		}

		void clear()noexcept
		{
			mFront.reset();
			mCount = 0;
		}

		//ELEMENT ACCESS
		iterator begin()noexcept {
			return iterator(mFront.get());
		}
		const_iterator begin()const noexcept {
			return const_iterator(mFront.get());
		}
		const_iterator cbegin()const noexcept {
			return const_iterator(mFront.get());
		}
		iterator end()noexcept {
			return iterator(nullptr);
		}
		const_iterator end()const noexcept {
			return const_iterator(nullptr);
		}
		const_iterator cend()const noexcept {
			return const_iterator(nullptr);
		}

		//BULLSHIT
		std::size_t size()const {
			return mCount;
		}
		bool isEmpty()const
		{
			return mFront == nullptr;
		}

	private:
		std::unique_ptr<Element> mFront = nullptr;
		std::size_t mCount = 0;
	};

	/*
			void pop()
		{
			if (mFront) {
				mFront = std::move(mFront->next);
				--mCount;
			}
		}

				T& top() {
			return mFront->value;
		}
		const T& top()const {
			return mFront->value;
		}
	*/















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
}