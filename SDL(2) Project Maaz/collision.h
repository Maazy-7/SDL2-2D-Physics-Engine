#pragma once


Vector2 contactList[2];
Vector2 impulseList[2];
Vector2 raList[2];
Vector2 rbList[2];
std::tuple<Vector2, Vector2> possibleContactPoints;
int contactCount = 0;

std::tuple<Vector2, float> pointSegmentDistance(Vector2 v, Vector2 w, Vector2 p)//v and w are the 2 points of the line segment and p is the actual point
{
	float lengthSqr = distanceSqr(v, w);//squared length so sqrt() function can be avoided
	if (lengthSqr == 0.0f) { return std::make_tuple(p,distanceSqr(p,v)); }// returns distance if line segment magnitude is zero, since if magnitude is zero its just dist between the 2 point
	float t = clamp(0.0f, 1.0f, dot(p - v, w - v) / lengthSqr);
	Vector2 projection = v + (w - v)*t;
	return std::make_tuple(projection, distance(p,projection));
}

Vector2 findCirclePolygonContactPoints(Object* o1, Object* o2)
{
	float minDist = INFINITY;
	Vector2 contact = Vector2();

	int size = o2->vertices.size();

	std::tuple<Vector2, float> vecAndDist = std::make_tuple(Vector2(),0.f);
	for (int i = 0; i < size; i++) 
	{
		vecAndDist = pointSegmentDistance(*(o2->vertices[i]),*(o2->vertices[(i==size-1) ? 0 : i+1]), o1->position);
		float dist = std::get<1>(vecAndDist);
		
		if ( dist < minDist) 
		{
			minDist = dist;
			contact = std::get<0>(vecAndDist);
		}
	}
	return contact;
}

Vector2 findCircleCircleContactPoints(Object* o1, Object* o2) 
{
	return o2->position + (o1->position - o2->position).normal()*o1->radius;
}

std::tuple<Vector2,Vector2> findPolygonPolygonContactPoints(Object* o1, Object* o2) 
{
	Vector2 contact1 = Vector2();
	Vector2 contact2 = Vector2();
	int contactCount = 0;
	float minDist = INFINITY;
	std::tuple<Vector2, float> vecAndDist = std::make_tuple(Vector2(), 0.f);

	
	for (int i = 0; i < o1->vertices.size(); i++) //testing every o1 vertex with every edge of o2
	{
		for (int j = 0; j < o2->vertices.size(); j++) 
		{
			vecAndDist = pointSegmentDistance(*(o2->vertices[j]),*(o2->vertices[j == o2->vertices.size()-1 ? 0 : j+1]), *(o1->vertices[i]));

			float dist = std::get<1>(vecAndDist);
			Vector2 point = std::get<0>(vecAndDist);

			//checking for another possible contact point if they are equally as close to the edge as the other contact point
			if (nearlyEqual(minDist,dist)) 
			{
				if (!nearlyEqual(contact1, point))//if the 2 points are not the same then add the contact point
				{
					contact2 = point;
					contactCount = 2;
				}
			}
			//finding the closest point to the edge and setting it as the contact point, the point with the least distance between the edge and vertex is the contact point
			else if (dist < minDist)
			{
				minDist = dist;
				contact1 = point;
				contactCount = 1;
			}
		}
	}

	for (int i = 0; i < o2->vertices.size(); i++) //testing every o2 vertex with every edge of o1
	{
		for (int j = 0; j < o1->vertices.size(); j++)
		{
			vecAndDist = pointSegmentDistance(*(o1->vertices[j]), *(o1->vertices[j == o1->vertices.size()-1 ? 0 : j + 1]), *(o2->vertices[i]));

			float dist = std::get<1>(vecAndDist);
			Vector2 point = std::get<0>(vecAndDist);

			//checking for another possible contact point if they are equally as close to the edge as the other contact point
			if (nearlyEqual(minDist, dist))
			{
				if (!nearlyEqual(contact1, point))//if the 2 points are not the same then add the contact point
				{
					contact2 = point;
					contactCount = 2;
				}
			}
			//finding the closest point to the edge and setting it as the contact point, the point with the least distance between the edge and vertex is the contact point
			else if (dist < minDist)
			{
				minDist = dist;
				contact1 = point;
				contactCount = 1;
			}
		}
	}

	std::tuple<Vector2, Vector2> contactPoints = std::make_tuple(Vector2(), Vector2());

	if (contactCount == 1)//if only 1 contact point then we return tuple with both of the same points so we only add 1 contact point to the contact point list
	{
		contactPoints = std::make_tuple(contact1, contact1);
	}
	else	
	{
		contactPoints = std::make_tuple(contact1, contact2);
	}
	return contactPoints;
}

int findClosestPointOnPolygon(Vector2 center, std::vector<Vector2*> vertices) 
{
	int result = -1;
	float minDist = INFINITY;
	for (int i = 0; i < vertices.size(); i++) 
	{
		float dist = distanceSqr(center, *vertices[i]);//distance square to avoid sqr as we are only comparing against themselves
		if (dist < minDist) 
		{
			dist = minDist;
			result = i;
		}
	}
	return result;//result is an index that returns the index of the closes vertex to the center
}

void resolveCollision(Object* o1, Object* o2, Vector2 normal)
{
	Vector2 relativeVelocity = o1->getVelocity() - o2->getVelocity();
	float dir = dot(relativeVelocity, normal);

	if (dir > 0.0f)
	{
		return;
	}

	float e = min(o1->restitution, o2->restitution);

	float j = -(1.f + e) * dir;
	j /= o1->invMass + o2->invMass;

	Vector2 impulse = normal * j;

	if (o2->isStatic) 
	{
		o1->addVelocity(impulse * o1->invMass);
	}
	else if(o1->isStatic) 
	{
		o2->addVelocity(impulse * -o2->invMass);
	}
	else
	{
		o1->addVelocity(impulse * o1->invMass);
		o2->addVelocity(impulse * -o2->invMass);
	}
}
int c = 0;
void resolveCollisionRotation(Object *o1, Object *o2, Vector2 normal, Vector2 contact1, Vector2 contact2, int contactCount) 
{
	//if (!o1->isStatic || !o2->isStatic) { c++; Log2("iteration: ", c); }
	float e = min(o1->restitution,o2->restitution);

	o1->contactDraw = true;
	o2->contactDraw = true;

	o1->contact = contact1;
	o2->contact = contact1;

	o1->contact2 = contact2;
	o2->contact2 = contact2;

	contactList[0] = contact1;
	contactList[1] = contact2;

	for (int i = 0; i < contactCount; i++) 
	{
		impulseList[i] = Vector2();
		raList[i] = Vector2();
		rbList[i] = Vector2();
	}

	for (int i = 0; i < contactCount; i++) 
	{
		//vector from center of shape to contact point
		Vector2 ra = contactList[i] - o1->position;
		Vector2 rb = contactList[i] - o2->position;

		raList[i] = ra;
		rbList[i] = rb;

		//calculations involving angles must be in radians
		
		Vector2 angularLinearVelocityA = cross(DegtoRad(o1->angularVelocity),ra);
		Vector2 angularLinearVelocityB = cross(DegtoRad(o2->angularVelocity),rb);

		Vector2 relativeVelocity = (o1->getVelocity() + angularLinearVelocityA) - (o2->getVelocity() + angularLinearVelocityB);

		float contactVelocityMag = dot(relativeVelocity,normal);

		if (contactVelocityMag > 0.f) 
		{
			continue;
		}

		float raCrossN = cross(ra,normal);
		float rbCrossN = cross(rb,normal);

		//if (!o1->isStatic || !o2->isStatic) { ra.print(); }

		float denom = o1->invMass + o2->invMass + (raCrossN * raCrossN)*o1->inverseInertia + (rbCrossN * rbCrossN) * o2->inverseInertia;

		float j = -(1.f + e) * contactVelocityMag;

		j /= denom;
		j /= (float)contactCount;

		Vector2 impulse = normal * j;

		impulseList[i] = impulse;
	}

	if (!o1->isStatic || !o2->isStatic)
	{
		for (int i = 0; i < contactCount; i++)
		{
			Vector2 impulse = impulseList[i];
			if (o2->isStatic) 
			{
				o1->addVelocity(impulse * o1->invMass);
				o1->addAngularVelocity(RadtoDeg(cross(raList[i], impulse) * o1->inverseInertia));
			}
			else if (o1->isStatic)
			{
				o2->addVelocity(impulse * -o2->invMass);
				o2->addAngularVelocity(RadtoDeg(-cross(rbList[i], impulse) * o2->inverseInertia));
			}
			else 
			{
				o1->addVelocity(impulse * o1->invMass);
				o1->addAngularVelocity(RadtoDeg(cross(raList[i], impulse) * o1->inverseInertia));

				o2->addVelocity(impulse * -o2->invMass);
				o2->addAngularVelocity(RadtoDeg(-cross(rbList[i], impulse) * o2->inverseInertia));
			}
		}
	}
}

bool circleCircleCollision(Object* o1, Object* o2)//circle vs circle collision
{
	float distance = distanceSqr(o1->position,o2->position);//distance squared so sqrt calculation is avoided
	int radii = (o1->radius + o2->radius);
	if (distance >= radii*radii)
	{
		return false;
	}

	Vector2 normal = (o1->position - o2->position).normal();

	distance = sqrt(distance);//if collision is detected then sqrt the distance to make it relative to the radius
	float depth = radii - distance;
	
	if (o1->isStatic) 
	{
		Vector2 vel = o2->getVelocity();
		
		o2->position += normal * -depth;
		
		o2->setVelocity(vel);
	}
	else if (o2->isStatic) 
	{
		Vector2 vel = o1->getVelocity();
		
		o1->position += normal * depth;
		
		o1->setVelocity(vel);
	}
	else 
	{
		Vector2 vel1 = o1->getVelocity();
		Vector2 vel2 = o2->getVelocity();

		o1->position += normal * depth * 0.5f;
		o2->position += normal * depth * -0.5f;
		
		o1->setVelocity(vel1);
		o2->setVelocity(vel2);
	}

	resolveCollision(o1,o2,normal);
	return true;
}

bool polygonPolygonCollision(Object* o1, Object* o2, Vector2 &normal) //SAT collision
{
	//Vector2 normal = Vector2();//axis variable declared
	float depth = INFINITY;// declaring depth to infinity as we need to get the smallest one

	for (int i = 0; i < o1->vertices.size();i++) 
	{
		Vector2 axis = *(o1->vertices[i==o1->vertices.size()-1 ? 0:i+1]) - *(o1->vertices[i]);//gets the edges of the shape

		axis = perpendicular(axis);//gets the perpendicular axis
		axis.normalize();

		float max1 = -INFINITY;
		float min1 = INFINITY;

		for (int j = 0; j < o1->vertices.size();j++) 
		{
			min1 = min(min1,dot(*(o1->vertices[j]),axis));//projecting o1 vertices onto axis
			max1 = max(max1,dot(*(o1->vertices[j]),axis));
		}

		float max2 = -INFINITY;
		float min2 = INFINITY;

		for (int j = 0; j < o2->vertices.size(); j++)
		{
			min2 = min(min2, dot(*(o2->vertices[j]), axis));//projecting o2 vertices onto axis
			max2 = max(max2, dot(*(o2->vertices[j]), axis));
		}

		if (min1 > max2 || min2 > max1) 
		{
			return false;
		}


		float currentDepth = min(max2-min1,max1-min2);

		if (currentDepth < depth)
		{
			depth = currentDepth;
			normal = axis;
		}
	}

	for (int i = 0; i < o2->vertices.size(); i++)
	{
		Vector2 axis = *(o2->vertices[i == o2->vertices.size()-1 ? 0 : i + 1]) - *(o2->vertices[i]);//gets the edges of the shape

		axis = perpendicular(axis);//gets the perpendicular axis
		axis.normalize();

		float max1 = -INFINITY;
		float min1 = INFINITY;

		for (int j = 0; j < o1->vertices.size(); j++)
		{
			min1 = min(min1, dot(*(o1->vertices[j]), axis));//projecting o1 vertices onto axis
			max1 = max(max1, dot(*(o1->vertices[j]), axis));
		}

		float max2 = -INFINITY;
		float min2 = INFINITY;

		for (int j = 0; j < o2->vertices.size(); j++)
		{
			min2 = min(min2, dot(*(o2->vertices[j]), axis));//projecting o2 vertices onto axis
			max2 = max(max2, dot(*(o2->vertices[j]), axis));
		}

		if (min1 > max2 || min2 > max1)
		{
			return false;
		}

		float currentDepth = min(max2 - min1, max1 - min2);

		if (currentDepth < depth)
		{
			depth = currentDepth;
			normal = axis;
		}
	}

	//collision direction 
	Vector2 direction = o1->position - o2->position;

	//check to see if one object is behind the other so then the normal is reversed in direction
	if (dot(direction,normal) < 0.0f) 
	{
		normal *= -1;
	}

	//moving objects
	if (o1->isStatic) //static objects dont move so full depth of penetration is applied to non static object
	{
		Vector2 vel = o2->getVelocity();

		o2->position += (normal * -depth);
		o2->movePos(normal * -depth);
		
		o2->setVelocity(vel);
	}
	else if (o2->isStatic) 
	{
		Vector2 vel = o1->getVelocity();
		
		o1->position += (normal * depth);
		o1->movePos(normal * depth);
		
		o1->setVelocity(vel);
	}
	else
	{
		Vector2 vel1 = o1->getVelocity();
		Vector2 vel2 = o2->getVelocity();
		
		o1->position += (normal * depth / 2);
		o1->movePos(normal * depth / 2);//move pos moves the objects vertices
		o2->position += (normal * depth / -2);
		o2->movePos(normal * depth / -2);

		o1->setVelocity(vel1);
		o2->setVelocity(vel2);
	}


	possibleContactPoints = findPolygonPolygonContactPoints(o2,o1);
	if (std::get<0>(possibleContactPoints) == std::get<1>(possibleContactPoints))
	{
		contactCount = 1;
	}
	else 
	{
		contactCount = 2;
	}

	//resolveCollisionRotation(o1, o2, normal, std::get<0>(possibleContactPoints), std::get<1>(possibleContactPoints),contactCount);
	
	//resolveCollision(o1, o2, normal);
	
	return true;
}

bool circlePolygonCollision(Object* o1, Object* o2) 
{
	//o2 is the polygon while o1 is the circle
	Vector2 normal = Vector2();
	float depth = INFINITY;

	for (int i = 0; i < o2->vertices.size();i++) 
	{
		Vector2 axis = *(o2->vertices[i == o2->vertices.size() - 1 ? 0 : i + 1]) - *(o2->vertices[i]);//gets the edges of the shape
		axis = perpendicular(axis);
		axis.normalize();

		float max1 = -INFINITY;
		float min1 = INFINITY;
		

		for (int j = 0; j < o2->vertices.size(); j++)
		{
			max1 = max(max1,(dot(*(o2->vertices[j]), axis)));
			min1 = min(min1,(dot(*(o2->vertices[j]), axis)));
		}

		//circle max and min, only 1 max and min since circle is same all around
		float min2 = dot(o1->position - axis * o1->radius,axis);
		float max2 = dot(o1->position + axis * o1->radius, axis);

		if (min2 > max2) 
		{
			std::swap(min2, max2);
		}

		if (min1 > max2 || min2 > max1) 
		{
			return false;
		}

		float axisDepth = min(max2-min1,max1-min2);

		if (axisDepth < depth) 
		{
			depth = axisDepth;
			normal = axis;
		}
	}

	Vector2 axis = *(o2->vertices[findClosestPointOnPolygon(o1->position, o2->vertices)]) - o1->position;
	axis.normalize();

	float max1 = -INFINITY;
	float min1 = INFINITY;

	for (int i = 0; i < o2->vertices.size(); i++) 
	{
		max1 = max(max1, (dot(*(o2->vertices[i]), axis)));
		min1 = min(min1, (dot(*(o2->vertices[i]), axis)));
	}

	//circle max and min, only 1 max and min since circle is same all around
	float min2 = dot(o1->position + axis * o1->radius, axis);
	float max2 = dot(o1->position - axis * o1->radius, axis);

	if (min2 > max2) 
	{
		std::swap(min2, max2);
	}

	if (min1 >= max2 || min2 >= max1)
	{
		return false;
	}

	float axisDepth = min(max2 - min1, max1 - min2);

	if (axisDepth < depth)
	{
		depth = axisDepth;
		normal = axis;
	}
 
	Vector2 direction = o1->position - o2->position;

	//check to see if one object is behind the other so then the normal is reversed in direction
	if (dot(direction, normal) < 0.0f)
	{
		normal *= -1;
	}

	//moving objects
	if (o1->isStatic) //static objects dont move so full depth of penetration is applied to non static object
	{
		Vector2 vel = o2->getVelocity();

		o2->position += (normal * -depth);
		o2->movePos(normal * -depth);

		o2->setVelocity(vel);
	}
	else if (o2->isStatic)
	{
		Vector2 vel = o1->getVelocity();
		
		o1->position += (normal * depth);

		o1->setVelocity(vel);
	}
	else
	{
		Vector2 vel1 = o1->getVelocity();
		Vector2 vel2 = o2->getVelocity();

		o1->position += (normal * depth / 2);
		o2->position += (normal * depth / -2);
		o2->movePos(normal * depth / -2);

		o1->setVelocity(vel1);
		o2->setVelocity(vel2);
	}

	Vector2 contact = findCirclePolygonContactPoints(o1,o2);
	resolveCollisionRotation(o1,o2,normal,contact,contact,1);
	//resolveCollision(o1, o2, normal);
	return true;
}

bool pointCircleCollision(Object* o1, Vector2 mousePos) 
{
	if (o1->isStatic) { return false; }
	if (distanceSqr(o1->position,mousePos) > o1->radius*o1->radius) 
	{
		return false;
	}
	return true;
}