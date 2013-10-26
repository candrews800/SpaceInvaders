#include "GameObject.h"

GameObject::GameObject()
{
	x=0;
	y=0;
	w=0;
	h=0;
	xVel=0;
	yVel=0;
	alive=false;
}

GameObject::GameObject(int X, int Y, int W, int H, int XVEL, int YVEL)
{
	x=X;
	y=Y;
	w=W;
	h=H;
	xVel=XVEL;
	yVel=YVEL;
}


bool GameObject::detect_collision(GameObject &o)
{
	if(x+w<o.x)
	{
		return false;
	}
	if(x>o.x+o.w)
	{
		return false;
	}
	if(y+h<o.y)
	{
		return false;
	}
	if(y>o.y+o.h)
	{
		return false;
	}
	return true;
}

bool GameObject::is_alive()
{
	if(alive)
	{
		return true;
	}

	return false;
}