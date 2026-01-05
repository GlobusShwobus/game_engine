#pragma once

#include "SequenceM.h"
#include "Rectangle.h"

//TODO asserts for constructor

namespace badEngine {
	
	template<typename T>
	class SpatialHashGrid {

		using Cell = SequenceM<T*>;

		inline std::size_t callIndex(std::size_t x, std::size_t y)const {
			return y * mColumns + x;
		}
	public:

		SpatialHashGrid(const AABB& bounds, std::size_t columns, std::size_t rows)
		:mBounds(bounds), mColumns(columns), mRows(rows)
		{
			mCellWidth = mBounds.w / mColumns;
			mCellHeight = mBounds.h / mRows;
			mCells.set_capacity(mColumns*mRows);
			mCells.resize(mColumns * mRows);//resize adds one by one, does not set appropriate capacity upfront
		}

		void insert(T* user_data, const AABB& box) {

		}

	private:
		SequenceM<Cell> mCells;
		AABB mBounds;
		std::size_t mColumns = 0;
		std::size_t mRows = 0;
		float mCellWidth;
		float mCellHeight;
	};
}