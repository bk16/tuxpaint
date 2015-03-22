/*
  polka_dots.c

  polka dots, Add polka dots effect to the image
  Tux Paint - A simple drawing program for children.

*/

#include <stdio.h>
#include <string.h>
#include <libintl.h>
#include "tp_magic_api.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include <math.h>
#include <limits.h>
#include <time.h>

#ifndef gettext_noop
#define gettext_noop(String) String
#endif

void polka_dots_click(magic_api *, int, int, SDL_Surface *, SDL_Surface *, int, int, SDL_Rect *);

static const int polka_dots_SIZE = 10;
static const int polka_dots_RADIUS = 5;
static const int polka_dots_AMOUNT = 400;			//can use this to increase polka dots density on a surface
static Uint8 polka_dots_r = 0;					// rgb values for the color selected in tux paint
static Uint8 polka_dots_g = 0;
static Uint8 polka_dots_b = 0;

enum 
{
	polka_dots,
	polka_dots_NUM_TOOLS
};

//static Mix_Chunk * polka_dots_snd_effect[polka_dots_NUM_TOOLS];

//const char * polka_dots_snd_filenames[polka_dots_NUM_TOOLS] = {
//  "polka_dots.ogg",
//};

const char * polka_dots_icon_filenames[polka_dots_NUM_TOOLS] =
{
 	"polka_dots.png",
};

const char * polka_dots_names[polka_dots_NUM_TOOLS] = 
{
  gettext_noop("Polka dots"),
};

const char * polka_dots_descs[polka_dots_NUM_TOOLS][2] = 
{
  	{
		gettext_noop("Click to place polka dots onto your picture."),
    		gettext_noop("Click to cover your picture with polka dots."),
	},
};

Uint32 polka_dots_api_version(void);
//return an integer value representing the version of the Tux Paint 'Magic' tool plugin API that plugin was built against

int polka_dots_init(magic_api * api);
//does initialization

int polka_dots_get_tool_count(magic_api * api);
//returns 1, the number of magic tools this plugin provides

SDL_Surface * polka_dots_get_icon(magic_api * api, int which);
// returns an SDL_Surface containing the image of the tool

char * polka_dots_get_name(magic_api * api, int which);
// returns a string, the name of the tool, as it will appear

char * polka_dots_get_description(magic_api * api, int which, int mode);
// returns a string, describing how to use this tool

static void do_polka_dots(void * ptr, int which, SDL_Surface * canvas, SDL_Surface * last,
                int x, int y);
// draws polka dots

static void polka_dots_linecb(void * ptr, int which,
                        SDL_Surface * canvas, SDL_Surface * last,
                        int x, int y);
//function for line callback

void polka_dots_drag(magic_api * api, int which, SDL_Surface * canvas,
	          SDL_Surface * last, int ox, int oy, int x, int y,
		  SDL_Rect * update_rect);
//The plugin should apply the appropriate 'Magic' tool on the'canvas' surface. The (ox,oy) and (x,y) coordinates are the location of the mouse at the beginning and end of the stroke.

void polka_dots_click(magic_api * api, int which, int mode,
	           SDL_Surface * canvas, SDL_Surface * last,
	           int x, int y, SDL_Rect * update_rect);
/* The plugin should apply the appropriate 'Magic' tool on the
             'canvas' surface. The (x,y) coordinates are where the mouse was
             (within the canvas) when the mouse button was clicked, and you
             are told which 'mode' your tool is in (i.e., 'MODE_PAINT' or
             'MODE_FULLSCREEN).*/

void polka_dots_release(magic_api * api, int which,
	           SDL_Surface * canvas, SDL_Surface * last,
	           int x, int y, SDL_Rect * update_rect);

/*
	The plugin should apply the appropriate 'Magic' tool on the
             'canvas' surface. The (x,y) coordinates are where the mouse was
             (within the canvas) when the mouse button was released.

             The plugin should report back what part of the canvas was
             affected, by filling in the (x,y) and (w,h) elements of
             'update_rect'.*/


void polka_dots_shutdown(magic_api * api);
// shut downs the tool on exit

void polka_dots_set_color(magic_api * api, Uint8 r, Uint8 g, Uint8 b);
//use it for different colors of polka dots

/*
	Tux Paint will call this function to inform the plugin of the
             RGB values of the currently-selected color in Tux Paint's
             'Colors' palette. (It will be called whenever one of the
             plugin's Magic tools that accept colors becomes active, and
             whenever the user picks a new color while such a tool is
             currently active.) */

int polka_dots_requires_colors(magic_api * api, int which);
//to be used. different colors for polka dots

/*
	Return a '1' if the 'Magic' tool accepts colors (the 'Colors'
             palette in Tux Paint will be available), or '0' if not. */

void polka_dots_switchin(magic_api * api, int which, int mode, SDL_Surface * canvas);

void polka_dots_switchout(magic_api * api, int which, int mode, SDL_Surface * canvas);

int polka_dots_modes(magic_api * api, int which);

Uint32 polka_dots_api_version(void)
{ 
	return(TP_MAGIC_API_VERSION); 
}

int polka_dots_init(magic_api * api ATTRIBUTE_UNUSED)
{

  	
	//int i;
  	//char fname[1024];
	//Load sounds
  	//for (i = 0; i < polka_dots_NUM_TOOLS; i++)
	//{
    		//snprintf(fname, sizeof(fname), "%s/sounds/magic/%s", api->data_directory, polka_dots_snd_filenames[i]);
    		//polka_dots_snd_effect[i] = Mix_LoadWAV(fname);
		// see if you can prepare a sound
  	//}
	
 	return(1);
}

int polka_dots_get_tool_count(magic_api * api ATTRIBUTE_UNUSED)
{
  	return(polka_dots_NUM_TOOLS);
}
// find an image to go with this
// Load our icons:
SDL_Surface * polka_dots_get_icon(magic_api * api, int which)
{
  	char fname[1024];
  	snprintf(fname, sizeof(fname), "%simages/magic/%s", api->data_directory, polka_dots_icon_filenames[which]);
  	return(IMG_Load(fname));
}

// Return our names, localized:
char * polka_dots_get_name(magic_api * api ATTRIBUTE_UNUSED, int which)
{
    	return(strdup(gettext_noop(polka_dots_names[which])));
}

// Return our descriptions, localized:
char * polka_dots_get_description(magic_api * api ATTRIBUTE_UNUSED, int which, int mode)
{
  	return(strdup(gettext_noop(polka_dots_descs[which][mode-1])));
}

// Do the effect:
static void do_polka_dots(void * ptr, int which ATTRIBUTE_UNUSED, SDL_Surface * canvas, SDL_Surface * last ATTRIBUTE_UNUSED,
                int x, int y)
{
  	magic_api * api = (magic_api *) ptr;

  	int xx, yy, centre_x=x, centre_y=y;
	//Uint8 r,g,b;

    					//to  increase dots density, cam iterate over the amount of dots to be drawn 

    	if (which == polka_dots)
	{
      		for (yy = -polka_dots_RADIUS; yy < polka_dots_RADIUS; yy++)
		{
		    	for (xx= -polka_dots_RADIUS; xx < polka_dots_RADIUS; xx++)
			{
            			if (api->in_circle(xx ,yy, polka_dots_RADIUS))
				{
              				//SDL_GetRGB(api->getpixel(last, centre_x + xx, centre_y + yy), canvas->format, &r, &g, &b);
              				api->putpixel(canvas, centre_x + xx, centre_y + yy, SDL_MapRGB(canvas->format, 										polka_dots_r, polka_dots_g,polka_dots_b));
            			}
          		}  
        	}
     	}
}
// callback function for a line of points
static void polka_dots_linecb(void * ptr, int which,SDL_Surface * canvas, SDL_Surface * last, int x, int y)
{
  	magic_api * api = (magic_api *) ptr;
  	SDL_Rect rect;

  	if (rand() % 10 == 0) // generating polka dots at random points 
 	{
		if( x < canvas->w && y < canvas->h )
		{ 		
			
			polka_dots_click(api, which, MODE_PAINT, canvas, last,
               		x + (rand() % polka_dots_SIZE * 2) - polka_dots_SIZE,
               		y + (rand() % polka_dots_SIZE * 2) - polka_dots_SIZE,
               		&rect);
		}
  	}
}

// Affect the canvas on drag:
void polka_dots_drag(magic_api * api, int which, SDL_Surface * canvas, SDL_Surface * last, int ox, int oy, int x, int y,
		  										SDL_Rect * update_rect)
{
  	api->line((void *) api, which, canvas, last, ox, oy, x, y, 1, polka_dots_linecb);

  	if (ox > x)
	{
		int tmp = ox; 
		ox = x; 
		x = tmp;
	}
  	if (oy > y)
	{ 
		int tmp = oy; 
		oy = y; 
		y = tmp; 
	}

  	update_rect->x = min(ox, x) - polka_dots_SIZE * 2;
	update_rect->y = min(oy, y) - polka_dots_SIZE * 2;
	update_rect->w = abs(ox - x) + polka_dots_SIZE * 4;
	update_rect->h = abs(oy - y) + polka_dots_SIZE * 4;
}

// Affect the canvas on click:
void polka_dots_click(magic_api * api, int which, int mode ATTRIBUTE_UNUSED, SDL_Surface * canvas, SDL_Surface * last,
	           					int x, int y, SDL_Rect * update_rect)
{

  	update_rect->x = 0;
  	update_rect->y = 0;
  	update_rect->w = canvas->w;
  	update_rect->h = canvas->h;
  
  	do_polka_dots((void *)api, which, canvas, last, x, y);
  	//api->playsound(polka_dots_snd_effect[which], 128, 255);
}

// Affect the canvas on release:
void polka_dots_release(magic_api * api ATTRIBUTE_UNUSED, int which ATTRIBUTE_UNUSED,
	           SDL_Surface * canvas ATTRIBUTE_UNUSED, SDL_Surface * last ATTRIBUTE_UNUSED,
	           int x ATTRIBUTE_UNUSED, int y ATTRIBUTE_UNUSED, SDL_Rect * update_rect ATTRIBUTE_UNUSED)
{
}

void polka_dots_shutdown(magic_api * api ATTRIBUTE_UNUSED)
{
	//Clean up sounds
	//int i;
	//for(i=0; i<polka_dots_NUM_TOOLS; i++){
	//	if(polka_dots_snd_effect[i] != NULL){
	//		Mix_FreeChunk(polka_dots_snd_effect[i]);
	//	}
	//}
}

// Record the color from Tux Paint:
void polka_dots_set_color(magic_api * api ATTRIBUTE_UNUSED, Uint8 r, Uint8 g, Uint8 b)
{
	polka_dots_r = r;
	polka_dots_g = g;
	polka_dots_b = b;
}

// Use colors: Yes
int polka_dots_requires_colors(magic_api * api ATTRIBUTE_UNUSED, int which ATTRIBUTE_UNUSED)
{
  return 1;
}

int polka_dots_modes(magic_api * api ATTRIBUTE_UNUSED, int which ATTRIBUTE_UNUSED)
{
  return(MODE_PAINT);
}

void polka_dots_switchin(magic_api * api ATTRIBUTE_UNUSED, int which ATTRIBUTE_UNUSED, int mode ATTRIBUTE_UNUSED, SDL_Surface * canvas ATTRIBUTE_UNUSED)
{
}

void polka_dots_switchout(magic_api * api ATTRIBUTE_UNUSED, int which ATTRIBUTE_UNUSED, int mode ATTRIBUTE_UNUSED, SDL_Surface * canvas ATTRIBUTE_UNUSED)
{
}




