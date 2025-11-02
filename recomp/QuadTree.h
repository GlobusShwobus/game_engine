#pragma once
#include "Rectangle.h"
#include <memory>
#include <array>

#include <vector>
#include <list>

#include "SequenceM.h"
//STATIC QUADTREE

namespace badEngine {

	/*
	template <typename OBJECT_TYPE>
	class StaticQuadTree
	{
		static constexpr std::size_t MAX_DEPTH = 8;
	public:
		StaticQuadTree(const rectF& size, const size_t nDepth = 0)
		{
			m_depth = nDepth;
			resize(size);
		}

		// Force area change on Tree, invalidates this and all child layers
		void resize(const rectF& rArea)
		{
			// Erase this layer
			clear();

			// Recalculate area of children
			m_rect = rArea;
			const float width = m_rect.w / 2.0f;
			const float height = m_rect.h / 2.0f;

			m_rChild[0] = rectF(m_rect.x, m_rect.y, width, height);
			m_rChild[1] = rectF(m_rect.x + width, m_rect.y, width, height);
			m_rChild[2] = rectF(m_rect.x, m_rect.y + height, width, height);
			m_rChild[3] = rectF(m_rect.x + width, m_rect.y + height, width, height);
		}

		// Clears the contents of this layer, and all child layers
		void clear()
		{
			// Erase any items stored in this layer
			m_pItems.clear();

			// Iterate through children, erase them too
			for (int i = 0; i < 4; i++)
			{
				if (m_pChild[i])
					m_pChild[i]->clear();
				m_pChild[i].reset();
			}
		}

		// Returns a count of how many items are stored in this layer, and all children of this layer
		size_t size() const
		{
			size_t nCount = m_pItems.size();
			for (int i = 0; i < 4; i++)
				if (m_pChild[i]) nCount += m_pChild[i]->size();
			return nCount;
		}

		// Inserts an object into this layer (or appropriate child layer), given the area the item occupies
		void insert(const OBJECT_TYPE& item, const rectF& itemsize)
		{
			// Check each child
			for (int i = 0; i < 4; i++)
			{
				// If the child can wholly contain the item being inserted
				if (m_rChild[i].contains_rect(itemsize))
				{
					// Have we reached depth limit?
					if (m_depth + 1 < MAX_DEPTH)
					{
						// No, so does child exist?
						if (!m_pChild[i])
						{
							// No, so create it
							m_pChild[i] = std::make_unique<StaticQuadTree<OBJECT_TYPE>>(m_rChild[i], m_depth + 1);
						}

						// Yes, so add item to it
						m_pChild[i]->insert(item, itemsize);
						return;
					}
				}
			}

			// It didnt fit, so item must belong to this quad
			m_pItems.push_back({ itemsize, item });
		}

		// Returns a list of objects in the given search area
		std::list<OBJECT_TYPE> search(const rectF& rArea) const
		{
			std::list<OBJECT_TYPE> listItems;
			search(rArea, listItems);
			return listItems;
		}

		// Returns the objects in the given search area, by adding to supplied list
		void search(const rectF& rArea, std::list<OBJECT_TYPE>& listItems) const
		{
			// First, check for items belonging to this area, add them to the list
			// if there is overlap
			for (const auto& p : m_pItems)
			{
				if (rArea.intersects_rect(p.first))
					listItems.push_back(p.second);
			}

			// Second, recurse through children and see if they can
			// add to the list
			for (int i = 0; i < 4; i++)
			{
				if (m_pChild[i])
				{
					// If child is entirely contained within area, recursively
					// add all of its children, no need to check boundaries
					if (rArea.contains_rect(m_rChild[i]))
						m_pChild[i]->items(listItems);

					// If child overlaps with search area then checks need
					// to be made
					else if (m_rChild[i].intersects_rect(rArea))
						m_pChild[i]->search(rArea, listItems);
				}
			}
		}

		void items(std::list<OBJECT_TYPE>& listItems) const
		{
			// No questions asked, just return child items
			for (const auto& p : m_pItems)
				listItems.push_back(p.second);

			// Now add children of this layer's items
			for (int i = 0; i < 4; i++)
				if (m_pChild[i]) m_pChild[i]->items(listItems);
		}


		std::list<OBJECT_TYPE> items() const
		{
			// No questions asked, just return child items
			std::list<OBJECT_TYPE> listItems;
			items(listItems);
			return listItems;
		}

		// Returns area of this layer
		const rectF& area()
		{
			return m_rect;
		}


	protected:
		// Depth of this StaticQuadTree layer
		size_t m_depth = 0;

		// Area of this StaticQuadTree
		rectF m_rect;

		// 4 child areas of this StaticQuadTree
		std::array<rectF, 4> m_rChild{};

		// 4 potential children of this StaticQuadTree
		std::array<std::unique_ptr<StaticQuadTree<OBJECT_TYPE>>, 4> m_pChild{};

		// Items which belong to this StaticQuadTree
		std::vector<std::pair<rectF, OBJECT_TYPE>> m_pItems;
	};


	template <typename OBJECT_TYPE>
	class StaticQuadTreeContainer
	{
		// Using a std::list as we dont want pointers to be invalidated to objects stored in the
		// tree should the contents of the tree change
		using QuadTreeContainer = std::list<OBJECT_TYPE>;

	protected:
		// The actual container
		QuadTreeContainer m_allItems;

		// Use our StaticQuadTree to store "pointers" instead of objects - this reduces
		// overheads when moving or copying objects
		StaticQuadTree<typename QuadTreeContainer::iterator> root;

	public:
		StaticQuadTreeContainer(const rectF& size, const size_t nDepth = 0) : root(size, nDepth)
		{

		}

		// Sets the spatial coverage area of the quadtree
		// Invalidates tree
		void resize(const rectF& rArea)
		{
			root.resize(rArea);
		}

		// Returns number of items within tree
		size_t size() const
		{
			return m_allItems.size();
		}

		// Returns true if tree is empty
		bool empty() const
		{
			return m_allItems.empty();
		}

		// Removes all items from tree
		void clear()
		{
			root.clear();
			m_allItems.clear();
		}


		// Convenience functions for ranged for loop
		typename QuadTreeContainer::iterator begin()
		{
			return m_allItems.begin();
		}

		typename QuadTreeContainer::iterator end()
		{
			return m_allItems.end();
		}

		typename QuadTreeContainer::const_iterator cbegin()
		{
			return m_allItems.cbegin();
		}

		typename QuadTreeContainer::const_iterator cend()
		{
			return m_allItems.cend();
		}


		// Insert item into tree in specified area
		void insert(const OBJECT_TYPE& item, const rectF& itemsize)
		{
			// Item is stored in container
			m_allItems.push_back(item);

			// Pointer/Area of item is stored in quad tree
			root.insert(std::prev(m_allItems.end()), itemsize);
		}

		// Returns a std::list of pointers to items within the search area
		std::list<typename QuadTreeContainer::iterator> search(const rectF& rArea) const
		{
			std::list<typename QuadTreeContainer::iterator> listItemPointers;
			root.search(rArea, listItemPointers);
			return listItemPointers;
		}

	};

	*/

	/*
	template <typename OBJECT_TYPE>
		requires IS_RULE_OF_FIVE_CLASS_T<OBJECT_TYPE>
	class QuadTree {

		static constexpr std::size_t FOR_EACH_WINDOW = 4;
		static constexpr std::size_t MAX_DEPTH = 8;

		struct QuadTreePair {
			rectF location;
			OBJECT_TYPE obj;
		};

		class QuadTreeBody {
		public:

			QuadTreeBody(const rectF& window, std::size_t depth = 0):mDepth(depth) {
				resize(window);
			}

			void resize(const rectF& newArea) {
				clear();
				const float width = newArea.w / 2.0f;
				const float height = newArea.h / 2.0f;

				mBranchPos[0] = rectF(newArea.x, newArea.y, width, height);
				mBranchPos[1] = rectF(newArea.x + width, newArea.y, width, height);
				mBranchPos[2] = rectF(newArea.x, newArea.y + height, width, height);
				mBranchPos[3] = rectF(newArea.x + width, newArea.y + height, width, height);

				mWindow = newArea;
			}
			void clear() {
				mObjects.clear();

				for (int i = 0; i < FOR_EACH_WINDOW; i++) {
					if (mBranchStorage[i]) {
						mBranchStorage[i]->clear();
						mBranchStorage[i].reset();
					}
				}
			}

			void insert(QuadTreePair& pair) {

				//first, check structures depth limit, if going deeper is fine, try going deeper
				if (mDepth + 1 < MAX_DEPTH) {

					//check all 4 branches
					for (int i = 0; i < FOR_EACH_WINDOW; i++) {
						//check which window the object exists in (this check wholely; in case all 4 checks fail, it must mean we add to this layer as this layer is larger)
						if (!mBranchPos[i].contains_rect(pair.location))
							continue;

						//check for nullptr and initalize the branch if not yet set
						if (!mBranchStorage[i])
							mBranchStorage[i] = std::make_unique<QuadTreeBody>(mBranchPos[i], mDepth + 1);

						//pass the item down the chain
						mBranchStorage[i]->insert(pair);
						//return early as there is now nothing else to do
						return;
					}

				}

				//in case the depth limit is reached or the objectArea doesn't fit into any sub branches, add it here
				mObjects.element_create(&pair);
			}

			void collect(const rectF& searchArea, SequenceM<OBJECT_TYPE*>& collecter) {

				//first check for items belonging to this layer
				for (auto& each : mObjects) {

					if (searchArea.intersects_rect(each->location))
						collecter.element_create(&each->obj);
				}

				//second check all branches and get their objects
				for (int i = 0; i < FOR_EACH_WINDOW; i++) {
					//check for nullptr, if null then it means there is nothing there
					if (!mBranchStorage[i]) continue;

					//check if searched area contains branch wholely, if yes then just add everything there
					if (searchArea.contains_rect(mBranchPos[i])) {
						mBranchStorage[i]->collect_unconditional(collecter);
					}//if it does not contain whole but does insersect then need recursive call for percise items
					else if (mBranchPos[i].intersects_rect(searchArea)) {
						mBranchStorage[i]->collect(searchArea, collecter);
					}
				}
			}
		private:

			void collect_unconditional(SequenceM<OBJECT_TYPE*>& collecter) {
				//first add all items from this layer
				for (auto& each : mObjects) {
					collecter.element_create(&each->obj);
				}
				//secondly add all items from branches
				for (int i = 0; i < FOR_EACH_WINDOW; i++) {

					if (mBranchStorage[i]) {
						mBranchStorage[i]->collect_unconditional(collecter);
					}
				}
			}

		private:
			//LAYER DEPTH
			std::size_t mDepth;

			//THE SIZE OF THIS WINDOW
			rectF mWindow;

			//BRANCHES, FIRST SIMPLY POSITION, SECOND THEIR SUBSTORAGE
			std::array<rectF, 4> mBranchPos;
			std::array<std::unique_ptr<QuadTreeBody>, 4> mBranchStorage;

			//STORE ALL OBJECTS BY VALUE, THIS IS THEIR OWNER
			SequenceM<QuadTreePair*> mObjects;
		};

	public:

		QuadTree(const rectF& window, std::size_t depth = 0):mRoot(window, depth), mTopLevelWindow(window){}
		// Sets the spatial coverage area of the quadtree
		// Invalidates tree

		void resize(const rectF& newArea){
			mRoot.resize(newArea);
		}
		void clear() {
			mRoot.clear();
			mAllObjects.clear();
		}
		std::size_t size() const {
			return mAllObjects.size_in_use();
		}
		bool empty() const {
			return mAllObjects.empty_in_use();
		}

		auto begin(){
			return mAllObjects.begin();
		}

		auto end(){
			return mAllObjects.end();
		}

		auto cbegin(){
			return mAllObjects.cbegin();
		}

		auto cend(){
			return mAllObjects.cend();
		}

		template<typename U>
			requires std::same_as<std::remove_cvref_t<U>, OBJECT_TYPE>
		void add_to_queue(U&& item, rectF itemsize){
			// Item is stored in pending
			mPendingInsert.element_create(QuadTreePair(std::move(itemsize), std::forward<U>(item)));
		}
		void update_queue() {
			// THIS WILL CAUSE POINTER INVALIDATION AS FUCK IF NOT DONE IN TWO STAGES
			// UNFORTUNATELY, IF mAllObject RESIZES, THE ENTIRE TREE IS KAPUT

			//first add pending items to all obj list. it can now invalidate old shit
			for (auto&& each : mPendingInsert) {
				mAllObjects.element_create(std::move(each));
			}
			//rebuild the tree (yikes but is what it is???)
			QuadTreeBody newBody(mTopLevelWindow);
			for (auto& each : mAllObjects) {
				newBody.insert(each);
			}
			//move assign new body into old (since QuadTreeBody does not own any unique memory, basic assignment should tidy everything up itself)
			mRoot = std::move(newBody);
			//clear up pending items
			mPendingInsert.clear();
		}

		SequenceM<OBJECT_TYPE*> search(const rectF& Area) {
			SequenceM<OBJECT_TYPE*> list;
			list.set_growth_resist_low();
			mRoot.collect(Area, list);
			return list;
		}

		//for remove, don't need to rebuild the whole tree.
	private:

		QuadTreeBody mRoot;//BAD AS LONG AS NOT YET DYNAMIC

		rectF mTopLevelWindow;

		SequenceM<QuadTreePair> mAllObjects;
		SequenceM<QuadTreePair> mPendingInsert;
	};

	*/

	template <typename OBJECT_TYPE>
		requires IS_RULE_OF_FIVE_CLASS_T<OBJECT_TYPE>
	class QuadTree {

		static constexpr std::size_t FOR_EACH_WINDOW = 4;
		static constexpr std::size_t MAX_DEPTH = 8;

		class QuadTreeBody {
		public:

			QuadTreeBody(const rectF& window, std::size_t depth = 0) :mDepth(depth) {
				resize(window);
			}

			void resize(const rectF& newArea) {
				clear();
				const float width = newArea.w / 2.0f;
				const float height = newArea.h / 2.0f;

				mBranchPos[0] = rectF(newArea.x, newArea.y, width, height);
				mBranchPos[1] = rectF(newArea.x + width, newArea.y, width, height);
				mBranchPos[2] = rectF(newArea.x, newArea.y + height, width, height);
				mBranchPos[3] = rectF(newArea.x + width, newArea.y + height, width, height);

				mWindow = newArea;
			}
			void clear() {
				mObjects.clear();

				for (int i = 0; i < FOR_EACH_WINDOW; i++) {
					if (mBranchStorage[i]) {
						mBranchStorage[i]->clear();
						mBranchStorage[i].reset();
					}
				}
			}
			void insert(const rectF& location, std::size_t index) {

				//first, check structures depth limit, if going deeper is fine, try going deeper
				if (mDepth + 1 < MAX_DEPTH) {

					//check all 4 branches
					for (int i = 0; i < FOR_EACH_WINDOW; i++) {
						//check which window the object exists in (this check wholely; in case all 4 checks fail, it must mean we add to this layer as this layer is larger)
						if (!mBranchPos[i].contains_rect(location))
							continue;

						//check for nullptr and initalize the branch if not yet set
						if (!mBranchStorage[i])
							mBranchStorage[i] = std::make_unique<QuadTreeBody>(mBranchPos[i], mDepth + 1);

						//pass the item down the chain
						mBranchStorage[i]->insert(location, index);
						//return early as there is now nothing else to do
						return;
					}
				}
				//in case the depth limit is reached or the objectArea doesn't fit into any sub branches, add it here
				mObjects.element_create(location, index);
			}
			void collect(const rectF& searchArea, SequenceM<std::size_t>& collecter) {

				//first check for items belonging to this layer
				for (auto& each : mObjects) {
					if (searchArea.intersects_rect(each.first))
						collecter.element_create(each.second);
				}

				//second check all branches and get their objects
				for (int i = 0; i < FOR_EACH_WINDOW; i++) {
					//check for nullptr, if null then it means there is nothing there
					if (!mBranchStorage[i]) continue;

					//check if searched area contains branch wholely, if yes then just add everything there
					if (searchArea.contains_rect(mBranchPos[i])) {
						mBranchStorage[i]->collect_unconditional(collecter);
					}//if it does not contain whole but does insersect then need recursive call for percise items
					else if (mBranchPos[i].intersects_rect(searchArea)) {
						mBranchStorage[i]->collect(searchArea, collecter);
					}
				}
			}
		private:

			void collect_unconditional(SequenceM<std::size_t>& collecter) {
				//first add all items from this layer
				for (auto& each : mObjects) {
					collecter.element_create(each.second);
				}
				//secondly add all items from branches
				for (int i = 0; i < FOR_EACH_WINDOW; i++) {
					if (mBranchStorage[i])
						mBranchStorage[i]->collect_unconditional(collecter);
				}
			}

		private:
			//LAYER DEPTH
			std::size_t mDepth;

			//THE SIZE OF THIS WINDOW
			rectF mWindow;

			//BRANCHES, FIRST SIMPLY POSITION, SECOND THEIR SUBSTORAGE
			std::array<rectF, 4> mBranchPos;
			std::array<std::unique_ptr<QuadTreeBody>, 4> mBranchStorage;

			//STORE ALL OBJECTS BY VALUE, THIS IS THEIR OWNER
			SequenceM<std::pair<rectF, std::size_t>> mObjects;
		};

	public:

		QuadTree(const rectF& window, std::size_t depth = 0) :mRoot(window, depth) {}
		// Sets the spatial coverage area of the quadtree
		// Invalidates tree

		void resize(const rectF& newArea) {
			mRoot.resize(newArea);
		}
		void clear() {
			mRoot.clear();
			mAllObjects.clear();
		}
		std::size_t size() const {
			return mAllObjects.size_in_use();
		}
		bool empty() const {
			return mAllObjects.empty_in_use();
		}

		auto begin() {
			return mAllObjects.begin();
		}
		auto end() {
			return mAllObjects.end();
		}
		auto cbegin() {
			return mAllObjects.cbegin();
		}
		auto cend() {
			return mAllObjects.cend();
		}

		template<typename U>
			requires std::same_as<std::remove_cvref_t<U>, OBJECT_TYPE>
		void insert(U&& item, rectF itemsize) {
			// Item is stored in pending
			//mPendingInsert.element_create(QuadTreePair(std::move(itemsize), std::forward<U>(item)));

			mAllObjects.element_create(std::forward<U>(item));
			mRoot.insert(itemsize, mAllObjects.size_in_use() - 1);//size_in_use returns last index
		}

		SequenceM<OBJECT_TYPE*> search(const rectF& Area) {

			SequenceM<std::size_t> collectIndex;
			collectIndex.set_growth_resist_low();

			mRoot.collect(Area, collectIndex);


			SequenceM<OBJECT_TYPE*> collectedReferences;

			for (auto& index : collectIndex) {
				collectedReferences.element_create(&mAllObjects[index]);
			}

			return collectedReferences;
		}

		//for remove, don't need to rebuild the whole tree.
	private:

		QuadTreeBody mRoot;//BAD AS LONG AS NOT YET DYNAMIC 
		SequenceM<OBJECT_TYPE> mAllObjects;
	};

}
