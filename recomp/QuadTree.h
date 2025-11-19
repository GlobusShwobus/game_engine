#pragma once
#include "Rectangle.h"
#include "SequenceM.h"

#include <memory>
#include <array>
#include <optional>
//REMEMBER TO INCLUDE SEARCH COLLISIONS ONLY FOR AREA, TO DO THAT INCLUDE A DEFAULT PARAMETER WITH SEARCH SIZE
namespace badEngine {
	//struct OBJECT_TYPE {
		//int meme = 0;
	//};
	template <typename OBJECT_TYPE>
		requires IS_SEQUENCE_COMPATIBLE<OBJECT_TYPE>
	class QuadTree {

		static constexpr std::size_t MAX_DEPTH = 8;

		class QuadWindow;

		struct ManagerNode {
			QuadWindow* mWorkingWindow = nullptr;
			std::size_t mWorkerIndex = 0;
		};
		struct WorkerNode {
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

				for (auto& subwindow : mSubWindows) {
					if (subwindow.mStorage) {
						subwindow.mStorage->clear();
						subwindow.mStorage.reset();
					}
				}
			}

			ManagerNode insert(const rectF& workingArea, std::size_t managerIndex) {

				//first, check structures depth limit, if going deeper is fine, try going deeper
				if (mDepth + 1 < MAX_DEPTH) {

					//check all 4 branches
					for (auto& subwindow : mSubWindows) {
						//check which window the object exists in (this check whole; in case all 4 checks fail, it must mean we add to this layer as this layer is larger)
						if (!subwindow.mArea.contains(workingArea)) continue;

						//check for nullptr and init the branch if not yet set
						if (!subwindow.mStorage)
							subwindow.mStorage = std::make_unique<QuadWindow>(subwindow.mArea, mDepth + 1);

						//pass the item down the chain
						return subwindow.mStorage->insert(workingArea, managerIndex);
					}
				}
				//in case the depth limit is reached or the objectArea doesn't fit into any sub branches, add it here
				mWorkers.emplace_back(workingArea, managerIndex);
				//return back the metadata so that parent knows where it's child is
				return ManagerNode(this, mWorkers.size() - 1);
			}
			void collect_area(const rectF& searchArea, SequenceM<std::size_t>& collector)const noexcept {
				//first check for items belonging to this layer
				for (const auto& worker : mWorkers)
					if (searchArea.intersects(worker.mWorkerArea))
						collector.emplace_back(worker.mManagerIndex);

				//second check all branches and get their objects
				for (const auto& subwindow : mSubWindows) {
					//check for nullptr, if null then it means there is nothing there
					if (!subwindow.mStorage)continue;

					if (searchArea.contains(subwindow.mArea))//if contains whole then add everything
						subwindow.mStorage->collect_area_all(collector);
					else if (searchArea.intersects(subwindow.mArea))//if not contains whole then detailed search
						subwindow.mStorage->collect_area(searchArea, collector);
				}
			}
			void collect_area_all(SequenceM<std::size_t>& collector)const noexcept {
				//first add all items from this layer
				for (const auto& worker : mWorkers)
					collector.emplace_back(worker.mManagerIndex);
				//secondly add all items from branches
				for (const auto& subwindow : mSubWindows)
					if (subwindow.mStorage)
						subwindow.mStorage->collect_area_all(collector);
			}

			bool remove(std::size_t workerIndex, std::size_t managerIndex, std::optional<std::size_t>& newManagerIndex) {
				//check if index is valid at all
				if (mWorkers.size() <= workerIndex)
					return false;
				//check if the observers match (maybe throw is better?)
				if (!(mWorkers[workerIndex].mManagerIndex == managerIndex))
					return false;

				//removing an object is done in an unordered way. object somewhere in the middle gets swapped with the last object and the new last is then cut off
				//this means that the previously last, but now in a new index worker has the wrong manager and needs to be updated
				// TWO THINGS TO KEEP IN MIND: 1) if removed item is the last item, then no new manager is to be updated
				//                             2) if the container is empty afterwards, then also no new manager. no worker no manager


				//swap places between local and last and remove the new last
				mWorkers.remove_unpreserved_order(mWorkers.begin() + workerIndex);

				//check if the container after removing is not empty and if we didn't remove the last item in the last step
				if (!mWorkers.empty() && !(mWorkers.size() == workerIndex)) {
					newManagerIndex = mWorkers[workerIndex].mManagerIndex;
				}
				else {
					newManagerIndex.reset();
				}
				return true;
			}
			void assign_new_manager(std::size_t localIndex, std::size_t newManager)noexcept {
				//check validity first and swap managers
				if (localIndex < mWorkers.size())
					mWorkers[localIndex].mManagerIndex = newManager;
			}
			bool is_no_workers_left_remove() noexcept {
				//first check this (top) layer
				bool hasWorkers = !mWorkers.empty();

				//check every subwindow
				for (auto& subwindow : mSubWindows) {

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

			bool assign_new_working_area(std::size_t workerIndex, const rectF& newArea) {
				//index check
				if (mWorkers.size() <= workerIndex)
					return false;
				//check if new area is withing this window
				if(!mWindow.contains(newArea))
					return false;
				//assign
				mWorkers[workerIndex].mWorkerArea = newArea;
				return true;
			}
	
			void collect_collisions(SequenceM<std::pair<std::size_t, std::size_t>>& collisions) const {
				//check all local workers for intersection
				for (size_t i = 0; i < mWorkers.size(); ++i)
					for (size_t j = i + 1; j < mWorkers.size(); ++j)
						if (mWorkers[i].mWorkerArea.intersects(mWorkers[j].mWorkerArea))
							collisions.emplace_back(mWorkers[i].mManagerIndex, mWorkers[j].mManagerIndex);
						
				//check sub windows
				for (auto& sub : mSubWindows) {
					if (!sub.mStorage) continue;

					//REMEMBER TO TEST OUT CONTAINS PART, IF IT IS SLOWER THAN JUST CHECKING INTERSECTS, OMIT IT
					for (const auto& worker : mWorkers) {

						if (worker.mWorkerArea.contains(sub.mArea)) 
							sub.mStorage->collect_all_collisions_with(worker.mManagerIndex, collisions);
						else if(worker.mWorkerArea.intersects(sub.mArea))
							sub.mStorage->collect_potential_collisions(worker, collisions);

					}
					// Then recurse
					sub.mStorage->collect_collisions(collisions);
				}
			}
			void collect_potential_collisions(const WorkerNode& parentWorker,
				SequenceM<std::pair<std::size_t, std::size_t>>& collisions) const
			{
				//first test against all locals
				for (const auto& worker : mWorkers) 
					if (worker.mWorkerArea.intersects(parentWorker.mWorkerArea))
						collisions.emplace_back( parentWorker.mManagerIndex, worker.mManagerIndex);
					
				//then also checks against sub windows
				for (auto& sub : mSubWindows) {
					if (!sub.mStorage) continue;

					if (parentWorker.mWorkerArea.contains(sub.mArea)) 
						sub.mStorage->collect_all_collisions_with(parentWorker.mManagerIndex, collisions);
					else 
						sub.mStorage->collect_potential_collisions(parentWorker, collisions);
				}
			}
			void collect_all_collisions_with(std::size_t managerIndex,
				SequenceM<std::pair<std::size_t, std::size_t>>& collisions) const
			{
				//all in this node
				for (const auto& worker : mWorkers) 
					collisions.emplace_back(managerIndex, worker.mManagerIndex);
				
				//all in sub nodes
				for (const auto& sub : mSubWindows)
					if (sub.mStorage)
						sub.mStorage->collect_all_collisions_with(managerIndex, collisions);
			}


		private:
			//LAYER DEPTH
			std::size_t mDepth = 0;

			//THIS WINDOW
			rectF mWindow;

			//POINTERS TO OTHER WINDOWNS NESTED INSIDE
			std::array<SubWindow, 4> mSubWindows;

			//STORE INFO
			SequenceM<WorkerNode> mWorkers;
		};

	public:

		QuadTree(const rectF& window) :mRoot(window, 0), topLevelWindow(window) {}

		template<typename U>
			requires std::same_as<std::remove_cvref_t<U>, OBJECT_TYPE>
		bool insert(U&& item, rectF itemsize) {

			if (!is_within_scope(itemsize))
				return false;
			//forward the data, node data is invalid here
			const std::size_t currentLastIndex = mManagers.size();
			mManagers.emplace_back(
				std::forward<U>(item),
				mRoot.insert(itemsize, currentLastIndex)
			);
			return true;
		}
		bool is_within_scope(const rectF& rect)const noexcept {
			return topLevelWindow.contains(rect);
		}

		SequenceM<std::size_t> search_area(const rectF& area)const noexcept {

			//collect index, set growth to minimum resist to splurge mem
			SequenceM<std::size_t> collector;
			collector.set_growth_resist_negative();
			//actually collect recursively
			mRoot.collect_area(area, collector);

			return collector;
		}
		SequenceM<std::pair<std::size_t, std::size_t>> search_collisions()const noexcept {

			//collect index, set growth to minimum resist to splurge mem
			SequenceM<std::pair<std::size_t, std::size_t>> collector;
			collector.set_growth_resist_low();
			//actually collect recursively
			mRoot.collect_collisions(collector);

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
			if (removeIndex >= mManagers.size())
				throw std::runtime_error("invalid index");


			//first step is to get rid of the index
			auto& REMOVE_NODE = mManagers[removeIndex].second;

			std::optional<std::size_t> newManagerIndex;

			//first remove the object internally from the tree
			if (REMOVE_NODE.mWorkingWindow->remove(REMOVE_NODE.mWorkerIndex, removeIndex, newManagerIndex)) {
				//since internally things got swapped around, this means the previous index of the worker (which now indexes a different object)
				//is now managed but the newManager (in other words, it's just bookkeeping)
				if (newManagerIndex.has_value())
					mManagers[newManagerIndex.value()].second.mWorkerIndex = REMOVE_NODE.mWorkerIndex;
			}
			else {
				throw std::runtime_error("index miss-match between parent and child nodes");
			}

			//previously, the object somewhere in the tree was removed. this meant a top level manager had to be notified of the change
			//in this step we are removing a top level manager object, this means a worker somewhere in the tree needs to be notified of the change

			//if the removed node is the last node, then the bookkeeping is already done, otherwise do the step
			if (removeIndex != mManagers.size() - 1) {
				auto& CURRENT_LAST_NODE = mManagers.back();
				CURRENT_LAST_NODE.second.mWorkingWindow->assign_new_manager(CURRENT_LAST_NODE.second.mWorkerIndex, removeIndex);
			}

			//finally remove the piece of shit
			mManagers.remove_unpreserved_order(mManagers.begin() + removeIndex);
		}
		void remove_dead_cells() {
			if (!mRoot.is_no_workers_left_remove()) {
				mRoot.clear();//last return of remove_dead_cells is about the top layer, if it's also empty call this (should be cheap anyway)
			}
		}

		void relocate(std::size_t itemIndex, const rectF& itemSize) {

			//index check
			if (itemIndex >= mManagers.size())
				throw std::runtime_error("invalid index");
			
			auto& RELOCATE_PAYLOAD = mManagers[itemIndex].second;
			
			//check if relocation is necessary at all or just update the box
			if (RELOCATE_PAYLOAD.mWorkingWindow->assign_new_working_area(RELOCATE_PAYLOAD.mWorkerIndex, itemSize))
				return;
	
			//do need to relocate
			std::optional<std::size_t> newManagerIndex;
			
			//handle removing the worker internally
			if (RELOCATE_PAYLOAD.mWorkingWindow->remove(RELOCATE_PAYLOAD.mWorkerIndex, itemIndex, newManagerIndex)) {
				//since internally things got swapped around, this means the previous index of the worker (which now indexes a different object)
				//is now managed but the newManager (in other words, it's just bookkeeping)
				if (newManagerIndex.has_value())
					mManagers[newManagerIndex.value()].second.mWorkerIndex = RELOCATE_PAYLOAD.mWorkerIndex;
			}
			else {
				throw std::runtime_error("index miss-match between parent and child nodes");
			}
			//reinsert it where ever
			RELOCATE_PAYLOAD = mRoot.insert(itemSize, itemIndex);
		}

		void re_initialize(const rectF& newArea) {
			mManagers.clear();
			mRoot = QuadWindow(newArea, 0);
			topLevelWindow = newArea;
		}
		std::size_t size() const {
			return mManagers.size();
		}
		std::size_t branch_count()const noexcept {
			std::size_t counter = 0;
			mRoot.count_branches(counter);
			return counter;
		}

		OBJECT_TYPE& operator[](std::size_t index)noexcept {
			return mManagers[index].first;
		}
		const OBJECT_TYPE& operator[](std::size_t index) const noexcept {
			return mManagers[index].first;
		}

		auto begin() {
			return mManagers.begin();
		}
		const auto cbegin()const {
			return mManagers.cbegin();
		}
		auto end() {
			return mManagers.end();
		}
		const auto cend()const {
			return mManagers.cend();
		}


	private:
		QuadWindow mRoot;
		SequenceM<std::pair<OBJECT_TYPE, ManagerNode>> mManagers;
		rectF topLevelWindow;
	};
}