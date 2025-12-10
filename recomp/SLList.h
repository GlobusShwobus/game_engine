#pragma once
#include <memory>

namespace badEngine {
	
	template <typename T>//basic restriction should be basically just deletable
	class SLList {
	private:

		struct Element {
			Element(const T& val, std::unique_ptr<Element> next)
				:value(val), next(std::move(next)) {}
			Element(T&& val, std::unique_ptr<Element> next)
				:value(std::move(val)), next(std::move(next)) {
			}
			T value;
			std::unique_ptr<Element> next = nullptr;
		};

	public:
		SLList() = default;
		//MODIFIERS

		void push_front(const T& val)
		{
			mFront = std::make_unique<Element>(val, std::move(mTop));
			++mCount;
		}
		void push_front(T&& value)
		{
			mFront = std::make_unique<Element>(std::move(val), std::move(mTop));
			++mCount;
		}
		void pop()
		{
			if (mFront) {
				mFront = std::move(mFront->next);
				--mCount;
			}
		}
		//ELEMENT ACCESS
		T& top() {
			return mFront->value;
		}
		const T& top()const {
			return mFront->value;
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