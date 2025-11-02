#pragma once
#include "Rectangle.h"
#include <memory>
#include <array>

#include "SequenceM.h"
//STATIC QUADTREE

namespace badEngine {

	template <typename OBJECT_TYPE>
		requires IS_RULE_OF_FIVE_CLASS_T<OBJECT_TYPE>
	class QuadTree {

		static constexpr std::size_t FOR_EACH_WINDOW = 4;
		static constexpr std::size_t MAX_DEPTH = 8;

		class QuadTreeBody;

		struct ParentNode {
			OBJECT_TYPE mData;
			QuadTreeBody* mChild = nullptr;
			std::size_t mChildLocalIndex = 0;
		};
		struct ChildNode {
			rectF mOccupiedArea;
			std::size_t mParentIndex = 0;
		};

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
				mChildNodes.clear();

				for (int i = 0; i < FOR_EACH_WINDOW; i++) {
					if (mBranchStorage[i]) {
						mBranchStorage[i]->clear();
						mBranchStorage[i].reset();
					}
				}
			}
			
			void insert(const rectF& occupiedArea, std::size_t parentIndex, std::size_t& childIndex, QuadTreeBody*& child) {

				//first, check structures depth limit, if going deeper is fine, try going deeper
				if (mDepth + 1 < MAX_DEPTH) {

					//check all 4 branches
					for (int i = 0; i < FOR_EACH_WINDOW; i++) {
						//check which window the object exists in (this check wholely; in case all 4 checks fail, it must mean we add to this layer as this layer is larger)
						if (!mBranchPos[i].contains_rect(occupiedArea))
							continue;

						//check for nullptr and initalize the branch if not yet set
						if (!mBranchStorage[i])
							mBranchStorage[i] = std::make_unique<QuadTreeBody>(mBranchPos[i], mDepth + 1);

						//pass the item down the chain
						mBranchStorage[i]->insert(occupiedArea, parentIndex, childIndex, child);
						return;
					}
				}
				//in case the depth limit is reached or the objectArea doesn't fit into any sub branches, add it here
				mChildNodes.element_create(occupiedArea, parentIndex);
				//return back the metadata so that parent knows where it's child is
				childIndex = mChildNodes.size_in_use() - 1;
				child = this;
			}
			void collect(const rectF& searchArea, SequenceM<std::size_t>& collecter) {

				//first check for items belonging to this layer
				for (auto& node : mChildNodes) {
					if (searchArea.intersects_rect(node.mOccupiedArea))
						collecter.element_create(node.mParentIndex);
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
		
			/*
			template<typename Func>
			bool remove(std::size_t index, Func func) {
				if (mSizeIndexPairs.size_in_use() < index) {
					return false;
				}
				//since global knows local, and local knows global BUT global gets removed at the end, no other information is mutated
				mSizeIndexPairs.depricate_unordered(mSizeIndexPairs.begin() + index);
				return true;
			}
			*/
		private:

			void collect_unconditional(SequenceM<std::size_t>& collecter) {
				//first add all items from this layer
				for (auto& node : mChildNodes) {
					collecter.element_create(node.mParentIndex);
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
			SequenceM<ChildNode> mChildNodes;
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
			//forward the data, node data is invalid here
			mAllObjects.element_create(std::forward<U>(item), nullptr, NULL);
			//get the index into which data got inserted into
			const std::size_t globalIndex = mAllObjects.size_in_use() - 1;
			//pass in the required data down to child, the child will tell the parent back where it exists
			mRoot.insert(itemsize, globalIndex, mAllObjects[globalIndex].mChildLocalIndex, mAllObjects[globalIndex].mChild);
		}

		SequenceM<std::size_t> search_index(const rectF& Area) {

			//collect index, set growth to minimum resist to splurge mem
			SequenceM<std::size_t> collectIndex;
			collectIndex.set_growth_resist_low();
			//actually collect recursively
			mRoot.collect(Area, collectIndex);

			return collectIndex;
		}

		OBJECT_TYPE& operator[](std::size_t index) {
			return mAllObjects[index].mData;
		}
		const OBJECT_TYPE& operator[](std::size_t index)const {
			return mAllObjects[index].mData;
		}
		/*
		bool remove(std::size_t index) {
			struct ass {
				QuadTreeBody* node = nullptr;
				std::size_t localIndex = 0;
			};
			SequenceM<std::pair<rectF, std::size_t>> local;
			SequenceM<std::pair<OBJECT_TYPE, ass>> global;

			//invalid index or something
			if (mAllObjects.size_in_use() < index) {
				return false;
			}

			auto& REMOVE_NODE = mAllObjects[index].second;
			std::size_t REMOVE_INDEX = index;

			auto& MOVE_NODE = mAllObjects.back().second;
			std::size_t MOVE_INDEX = mAllObjects.size_in_use() - 1;

			//FIRST REMOVE FROM REMOVE

			return true;
		}
		*/

	private:

		QuadTreeBody mRoot;

		//NOW GLOBAL KNOWS WHERE IT IS STORED IN LOCAL, AND LOCAL KNOWS WHERE IT IS STORED IN GLOBAL

		SequenceM<ParentNode> mAllObjects;
	};

}
