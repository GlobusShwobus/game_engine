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
		//insert does not check if box is within the bounds of the UniformGrid which means it will cause invalid memory access violations
		//either check if insertable is within bounds manually and prune insertables assure otherwise your box is within bounds
		//box right and bottom sides CAN be outside of the grid bounds but top left origin can not
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

		//returns all potential collision candidates, includes duplicates
		void query_pairs(SequenceM<std::pair<int, int>>& pairs)noexcept {
			//duplicate entries are fine, tested it with unordered_set hashing bs, and it's night and day
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
		void query_region(const AABB& region, SequenceM<int>& results)noexcept {
			//calculate the range of cells region is within
			int startx = static_cast<int>((region.x - mBounds.x) * invCellW);
			int starty = static_cast<int>((region.y - mBounds.y) * invCellH);
			int endx   = static_cast<int>((region.x + region.w - mBounds.x) * invCellW);
			int endy   = static_cast<int>((region.y + region.h - mBounds.y) * invCellH);
			//unlike in insert, here a clamp is required
			//in insert, it is assumed the user knows his box is within bounds of the grid system, plus it removes a branch
			//here however it is required to allow the user to query a region that is partially or totally outside of the grid
			
			//TODO:: check if checking validity is better than clamps
			startx = bad_clamp(startx, 0, mColumns - 1);
			starty = bad_clamp(starty, 0, mRows - 1);
			endx   = bad_clamp(endx + 1, 0, mColumns);
			endy   = bad_clamp(endy + 1, 0, mRows);
		
		
			for (int y = starty; y < endy; ++y) {
				for (int x = startx; x < endx; ++x) {
					std::size_t index = static_cast<std::size_t>(y) * mColumns + x;
					for (int id : mCells[index]) {
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
