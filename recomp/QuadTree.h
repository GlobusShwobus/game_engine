#pragma once
#include "Rectangle.h"
#include <memory>
#include <array>

#include "SequenceM.h"
//STATIC QUADTREE

namespace badEngine {

	template <typename OBJECT_TYPE>
	class QuadTree {

		static constexpr std::size_t FOR_EACH_WINDOW = 4;
		static constexpr std::size_t MAX_DEPTH = 8;

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
					}
					mBranchStorage[i].reset();
				}
			}
			
			void insert(typename SequenceM<OBJECT_TYPE>::iterator objectIter,  rectF&& objectArea) {

				//first, check structures depth limit, if going deeper is fine, try going deeper
				if (mDepth + 1 < MAX_DEPTH) {

					//check all 4 branches
					for (int i = 0; i < FOR_EACH_WINDOW; i++) {
						//check which window the object exists in (this check wholely; in case all 4 checks fail, it must mean we add to this layer as this layer is larger)
						if (!mBranchPos[i].contains_rect(objectArea))
							continue;

						//check for nullptr and initalize the branch if not yet set
						if (!mBranchStorage[i])
							mBranchStorage[i] = std::make_unique<QuadTreeBody>(mBranchPos[i], mDepth + 1);

						//pass the item down the chain
						mBranchStorage[i]->insert(objectIter, std::move(objectArea));
						//return early as there is now nothing else to do
						return;
					}

				}

				//in case the depth limit is reached or the objectArea doesn't fit into any sub branches, add it here
				mObjects.element_create(std::move(objectArea), objectIter);
			}

			void collect(const rectF& searchArea, SequenceM<typename SequenceM<OBJECT_TYPE>::iterator>& collecter) {

				//first check for items belonging to this layer
				for (auto& item : mObjects) {

					if (searchArea.intersects_rect(item.first))
						collecter.element_create(item.second);
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

			void collect_unconditional(SequenceM<typename SequenceM<OBJECT_TYPE>::iterator>& collecter) {
				//first add all items from this layer
				for (auto& each : mObjects) {
					collecter.element_create(each.second);
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
			SequenceM<std::pair<rectF, typename SequenceM<OBJECT_TYPE>::iterator>> mObjects;
		};

	public:

		QuadTree(const rectF& window, std::size_t depth = 0):mRoot(window, depth){}
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
			mPendingObjects.element_create(std::move(itemsize), std::forward<U>(item));
		}
		void update_queue() {
			for (auto& each : mPendingObjects) {
				mAllObjects.element_create(std::move(each.second));

				mRoot.insert(mAllObjects.end() - 1, std::move(each.first));
			}
			mPendingObjects.clear();
		}

		SequenceM<typename SequenceM<OBJECT_TYPE>::iterator> search(const rectF& Area) {
			SequenceM<typename SequenceM<OBJECT_TYPE>::iterator> list;
			list.set_growth_resist_low();
			mRoot.collect(Area, list);
			return list;
		}


	private:

		QuadTreeBody mRoot;//BAD AS LONG AS NOT YET DYNAMIC

		SequenceM<OBJECT_TYPE> mAllObjects;
		SequenceM<std::pair<rectF, OBJECT_TYPE>> mPendingObjects;
	};

}
