#pragma once
#include "Rectangle.h"
#include "SequenceM.h"

#include <memory>
#include <array>
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
		void insert_leaf(std::size_t proxyID) {
			++mInsertCount;
			//we are the start of the tree set root node
			if (mRoot == nullnode) {
				mRoot = proxyID;
				mNodes[mRoot].parent = nullnode; //top has no parent
				return;
			}
			//find the best sibling for this node
			rectF leafAABB = mNodes[proxyID].aabb;
			int index = mRoot;                      //start from the top of the tree
			while (!mNodes[index].is_leaf()) {
				auto& node = mNodes[index];

				int child1 = node.child1;
				int child2 = node.child2;

				float area = node.aabb.perimeter();

				rectF combinedAABB
			}

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
