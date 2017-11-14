#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"
#include "j1Map.h"

j1PathFinding::j1PathFinding() : j1Module(), last_path(nullptr), width(0), height(0)
{
	name.create("pathfinding");
	maps.clear();
	for (uint i = 0; i < LayerID::LAYER_AMOUNT; i++)
		maps.add(new uint());
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	for (p2List_item<uint*>* map = maps.start; map != nullptr; map = map->next)
		RELEASE_ARRAY(map->data);
	maps.clear();
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");
	
	for (p2List_item<uint*>* map = maps.start; map != nullptr; map = map->next)
		RELEASE_ARRAY(map->data);
	maps.clear();

	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, const LayerID layer, uint* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(maps[(const uint)layer]);
	maps[(const uint)layer] = new uint[width*height];
	memcpy(maps[(const uint)layer], data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos, const LayerID layer) const
{
	uchar t = GetTileAt(pos, layer);
	return t == 0;//INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos, const LayerID layer) const
{
	if(CheckBoundaries(pos))
		return maps.At((const uint)layer)->data[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const p2DynArray<iPoint>* j1PathFinding::GetLastPath() const
{
	return last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while(item)
	{
		if(item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = list.end;
	while(item)
	{
		if(item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.count();
	LayerID layer = list_to_fill.layer;

	// north
	cell.create(pos.x, pos.y + 1);
	if(App->pathfinding->IsWalkable(cell, layer))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if(App->pathfinding->IsWalkable(cell, layer))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if(App->pathfinding->IsWalkable(cell, layer))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if(App->pathfinding->IsWalkable(cell, layer))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	return list_to_fill.list.count();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination, const LayerID layer, p2DynArray<iPoint>* path)
{
	// TODO 1: if origin or destination are not walkable, return -1
	if (!(IsWalkable(origin, layer) || !IsWalkable(destination, layer)))
		return -1;

	// TODO 2: Create two lists: open, closed
	// Add the origin tile to open
	// Iterate while we have tiles in the open list
	PathList open, closed;
	open.layer = layer;
	closed.layer = layer;
	PathNode node;
	node.pos = origin;
	open.list.add(node);

	last_path = path;

	while (open.list.count() > 0)
	{
		// TODO 3: Move the lowest score cell from open list to the closed list
		p2List_item<PathNode>* current_item = open.GetNodeLowestScore();
		p2List_item<PathNode>* curr_node = closed.list.add(current_item->data);


		// TODO 4: If we just added the destination, we are done!
		// Backtrack to create the final path
		// Use the Pathnode::parent and Flip() the path when you are finish
		if (curr_node->data.pos == destination)
		{
			last_path->Clear();
			while (curr_node->data.parent != nullptr) {
				last_path->PushBack(curr_node->data.pos);
				curr_node = closed.Find(curr_node->data.parent->pos);
			}
			//last_path->PushBack(curr_node->data.pos);
			last_path->Flip();
			return last_path->Count();
		}

		// TODO 5: Fill a list of all adjancent nodes
		PathList neighbours;
		neighbours.layer = layer;
		curr_node->data.FindWalkableAdjacents(neighbours);

		// TODO 6: Iterate adjancent nodes:
		// ignore nodes in the closed list
		// If it is NOT found, calculate its F and add it to the open list
		// If it is already in the open list, check if it is a better path (compare G)
		// If it is a better path, Update the parent
		for (p2List_item<PathNode>* neighbour_node = neighbours.list.start; neighbour_node != nullptr; neighbour_node = neighbour_node->next)
		{
			iPoint neighbour_pos = neighbour_node->data.pos;
			if (closed.Find(neighbour_pos) != nullptr)
				continue;

			p2List_item<PathNode>* node_open = open.Find(neighbour_pos);
			if (node_open == nullptr)
			{
				neighbour_node->data.CalculateF(destination);
				open.list.add(neighbour_node->data);
			} else {
				int new_g = neighbour_node->data.parent->g + 1;
				if (new_g < node_open->data.g)
				{
					node_open->data.g = new_g;
					node_open->data.parent = &curr_node->data;
				}
			}
		}
		open.list.del(current_item);
	}
	return -1;
}

void j1PathFinding::LoadNodeMap(const MapData map_data)
{
	for (int x = 0; x < map_data.width; x++) {
		for (int y = map_data.height; y >= 0; y-- ) {
			
		}
	}
}