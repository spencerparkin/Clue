#include "BoardGraph.h"
#include <assert.h>

#define CLUE_BOARD_WIDTH			24
#define CLUE_BOARD_HEIGHT			25

#define KN			Clue::Room::Kitchen
#define DR			Clue::Room::DiningRoom
#define LG			Clue::Room::Lounge
#define BR			Clue::Room::BallRoom
#define HL			Clue::Room::Hall
#define CY			Clue::Room::Conservatory
#define LR			Clue::Room::BilliardRoom
#define LY			Clue::Room::Library
#define SY			Clue::Room::Study
#define HW			-1
#define CR			-2
#define UU			-3

using namespace Clue;

static int boardMatrix[CLUE_BOARD_HEIGHT][CLUE_BOARD_WIDTH] =
{
	{UU, UU, UU, UU, UU, UU, UU, UU, UU, HW, UU, UU, UU, UU, HW, UU, UU, UU, UU, UU, UU, UU, UU, UU},
	{KN, KN, KN, KN, KN, KN, UU, HW, HW, HW, BR, BR, BR, BR, HW, HW, HW, UU, CY, CY, CY, CY, CY, CY},
	{KN, KN, KN, KN, KN, KN, HW, HW, BR, BR, BR, BR, BR, BR, BR, BR, HW, HW, CY, CY, CY, CY, CY, CY},
	{KN, KN, KN, KN, KN, KN, HW, HW, BR, BR, BR, BR, BR, BR, BR, BR, HW, HW, CY, CY, CY, CY, CY, CY},
	{KN, KN, KN, KN, KN, KN, HW, HW, BR, BR, BR, BR, BR, BR, BR, BR, HW, HW, CY, CY, CY, CY, CY, CY},
	{KN, KN, KN, KN, KN, KN, HW, HW, BR, BR, BR, BR, BR, BR, BR, BR, HW, HW, HW, CY, CY, CY, CY, UU},
	{UU, KN, KN, KN, KN, KN, HW, HW, BR, BR, BR, BR, BR, BR, BR, BR, HW, HW, HW, HW, HW, HW, HW, HW},
	{HW, HW, HW, HW, HW, HW, HW, HW, BR, BR, BR, BR, BR, BR, BR, BR, HW, HW, HW, HW, HW, HW, HW, UU},
	{UU, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, LR, LR, LR, LR, LR, LR},
	{DR, DR, DR, DR, DR, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, LR, LR, LR, LR, LR, LR},
	{DR, DR, DR, DR, DR, DR, DR, DR, HW, HW, CR, CR, CR, CR, CR, HW, HW, HW, LR, LR, LR, LR, LR, LR},
	{DR, DR, DR, DR, DR, DR, DR, DR, HW, HW, CR, CR, CR, CR, CR, HW, HW, HW, LR, LR, LR, LR, LR, LR},
	{DR, DR, DR, DR, DR, DR, DR, DR, HW, HW, CR, CR, CR, CR, CR, HW, HW, HW, LR, LR, LR, LR, LR, LR},
	{DR, DR, DR, DR, DR, DR, DR, DR, HW, HW, CR, CR, CR, CR, CR, HW, HW, HW, HW, HW, HW, HW, HW, UU},
	{DR, DR, DR, DR, DR, DR, DR, DR, HW, HW, CR, CR, CR, CR, CR, HW, HW, HW, LY, LY, LY, LY, LY, UU},
	{DR, DR, DR, DR, DR, DR, DR, DR, HW, HW, CR, CR, CR, CR, CR, HW, HW, LY, LY, LY, LY, LY, LY, LY},
	{UU, HW, HW, HW, HW, HW, HW, HW, HW, HW, CR, CR, CR, CR, CR, HW, HW, LY, LY, LY, LY, LY, LY, LY},
	{HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, HW, LY, LY, LY, LY, LY, LY, LY},
	{UU, HW, HW, HW, HW, HW, HW, HW, HW, HL, HL, HL, HL, HL, HL, HW, HW, HW, LY, LY, LY, LY, LY, UU},
	{LG, LG, LG, LG, LG, LG, LG, HW, HW, HL, HL, HL, HL, HL, HL, HW, HW, HW, HW, HW, HW, HW, HW, HW},
	{LG, LG, LG, LG, LG, LG, LG, HW, HW, HL, HL, HL, HL, HL, HL, HW, HW, HW, HW, HW, HW, HW, HW, UU},
	{LG, LG, LG, LG, LG, LG, LG, HW, HW, HL, HL, HL, HL, HL, HL, HW, HW, SY, SY, SY, SY, SY, SY, SY},
	{LG, LG, LG, LG, LG, LG, LG, HW, HW, HL, HL, HL, HL, HL, HL, HW, HW, SY, SY, SY, SY, SY, SY, SY},
	{LG, LG, LG, LG, LG, LG, LG, HW, HW, HL, HL, HL, HL, HL, HL, HW, HW, SY, SY, SY, SY, SY, SY, SY},
	{LG, LG, LG, LG, LG, LG, UU, HW, UU, UU, UU, UU, UU, UU, UU, UU, HW, UU, SY, SY, SY, SY, SY, SY}
};

struct Doorway
{
	int i, j;
	int di, dj;
};

static Doorway doorwayArray[] = {
	{7, 4, -1, 0},
	{5, 7, 0, 1},
	{5, 16, 0, -1},
	{5, 18, -1, 0},
	{8, 9, -1, 0},
	{8, 14, -1, 0},
	{12, 8, 0, -1},
	{9, 17, 0, 1},
	{13, 20, 1, 0},
	{13, 22, -1, 0},
	{16, 6, -1, 0},
	{16, 16, 0, 1},
	{17, 11, 1, 0},
	{17, 12, 1, 0},
	{18, 6, 1, 0},
	{20, 15, 0, -1},
	{20, 17, 1, 0}
};

//-------------------------------------- BoardGraph --------------------------------------

BoardGraph::BoardGraph()
{
}

/*virtual*/ BoardGraph::~BoardGraph()
{
}

std::shared_ptr<BoardGraph::Node> BoardGraph::GetCharacterStartLocation(Character character)
{
	static Vector2D startLocationArray[] =
	{
		Vector2D(7, CLUE_BOARD_HEIGHT - 1 - 24),
		Vector2D(0, CLUE_BOARD_HEIGHT - 1 - 17),
		Vector2D(9, CLUE_BOARD_HEIGHT - 1 - 0),
		Vector2D(14, CLUE_BOARD_HEIGHT - 1 - 0),
		Vector2D(23, CLUE_BOARD_HEIGHT - 1 - 6),
		Vector2D(23, CLUE_BOARD_HEIGHT - 1 - 19)
	};

	for (std::shared_ptr<Node>& node : this->nodeArray)
		if (node->location == startLocationArray[character])
			return node;

	return std::shared_ptr<BoardGraph::Node>();
}

std::shared_ptr<BoardGraph::Node> BoardGraph::GetNodeContainingLocation(const Vector2D& location)
{
	for (std::shared_ptr<Node>& node : this->nodeArray)
		if (node->GetBox().ContainsPoint(location))
			return node;

	return std::shared_ptr<BoardGraph::Node>();
}

bool BoardGraph::IsGenerated() const
{
	return this->nodeArray.size() > 0;
}

void BoardGraph::Regenerate()
{
	this->nodeArray.clear();

	std::shared_ptr<Node> nodeMatrix[CLUE_BOARD_HEIGHT][CLUE_BOARD_WIDTH];

	uint32_t id = 0;

	for (int i = 0; i < CLUE_BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < CLUE_BOARD_WIDTH; j++)
		{
			std::optional<Room> room;
			int roomCode = boardMatrix[i][j];
			if (roomCode >= 0)
				room = Room(roomCode);

			std::shared_ptr<Node> node = std::make_shared<Node>(id++, room);

			node->location.x = double(j);
			node->location.y = double(CLUE_BOARD_HEIGHT - 1 - i);

			node->box.minCorner = node->location - Vector2D(0.5, 0.5);
			node->box.maxCorner = node->location + Vector2D(0.5, 0.5);

			nodeMatrix[i][j] = node;
			this->nodeArray.push_back(node);
		}
	}

	for (int i = 0; i < CLUE_BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < CLUE_BOARD_WIDTH; j++)
		{
			std::shared_ptr<Node> node = nodeMatrix[i][j];

			if (i - 1 >= 0)
				node->AddAdjacency(nodeMatrix[i - 1][j].get());

			if (i + 1 < CLUE_BOARD_HEIGHT)
				node->AddAdjacency(nodeMatrix[i + 1][j].get());

			if (j - 1 >= 0)
				node->AddAdjacency(nodeMatrix[i][j - 1].get());

			if (j + 1 < CLUE_BOARD_WIDTH)
				node->AddAdjacency(nodeMatrix[i][j + 1].get());
		}
	}

	for (int i = 0; i < CLUE_BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < CLUE_BOARD_WIDTH; j++)
		{
			std::shared_ptr<Node> node = nodeMatrix[i][j];

			if (i - 1 >= 0 && nodeMatrix[i - 1][j]->room == node->room)
				node->AddPathway(nodeMatrix[i - 1][j].get());

			if (i + 1 < CLUE_BOARD_HEIGHT && nodeMatrix[i + 1][j]->room == node->room)
				node->AddPathway(nodeMatrix[i + 1][j].get());

			if (j - 1 >= 0 && nodeMatrix[i][j - 1]->room == node->room)
				node->AddPathway(nodeMatrix[i][j - 1].get());

			if (j + 1 < CLUE_BOARD_WIDTH && nodeMatrix[i][j + 1]->room == node->room)
				node->AddPathway(nodeMatrix[i][j + 1].get());
		}
	}

	for (const Doorway& doorway : doorwayArray)
	{
		std::shared_ptr<Node> nodeA = nodeMatrix[doorway.i][doorway.j];
		std::shared_ptr<Node> nodeB = nodeMatrix[doorway.i + doorway.di][doorway.j + doorway.dj];

		assert(!nodeA->IsRoom() && nodeB->IsRoom());

		nodeA->AddPathway(nodeB.get());
	}

	// TODO: What about the secret passages?
	//       Make some adjacencies between rooms on opposite corners of the board.
}

Box2D BoardGraph::GetBoundingBox() const
{
	Box2D boundingBox;
	for (const std::shared_ptr<Node>& node : this->nodeArray)
		boundingBox.ExpandToIncludePoint(node->location);

	return boundingBox;
}

void BoardGraph::ForAllNodes(std::function<bool(const Node*)> callback) const
{
	for (const std::shared_ptr<Node>& node : this->nodeArray)
		if (!callback(node.get()))
			break;
}

bool BoardGraph::FindShortestPathBetweenNodes(Node* nodeA, Node* nodeB, std::vector<Node*>& nodeArray)
{
	nodeArray.clear();

	for (std::shared_ptr<Node>& node : this->nodeArray)
	{
		node->parentNode = nullptr;
		node->considered = false;
	}

	std::list<Node*> queue;
	queue.push_back(nodeB);
	nodeB->considered = true;

	while (queue.size() > 0)
	{
		Node* node = *queue.begin();
		queue.pop_front();

		if (node == nodeA)
		{
			while (node != nodeB)
			{
				nodeArray.push_back(node);
				node = node->parentNode;
			}

			nodeArray.push_back(nodeB);
			break;
		}

		for (Node* pathwayNode : node->pathwayNodeSet)
		{
			if (!pathwayNode->considered)
			{
				queue.push_back(pathwayNode);
				pathwayNode->parentNode = node;
				pathwayNode->considered = true;
			}
		}
	}

	return nodeArray.size() > 0;
}

/*static*/ int BoardGraph::CalculatePathCost(const std::vector<Node*>& nodeArray)
{
	int numHallwayTiles = 0;
	int numRoomTiles = 0;

	for (const Node* node : nodeArray)
	{
		if (node->IsRoom())
			numRoomTiles++;
		else
			numHallwayTiles++;
	}

	return numHallwayTiles - 1 + ((numRoomTiles > 0) ? 1 : 0);
}

std::shared_ptr<BoardGraph::Node> BoardGraph::FindNodeWithID(int id)
{
	for (std::shared_ptr<Node>& node : this->nodeArray)
		if (node->id == id)
			return node;

	return std::shared_ptr<Node>();
}

//-------------------------------------- BoardGraph::Node --------------------------------------

BoardGraph::Node::Node(int id, std::optional<Room> room)
{
	this->id = id;
	this->room = room;
	this->parentNode = nullptr;
	this->considered = false;
}

/*virtual*/ BoardGraph::Node::~Node()
{
}

uint32_t BoardGraph::Node::GetId() const
{
	return this->id;
}

bool BoardGraph::Node::IsRoom() const
{
	return this->room.has_value();
}

std::optional<Room> BoardGraph::Node::GetRoom() const
{
	return this->room;
}

const Vector2D& BoardGraph::Node::GetLocation() const
{
	return this->location;
}

const Box2D& BoardGraph::Node::GetBox() const
{
	return this->box;
}

void BoardGraph::Node::AddAdjacency(Node* node)
{
	this->adjacentNodeSet.insert(node);
	node->adjacentNodeSet.insert(this);
}

void BoardGraph::Node::AddPathway(Node* node)
{
	this->pathwayNodeSet.insert(node);
	node->pathwayNodeSet.insert(this);
}

void BoardGraph::Node::GetAdjacencies(std::vector<const Node*>& adjacentNodeArray) const
{
	adjacentNodeArray.clear();
	for (Node* node : this->adjacentNodeSet)
		adjacentNodeArray.push_back(node);
}

bool BoardGraph::Node::IsPathway(const Node* node) const
{
	auto iter = this->pathwayNodeSet.find(const_cast<Node*>(node));
	return iter != this->pathwayNodeSet.end();
}