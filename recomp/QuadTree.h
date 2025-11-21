#pragma once
#include "Rectangle.h"
#include "SequenceM.h"

#include <memory>
#include <array>

//TODO:: INCLUDE SEARCH COLLISIONS ONLY FOR AREA, TO DO THAT INCLUDE A DEFAULT PARAMETER WITH SEARCH SIZE

namespace badEngine {
	struct OBJECT_TYPE {
		int meme = 0;
	};
	//template <typename OBJECT_TYPE>
		//requires IS_SEQUENCE_COMPATIBLE<OBJECT_TYPE>
	class QuadTree {

		static constexpr std::size_t MAX_DEPTH = 6;

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

			bool remove(std::size_t workerIndex, std::size_t managerIndex, std::size_t& newManagerIndex) {
				//cache
				auto& worker = mWorkers[workerIndex];
				//important shittest, if manager indexes don't match it means fatal bookkeeping error
				if (worker.mManagerIndex != managerIndex)
					return false;
				//cache iterators
				auto it = mWorkers.begin() + workerIndex;
				auto back = mWorkers.end() - 1;
				//if the item removed is the last item, then no bookkeeping required upstream, otherwise need to know which manager to update
				if (it < back)
					newManagerIndex = mWorkers.back().mManagerIndex;
			
				//swap places with last and it and pop
				mWorkers.remove_unpreserved_order(it);
				
				return true;
			}
			bool has_workers_recursive()const noexcept {
				if (!mWorkers.empty())return true;

				for (const auto& sub:mSubWindows) 
					if (sub.mStorage && sub.mStorage->has_workers_recursive())
						return true;
				return false;
			}
			void prune_empty_branches()noexcept {
				for (auto& sub : mSubWindows) {
					if (!sub.mStorage) continue;

					if (!sub.mStorage->has_workers_recursive()) 
						sub.mStorage.reset();
					else
						sub.mStorage->prune_empty_branches();
				}
			}

			void count_branches(std::size_t& counter)const noexcept {
				for (const auto& subwindow : mSubWindows) {
					if (subwindow.mStorage) {
						counter++;
						subwindow.mStorage->count_branches(counter);
					}
				}
			}

			void set_new_manager_to_worker(std::size_t workerIndex, std::size_t newManager)noexcept {
				mWorkers[workerIndex].mManagerIndex = newManager;
			}

			bool assign_new_working_area(std::size_t workerIndex, const rectF& newArea) {
				//index check
				if (mWorkers.size() <= workerIndex)
					return false;
				//check if new area is withing this window
				if (!mWindow.contains(newArea))
					return false;
				//assign
				mWorkers[workerIndex].mWorkerArea = newArea;
				return true;
			}

			void collect_collisions(SequenceM<std::pair<std::size_t, std::size_t>>& collisions) const noexcept {
				//check all local workers for intersection
				const std::size_t workerCount = mWorkers.size();
				for (size_t i = 0; i < workerCount; ++i) {
					const auto& workerI = mWorkers[i];
					const auto& areaI = workerI.mWorkerArea;

					for (size_t j = i + 1; j < workerCount; ++j) {
						const auto& workerJ = mWorkers[j];
						if (areaI.intersects(workerJ.mWorkerArea))
							collisions.emplace_back(workerI.mManagerIndex, workerJ.mManagerIndex);
					}
				}
				//check sub windows
				for (auto& sub : mSubWindows) {
					if (!sub.mStorage) continue;

					for (const auto& worker : mWorkers)
						sub.mStorage->check_worker_intersects(worker, collisions);

					//recurse
					sub.mStorage->collect_collisions(collisions);
				}
			}
		private:
			void check_worker_intersects(const WorkerNode& parentWorker,
				SequenceM<std::pair<std::size_t, std::size_t>>& collisions) const noexcept
			{
				const auto& parentArea = parentWorker.mWorkerArea;
				if (!parentArea.intersects(mWindow)) {
					return;//early return if no intersect
				}

				//first test against all locals
				for (const auto& worker : mWorkers)
					if (worker.mWorkerArea.intersects(parentArea))
						collisions.emplace_back(parentWorker.mManagerIndex, worker.mManagerIndex);

				//then also checks against sub windows
				for (auto& sub : mSubWindows) {
					if (!sub.mStorage) continue;

					sub.mStorage->check_worker_intersects(parentWorker, collisions);
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
		bool insert(U&& item, rectF itemSize) {

			if (!is_within_scope(itemSize))
				return false;
			//forward the data, node data is invalid here
			const std::size_t currentLastIndex = mManagers.size();
			mManagers.emplace_back(
				std::forward<U>(item),
				mRoot.insert(itemSize, currentLastIndex)
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
			/*
			1. when i remove, i need to remove from both top level and bottom level
			1.2 this means first remove bottom layer

			2. in the bottom layer, removing an object means swap object with last, then cut
			2.1 the cut object is irrelevant, it gets removed
			2.2 the object that got moved views the correct manager
			2.3 but the manager of this moved object does not view the correct worker any longer
			2.4 i can access this new manager directly and assign it the new index of the worker
			
			3. if the removed object in the bottom layer is the last in the list, this means no manager needs updating because it is the one in the process of removal
			
			4. removing the object from the top layer
			4.1 if the object is last then there is no further bookkeeping 
			4.2 if the object is not last then it gets swapped with another top layer object
			4.3 this means whoever this top layer object is, it is observing the correct worker
			4.4 but the worker it is observing no longer observes the correct manager, so the worker can be accessed directly and updated

			5. remove the top layer object
			*/

			//test valid index (meh)
			if (removeIndex >= mManagers.size())
				throw std::runtime_error("invalid index");
			//cache remove node
			auto& REMOVE_NODE = mManagers[removeIndex].second;
			//assign variable incase a manager needs to update who it observes
			std::size_t internalManagerSwap = std::numeric_limits<std::size_t>::max();
			
			//remove the node, throw if managers miss match. generally should never happen but if it does... we done goofed
			if (REMOVE_NODE.mWorkingWindow->remove(REMOVE_NODE.mWorkerIndex, removeIndex, internalManagerSwap)) 
				//if the worker moved around, then the worker knows who his manager is, but the manager doesn't know who his worker is anymore, thus bookkeeping
				if (internalManagerSwap != std::numeric_limits<std::size_t>::max())
					mManagers[internalManagerSwap].second.mWorkerIndex = REMOVE_NODE.mWorkerIndex;
			else 
				throw std::runtime_error("index miss-match between parent and child nodes");

			//cache iters
			auto it = mManagers.begin() + removeIndex;
			auto back = mManagers.end() - 1;
			//if removed object is not last
			if (it < back) {
				//then the worker of the manager that is about to be moved, needs to be told who his new manager index is (manager still knows the worker)
				auto& CURRENT_LAST_NODE = mManagers.back();
				//go to the branch, then tell it to change the worker (worker index), the manager to the index of remove item, because the remvoed obj is not the last and they get swaped
				CURRENT_LAST_NODE.second.mWorkingWindow->set_new_manager_to_worker(CURRENT_LAST_NODE.second.mWorkerIndex, removeIndex);
			}
		
			//finally remove the piece of shit
			mManagers.remove_unpreserved_order(it);
		}

		void remove_dead_cells() {
			mRoot.prune_empty_branches();
			if (!mRoot.has_workers_recursive()) {
				mRoot.clear();
			}
		}

		void relocate(std::size_t itemIndex, const rectF& newBoundingBox) {
			/*
			1. relocating does not DELETE the object
			1.1 this means top level object stays in place
			1.2 this means leaf gets moved
			1.3 this means remove the leaf
			1.4 this needs to reinsert the leaf
			1.5 this assumes the reinserted leaf views the correct top level (can check from the previous ancestor?)
			1.6 this means top levels bookkeeping, who it views must be updated

			2.0 is it true that if the leaf does not escape its node, i can skip the process and just assign it the new area if it passes the check?
			
			*/
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
