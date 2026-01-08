#pragma once

#include "SequenceM.h"
#include "Rectangle.h"


//TODO asserts for constructor
//TODO:: check if checking validity is better than clamps in query_region

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
		//insert does not check if box top left is within the range of the grid
		//if x or y aren't in range they will be inserted to the clamped index at x, y or both x and y axis
		void insert(int user_index, const AABB& box)noexcept
		{
			//calculate the range of cells the box will be stored in
			//box is potentially sotored is multiple cells
			int startx = static_cast<int>((box.x - mBounds.x) * invCellW);
			int starty = static_cast<int>((box.y - mBounds.y) * invCellH);
			int endx = static_cast<int>(std::ceil((box.x + box.w - mBounds.x) * invCellW));
			int endy = static_cast<int>(std::ceil((box.y + box.h - mBounds.y) * invCellH));
		
			//starting positions must be clamped to beginning edges of the screen or 1 before the last
			startx = bad_clamp(startx, 0, mColumns - 1);
			starty = bad_clamp(starty, 0, mRows - 1);
			endx = bad_clamp(endx, 0, mColumns);
			endy = bad_clamp(endy, 0, mRows);
		
			//insert user_data into all overlapping indexes
			for (int y = starty; y < endy; ++y) {
				const int offset = y * mColumns;
				for (int x = startx; x < endx; ++x) {
					mCells[static_cast<std::size_t>(offset + x)].emplace_back(user_index);
				}
			}
		}


		//returns all potential collision candidates, includes duplicates
		//doing basic intersecion tests with duplicates should be always be better than cache misses, otherwise sorting is left to the user
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
		//collects all elements within the reguion
		//the rectangle can be partially intersecting the bounds of the grid but returns no results of the region is fully outside
		void query_region(const AABB& region, SequenceM<int>& results)noexcept {
			//calculate the range of cells region is within
			int startx = static_cast<int>((region.x - mBounds.x) * invCellW);
			int starty = static_cast<int>((region.y - mBounds.y) * invCellH);
			int endx   = static_cast<int>(std::ceil((region.x + region.w - mBounds.x) * invCellW));
			int endy   = static_cast<int>(std::ceil((region.y + region.h - mBounds.y) * invCellH));

			
			startx = bad_clamp(startx, 0, mColumns - 1);
			starty = bad_clamp(starty, 0, mRows - 1);
			endx   = bad_clamp(endx, 0, mColumns);
			endy   = bad_clamp(endy, 0, mRows);
		
			for (int y = starty; y < endy; ++y) {
				const int offset = y * mColumns;
				for (int x = startx; x < endx; ++x) {					
					for (int id : mCells[static_cast<std::size_t>(offset + x)]) {
						results.emplace_back(id);
					}
				}
			}
		}
		//converts a point to a cell index, returns -1 if point is outside of the grid
		inline int query_cell_index(const float2& point)const noexcept {
			int x = static_cast<int>((point.x - mBounds.x) * invCellW);
			int y = static_cast<int>((point.y - mBounds.y) * invCellH);

			if (x < 0 || x >= mColumns || y < 0 || y >= mRows) {
				return -1;
			}

			return y * mColumns + x;
		}
		
		void query_ray() {

		}

		//if this function is called on a populated grid, it will remove elemnts
		//intended usage: call it periodically IF there are a lot of moving objects on a cleared out grid
		void maintain_uniform_memory(std::size_t cell_capacity_target) {

		}

		void clear()noexcept {
			for (auto& cell : mCells) {
				cell.clear();
			}
		}

		const SequenceM<Cell>& get_cells()const noexcept {
			return mCells;
		}
		const AABB& get_grid_bounds()noexcept {
			return mBounds;
		}

		std::size_t debug_elements_count()const {
			std::size_t counter = 0;
			for (auto& cell : mCells) {
				counter += cell.size();
			}
			return counter;
		}

	private:
		SequenceM<Cell> mCells;
		AABB mBounds;
		
		int mColumns;
		int mRows;

		float mCellWidth;
		float mCellHeight;

		//invCells are better than mCellWidth for math, so if cell sized don't matter remove them later
		float invCellW;
		float invCellH;
	};
}
