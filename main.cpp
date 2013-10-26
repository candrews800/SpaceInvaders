//Headers
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include <string>
#include <sstream>
#include <vector>
#include "Timer.h"
#include "GameObject.h"

//Screen attributes
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

//The frame rate
const int FRAMES_PER_SECOND = 60;

//The surfaces
SDL_Surface *screen = NULL;
SDL_Surface *background = NULL;
SDL_Surface *intro = NULL;
SDL_Surface *win = NULL;
SDL_Surface *lose = NULL;
TTF_Font *font = NULL;

SDL_Color textColor = {255,255,255};

//The event structure
SDL_Event event;

//SDL Functions
SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

	//Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;    
    }

	//Initialize SDL_mixer
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return false;    
    }
	background = load_image("background.png");
	intro = load_image("intro.png");
	win = load_image("win.png");
	lose = load_image("lose.png");

    //Set the window caption
    SDL_WM_SetCaption( "Tribal Invaders by Clinton Andrews", NULL );

    //If everything initialized fine
    return true;
}

void draw_screen()
{
	apply_surface(0,0,background,screen);

}


int main( int argc, char* args[] )
{
    bool quit = false;
	bool introquit = false;
	bool closequit = false;
	Timer fps;
	int lastshot=0;
	int tribalsLeft=30;
	bool outofrange=false;
	bool goingright=true;
	int tribalmovement=1;
	int lives=3;

    if( init() == false )
    {
        return 1;
    }
		
	GameObject hero(375,550,45,50,0,0);
	hero.image=load_image("bow.png");

	GameObject arrow[2]= {GameObject(0,0,8,0,0,0), GameObject(0,0,8,0,0,0)} ;
	arrow[0].image=load_image("arrow.png");
	arrow[0].alive=false;
	arrow[1].image=load_image("arrow.png");
	arrow[1].alive=false;

	GameObject spear[6];
	int lastspear[6]={0,0,0,0,0,0};

	GameObject rocks[3][5];
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<5;j++)
		{
			if(j<2)
			{
				rocks[i][j].x=210+j*30+i*150;
				rocks[i][j].y=450;
				rocks[i][j].w=30;
				rocks[i][j].h=30;
				rocks[i][j].quality=2;
				rocks[i][j].alive=true;
				rocks[i][j].image=load_image("rock.png");
			}
			else
			{
				rocks[i][j].x=135+j*30+i*150;
				rocks[i][j].y=478;
				rocks[i][j].w=30;
				rocks[i][j].h=30;
				rocks[i][j].quality=2;
				rocks[i][j].alive=true;
				rocks[i][j].image=load_image("rock.png");
			}
		}
	}

	GameObject tribals[5][6];
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<6;j++)
		{
			tribals[i][j].x=150+80*j;
			tribals[i][j].y=275-60*i;
			tribals[i][j].w=40;
			tribals[i][j].h=30;
			tribals[i][j].alive=true;
			if(i<2)
			{
				tribals[i][j].image=load_image("tribals.png");
			}
			else if(i<4)
			{
				tribals[i][j].image=load_image("fire.png");
			}
			else
			{
				tribals[i][j].image=load_image("devil.png");
			}
		}
	}

	//Intro Screen
	while ( quit == false && introquit == false)
	{
		apply_surface(0,0,intro,screen);
		while( SDL_PollEvent( &event ) )
        {
			
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }

			if(event.key.keysym.sym == SDLK_SPACE)
			{
				introquit=true;
			}

			SDL_Flip( screen );
        }
	}

	//Game Loop
    while( quit == false )
    {
		//Start the frame timer
        fps.start();
		
		//Input
        //While there's events to handle
        while( SDL_PollEvent( &event ) )
        {
		
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
				closequit = true;
            }
        }
		//Set Hero Velocity to 0, allow key state to modify the velocity
		hero.xVel=0;
		Uint8 *keystates = SDL_GetKeyState( NULL );
		//If left is pressed
        if( keystates[ SDLK_LEFT ] )
        {
			if(hero.xVel>=0)
            hero.xVel+=-5;
        }
    
        //If right is pressed
        if( keystates[ SDLK_RIGHT ] )
        {
			if(hero.xVel<=0)
            hero.xVel+=5;
        }

		//If right is pressed && hero is alive
        if( keystates[ SDLK_UP ] && hero.alive)
        {
			if(arrow[0].alive==false && arrow[1].alive==false && SDL_GetTicks()>500+lastshot)
			{
				arrow[0].x=hero.x+20;
				arrow[0].y=hero.y-20;
				arrow[0].yVel=5;
				arrow[0].alive=true;
				lastshot=SDL_GetTicks();
			}
			if(arrow[0].alive==false && SDL_GetTicks()>250+lastshot)
			{
				arrow[0].x=hero.x+20;
				arrow[0].y=hero.y-20;
				arrow[0].yVel=5;
				arrow[0].alive=true;
				lastshot=SDL_GetTicks();
			}
			if(arrow[1].alive==false && SDL_GetTicks()>250+lastshot)
			{
				arrow[1].x=hero.x+20;
				arrow[1].y=hero.y-20;
				arrow[1].yVel=5;
				arrow[1].alive=true;
				lastshot=SDL_GetTicks();			
			}
        }

		//Tribal Movement
		int yChange=0;
		if(outofrange)
		{
			yChange=5+(32-tribalsLeft)/8;
			outofrange=false;

			if(hero.alive==false)
			{
				yChange=15;
			}
		}
			
		if(tribalsLeft<25){
			tribalmovement=2;
		}
		
		if(tribalsLeft<15){
			tribalmovement=4;
		}
		if(tribalsLeft<5){
			tribalmovement=6;
		}
		if(tribalsLeft<2){
			tribalmovement=8;
		}

		if(hero.alive==false)
		{
			tribalmovement=10;
		}

		for(int i=0;i<5;i++)
		{
			for(int j=0;j<6;j++)
			{
				if(goingright)
				{
					tribals[i][j].x+=tribalmovement;
				}
				else
				{
					tribals[i][j].x-=tribalmovement;
				}
				tribals[i][j].y+=yChange;

				if(tribals[i][j].alive && tribals[i][j].x+tribalmovement>650)
				{
					outofrange=true;
					goingright=false;
				}
				else if(tribals[i][j].alive && tribals[i][j].x+tribalmovement<100)
				{
					outofrange=true;
					goingright=true;
				}
			
			}
		}
		//Logic
		//AI for Shooting Bullets
		//Find Lowest Tribal in Row and Fire Bullet with 2 second cooldown

		for(int i=0;i<6;i++)
		{
			if( (hero.x + hero.w + tribalsLeft *2) > tribals[0][i].x && hero.x < (tribals[0][i].x + tribals[0][i].w +tribalsLeft*2) )
			{
				if(spear[i].alive==false && SDL_GetTicks()>10+66*tribalsLeft+lastspear[i])
				{
					if(tribals[0][i].alive==true)
					{
						spear[i].alive=true;
						spear[i].x=tribals[0][i].x+15;
						spear[i].y=tribals[0][i].y+40;
						spear[i].w=8;
						spear[i].h=5;
						spear[i].image=load_image("spear.png");
						spear[i].yVel=5;
						lastspear[i]=SDL_GetTicks();
					}
					else if(tribals[1][i].alive==true)
					{
						spear[i].alive=true;
						spear[i].x=tribals[1][i].x+15;
						spear[i].y=tribals[1][i].y+40;
						spear[i].w=8;
						spear[i].h=5;
						spear[i].image=load_image("spear.png");
						spear[i].yVel=5;
						lastspear[i]=SDL_GetTicks();
					}
					else if(tribals[2][i].alive==true)
					{
						spear[i].alive=true;
						spear[i].x=tribals[2][i].x+15;
						spear[i].y=tribals[2][i].y+40;
						spear[i].w=15;
						spear[i].h=5;
						spear[i].image=load_image("fireammo.png");
						spear[i].yVel=5;
						lastspear[i]=SDL_GetTicks();
					}
					else if(tribals[3][i].alive==true)
					{
						spear[i].alive=true;
						spear[i].x=tribals[3][i].x+15;
						spear[i].y=tribals[3][i].y+40;
						spear[i].w=15;
						spear[i].h=5;
						spear[i].image=load_image("fireammo.png");
						spear[i].yVel=5;
						lastspear[i]=SDL_GetTicks();
					}
					else if(tribals[4][i].alive==true)
					{
						spear[i].alive=true;
						spear[i].x=tribals[4][i].x+15;
						spear[i].y=tribals[4][i].y+40;
						spear[i].w=20;
						spear[i].h=5;
						spear[i].image=load_image("devilspear.png");
						spear[i].yVel=5;
						lastspear[i]=SDL_GetTicks();
					}
				}
			}
		}
		
		//Check for collisions
		hero.x+=hero.xVel;
		if(hero.x<0 || hero.x+hero.w>SCREEN_WIDTH)
		{
			hero.x-=hero.xVel;
		}
		if(arrow[0].alive)
		{
			arrow[0].y-=arrow[0].yVel;
			if(arrow[0].y<0)
			{	
				arrow[0].alive=false;
			}
		}
		if(arrow[1].alive)
		{
			arrow[1].y-=arrow[1].yVel;
			if(arrow[1].y<0)
			{	
				arrow[1].alive=false;
			}
		}
		for(int i=0;i<5;i++)
		{
			for(int j=0;j<6;j++)
			{
				if(tribals[i][j].alive)
				{
					if(arrow[0].alive)
					{
						if(arrow[0].detect_collision(tribals[i][j]))
						{
							arrow[0].alive=false;
							tribals[i][j].alive=false;
							tribalsLeft-=1;
						}
					}
					if(arrow[1].alive)
					{
						if(arrow[1].detect_collision(tribals[i][j]))
						{
							arrow[1].alive=false;
							tribals[i][j].alive=false;
							tribalsLeft-=1;
						}
					}
					
				}
			}
		}

		for(int i=0;i<6;i++)
		{
			if(spear[i].detect_collision(hero))
			{
				if(spear[i].alive)
				{
					// Hero is Hit
					lives-=1;
					spear[i].alive=false;
					if(lives<=0)
					{
						hero.alive=false;
					}
				}
			}
			for(int j=0;j<3;j++)
			{
				for(int k=0;k<5;k++)
				{
					if(rocks[j][k].alive)
					{
						if(spear[i].detect_collision(rocks[j][k]))
						{
							if(spear[i].alive)
							{
								spear[i].alive=false;
								rocks[j][k].quality-=1;
								if(rocks[j][k].quality==1)
								{
									rocks[j][k].image=load_image("cracked.png");
								}
								else if(rocks[j][k].quality==0)
								{
									rocks[j][k].alive=false;
								}
							}
						}
						if(arrow[0].alive)
						{
							if(arrow[0].detect_collision(rocks[j][k]))
							{
								arrow[0].alive=false;
								rocks[j][k].quality-=1;
								if(rocks[j][k].quality==1)
								{
									rocks[j][k].image=load_image("cracked.png");
								}
								else if(rocks[j][k].quality==0)
								{
									rocks[j][k].alive=false;
								}
							}
						}
						if(arrow[1].alive)
						{
							if(arrow[1].detect_collision(rocks[j][k]))
							{
								arrow[1].alive=false;
								rocks[j][k].quality-=1;
								if(rocks[j][k].quality==1)
								{
									rocks[j][k].image=load_image("cracked.png");
								}
								else if(rocks[j][k].quality==0)
								{
									rocks[j][k].alive=false;
								}
							}
						}
						for(int q=0;q<5;q++)
						{
							for(int w=0;w<6;w++)
							{
								if(tribals[q][w].alive)
								{
									if(tribals[q][w].detect_collision(rocks[j][k]))
									{
										rocks[j][k].alive=false;
									}
								}
							}
						}
					}
				}
			}
		}


		for(int i=0;i<6;i++)
		{
			if(spear[i].alive)
			{
				spear[i].y+=spear[i].yVel;
				if(spear[i].y>600)
				{	
					spear[i].alive=false;
				}
			}
		}

		//Update the screen
		draw_screen();
		if(hero.alive)
		{
			apply_surface(hero.x,hero.y,hero.image,screen);
		}
		if(arrow[0].alive)
		{
			apply_surface(arrow[0].x,arrow[0].y,arrow[0].image,screen);
		}
		if(arrow[1].alive)
		{
			apply_surface(arrow[1].x,arrow[1].y,arrow[1].image,screen);
		}
		for(int i=0;i<6;i++)
		{
			if(spear[i].alive)
			{
				apply_surface(spear[i].x,spear[i].y,spear[i].image,screen);
			}
		}
		for(int i=0;i<5;i++)
		{
			for(int j=0;j<6;j++)
			{
				if(tribals[i][j].alive)
					apply_surface(tribals[i][j].x,tribals[i][j].y,tribals[i][j].image,screen);
			}
		}
		for(int i=0;i<3;i++)
		{
			for(int j=0;j<5;j++)
			{
				if(rocks[i][j].alive)
				{
					apply_surface(rocks[i][j].x,rocks[i][j].y,rocks[i][j].image,screen);
				}
			}
		}

		for(int i=0;i<lives-1;i++)
		{
			apply_surface(20+i*60,575,hero.image,screen);
		}
		
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }


		//Cap the frame rate
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        {
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        }

		if(lives==0)
		{
			quit=true;
		}
		if(tribalsLeft==0)
		{
			quit=true;
		}

    }

	while ( closequit==false)
	{
		if(lives==0)
		{
			apply_surface(0,0,lose,screen);
		}
		if(tribalsLeft==0)
		{
			apply_surface(0,0,win,screen);
		}
		
		while( SDL_PollEvent( &event ) )
        {
			
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
				closequit=true;
            }	
        }

		SDL_Flip( screen );
	}

    //Clean up
    SDL_Quit();

    return 0;
}
