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

		template<std::input_iterator InputIt>
			requires std::same_as<std::remove_cvref_t<std::iter_reference_t<InputIt>>, AABB>
		void insert(InputIt first, InputIt last, int begin_index_naming) {
			for (; first != last; ++first) {
				insert(begin_index_naming++, *first);
			}
		}
		void insert(int user_index, const AABB& box)noexcept
		{
			//calculate the range of cells the box will be stored in
			//box is potentially sotored is multiple cells
			std::size_t startx = static_cast<std::size_t>((box.x - mBounds.x) * invCellW);
			std::size_t starty = static_cast<std::size_t>((box.y - mBounds.y) * invCellH);
			std::size_t endx = static_cast<std::size_t>((box.x + box.w - mBounds.x) * invCellW);
			std::size_t endy = static_cast<std::size_t>((box.y + box.h - mBounds.y) * invCellH);

			//box may be partially on the grid from any side. this has to be legal logically as collision with even a partial is correct
			//to avoid accessing invalid indexes, clamp values
			startx = bad_minV(startx, mColumns-1);
			starty = bad_minV(starty, mRows-1);
			endx   = bad_minV(endx, mColumns-1);
			endy   = bad_minV(endy, mRows -1);

			//insert user_data into all overlapping indexes
			for (std::size_t y = starty; y < endy; ++y) {
				for (std::size_t x = startx; x < endx; ++x) {
					std::size_t index = y * mColumns + x;
					mCells[index].emplace_back(user_index);
				}
			}
		}
		void clear()noexcept {
			for (auto& cell:mCells) {
				cell.clear();
			}
		}

		void query_pairs(SequenceM<std::pair<int, int>>& pairs)noexcept {
			//for every cell
			for (const auto& cell : mCells) {
				//collect all potential collisions
				for (std::size_t a = 0; a < cell.size(); ++a) {
					for (std::size_t b = a + 1; b < cell.size(); ++b) {
						pairs.emplace_back(cell[a], cell[b]);
					}
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
