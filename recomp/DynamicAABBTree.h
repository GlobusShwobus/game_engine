#pragma once
#include "Rectangle.h"
#include "SequenceM.h"

#include <memory>
#include <array>

/*
TODO:: credit box2d and also changes?
*/
namespace badEngine {

	class DynamicAABBTree {

		static constexpr float aabbExtension = 0.1f;

		enum node_type {
			nullnode, parent, leaf
		};
		struct Node {
			rectF aabb;
			void* user_data;//later T*

			int parent;
			int child1;
			int child2;

			node_type type;
		};

	public:

		DynamicAABBTree() {

		}
		~DynamicAABBTree() = default;

		/*
		NOTE:
			fat aabb is an optimization. margin of 0.1f is pretty good and will work nicely
			with correct on user update/on user render loop working with delta time instead of frame counting
			fat aabb is a threshhold from when to rebalance(?) the tree
		*/
		std::size_t create_proxy(const rectF& aabb, void* user_data) {
			return 0;
		}

	private:
		/*
		NOTE:
			freelist is assigned to node count but it actually has to make sure not to go over the real bounds
			assigning node count to it reads correct index but only upto the point of previous reallocation
			for example starts at 16, goes to 0. reallocate to 32 and now should go from 32 to 16 (more or less)
		*/
		std::size_t allocate_node() {

			return 0;
		}
		/*
			NOTE:
				the tree structure is binary. each entry is either a node or a leaf, a node can hold a leaf or a node to another branch etc etc
				the way BHV works is creating unions of rectangles where the surface area of the node branch is the union of its two leaf nodes
				if the node has a leaf and another node with 2 leafs, then the size is the union of [A union (B union C)] and so on
				importantly inserting a new rectangle, the function must find the a place for it where it will create the least amount of change
				which makes sense because close rectangles should also be close in the tree creating a union
				to do it though it is required to compare the areas of child1 and child2 to determine the cheapest route
				once the route is chosen it must then communicate backwards updating the union of the parent chain
		*/
		void insert_leaf(std::size_t proxyID) {

		}

	private:

		SequenceM<Node> mNodes;

		int mRoot;
		int mNodeCount;
		int mNodeCapacity;
		int mFreeList;
	};
}
