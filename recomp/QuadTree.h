#pragma once
#include "Rectangle.h"
#include <memory>
#include <array>
#include <list>
#include "SequenceM.h"
//STATIC QUADTREE

namespace badEngine {


	template <typename OBJECT_TYPE>
	class StaticQuadTree {

		static constexpr std::size_t FOUR_WINDOWS = 4;
		static constexpr std::size_t MAX_DEPTH = 8;

		struct Branch {
			rectF mBranchRect;
			std::shared_ptr<StaticQuadTree<OBJECT_TYPE>> mBranch;
		};

	public:

		StaticQuadTree(const rectF size , const std::size_t depth = 0):mDepth(depth)
		{
			resize(size);
		}

		void resize(const rectF& area) {
			clear();
			mWindow = area;

			const float width = mWindow.w / 2.0f;
			const float height = mWindow.h / 2.0f;

			mBranches[0].mBranchRect = rectF(mWindow.x, mWindow.y, width, height);
			mBranches[1].mBranchRect = rectF(mWindow.x + width, mWindow.y, width, height);
			mBranches[2].mBranchRect = rectF(mWindow.x, mWindow.y + height, width, height);
			mBranches[3].mBranchRect = rectF(mWindow.x + width, mWindow.y + height, width, height);
		}

		void clear() {
			//first clear objects in this layer
			mObjects.clear();

			//then recursivly call clear on all branches + set the branch to nullptr afterwards
			for (auto& each : mBranches) {
				if (each.mBranch) {
					each.mBranch->clear();
				}
				each.mBranch.reset();
			}
		}

		std::size_t size()const {
			//first take the count of the objects in this layer
			std::size_t nCount = mObjects.size_in_use();

			//then recurisvly add the counts of objects in branches
			for (auto& each : mBranches) {
				if (each.mBranch) {
					nCount += each.mBranch->size();
				}
			}
		}

		void insert(const OBJECT_TYPE& item, const rectF& itemsize) {
			
			//first check if the given object may fit into any of the branches
			for (auto& each : mBranches) {

				if (each.mBranchRect.contains_rect(itemsize)) {
					//if objects fits onto a branch, check depth and limit it

					if (mDepth+1< MAX_DEPTH) {

						//check for nullptr
						if (!each.mBranch) {
							//make new branch if nullptr
							each.mBranch = std::make_shared<StaticQuadTree<OBJECT_TYPE>>(each.mBranchRect, mDepth + 1);
						}
						//add the item into the branch (IF IT FITS)
						each.mBranch->insert(item, itemsize);//recursive call to heck futher if the item fits
						return;
					}
				}

			}
			//if the item does not fit then it must mean it fits into the current layer
			mObjects.element_create(itemsize, item);
		}

		std::list<OBJECT_TYPE> search(const rectF& searchArea)const {
			std::list<OBJECT_TYPE> itemList;
			search(searchArea, itemList);
			return itemList;
		}
		void search(const rectF& searchArea, std::list<OBJECT_TYPE>& list)const {
			
			//first check for items belonging to this layer
			for (const auto& item: mObjects) {

				if (searchArea.intersects_rect(item.first)) {
					list.push_back(item.second);
				}

			}

			//second check all branches and get their objects

			for (auto& each : mBranches) {

				//check for nullptr, if null then it means there is nothing there
				if (!each.mBranch) continue;

				//check if searched area contains branch wholely, if yes then just add everything there
				if (searchArea.contains_rect(each.mBranchRect)) {
					each.mBranch->items(list);
				}//if it does not contain whole but does insersect then need recursive call for percise items
				else if (each.mBranchRect.intersects_rect(searchArea)) {
					each.mBranch->search(searchArea, list);
				}

			}
		}
		void items(std::list<OBJECT_TYPE>& listItems)const  {
			//first add all items from this layer
			for (const auto& each : mObjects) {
				listItems.push_back(each.second);
			}
			//secondly add all items from branches
			for (auto& each : mBranches) {
				if (each.mBranch) {
					each.mBranch->items(listItems);
				}
			}
		}

		const rectF& get_area()const {
			return mWindow;
		}

	protected:
		//depth tracker
		std::size_t mDepth = 0;

		//area of the quadtree
		rectF mWindow;

		//4 child trees
		std::array<Branch, FOUR_WINDOWS> mBranches;

		//objects that belong to this QuadTree, containing area of the object and object itself
		SequenceM<std::pair<rectF, OBJECT_TYPE>> mObjects;
	};

}