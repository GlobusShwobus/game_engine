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

		static constexpr int nullnode = -1;
		static constexpr float aabbExtension = 0.1f;

		struct Node {
			rectF aabb;
			void* user_data;//later T*

			union {
				int parent;
				int next;
			};

			int child1;
			int child2;

			int height;//leaf = 0, free node = -1

			bool is_leaf()const {
				return child1 == nullnode;
			}
		};

	public:

		DynamicAABBTree() {
			mRoot = nullnode;

			mNodeCapacity = 16;
			mNodeCount = 0;

			mNodes.set_capacity(mNodeCapacity);
			mNodes.resize(mNodeCapacity);

			//build a linked list for the free list
			for (int i = 0; i < mNodeCapacity - 1; ++i) {
				mNodes[i].next = i + 1;
				mNodes[i].height = -1;
			}
			//set the tail end
			mNodes[mNodeCapacity - 1].next = nullnode;
			mNodes[mNodeCapacity - 1].height = -1;
			mFreeList = 0;
			mPath = 0;
			mInsertCount = 0;
		}
		~DynamicAABBTree() = default;

		/*
		NOTE:
			fat aabb is an optimization. margin of 0.1f is pretty good and will work nicely
			with correct on user update/on user render loop working with delta time instead of frame counting
			fat aabb is a threshhold from when to rebalance(?) the tree
		*/
		std::size_t create_proxy(const rectF& aabb, void* user_data) {
			std::size_t proxyID = allocate_node();

			Node& node = mNodes[proxyID];
			auto& fatbox = node.aabb;
			fatbox = aabb;
			fatbox.x -= aabbExtension;
			fatbox.y -= aabbExtension;
			fatbox.w += 2 * aabbExtension;
			fatbox.h += 2 * aabbExtension;
			node.user_data = user_data;
			node.height = 0;

			insert_leaf(proxyID);

			return proxyID;
		}

	private:
		/*
		NOTE:
			freelist is assigned to node count but it actually has to make sure not to go over the real bounds
			assigning node count to it reads correct index but only upto the point of previous reallocation
			for example starts at 16, goes to 0. reallocate to 32 and now should go from 32 to 16 (more or less)
		*/
		std::size_t allocate_node() {
			//check if we're out of capacity
			if (mFreeList == nullnode) {
				assert(mNodeCount == mNodeCapacity);
				//free list is empty, reallocate more mem and size
				mNodeCapacity *= 2;
				mNodes.set_capacity(mNodeCapacity);
				mNodes.resize(mNodeCapacity);

				//build a linked list for the free list for the new members
				for (int i = mNodeCount; i < mNodeCapacity - 1; ++i) {
					mNodes[i].next = i + 1;
					mNodes[i].height = -1;
				}
				//set tail end
				mNodes[mNodeCapacity - 1].next = nullnode;
				mNodes[mNodeCapacity - 1].height = -1;
				mFreeList = mNodeCount;
			}

			//get a node off freelist
			std::size_t nodeID = mFreeList;
			Node& node = mNodes[nodeID];
			mFreeList = node.next;
			node.parent = nullnode;
			node.child1 = nullnode;
			node.child2 = nullnode;
			node.height = 0;
			node.user_data = nullptr;
			++mNodeCount;
			return nodeID;
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
			++mInsertCount;
			//we are the start of the tree set root node
			if (mRoot == nullnode) {
				mRoot = proxyID;
				mNodes[mRoot].parent = nullnode; //top has no parent
				return;
			}
			//find the best sibling for this node == finding a sibling that adds the least surface area to the tree
			rectF leafAABB = mNodes[proxyID].aabb;
			int index = mRoot;                      //start from the top of the tree
			while (!mNodes[index].is_leaf()) {
				//nodes
				auto& parent = mNodes[index];
				auto& child1 = mNodes[parent.child1];
				auto& child2 = mNodes[parent.child2];

				float area = parent.aabb.perimeter();
				rectF unionAABB = Rectangle<float>::union_rect(parent.aabb, leafAABB);
				float unionArea = unionAABB.perimeter();

				//cost of creating a new parent for this node and the new leaf
				float cost = 2.0f * unionArea;

				//minimum cost of pushing the leaf further down the tree
				float inheritanceCost = 2.0f * (unionArea - area);

				//cost of decending into child1
				float cost1;
				if (child1.is_leaf()) {
					rectF aabb = Rectangle<float>::union_rect(child1.aabb, leafAABB);
					cost1 = aabb.perimeter() + inheritanceCost;
				}
				else {
					rectF aabb = Rectangle<float>::union_rect(child1.aabb, leafAABB);
					float oldArea = child1.aabb.perimeter();
					float newArea = aabb.perimeter();
					cost1 = (newArea - oldArea) + inheritanceCost;
				}
				//cost of decending into child2
				float cost2;
				if (child2.is_leaf()) {
					rectF aabb = Rectangle<float>::union_rect(child2.aabb, leafAABB);
					cost2 = aabb.perimeter() + inheritanceCost;
				}
				else {
					rectF aabb = Rectangle<float>::union_rect(child2.aabb, leafAABB);
					float oldArea = child2.aabb.perimeter();
					float newArea = aabb.perimeter();
					cost2 = (newArea - oldArea) + inheritanceCost;
				}
				//descend according to the minimum cost
				//if its not worth it to go further down
				if (cost < cost1 && cost < cost2) {
					break;
				}

				//descend
				if (cost1 < cost2) {
					index = parent.child1;
				}
				else {
					index = parent.child2;
				}
			}
			//SETTERS


		}

	private:

		SequenceM<Node> mNodes;

		int mRoot;
		int mNodeCount;
		int mNodeCapacity;
		int mFreeList;

		/// This is used to incrementally traverse the tree for re-balancing.
		int mPath;

		int mInsertCount;
	};
}
