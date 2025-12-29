#pragma once
#include "Rectangle.h"
#include "SequenceM.h"

#include <memory>
#include <array>

namespace badEngine {

	/*
	IF node is leaf, children must be -1 and user_data must be dereferable
	IF a node is an internal node (height other than 0) then user_data must be nullptr

	Trees free list and nodes nextFreeNode MUST communicate
	*/

	static constexpr float aabbExtension = 0.1f;
	static constexpr int nullnode = -1;

	//using T = int;//later tempalte
	template<typename T>
	class BVHTree {

		struct Node {
			rectF aabb;//fattened
			T* user_data = nullptr;

			int parent = nullnode;
			int child1 = nullnode;
			int child2 = nullnode;

			int height = 0;//leaf = 0; internal = max(child)+1

			int nextFreeNode = nullnode;

			constexpr bool is_leaf()const noexcept {
				return child1 == nullnode;//probably simpler instead of using height
			}
			//used in BHVTree constructor to only set the list, rest by default
			Node(int next) :nextFreeNode(next) {}
		};

	public:
		/*
				template<std::input_iterator InputIt>
			requires std::same_as<T, std::iter_reference_t<InputIt>>
		BVHTree(InputIt begin, InputIt end, std::size_t element_count)
		{
			mRoot = nullnode;

			const std::size_t inital_capacity = 16;
			mNodes.set_capacity(inital_capacity);

			//create a chain on nodes, except set last manually
			for (std::size_t i = 0; i < inital_capacity - 1; ++i) {
				mNodes.emplace_back(static_cast<int>(i+1));
			}
			mNodes.emplace_back(static_cast<int>(nullnode));
			//iterating the freelist starts from 0
			mFreeList = 0;
		}
		*/
		BVHTree(std::size_t inital_size)
		{
			mRoot = nullnode;

			mNodes.set_capacity(inital_size);

			//create a chain on nodes, except set last manually
			for (std::size_t i = 0; i < inital_size - 1; ++i) {
				mNodes.emplace_back(static_cast<int>(i+1));
			}
			mNodes.emplace_back(static_cast<int>(nullnode));
			//iterating the freelist starts from 0
			mFreeList = 0;
		}

		std::size_t create_proxy(const rectF& aabb, T* user_data) {

			auto proxyID = build_node();
			Node& node = mNodes[proxyID];
			auto& fatbox = node.aabb;
			fatbox = aabb;
			fatbox.x -= aabbExtension;
			fatbox.y -= aabbExtension;
			fatbox.w += 2 * aabbExtension;
			fatbox.h += 2 * aabbExtension;
			node.user_data = user_data;

			insert_leaf(proxyID);

			return proxyID;
		}

		~BVHTree() = default;

	private:

		std::size_t build_node() {

			//if free list is out of nexts
			if (mFreeList == nullnode) {

				auto currentCap = mNodes.capacity();
				auto newCap = currentCap * 2;
				mNodes.set_capacity(newCap);

				//create a chain on nodes, except set last manually
				for (std::size_t i = currentCap; i < newCap - 1; ++i) {
					mNodes.emplace_back(i + 1);
				}
				mNodes.emplace_back(nullnode);
				mFreeList = currentCap;
			}

			//pop a node from freelist
			int nodeId = mFreeList;
			//cache the value
			auto& node = mNodes[nodeId];
			//asign the freelist to the nextfree node
			mFreeList = node.nextFreeNode;
			//set height to leaf
			node.height = 0;
			return nodeId;
		}

		void insert_leaf(const std::size_t proxyID) {
			//the case of the very first leaf
			if (mRoot == nullnode) {
				mRoot = proxyID;
				mNodes[mRoot].parent = nullnode;
				return;
			}
			//stage 1: find the best sibling for the new leaf
			//instead of looping over all elements finding the best option which is O(n)
			//loop by finding the path that causes the least amount of change by comparing left and right paths (child 1 and child 2)
			//this is called branch and bound
			int bestSibling = mRoot;
			rectF leafAABB = mNodes[proxyID].aabb;

			while (!mNodes[bestSibling].is_leaf()) {
				//cahce node
				auto& node = mNodes[bestSibling];
				//get the surface area of the the union of the entire section of the tree
				float structureArea = node.aabb.perimeter();
				//combine leaf and the current structure and get the SA of the union
				const rectF combinedAABB = Rectangle<float>::union_rect(leafAABB, node.aabb);
				float combinedArea = combinedAABB.perimeter();

				//get the cost of staying on this level as it may be true that going deeper is not worth it
				//2x multiplier is a SAH math bullshit. im regular cases it doesn't matter, but in some it basically says "this layer is better than children"
				float directCost = 2.0f * combinedArea;
				//the minimum possible cost of going down
				float inheritedCost = 2.0f * (combinedArea - structureArea);
				//get the cost of children
				float child1Cost = child_cost(node.child1, inheritedCost, leafAABB);
				float child2Cost = child_cost(node.child2, inheritedCost, leafAABB);

				//if cost of adding here is less than into either child, stop
				if (directCost < child1Cost && directCost < child2Cost) {
					break;
				}
				//otherwise descend into either child 1 or 2
				bestSibling = (child1Cost < child2Cost) ? node.child1 : node.child2;
			}

			//stage 2: creating and building a new set of node and leaves
			//each internal node must have 2 leaf nodes, a leaf node may not have any children, thus the leaf nodes children are denoted as nullnodes
			//in this implementation a leaf node stores only 1 primitive, not a bucket
			//the most reasonable way to achieve this is by creating a new parent node which is attached to the previous parent
			//however if input is sorted this will break down and cause the creation of a linked list due to SAH "failing"
			//this requires rebalancing the tree later
			
			//previous leaf will become an internal node, thus the leaf will have to move
			std::size_t oldParent = mNodes[bestSibling].parent;
			//build a new parent
			std::size_t newParent = build_node();
			//cache nodes for setting immediate data about the node set
			auto& child1 = mNodes[bestSibling];
			auto& child2 = mNodes[proxyID];
			auto& newParentNode = mNodes[newParent];
			//set new parent info
			newParentNode.parent = oldParent;
			newParentNode.child1 = bestSibling;
			newParentNode.child2 = proxyID;
			newParentNode.height = child1.height + 1;
			newParentNode.aabb = Rectangle<float>::union_rect(child1.aabb, child2.aabb);
			//link up children and new parent
			child1.parent = newParent;
			child2.parent = newParent;

			//link new parent and old parent, or set as root if old parent was nullnode
			if (oldParent != nullnode) {
				auto& oldParentNode = mNodes[oldParent];
				//determine which side to attach to
				if (oldParentNode.child1 == bestSibling) {
					oldParentNode.child1 = newParent;
				}
				else {
					oldParentNode.child2 = newParent;
				}
			}
			else {
				mRoot = newParent;
			}

			//stage 3: walking back up from point of creation of a new parent and resizing all parent node AABBs

			std::size_t currentNode = newParent;
			while (currentNode != nullnode) {

				//balance here? idk that's explained way later so no balance currently
				//####################################################################
				
				auto& node_at = mNodes[currentNode];

				assert(node_at.child1 != nullnode);
				assert(node_at.child2 != nullnode);

				auto& child1_at = mNodes[node_at.child1];
				auto& child2_at = mNodes[node_at.child2];
				//height is most importantly used for rebalancing 
				node_at.height = 1 + bad_maxV(child1_at.height, child2_at.height);
				//set SAH
				node_at.aabb = Rectangle<float>::union_rect(child1_at.aabb, child2_at.aabb);

				currentNode = node_at.parent;
			}
		}

	private:

		SequenceM<Node> mNodes;

		int mRoot = nullnode;
		int mFreeList = nullnode;

	private:
		//garbag, later maybe inline into functions
		float child_cost(std::size_t index, float inheritedCost, const rectF& insertedAABB) {
			float cost;
			auto& child = mNodes[index];
			rectF combined = Rectangle<float>::union_rect(insertedAABB, child.aabb);

			if (child.is_leaf()) {
				cost = combined.perimeter() + inheritedCost;
			}
			else {
				float oldArea = child.aabb.perimeter();
				float newArea = combined.perimeter();
				cost = (newArea - oldArea) + inheritedCost;
			}
			return cost;
		}
	};
}
