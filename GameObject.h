#include "SDL.h"
#include "SDL_image.h"

class GameObject
{
public:
	int x, y;
	int w, h;
	int xVel, yVel;
	bool alive;
	int quality;

	SDL_Surface *image;

	GameObject();
	GameObject(int X, int Y, int W, int H, int XVEL, int YVEL);
	bool detect_collision(GameObject &o);
	bool is_alive();
};