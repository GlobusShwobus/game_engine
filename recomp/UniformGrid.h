#pragma once

#include "SequenceM.h"
#include "Rectangle.h"

//TODO asserts for constructor

namespace badEngine {

	static constexpr std::size_t CELL_ADDATIVE = 3;

	template<typename T>
	class UniformGrid {

		using Cell = SequenceM<T*>;

	public:

		UniformGrid(const AABB& bounds, float cellWidth, float cellHeight)
			:mBounds(bounds), mCellWidth(cellWidth), mCellHeight(cellHeight)
		{
			mColumns = bounds.w / cellWidth;
			mRows = bounds.h / cellHeight;

			mCells.resize(mColumns * mRows);
			
			//for (auto& cell : mCells) {
			//	cell.set_additive(CELL_ADDATIVE);//actually pretty good in this scenario, massive help for early inserts while not doing potentially wasteful allocations
			//}
		}

		//void insert(T* user_data, const AABB& box)
		//{
		//	//calculate the range of cells the box will be stored in
		//	//box is potentially sotored is multiple cells
		//	std::size_t startx = static_cast<std::size_t>((box.x - mBounds.x) / mCellWidth);
		//	std::size_t starty = static_cast<std::size_t>((box.y - mBounds.y) / mCellHeight);
		//	std::size_t endx = static_cast<std::size_t>((box.x + box.w - mBounds.x) / mCellWidth);
		//	std::size_t endy = static_cast<std::size_t>((box.y + box.h - mBounds.y) / mCellHeight);
		//
		//	//box may be partially on the grid from any side. this has to be legal logically as collision with even a partial is correct
		//	//to avoid accessing invalid indexes, clamp values
		//	startx = bad_minV(startx, mColumns - 1);
		//	starty = bad_minV(starty, mRows - 1);
		//	endx   = bad_minV(endx, mColumns - 1);
		//	endy   = bad_minV(endy, mRows - 1);
		//
		//	//insert user_data into all overlapping indexes
		//
		//	for (std::size_t y = starty; y < endy; ++y) {
		//		for (std::size_t x = startx; x < endx; ++x) {
		//			std::size_t index = y * mColumns + x;
		//			mCells[index].push_back(user_data);
		//		}
		//	}
		//}

		void insert(T* user_data, const AABB& box)
		{
			//calculate the range of cells the box will be stored in
			//box is potentially sotored is multiple cells
			std::size_t startx = static_cast<std::size_t>((box.x - mBounds.x) / mCellWidth);
			std::size_t starty = static_cast<std::size_t>((box.y - mBounds.y) / mCellHeight);
			std::size_t endx = static_cast<std::size_t>((box.x + box.w - mBounds.x) / mCellWidth);
			std::size_t endy = static_cast<std::size_t>((box.y + box.h - mBounds.y) / mCellHeight);

			//box may be partially on the grid from any side. this has to be legal logically as collision with even a partial is correct
			//to avoid accessing invalid indexes, clamp values
			startx = startx < mColumns ? startx : mColumns - 1;
			starty = starty < mRows ? starty : mRows - 1;
			endx = endx < mColumns ? endx : mColumns - 1;
			endy = endy < mRows ? endy : mRows - 1;


			//insert user_data into all overlapping indexes

			for (std::size_t y = starty; y < endy; ++y) {
				for (std::size_t x = startx; x < endx; ++x) {
					std::size_t index = y * mColumns + x;
					mCells[index].push_back(user_data);
				}
			}
		}

		const AABB& get_grid_bounds()noexcept {
			return mBounds;
		}

	private:
		SequenceM<Cell> mCells;
		AABB mBounds;
		std::size_t mColumns;
		std::size_t mRows;
		float mCellWidth;
		float mCellHeight;
	};
}

