#pragma once

class Object 
{
public:
	float radius;
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	Vector2 oldPositon = position;
	Vector2 force;
	float angle;
	float oldAngle = angle;
	float angularVelocity = 0.f;
	float angularAcceleration = 0.f;
	float mass;
	float invMass = mass == 0 ? 0 : 1/mass;
	float inertia;
	float inverseInertia;
	Uint8 r,g,b;
	int verts; //number of vertices, less than 3 is circle and more is just a polygon
	std::vector <Vector2*> vertices;
	float deltaTime;
	float restitution;
	bool polygon;
	bool isStatic;
	bool contactDraw;
	Vector2 contact;
	Vector2 contact2;

	Object(Vector2 position_, float radius_, Uint8 r_, Uint8 g_, Uint8 b_,int verts_,float angle_ = 0, float mass_ = 10.f,bool isStatic_ = false, float restitution_ = 0.8) : 
		radius(radius_), position(position_), r(r_), g(g_), b(b_), verts(verts_), angle(angle_), isStatic(isStatic_), mass(mass_), restitution(restitution_)
	{
		if (verts > 2) 
		{
			float theta = 360 / verts;
			for (int i = 0; i<verts; i++)
			{
				vertices.push_back(new Vector2(Vector2(0,radius).rotated(theta*i+angle)+position));
			}
			polygon = true;

			if (verts == 4) //square inertia
			{
				inertia = (1.f / 6.f) * mass * radius * radius;
				inverseInertia = (inertia == 0) ? 0 : 1 / inertia;
			}
			else if (verts == 3) //triangle inertia 
			{
				inertia = (5.f/36.f) * (mass) * 2 * radius*radius;
				inverseInertia = (inertia == 0) ? 0 : 1 / inertia;
			}
		}
		else //circle inertia
		{
			inertia = 0.5 * mass * radius * radius;
			inverseInertia = (inertia == 0) ? 0 : 1 / inertia;
			polygon = false;
		}
		if (isStatic) { velocity = Vector2(); angularVelocity = 0.f; }
	} 


	void updateE(float dt) 
	{//euler method update
		velocity += acceleration*dt;
		position += velocity*dt;
		movePos(velocity*dt);

		if (position.y <= (-HEIGHT / 2) + 2*radius)
		{
			position.y = (-HEIGHT / 2) + 2 * radius;
		}
		if (position.y >= (HEIGHT / 2) - 2*radius)
		{
			velocity.y *= -1;
		}
		if (position.x <= (-WIDTH / 2) + 2*radius)
		{
			velocity.x *= -1;
		}
		if (position.x >= (WIDTH / 2) - 2*radius)
		{
			velocity.x *= -1;
		}

		if (position.x <= -100)
		{
			position.x = -100;
		}
		if (position.x >= 100)
		{
			position.x = 100;
		}

		acceleration = Vector2();
	}
	
	void applyCircleConstraint(Vector2 constraintCenter,float constraintRadius) 
	{
		float dist = distance(constraintCenter, position);//distance between the center and the object
		Vector2 dir = constraintCenter - position;//direction of the object (bascially what direction the object is heading/facing)
		if (dist > constraintRadius-radius)
		{
			Vector2 n = dir / dist;//normal vector (direction vector normalized)
			position = constraintCenter - n * (constraintRadius - radius);//setting position on the outer edge of circle if outside circle 
		}
	}

	void applyBoxConstraint(Vector2 constraintCenter, float width, float height) 
	{
		
		if (position.x+radius > constraintCenter.x+width) 
		{
			position.x = constraintCenter.x + width - radius;
		}

		if (position.x - radius < constraintCenter.x - width)
		{
			position.x = constraintCenter.x - width + radius;
		}

		if (position.y - radius > constraintCenter.y + height)
		{
			position.y = constraintCenter.y + height - radius;
		}

		if (position.y - radius < constraintCenter.y - height)
		{
			position.y = constraintCenter.y - height + radius;
		}
	}

	void applyDistanceJointConstraint(Vector2 center, float dist) 
	{
		float difference = distanceSqr(center, position);
		if (difference != dist) 
		{
			difference = sqrt(difference);
			Vector2 axis = center - position;
			axis.normalize();
			position += axis * (difference - dist);
		}
	}

	void updateV(float dt)
	{// verlet integration update
		deltaTime = dt;
		if (!isStatic)
		{
			velocity = position - oldPositon;
			oldPositon = position;
			acceleration += force * invMass;

			Vector2 increment = velocity + acceleration * dt * dt;//calculate position increment and storing it in var so calculation only done once

			position += increment;//move object (moves its origin)
			movePos(increment);//move vertices

			//euler method integration for rotational motion
			angularVelocity += angularAcceleration * dt;
			
			
			angle += angularVelocity * dt;

			rotate(angle-oldAngle);//rotate shape

			oldAngle = angle;

			angularAcceleration = 0.f;
			acceleration = Vector2();//sets acceleration to 0
			force = Vector2();
		}
	}
	
	void rotate(float angle)
	{
		if (polygon && !isStatic)
		{
			for (int i = 0; i < vertices.size(); i++) 
			{ 
				vertices[i]->rotate(angle, position); 
			}
		}
	}

	void movePos(Vector2 pos) 
	{
		for (int i = 0; i < vertices.size();i++) 
		{
			*(vertices[i]) += pos;
		}
	}

	void setPos(Vector2 pos) 
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			*(vertices[i]) = pos;
		}
	}

	Vector2 getVelocity() 
	{
		return (position - oldPositon) / deltaTime;
	}

	void setVelocity(Vector2 vel) 
	{
		oldPositon = position - (vel*deltaTime);
	}

	void addVelocity(Vector2 vel) 
	{
		oldPositon -= vel * deltaTime;
	}

	void accelerate(Vector2 acc) 
	{
		acceleration += acc;
	}
	
	void addForce(Vector2 Force_) 
	{
		force += Force_;
	}

	void addAngularVelocity(float vel) 
	{
		angularVelocity += vel;
	}

	void setAngularVelocity(float vel) 
	{
		angularVelocity = vel;
	}

	void accelerateAngular(float acc) 
	{
		angularAcceleration += acc;
	}

	void setColour(Uint8 r_, Uint8 g_, Uint8 b_) 
	{
		r = r_;
		g = g_;
		b = b_;

	}
	
	void draw(SDL_Renderer* renderer)
	{
		/*if (contactDraw) 
		{
			drawSquare(renderer, white, contact.x, contact.y, 5);
			drawSquare(renderer, white, contact2.x, contact2.y, 5);
			Vector2 r1 = contact - position;
			Vector2 r2 = contact2 - position;
			drawLine(renderer, white, position.x, position.y, contact.x, contact.y);
			drawLine(renderer, white, position.x, position.y, contact2.x, contact2.y);
		}*/
		if (polygon)
		{
			drawPolygon(renderer, r, g, b, vertices);
		}
		else
		{
			drawCircle(renderer, r, g, b, position.x, position.y, radius);
		}
	}

};