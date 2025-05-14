#pragma once
class Spring 
{
public:
	Object* o1;
	Object* o2;
	float stiffness;
	float damping;
	float restLength;
	float force;

	Spring(Object* o1_, Object* o2_, float restLength_, float stiffness_, float damping_) :
		o1(o1_), o2(o2_), restLength(restLength_), stiffness(stiffness_), damping(damping_) {}


	Vector2 springCenter()
	{
		return (o2->position + o1->position) * 0.5f;
	}

	float length()
	{
		return distance(o1->position, o2->position);
	}

	void applySpringForce()
	{
		if (!o1 || !o2) { delete this; return; }
		Vector2 dir = o2->position - o1->position;
		float length = dir.distance();
		float deltaLength = length - restLength;

		float stiffForce = deltaLength * stiffness;

		dir.normalize();

		Vector2 relativeVelocity = o2->velocity - o1->velocity;
		float dampForce = dot(relativeVelocity, dir) * damping;
		o1->addForce((this->springCenter() - o1->position) * (stiffForce + dampForce));
		o2->addForce((this->springCenter() - o2->position) * (stiffForce + dampForce));
	}

	void draw(SDL_Renderer* renderer) 
	{
		if (!o1 || !o2) { delete this; return; }
		drawLine(renderer,o1->r,o1->g,o1->b,o1->position.x,o1->position.y,o2->position.x,o2->position.y);
	}
};