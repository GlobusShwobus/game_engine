#pragma once
#include "Rectangle.h"
#include "SequenceM.h"

#include <memory>
#include <array>
#include <optional>

#include "Color.h"//temp

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
				clear();
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
				return ManagerNode(this, mWorkers.size_in_use() - 1);
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


			bool remove(std::size_t workerIndex, std::size_t managerIndex, std::optional<std::size_t>& newManagerIndex) {
				//check if index is valid at all
				if (mWorkers.size_in_use() <= workerIndex) 
					return false;
				//check if the observers match (maybe throw is better?)
				if (!(mWorkers[workerIndex].mManagerIndex == managerIndex))
					return false;
			
				//removing an object is done in an unordered way. object somewhere in the middle gets swaped with the last object and the new last is then cut off
				//this means that the previously last, but now in a new index worker has the wrong manager and needs to be updated
				// TWO THINGS TO KEEP IN MIND: 1) if removed item is the last item, then no new manager is to be updated
				//                             2) if the container is empty afterwards, then also no new manager. no worker no manager


				//swap places between local and last and remove the new last
				mWorkers.depricate_unordered(mWorkers.begin() + workerIndex);

				//check if the conainer after deprication is not empty and if we didn't remove the last item in the last step
				if (!mWorkers.empty_in_use() && !(mWorkers.size_in_use() == workerIndex)) {
					newManagerIndex = mWorkers[workerIndex].mManagerIndex;
				}
				else {
					newManagerIndex.reset();
				}
				return true;
			}
			void assign_new_parent(std::size_t localIndex, std::size_t newManager)noexcept {
				//check validity first and swap managers
				if (localIndex < mWorkers.size_in_use()) 
					mWorkers[localIndex].mManagerIndex = newManager;	
			}
			bool is_no_workers_left_remove() noexcept {
				//first check this (top) layer
				bool hasWorkers = !mWorkers.empty_in_use();

				//check every subwindow
				for (auto& subwindow:mSubWindows) {

					if (subwindow.mStorage) {

						//subwindow calls is_no_workers_left_remove recursively going to the bottom of each branch
						//then as it moves back up removes shit
						bool keep = subwindow.mStorage->is_no_workers_left_remove();

						if (!keep) //if empty remove 
							subwindow.mStorage.reset();
						else //else keep but this means top layer must also keep it, thus hasWorkers must also be set as true
							hasWorkers = true;
					}

				}
				return hasWorkers;
			}

			void count_branches(std::size_t& counter)const noexcept {
				for (const auto& subwindow : mSubWindows) {
					if (subwindow.mStorage) {
						counter++;
						subwindow.mStorage->count_branches(counter);
					}
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

		void remove_area(const rectF& area) {
			//there is an issue with removing multiple items at once and that is indexes being chaotic
			//to avoid accessing out of bounds memory the removal has to be done from the greatest to lowest order

			//get list
			auto list = search_area(area);
			//sort from greatest to lowest
			std::sort(list.begin(), list.end(), std::greater<>());
			//remove
			for (auto& id : list) 
				remove(id);
		}
		void remove(std::size_t removeIndex) {
			//invalid index or something
			if (removeIndex >= mManagers.size_in_use())
				throw std::runtime_error("invalid index");
			

			//first step is to get rid of the index
			auto& REMOVE_NODE = mManagers[removeIndex];

			std::optional<std::size_t> newManagerIndex;

			//first remove the object internally from the tree
			if (REMOVE_NODE.mWorkingWindow->remove(REMOVE_NODE.mWorkerIndex, removeIndex, newManagerIndex)) {
				//since internally things got swaped around, this means the previous index of the worker (which now indexes a different object)
				//is now managed but the newManager (in other words, it's just bookkeeping)
				if (newManagerIndex.has_value()) 
					mManagers[newManagerIndex.value()].mWorkerIndex = REMOVE_NODE.mWorkerIndex;
			}
			else {
				throw std::runtime_error("index missmatch between parent and child nodes");
			}

			//previously, the object somewhere in the tree was removed. this meant a top level manager had to be notifed of the change
			//in this step we are removing a top level manager object, this means a worker somewhere in the tree needs to be notfied of the change

			//if the removed node is the last node, then the bookkeeping is already done, otherwise do the step
			if (removeIndex != mManagers.size_in_use() - 1) {
				auto& CURRENT_LAST_NODE = mManagers.back();
				CURRENT_LAST_NODE.mWorkingWindow->assign_new_parent(CURRENT_LAST_NODE.mWorkerIndex, removeIndex);
			}

			//finally remove the piece of shit
			mManagers.depricate_unordered(mManagers.begin() + removeIndex);
		}
		void remove_dead_cells() {
			if (!mRoot.is_no_workers_left_remove()) {
				mRoot.clear();//last return of remove_dead_cells is about the top layer, if it's also empty call this (should be cheap anyway)
			}
		}

		void re_initalize(const rectF& newArea) {
			mRoot = QuadWindow(newArea, 0);
		}
		std::size_t size() const {
			return mManagers.size_in_use();
		}
		std::size_t branch_count()const noexcept {
			std::size_t counter = 0;
			mRoot.count_branches(counter);
			return counter;
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
*/