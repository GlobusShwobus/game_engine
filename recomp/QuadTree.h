#pragma once
#include "Rectangle.h"
#include <memory>
#include <array>

#include <optional>
#include "SequenceM.h"

//TODO:: currently if the tree is large but removes items, there is no way to trim down the mem. just add it later
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
						if (!mBranchStorage[i]) {
							mBranchStorage[i] = std::make_unique<QuadTreeBody>(mBranchPos[i], mDepth + 1);
						}

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
		
			
			bool remove(std::size_t localIndex, std::size_t parentIndex, std::optional<std::size_t>& newParentOfLocalIndex) {
				//check if index is valid at all
				if (mChildNodes.size_in_use() <= localIndex) {
					return false;
				}
				//check if the observers match
				if (!(mChildNodes[localIndex].mParentIndex == parentIndex)) {
					return false;
				}

				//if the observers match, and we are about to swap the places in local vector between localIndex(removed item)
				//with the last item, then we must also communicate back who is the new parent of localIndex
				//we already know local index, but not the parent
				//there is also the possibility that the removed item is the last item, in which case the container is actually now empty
				//second possiblity is that the removed item is the last item but the container is not empty afterwards
				//thus the simplest is telling size_t is nullptr

				//swap places between local and last and remove the new last
				mChildNodes.depricate_unordered(mChildNodes.begin() + localIndex);

				if (!mChildNodes.empty_in_use() && mChildNodes.size_in_use() > localIndex) {
					newParentOfLocalIndex = mChildNodes[localIndex].mParentIndex;
				}
				else {
					newParentOfLocalIndex.reset();
				}
				return true;
			}
			void assign_new_parent(std::size_t localIndex, std::size_t newParent) {
				//check validity first
				if (localIndex < mChildNodes.size_in_use()) {
					//change observer
					mChildNodes[localIndex].mParentIndex = newParent;
				}
			}
		
			std::size_t get_parent_index(std::size_t localIndex) {
				return mChildNodes[localIndex].mParentIndex;
			}
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
		
		void remove_list(const SequenceM<std::size_t>& idList) {
			//if there is a list of items to delete and the actual deletion happens one by one
			//there is a problem if deleting in the right order because of the ways deletion is handled internally
			//to avoid such errors the deleted IDs should be sorted from greater to smaller order
			SequenceM<std::size_t> sortedList = idList;
			std::sort(sortedList.begin(), sortedList.end(), std::greater<>());

			for (auto& id: sortedList) {
				remove(id);
			}
		}
		void remove(std::size_t removeIndex) {
			//invalid index or something
			if (removeIndex >= mAllObjects.size_in_use()) {
				throw std::runtime_error("invalid index");
			}

			//first step is to get rid of the index (the simple step)
			auto& REMOVE_NODE = mAllObjects[removeIndex];

			std::optional<std::size_t> newParentOfLocalIndex;

			if (REMOVE_NODE.mChild->remove(REMOVE_NODE.mChildLocalIndex, removeIndex, newParentOfLocalIndex)) {
				//if successful then the parent needs to told about what correct child to look at
				//the body of the child is still correct, the only things that swaped were observers
				//also there is potential that child was now empty, in which case no parent to be updated and move on to remove the node
				if (newParentOfLocalIndex.has_value()) {
					mAllObjects[newParentOfLocalIndex.value()].mChildLocalIndex = REMOVE_NODE.mChildLocalIndex;
				}
			}
			else {
				throw std::runtime_error("index missmatch between parent and child nodes");
			}
			//second step, once dealt with swaping indexes and making sure correct parent and child observe each other
			//is to get rid of REMOVE_NODE. REMOVE_NODE is now a junk node because we dealt with observer pattern mumbo jump already
			//the question is if we now swap places of this and last item and then remove the last, do we also need to
			//take care of the nodes of the last item?
			//before, the parent had to be notified about changing child
			//but now the child has to be notifed of changing parent
			//there is also the possibility of the removed node being the last node, in which case the step can be skiped

			const std::size_t lastIndex = mAllObjects.size_in_use() - 1;
			if (removeIndex != lastIndex) {
				auto& CURRENT_LAST_NODE = mAllObjects.back();
				CURRENT_LAST_NODE.mChild->assign_new_parent(CURRENT_LAST_NODE.mChildLocalIndex, removeIndex);
			}

			//finally remove the piece of shit
			mAllObjects.depricate_unordered(mAllObjects.begin() + removeIndex);
		}

	private:

		QuadTreeBody mRoot;

		//NOW GLOBAL KNOWS WHERE IT IS STORED IN LOCAL, AND LOCAL KNOWS WHERE IT IS STORED IN GLOBAL

		SequenceM<ParentNode> mAllObjects;
	};
}
