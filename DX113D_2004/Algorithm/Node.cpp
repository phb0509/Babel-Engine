#include "Framework.h"

Node::Node(Vector3 pos, int index, Float2 interval): 
	mPosition(pos), 
	mIndex(index), 
	mDistanceBetweenNodes(interval),
	mbIsCheck(false),
	mVia(-1), 
	mF(0), mG(0), mH(0), 
	mState(NONE), 
	mObstacle(nullptr)
{
	mCollider = new SphereCollider();
	mCollider->mPosition = pos;
}

Node::~Node()
{
	delete mCollider;
	if(mObstacle != nullptr)
		delete mObstacle;

	for (EdgeInfo* edge : edges)
		delete edge;
}

void Node::Render()
{
	switch (mState)
	{
	case Node::NONE:
		mCollider->SetColor(Float4(1, 1, 1, 1));
		break;
	case Node::OPEN:
		mCollider->SetColor(Float4(0, 0, 1, 1));
		break;
	case Node::CLOSED:
		mCollider->SetColor(Float4(1, 0, 0, 1));
		break;
	case Node::USING:
		mCollider->SetColor(Float4(0, 1, 0, 1));
		break;
	case Node::OBSTACLE:
		mCollider->SetColor(Float4(0, 0, 0, 1));
		break;	
	case Node::DIRECT:
		mCollider->SetColor(Float4(1, 1, 0, 1));
		break;
	default:
		break;
	}

	if(mbIsCheck)
	{
		mCollider->SetColor(Float4(0.6f, 0.0f, 1.0f, 1.0f));
	}


	mCollider->Render();
	if(mObstacle != nullptr)
		mObstacle->Render();
}

void Node::AddEdge(Node* node)
{
	EdgeInfo* edge = new EdgeInfo();
	edge->index = node->mIndex;
	edge->edgeCost = Distance(mPosition, node->mPosition);

	edges.emplace_back(edge);
}

Collider* Node::MakeObstacle()
{
	mState = OBSTACLE;

	Vector3 min = { -mDistanceBetweenNodes.x * 0.6f, -20, -mDistanceBetweenNodes.y * 0.6f };
	Vector3 max = min * -1.0f;

	mObstacle = new BoxCollider(min, max);
	mObstacle->mPosition = mCollider->mPosition;

	return mObstacle;
}
