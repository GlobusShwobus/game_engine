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

			int height = -1;//leaf = 0; internal = max(child)+1

			int nextFreeNode = nullnode;

			constexpr bool is_leaf()const noexcept {
				return height == 0;
			}
			//used in BHVTree constructor to only set the list, rest by default
			Node(int next) :nextFreeNode(next) {}
		};

	public:

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

		std::size_t create_proxy(const rectF& aabb, T* user_data) 
		{
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

		std::size_t build_node()
		{
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

			nodeCount++;
			return nodeId;
		}

		void insert_leaf(const std::size_t proxyID)
		{
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
				float currentNodeArea = node.aabb.perimeter();
				//combine leaf and the current structure and get the SA of the union
				const rectF newBounds = rectF::union_rect(leafAABB, node.aabb);
				float combinedArea = newBounds.perimeter();

				//get the cost of staying on this level as it may be true that going deeper is not worth it
				//2x multiplier is a SAH math bullshit. im regular cases it doesn't matter, but in some it basically says "this layer is better than children"
				//importantly if the child node is not a leaf then the cost is the difference between new and old because to go down that path would mean creating a new node child pairs
				//where the original child is still the same size
				float directCost = 2.0f * combinedArea;
				//the minimum possible cost of going down
				float inheritedCost = 2.0f * (combinedArea - currentNodeArea);
				//get the cost of children
				float child1Cost;
				if (mNodes[node.child1].is_leaf()) {
					rectF path1 = rectF::union_rect(leafAABB, mNodes[node.child1].aabb);
					child1Cost = path1.perimeter() + inheritedCost;
				}
				else {
					rectF path1 = rectF::union_rect(leafAABB, mNodes[node.child1].aabb);
					float oldArea = mNodes[node.child1].aabb.perimeter();
					float newArea = path1.perimeter();
					child1Cost = (newArea - oldArea) + inheritedCost;
				}

				float child2Cost;
				if (mNodes[node.child2].is_leaf()) {
					rectF path2 = rectF::union_rect(leafAABB, mNodes[node.child2].aabb);
					child2Cost = path2.perimeter() + inheritedCost;
				}
				else {
					rectF path2 = rectF::union_rect(leafAABB, mNodes[node.child2].aabb);
					float oldArea = mNodes[node.child2].aabb.perimeter();
					float newArea = path2.perimeter();
					child2Cost = (newArea - oldArea) + inheritedCost;
				}
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
			newParentNode.aabb = rectF::union_rect(child1.aabb, child2.aabb);
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

				currentNode = bottom_up_balance(currentNode);
				
				auto& node_at = mNodes[currentNode];

				assert(node_at.child1 != nullnode);
				assert(node_at.child2 != nullnode);

				auto& child1_at = mNodes[node_at.child1];
				auto& child2_at = mNodes[node_at.child2];
				//height is most importantly used for rebalancing 
				node_at.height = 1 + bad_maxV(child1_at.height, child2_at.height);
				//set SAH
				node_at.aabb = rectF::union_rect(child1_at.aabb, child2_at.aabb);

				currentNode = node_at.parent;
			}
		}
		/*
		RULES:
			a internal node cannot be swapped with another internal node
			a leaf node cannot be swapped with another leaf node
			a parent cannot be swapped with its own leaf nodes
			ONLY an internal node <- -> leaf node from another subtree is swappable

			the function does 2 modifications:
				one structural promoting a node upward with all its contents
				one subtree selection. move deeper closer to the root to reduce SAH costs
			leaf height = 0, internal height = max(of children) + 1
			leaf nodes children are -1 height (default set)
		*/
		std::size_t bottom_up_balance(std::size_t index)
		{
			auto iA = index;
			auto& A = mNodes[index];
			//as per rules of rotation, node can not be a leaf nor a parent with 2 leafs
			if (A.height < 2) {
				return index;
			}
			auto iB = A.child1;
			auto iC = A.child2;
			auto& B = mNodes[A.child1];
			auto& C = mNodes[A.child2];

			int balance = B.height - C.height;
			//if balance > 0 == child1 is taller
			//if balance < 0 == child2 is taller
			//if |balance| <= 1 == perfectly balanced NO OP
			//same rule applies as before except because of math it can now be +-1 and difference of 1 should be ignored
			//that's why comparisons exclude 1 and -1

			//ALSO: not all values are explicitly set. for example in case balance > 1. don't explicityl set A.child2 and D.parent. it is a significant win (tested).

			//promote child1 up
			if (balance > 1) {
				//children of B
				auto iD = B.child1;
				auto iE = B.child2;
				auto& D = mNodes[iD];
				auto& E = mNodes[iE];
				//       A	  
				//      / \	  
				//     B   C  
				//    / \	  
				//   D   E	  
				B.parent = A.parent;
				A.parent = iB;
				B.child1 = iA;  //convention, can be child 2 too
				//       B	  
				//      / \	  
				//     A   ?  
				//    / \	  
				//   ?   C	  

				//fix original parent of A to point to B
				if (B.parent != nullnode) {
					auto& root = mNodes[B.parent];
					//if roots child 1 or child 2 pointed to iA
					if (root.child1 == iA) 
						root.child1 = iB;
					else 
						root.child2 = iB;
				}
				else {
					mRoot = iB;
				}

				//determine which OG child of B stays with B, which goes to A depending on height of B's children
				//promote the subtree with larger spatial extent upwards so its BB is evaluated earlier
				if (D.height > E.height) {
					B.child2 = iD;
					A.child1 = iE;
					E.parent = iA;
					//       B	 
					//      / \	 
					//     A   D 
					//    / \	 
					//   E   C	 
					//set rest of BVH related data
					A.aabb = rectF::union_rect(E.aabb, C.aabb);
					B.aabb = rectF::union_rect(A.aabb, D.aabb);

					A.height = bad_maxV(E.height, C.height) + 1;
					B.height = bad_maxV(A.height, D.height) + 1;
				}
				else {
					B.child2 = iE;
					A.child1 = iD;
					D.parent = iA;
					//       B	 
					//      / \	 
					//     A   E 
					//    / \	 
					//   D   C
					A.aabb = rectF::union_rect(D.aabb, C.aabb);
					B.aabb = rectF::union_rect(A.aabb, E.aabb);

					A.height = bad_maxV(D.height, C.height) + 1;
					B.height = bad_maxV(A.height, E.height) + 1;
				}
				return iB;
			}
			//promote child2 up
			else if (balance < -1) {
				//children of C
				auto iF = C.child1;
				auto iG = C.child2;
				auto& F = mNodes[iF];
				auto& G = mNodes[iG];
				//       A	  
				//      / \	  
				//     B   C  
				//        / \	  
				//       F   G
				C.parent = A.parent;
				A.parent = iC;
				C.child1 = iA;
				//       C	  
				//      / \	  
				//     A   ?  
				//    / \	  
				//   B   ?
				//fix original parent of A to point to C
				if (C.parent != nullnode) {
					auto& root = mNodes[C.parent];
					//if roots child 1 or child 2 pointed to iA
					if (root.child1 == iA)
						root.child1 = iC;
					else
						root.child2 = iC;
				}
				else {
					mRoot = iC;
				}
				//determine which OG child of C stays with C, which goes to A depending on height of C's children
				//promote the subtree with larger spatial extent upwards so its BB is evaluated earlier
				if (F.height > G.height) {
					C.child2 = iF;
					A.child2 = iG;
					G.parent = iA;
					//       C	  
					//      / \	  
					//     A   F  
					//    / \	  
					//   B   G
					A.aabb = rectF::union_rect(B.aabb, G.aabb);
					C.aabb = rectF::union_rect(A.aabb, F.aabb);
					A.height = bad_maxV(B.height, G.height) + 1;
					C.height = bad_maxV(A.height, F.height) + 1;
				}
				else {
					C.child2 = iG;
					A.child2 = iF;
					F.parent = iA;
					//       C	  
					//      / \	  
					//     A   G  
					//    / \	  
					//   B   F
					A.aabb = rectF::union_rect(B.aabb, F.aabb);
					C.aabb = rectF::union_rect(A.aabb, G.aabb);
					A.height = bad_maxV(B.height, F.height) + 1;
					C.height = bad_maxV(A.height, G.height) + 1;
				}
				return iC;
			}
			return iA;
		}
	private:

		SequenceM<Node> mNodes;

		int mRoot = nullnode;
		int mFreeList = nullnode;

	public:
		//just for testing and such
		const SequenceM<Node>& myNodes()const {
			return mNodes;
		}

		int node_count()const {
			return nodeCount;
		}
		int nodeCount = 0;
	};
}

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

