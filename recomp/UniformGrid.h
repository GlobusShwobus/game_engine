#pragma once

#include "SequenceM.h"
#include "Rectangle.h"

//TODO asserts for constructor

namespace badEngine {

	static constexpr std::size_t CELL_ADDATIVE = 3;
	class UniformGrid {

		using Cell = SequenceM<int>;

	public:

		UniformGrid(const AABB& bounds, float cellWidth, float cellHeight)
			:mBounds(bounds), mCellWidth(cellWidth), mCellHeight(cellHeight)
		{
			mColumns = bounds.w / cellWidth;
			mRows = bounds.h / cellHeight;

			invCellW = 1.0f / mCellWidth;
			invCellH = 1.0f / mCellHeight;

			mCells.resize(mColumns * mRows);

			for (auto& cell : mCells) {
				cell.set_additive(CELL_ADDATIVE);//actually pretty good in this scenario, massive help for early inserts while not doing potentially wasteful allocations
			}
		}

		void insert(int user_index, const AABB& box)
		{
			//calculate the range of cells the box will be stored in
			//box is potentially sotored is multiple cells
			std::size_t startx = static_cast<std::size_t>((box.x - mBounds.x) * invCellW);
			std::size_t starty = static_cast<std::size_t>((box.y - mBounds.y) * invCellH);
			std::size_t endx = static_cast<std::size_t>((box.x + box.w - mBounds.x) * invCellW);
			std::size_t endy = static_cast<std::size_t>((box.y + box.h - mBounds.y) * invCellH);

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
					mCells[index].emplace_back(user_index);
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

		//invCells are better than mCellWidth for math, so if cell sized don't matter remove them later
		float invCellW;
		float invCellH;
	};

}

/*
	class UniformGrid {

		using Cell = SequenceM<int>;

	public:

		UniformGrid(const AABB& bounds, float cellWidth, float cellHeight)
			:mBounds(bounds), mCellWidth(cellWidth), mCellHeight(cellHeight)
		{
			mColumns = bounds.w / cellWidth;
			mRows = bounds.h / cellHeight;

			invCellW = 1.0f / mCellWidth;
			invCellH = 1.0f / mCellHeight;

			mCells.resize(mColumns * mRows);

			for (auto& cell : mCells) {
				cell.set_additive(CELL_ADDATIVE);//actually pretty good in this scenario, massive help for early inserts while not doing potentially wasteful allocations
			}
		}
		//TODO: later test with iterators/raw pointers/multi thread
		void build(const SequenceM<AABB>& sizes)
		{
			//TODO: clear later
			struct Range {
				std::size_t minx, miny, maxx, maxy;
			};
			SequenceM<Range> ranges;
			ranges.set_capacity(sizes.size());

			for (const auto& box : sizes) {
				ranges.emplace_back();
				auto& back = ranges.back();
				back.minx = static_cast<std::size_t>((box.x - mBounds.x) * invCellW);
				back.miny = static_cast<std::size_t>((box.y - mBounds.y) * invCellH);
				back.maxx = static_cast<std::size_t>((box.x + box.w - mBounds.x) * invCellW);
				back.maxy = static_cast<std::size_t>((box.y + box.h - mBounds.y) * invCellH);

				back.minx = back.minx < mColumns ? back.minx : mColumns - 1;
				back.miny = back.miny < mRows ? back.miny : mRows - 1;
				back.maxx = back.maxx < mColumns ? back.maxx : mColumns - 1;
				back.maxy = back.miny < mRows ? back.maxy : mRows - 1;
			}
			int user_index = 0;
			for (const auto& each : ranges) {
				for (std::size_t y = each.miny; y < each.maxy; ++y) {
					for (std::size_t x = each.minx; x < each.maxx; ++x) {
						std::size_t index = y * mColumns + x;
						mCells[index].emplace_back(user_index++);
					}
				}
			}

		}
		void insert(int user_index, const AABB& box)
		{
			//calculate the range of cells the box will be stored in
			//box is potentially sotored is multiple cells
			std::size_t startx = static_cast<std::size_t>((box.x - mBounds.x) * invCellW);
			std::size_t starty = static_cast<std::size_t>((box.y - mBounds.y) * invCellH);
			std::size_t endx   = static_cast<std::size_t>((box.x + box.w - mBounds.x) * invCellW);
			std::size_t endy   = static_cast<std::size_t>((box.y + box.h - mBounds.y) * invCellH);

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
					mCells[index].emplace_back(user_index);
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

		//invCells are better than mCellWidth for math, so if cell sized don't matter remove them later
		float invCellW;
		float invCellH;
	};
*/