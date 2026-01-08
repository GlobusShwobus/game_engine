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
				//cell.set_capacity(10);
				cell.set_additive(CELL_ADDATIVE);//actually pretty good in this scenario, massive help for early inserts while not doing potentially wasteful allocations
			}
		}
		//insert does not check if box top left is within the range of the grid
		//if x or y aren't in range they will be inserted to the clamped index at x, y or both x and y axis
		void insert(int user_index, const AABB& box)noexcept
		{
			const auto g4 = grid_range(box);
			for (int y = g4.miny; y < g4.maxy; ++y) {
				const int offset = y * mColumns;//cache multiplication, minor thing...
				for (int x = g4.minx; x < g4.maxx; ++x) {
					mCells[static_cast<std::size_t>(offset + x)].emplace_back(user_index);
				}
			}
		}
		template<std::input_iterator InputIt>
			requires std::same_as<std::remove_cvref_t<std::iter_reference_t<InputIt>>, AABB>
		void insert(InputIt first, InputIt last, int begin_index_naming) {
			for (; first != last; ++first) {
				insert(begin_index_naming++, *first);
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
		//collects all elements within the region
		//the rectangle can be partially intersecting the bounds of the grid but returns no results if the region is fully outside
		void query_region(const AABB& region, SequenceM<int>& results)noexcept {
			const auto g4 = grid_range(region);
		
			for (int y = g4.miny; y < g4.maxy; ++y) {
				const int offset = y * mColumns;
				for (int x = g4.minx; x < g4.maxx; ++x) {					
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
		//empties out all cells, leaves capacity intact
		void clear()noexcept {
			for (auto& cell : mCells) {
				cell.clear();
			}
		}
		//returns the structure itself
		const SequenceM<Cell>& get_cells()const noexcept {
			return mCells;
		}
		//get the bounds of the grid
		const AABB& get_grid_bounds()noexcept {
			return mBounds;
		}
		//get how much elements are actually stored
		//keep in mind an object can overlap multiple cells
		std::size_t debug_elements_count()const {
			std::size_t counter = 0;
			for (auto& cell : mCells) {
				counter += cell.size();
			}
			return counter;
		}
	private:

		struct GridInt4 {
			int minx;
			int miny;
			int maxx;
			int maxy;
		};

		inline GridInt4 grid_range(const AABB& box)const noexcept
		{
			GridInt4 g4;
			//NOTE: the reason for std::ceil is because we must include partially overlapping cells
			g4.minx = static_cast<int>((box.x - mBounds.x) * invCellW);					     //left edge (round down)
			g4.miny = static_cast<int>((box.y - mBounds.y) * invCellH);					     //top edge (round down)
			g4.maxx = static_cast<int>(std::ceil((box.x + box.w - mBounds.x) * invCellW));	 //right edge (round up)
			g4.maxy = static_cast<int>(std::ceil((box.y + box.h - mBounds.y) * invCellH));	 //bottom edge (round up)

			//NOTE: the reason for clamp high difference is because we do 0 based indexing AND the loop is exclusionary (using < instead of <=)
			g4.minx = bad_clamp(g4.minx, 0, mColumns - 1);	    //clamp x to left edge if negative, to right if wider than width, or keep
			g4.miny = bad_clamp(g4.miny, 0, mRows - 1);		    //clamp y to top edge if negative, to bottom if deeper than height, or keep
			g4.maxx = bad_clamp(g4.maxx, 0, mColumns);			//clamp x to left edge if negative, to right if wider than width, or keep
			g4.maxy = bad_clamp(g4.maxy, 0, mRows);				//clamp y to top edge if negative, to bottom if deeper than height, or keep
			return g4;
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
