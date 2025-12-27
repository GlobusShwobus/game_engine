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

			int parent;
			int child1;
			int child2;

			bool isLeaf;
		};

	public:

		DynamicAABBTree() {
			//mRoot = nullnode;
			//
			//mNodeCapacity = 16;
			//mNodeCount = 0;
			//
			//mNodes.set_capacity(mNodeCapacity);
			//
			////build a linked list for the free list
			//for (int i = 0; i < mNodeCapacity - 1; ++i) {
			//	Node node;
			//	node.next = i + 1;
			//	node.height = -1;
			//	mNodes.emplace_back(node);
			//}
			////set the tail end
			//mNodes[mNodeCapacity - 1].next = nullnode;
			//
			//mFreeList = 0;
			//mPath = 0;
			//mInsertCount = 0;
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
				auto oldCap = mNodeCapacity;
				mNodeCapacity *= 2;
				mNodes.set_capacity(mNodeCapacity);

				//build a linked list for the free list for the new members
				for (auto i = oldCap; i < mNodeCapacity; ++i) {
					Node node;
					node.next = i + 1;
					node.height = -1;
					mNodes.emplace_back(node);
				}
				//set tail end
				mNodes[mNodeCapacity - 1].next = nullnode;
				mFreeList = oldCap;
			}

			//pop from free list
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
			//we are the start of the tree set root node
			if (mRoot == nullnode) {
				mRoot = proxyID;
				mNodes[mRoot].parent = nullnode; //top has no parent
				return;
			}
			//find the best sibling for this node == finding a sibling that adds the least surface area to the tree

			int index = mRoot;
			rectF leafAABB = mNodes[proxyID].aabb;

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
			//stage 2 create new parent

			int siblingIndex = index;
			auto& sibling = mNodes[siblingIndex];

			int oldParentIndex = sibling.parent;
			auto& oldParent = mNodes[oldParentIndex];

			int newParentIndex = allocate_node();
			auto& newParent = mNodes[newParentIndex];

			newParent.parent = oldParentIndex;
			newParent.user_data = nullptr;
			newParent.aabb = Rectangle<float>::union_rect(leafAABB, sibling.aabb);
			newParent.height = sibling.height + 1;

			if (oldParentIndex != nullnode) {
				//the sibling was not the root
				if (oldParent.child1 == siblingIndex) {
					oldParent.child1 = newParentIndex;
				}
				else {
					oldParent.child2 = newParentIndex;
				}

				newParent.child1 = siblingIndex;
				newParent.child2 = proxyID;
				sibling.parent = newParentIndex;
				mNodes[proxyID].parent = newParentIndex;
			}
			else {
				//the sibling was the root
				newParent.child1 = siblingIndex;
				newParent.child2 = proxyID;
				sibling.parent = newParentIndex;
				mNodes[proxyID].parent = newParentIndex;
				mRoot = newParentIndex;
			}
			//stage 3 walk back up the tree fixing heights and AABBs
			while (index != nullnode) {
				index = balance(index);

				auto& node = mNodes[index];
				int child1 = node.child1;
				int child2 = node.child2;

				assert(child1 != nullnode);
				assert(child2 != nullnode);

				node.height = 1 + bad_maxV(mNodes[child1].height, mNodes[child2].height);
				node.aabb = Rectangle<float>::union_rect(mNodes[child1].aabb, mNodes[child2].aabb);

				index = node.parent;
			}
		}
		int balance(int iA){
			assert(iA != nullnode);
			const auto begin = mNodes.begin();
			auto A = begin + iA;
			if (A->is_leaf() || A->height < 2)
			{
				return iA;
			}

			int iB = A->child1;
			int iC = A->child2;
			assert(0 <= iB && iB < mNodeCapacity);
			assert(0 <= iC && iC < mNodeCapacity);

			auto B = begin + iB;
			auto C = begin + iC;

			int balance = C->height - B->height;

			// Rotate C up
			if (balance > 1)
			{
				int iF = C->child1;
				int iG = C->child2;
				auto F = begin + iF;
				auto G = begin + iG;
				assert(0 <= iF && iF < mNodeCapacity);
				assert(0 <= iG && iG < mNodeCapacity);

				// Swap A and C
				C->child1 = iA;
				C->parent = A->parent;
				A->parent = iC;

				// A's old parent should point to C
				if (C->parent != nullnode)
				{
					if (mNodes[C->parent].child1 == iA)
					{
						mNodes[C->parent].child1 = iC;
					}
					else
					{
						assert(mNodes[C->parent].child2 == iA);
						mNodes[C->parent].child2 = iC;
					}
				}
				else
				{
					mRoot = iC;
				}

				// Rotate
				if (F->height > G->height)
				{
					C->child2 = iF;
					A->child2 = iG;
					G->parent = iA;
					A->aabb = Rectangle<float>::union_rect(B->aabb, G->aabb);
					C->aabb = Rectangle<float>::union_rect(A->aabb, F->aabb);

					A->height = 1 + bad_maxV(B->height, G->height);
					C->height = 1 + bad_maxV(A->height, F->height);
				}
				else
				{
					C->child2 = iG;
					A->child2 = iF;
					F->parent = iA;
					A->aabb = Rectangle<float>::union_rect(B->aabb, F->aabb);
					C->aabb = Rectangle<float>::union_rect(A->aabb, G->aabb);

					A->height = 1 + bad_maxV(B->height, F->height);
					C->height = 1 + bad_maxV(A->height, G->height);
				}

				return iC;
			}

			// Rotate B up
			if (balance < -1)
			{
				int iD = B->child1;
				int iE = B->child2;
				auto D = begin + iD;
				auto E = begin + iE;
				assert(0 <= iD && iD < mNodeCapacity);
				assert(0 <= iE && iE < mNodeCapacity);

				// Swap A and B
				B->child1 = iA;
				B->parent = A->parent;
				A->parent = iB;

				// A's old parent should point to B
				if (B->parent != nullnode)
				{
					if (mNodes[B->parent].child1 == iA)
					{
						mNodes[B->parent].child1 = iB;
					}
					else
					{
						assert(mNodes[B->parent].child2 == iA);
						mNodes[B->parent].child2 = iB;
					}
				}
				else
				{
					mRoot = iB;
				}

				// Rotate
				if (D->height > E->height)
				{
					B->child2 = iD;
					A->child1 = iE;
					E->parent = iA;
					A->aabb = Rectangle<float>::union_rect(C->aabb, E->aabb);
					B->aabb = Rectangle<float>::union_rect(A->aabb, D->aabb);

					A->height = 1 + bad_maxV(C->height, E->height);
					B->height = 1 + bad_maxV(A->height, D->height);
				}
				else
				{
					B->child2 = iE;
					A->child1 = iD;
					D->parent = iA;
					A->aabb = Rectangle<float>::union_rect(C->aabb, D->aabb);
					B->aabb = Rectangle<float>::union_rect(A->aabb, E->aabb);

					A->height = 1 + bad_maxV(C->height, D->height);
					B->height = 1 + bad_maxV(A->height, E->height);
				}

				return iB;
			}

			return iA;
		}

	private:

		SequenceM<Node> mNodes;

		int mRoot;
		int mNodeCount;
		int mNodeCapacity;
		int mFreeList;
	};
}
