#pragma once
#include "Rectangle.h"
#include <memory>
#include <array>

#include <optional>
#include "SequenceM.h"
#include <map>

#include "Color.h"

//TODO:: currently if the tree is large but removes items, there is no way to trim down the mem. just add it later
namespace badEngine {

	struct SomeObjWithArea {
		rectF rect;
		vec2f vel;
		Color col;
	};

	//template <typename OBJECT_TYPE>
	//	requires IS_RULE_OF_FIVE_CLASS_T<OBJECT_TYPE>
	class QuadTree {

		static constexpr std::size_t FOUR_WINDOWS = 4;
		static constexpr std::size_t MAX_DEPTH = 8;

		class QuadWindow;

		struct ManagerNode {
			QuadWindow* mWorkingWindow = nullptr;
			std::size_t mWorkerIndex = 0;
			bool isEmpty = true;
		};
		struct WorkerNode {
			SomeObjWithArea mWorker;
			rectF mWorkerArea;
			std::size_t mManagerIndex = 0;
		};

		class QuadWindow {
		private:

			struct SubWindow {
				rectF mArea;
				std::unique_ptr<QuadWindow> mStorage;
			};

		public:

			QuadWindow(const rectF& window, std::size_t depth) {
				const float width = window.w / 2.0f;
				const float height = window.h / 2.0f;

				mSubWindows[0].mArea = rectF(window.x, window.y, width, height);
				mSubWindows[1].mArea = rectF(window.x + width, window.y, width, height);
				mSubWindows[2].mArea = rectF(window.x, window.y + height, width, height);
				mSubWindows[3].mArea = rectF(window.x + width, window.y + height, width, height);

				mWindow = window;
				mDepth = depth;
			}

			void clear() {
				mWorkers.clear();

				for (int i = 0; i < FOUR_WINDOWS; i++) {
					if (mSubWindows[i].mStorage) {
						mSubWindows[i].mStorage->clear();
						mSubWindows[i].mStorage.reset();
					}
				}
			}


			ManagerNode insert(SomeObjWithArea&& payload, const rectF& workingArea, std::size_t managerIndex) {

				//first, check structures depth limit, if going deeper is fine, try going deeper
				if (mDepth + 1 < MAX_DEPTH) {

					//check all 4 branches
					for (auto& subwindow : mSubWindows) {
						//check which window the object exists in (this check wholely; in case all 4 checks fail, it must mean we add to this layer as this layer is larger)
						if (!subwindow.mArea.contains_rect(workingArea)) continue;

						//check for nullptr and initalize the branch if not yet set
						if (!subwindow.mStorage)
							subwindow.mStorage = std::make_unique<QuadWindow>(subwindow.mArea, mDepth + 1);

						//pass the item down the chain
						return subwindow.mStorage->insert(std::move(payload), workingArea, managerIndex);
					}
				}
				//in case the depth limit is reached or the objectArea doesn't fit into any sub branches, add it here
				mWorkers.element_create(std::move(payload), workingArea, managerIndex);
				//return back the metadata so that parent knows where it's child is
				return ManagerNode(this, mWorkers.size_in_use() - 1, false);
			}
			void collect(const rectF& searchArea, SequenceM<std::size_t>& collector)const noexcept {
				//first check for items belonging to this layer
				for (const auto& worker : mWorkers)
					if (searchArea.intersects_rect(worker.mWorkerArea))
						collector.element_create(worker.mManagerIndex);

				//second check all branches and get their objects
				for (const auto& subwindow : mSubWindows) {
					//check for nullptr, if null then it means there is nothing there
					if (!subwindow.mStorage)continue;

					if (searchArea.contains_rect(subwindow.mArea))//if contains whole then add everything
						subwindow.mStorage->collect_all(collector);
					else if (searchArea.intersects_rect(subwindow.mArea))//if not contains whole then detailed search
						subwindow.mStorage->collect(searchArea, collector);
				}
			}

			SomeObjWithArea& operator[](std::size_t index)noexcept {//you know what, if you can't index right, you deserve termination
				return mWorkers[index].mWorker;
			}
			const SomeObjWithArea& operator[](std::size_t& index)const noexcept{
				return mWorkers[index].mWorker;
			}

		private:

			void collect_all(SequenceM<std::size_t>& collector)const noexcept {
				//first add all items from this layer
				for (const auto& worker : mWorkers)
					collector.element_create(worker.mManagerIndex);
				//secondly add all items from branches
				for (const auto& subwindow : mSubWindows)
					if (subwindow.mStorage)
						subwindow.mStorage->collect_all(collector);
			}

			void init(const rectF& newArea)noexcept {
				clear();
				const float width = newArea.w / 2.0f;
				const float height = newArea.h / 2.0f;

				mSubWindows[0].mArea = rectF(newArea.x, newArea.y, width, height);
				mSubWindows[1].mArea = rectF(newArea.x + width, newArea.y, width, height);
				mSubWindows[2].mArea = rectF(newArea.x, newArea.y + height, width, height);
				mSubWindows[3].mArea = rectF(newArea.x + width, newArea.y + height, width, height);

				mWindow = newArea;
			}
		private:
			//LAYER DEPTH
			std::size_t mDepth = 0;

			//THIS WINDOW
			rectF mWindow;

			//POINTERS TO OTHER WINDOWNS NESTED INSIDE
			std::array<SubWindow, FOUR_WINDOWS> mSubWindows;

			//STORE PAYLOAD
			SequenceM<WorkerNode> mWorkers;
		};

	public:

		QuadTree(const rectF& window) :mRoot(window, 0), topLevelWindow(window) {}

		//template<typename U>
		//	requires std::same_as<std::remove_cvref_t<U>, OBJECT_TYPE>
		void insert(SomeObjWithArea&& item, rectF itemsize) {

			if (!is_within_scope(itemsize))
				return;
			//forward the data, node data is invalid here
			const std::size_t currentLastIndex = mManagers.size_in_use();
			mManagers.element_create(
				mRoot.insert(std::move(item), itemsize, currentLastIndex)
			);
		}
		bool is_within_scope(const rectF& rect)const noexcept {
			return topLevelWindow.contains_rect(rect);
		}

		SequenceM<std::size_t> search_area(const rectF& area)const noexcept {

			//collect index, set growth to minimum resist to splurge mem
			SequenceM<std::size_t> collector;
			collector.set_growth_resist_low();
			//actually collect recursively
			mRoot.collect(area, collector);

			return collector;
		}

		SomeObjWithArea& operator[](std::size_t index)noexcept {
			return (*mManagers[index].mWorkingWindow)[mManagers[index].mWorkerIndex];
		}
		const SomeObjWithArea& operator[](std::size_t index) const noexcept {
			return (*mManagers[index].mWorkingWindow)[mManagers[index].mWorkerIndex];
		}

	private:
		QuadWindow mRoot;
		SequenceM<ManagerNode> mManagers;
		rectF topLevelWindow;
	};
}
/*
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

				void remove_list(const SequenceM<std::size_t>& idList) {
			//if there is a list of items to delete and the actual deletion happens one by one
			//there is a possibility that the index to be deleted becomes out of range at some point
			//to address this the list MUST be sorted from greater to smaller
			//alternative to calling this function is sorting the list yourself
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
*/