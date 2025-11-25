#pragma once
#include "Rectangle.h"
#include "SequenceM.h"

#include <memory>
#include <array>


namespace badEngine {
	//struct OBJECT_TYPE {
		//int aaa;
	//};
	template <typename OBJECT_TYPE>
		requires IS_SEQUENCE_COMPATIBLE<OBJECT_TYPE>
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

		struct RemoveHandle {
			std::size_t mWorkerIndex = 0;
			std::size_t mManagerIndex = 0;
			bool confirmBookKeeping = false;
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
			bool has_workers_recursive()const noexcept {
				if (!mWorkers.empty())return true;

				for (const auto& sub : mSubWindows)
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
		public:

			RemoveHandle remove(std::size_t workerIndex, std::size_t callerIndex) {

				//important test, if manager indexes don't match it means fatal bookkeeping error
				if (mWorkers[workerIndex].mManagerIndex != callerIndex)
					throw std::runtime_error("index miss-match between parent and child nodes");

				RemoveHandle handle;

				handle.confirmBookKeeping = workerIndex < mWorkers.size() - 1;

				if (handle.confirmBookKeeping) {
					handle.mWorkerIndex = workerIndex;
					handle.mManagerIndex = mWorkers.back().mManagerIndex;
				}

				//swap places with last and it and pop
				mWorkers.remove_unpreserved_order(mWorkers.begin() + workerIndex);

				return handle;
			}
			void update_manager_after_remove(const RemoveHandle& handle)noexcept {
				if (handle.confirmBookKeeping) {
					mWorkers[handle.mWorkerIndex].mManagerIndex = handle.mManagerIndex;
				}
			}
			//BUGED LOGIC
			bool try_set_new_pos_to_worker(std::size_t workerIndex, const rectF& newSize) {
				//check if new area is withing this window
				if (!mWindow.contains(newSize))
					return false;
				//assign
				mWorkers[workerIndex].mWorkerArea = newSize;
				return true;
			}
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
		void remove_dead_cells() {
			mRoot.prune_empty_branches();
			if (!mRoot.has_workers_recursive()) {
				mRoot.clear();
			}
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

	public:

		void remove_area(const rectF& area) {
			auto list = search_area(area);
			std::sort(list.begin(), list.end(), std::greater<>());

			for (auto index : list)
				remove(index);
		}
		void remove(std::size_t removeIndex) {
			//test valid index (meh)
			if (removeIndex >= mManagers.size())
				throw std::runtime_error("invalid index");
			//cache remove node
			auto& REMOVE_NODE = mManagers[removeIndex].second;

			auto tellManagerAboutWorker = REMOVE_NODE.mWorkingWindow->remove(REMOVE_NODE.mWorkerIndex, removeIndex);

			if (tellManagerAboutWorker.confirmBookKeeping)
				mManagers[tellManagerAboutWorker.mManagerIndex].second.mWorkerIndex = tellManagerAboutWorker.mWorkerIndex;

			RemoveHandle stemHandle;
			stemHandle.confirmBookKeeping = removeIndex < mManagers.size() - 1;

			if (stemHandle.confirmBookKeeping) {
				auto& tellWorkerAboutManager = mManagers.back().second;
				stemHandle.mManagerIndex = removeIndex;
				stemHandle.mWorkerIndex = tellWorkerAboutManager.mWorkerIndex;

				tellWorkerAboutManager.mWorkingWindow->update_manager_after_remove(stemHandle);
			}
			mManagers.remove_unpreserved_order(mManagers.begin() + removeIndex);
		}
		void relocate(SequenceM<std::pair<std::size_t, rectF>>& pendingRelocations) {


			//std::sort(pendingRelocations.begin(), pendingRelocations.end(), [](const std::pair<std::size_t, rectF>& A, const std::pair<std::size_t, rectF>& B) {
			//	return A.first < B.first;
			//	});

			for (std::size_t i = 0; i < pendingRelocations.size(); ++i) {
				//cache info
				auto& pender = pendingRelocations[i];
				auto& RELOCATE_NODE = mManagers[pender.first].second;//this is ManagerNode
				//try this
				//if (RELOCATE_NODE.mWorkingWindow->try_set_new_pos_to_worker(RELOCATE_NODE.mWorkerIndex, pender.second))
					//continue;
				//then that
				auto tellManagerAboutWorker = RELOCATE_NODE.mWorkingWindow->remove(RELOCATE_NODE.mWorkerIndex, pender.first);
				//maybe need to update manager
				if (tellManagerAboutWorker.confirmBookKeeping) {
					mManagers[tellManagerAboutWorker.mManagerIndex].second.mWorkerIndex = tellManagerAboutWorker.mWorkerIndex;
				}

				auto NEW_NODE = mRoot.insert(pender.second, pender.first);

				RELOCATE_NODE.mWorkingWindow = NEW_NODE.mWorkingWindow;
				RELOCATE_NODE.mWorkerIndex = NEW_NODE.mWorkerIndex;
			}
		}
	};
}
