#include "Framework.h"

Node::Node(Vector3 pos, int index, Float2 interval)
	: pos(pos), index(index), interval(interval),
	via(-1), f(0), g(0), h(0), state(NONE), obstacle(nullptr)
{
	collider = new SphereCollider();
	collider->position = pos;
}

Node::~Node()
{
	delete collider;
	if(obstacle != nullptr)
		delete obstacle;

	for (EdgeInfo* edge : edges)
		delete edge;
}

void Node::Render()
{
	switch (state)
	{
	case Node::NONE:
		collider->SetColor(Float4(1, 1, 1, 1));
		break;
	case Node::OPEN:
		collider->SetColor(Float4(0, 0, 1, 1));
		break;
	case Node::CLOSED:
		collider->SetColor(Float4(1, 0, 0, 1));
		break;
	case Node::USING:
		collider->SetColor(Float4(0, 1, 0, 1));
		break;
	case Node::OBSTACLE:
		collider->SetColor(Float4(0, 0, 0, 1));
		break;	
	}

	collider->Render();
	if(obstacle != nullptr)
		obstacle->Render();
}

void Node::AddEdge(Node* node)
{
	EdgeInfo* edge = new EdgeInfo();
	edge->index = node->index;
	edge->edgeCost = Distance(pos, node->pos);

	edges.emplace_back(edge);
}

Collider* Node::MakeObstacle()
{
	state = OBSTACLE;

	Vector3 min = { -interval.x * 0.6f, -20, -interval.y * 0.6f };
	Vector3 max = min * -1.0f;

	obstacle = new BoxCollider(min, max);
	obstacle->position = collider->position;

	return obstacle;
}
