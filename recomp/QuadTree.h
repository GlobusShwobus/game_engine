#pragma once
#include "Rectangle.h"
#include <memory>
#include <array>
#include "SequenceM.h"
//STATIC QUADTREE

namespace badEngine {


	template<typename OBJECT>
	class QuadTree {

		static constexpr std::size_t DEFAULT_LAYER_INDEX = 0;
		static constexpr std::size_t FOUR_WINDOWS = 4;
		static constexpr std::size_t MAX_DEPTH = 8;


		class QuadTreeBody {

		public:

			QuadTreeBody(const rectF& window, std::size_t depth = 0) : mDepth(depth) {
				create_body(window);
			}


			void create_body(const rectF& size) {
				clear();
				mWindow = size;
				const float width = mWindow.w / 2.0f;
				const float height = mWindow.h / 2.0f;

				mBranchPos[0] = rectF(mWindow.x, mWindow.y, width, height);
				mBranchPos[1] = rectF(mWindow.x + width, mWindow.y, width, height);
				mBranchPos[2] = rectF(mWindow.x, mWindow.y + height, width, height);
				mBranchPos[3] = rectF(mWindow.x + width, mWindow.y + height, width, height);
			}

			void clear() {
				mObjects.clear();

				for (int i = 0; i < 4; i++) {
					if (mBranchStorage[i]) {
						mBranchStorage[i]->clear();
					}
					mBranchStorage[i].reset();
				}
			}

			void insert(const OBJECT& object, const rectF& objectArea, std::size_t& sizeCounter) {

				//first, check structures depth limit, if going deeper is fine, try going deeper
				if (mDepth + 1 < MAX_DEPTH) {

					//check all 4 branches
					for (int i = 0; i < 4; i++) {
						//check which window the object exists in (this check wholely; in case all 4 checks fail, it must mean we add to this layer as this layer is larger)
						if (!mBranchPos[i].contains_rect(objectArea))
							continue;

						//check for nullptr and initalize the branch if not yet set
						if (!mBranchStorage[i])
							mBranchStorage[i] = std::make_unique<QuadTreeBody>(mBranchPos[i], mDepth + 1);


						//add the item into the branch
						mBranchStorage[i]->insert(object, objectArea, sizeCounter);//recursive call to heck futher if the item fits
						//return early as there is now nothing else to do
						return;
					}

				}

				//in case the depth limit is reached or the objectArea doesn't fit into any sub branches, add it here
				mObjects.element_create(objectArea, object);
				sizeCounter++;
			}
			void conditional_add(const rectF& searchArea, SequenceM<OBJECT*>& list) {

				//first check for items belonging to this layer
				for (auto& item : mObjects) {

					if (searchArea.intersects_rect(item.first)) {
						list.element_assign(&item.second);
					}

				}

				//second check all branches and get their objects
				for (int i = 0; i < 4; i++) {
					//check for nullptr, if null then it means there is nothing there
					if (!mBranchStorage[i]) continue;

					//check if searched area contains branch wholely, if yes then just add everything there
					if (searchArea.contains_rect(mBranchPos[i])) {
						mBranchStorage[i]->unconditional_add(list);
					}//if it does not contain whole but does insersect then need recursive call for percise items
					else if (mBranchPos[i].intersects_rect(searchArea)) {
						mBranchStorage[i]->conditional_add(searchArea, list);
					}
				}
			}

			void unconditional_add(SequenceM<OBJECT*>& listItems) {
				//first add all items from this layer
				for (auto& each : mObjects) {
					listItems.element_assign(&each.second);
				}
				//secondly add all items from branches
				for (int i = 0; i < 4; i++) {

					if (mBranchStorage[i]) {
						mBranchStorage[i]->unconditional_add(listItems);
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



			//TODO: LOOK INTO FUCKING THIS UP AND STORE IN THE MAIN PART AS A MAP, WHILE BODY HOLDS INDEXES, PROB NOT AS SIMPLE
			//STORE ALL OBJECTS BY VALUE, THIS IS THEIR OWNER
			SequenceM<std::pair<rectF, OBJECT>> mObjects;
		};

	public:

		QuadTree(const rectF& window) : mBody(window, DEFAULT_LAYER_INDEX) {}

		void clear() {
			mBody.clear();
			mSizeTracker = 0;
		}
		void insert(const OBJECT& object, const rectF& objectArea) {
			mBody.insert(object, objectArea, mSizeTracker);
		}

		SequenceM<OBJECT*> search(const rectF& searchArea) {
			SequenceM<OBJECT*> itemList(mSizeTracker);
			mBody.conditional_add(searchArea, itemList);
			return itemList;
		}

	private:

		//quad tree body
		QuadTreeBody mBody;

		//THE WHOLE STRUCTURE SIZE;
		std::size_t mSizeTracker = 0;
	};

}
