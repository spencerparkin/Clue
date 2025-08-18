#pragma once

#include "Defines.h"
#include "Math/Vector2D.h"
#include "Math/Box2D.h"
#include "Card.h"
#include <memory>
#include <vector>
#include <unordered_set>
#include <optional>
#include <functional>

namespace Clue
{
	/**
	 *
	 */
	class CLUE_LIBRARY_API BoardGraph
	{
	public:
		BoardGraph();
		virtual ~BoardGraph();

		class Node;

		void Regenerate();
		bool IsGenerated() const;
		Box2D GetBoundingBox() const;
		std::shared_ptr<Node> GetCharacterStartLocation(Character character);
		std::shared_ptr<Node> GetNodeContainingLocation(const Vector2D& location);
		bool FindShortestPathBetweenNodes(Node* nodeA, Node* nodeB, std::vector<Node*>& nodeArray);
		static int CalculatePathCost(const std::vector<Node*>& nodeArray);

		class Node
		{
			friend class BoardGraph;

		public:
			Node(std::optional<Room> room);
			virtual ~Node();

			bool IsRoom() const;
			const Vector2D& GetLocation() const;
			const Box2D& GetBox() const;

			void GetAdjacencies(std::vector<const Node*>& adjacentNodeArray) const;
			bool IsPathway(const Node* node) const;

			std::optional<Room> GetRoom() const;

		private:
			void AddAdjacency(Node* node);
			void AddPathway(Node* node);

			std::unordered_set<Node*> adjacentNodeSet;
			std::unordered_set<Node*> pathwayNodeSet;
			std::optional<Room> room;

			Vector2D location;
			Box2D box;
			mutable Node* parentNode;
			mutable bool considered;
		};

		void ForAllNodes(std::function<bool(const Node*)> callback) const;

	private:
		std::vector<std::shared_ptr<Node>> nodeArray;
	};
}