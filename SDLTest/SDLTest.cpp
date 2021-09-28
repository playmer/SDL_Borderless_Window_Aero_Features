/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image
SDL_Surface* loadSurface( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
  
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Current displayed image
SDL_Surface* gStretchedSurface = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Current displayed texture
SDL_Texture* gTexture = NULL;

//Loads individual image as texture
SDL_Texture* loadTexture( std::string path );

SDL_HitTestResult HitTestCallback(SDL_Window* win, const SDL_Point* area, void* data) {
  //printf("Hittest\n");

  int winWidth, winHeight;
  SDL_GetWindowSize(win, &winWidth, &winHeight);
        
  const int RESIZE_AREA = 4;
  const int RESIZE_AREAC = RESIZE_AREA*2;

  // Resize top
  if (area->x < RESIZE_AREAC && area->y < RESIZE_AREAC) return SDL_HITTEST_RESIZE_TOPLEFT;
  if (area->x > winWidth-RESIZE_AREAC && area->y < RESIZE_AREAC) return SDL_HITTEST_RESIZE_TOPRIGHT;
  if (area->x < RESIZE_AREA) return SDL_HITTEST_RESIZE_LEFT;
  if (area->y < RESIZE_AREA) return SDL_HITTEST_RESIZE_TOP;

  // Title bar
  if (area->y < 22 && area->x < winWidth-128) return SDL_HITTEST_DRAGGABLE;

  if (area->x < RESIZE_AREAC && area->y > winHeight-RESIZE_AREAC) return SDL_HITTEST_RESIZE_BOTTOMLEFT;
  if (area->x > winWidth-RESIZE_AREAC && area->y > winHeight-RESIZE_AREAC) return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
  if (area->x > winWidth-RESIZE_AREA) return SDL_HITTEST_RESIZE_RIGHT;
  if (area->y > winHeight-RESIZE_AREA) return SDL_HITTEST_RESIZE_BOTTOM;
  
  //printf("SDL_HITTEST_NORMAL\n");
  return SDL_HITTEST_NORMAL;
}

SDL_Window* CreateSdlWindow();

bool init()
{
  //Initialization flag
  bool success = true;

  //Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    success = false;
  }
  else
  {
    //Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
      printf( "Warning: Linear texture filtering not enabled!" );
    }

    gWindow = CreateSdlWindow();

    if( gWindow == NULL )
    {
      printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
      success = false;
    }
    else
    {
      //Create renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
      if( gRenderer == NULL )
      {
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        success = false;
      }
      else
      {
        //Initialize renderer color
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

        //Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;
        if( !( IMG_Init( imgFlags ) & imgFlags ) )
        {
          printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
          success = false;
        }
      }
    }
  }

  return success;
}

bool loadMedia()
{
  //Loading success flag
  bool success = true;

  //Load PNG texture
  gTexture = loadTexture( "texture.png" );
  if( gTexture == NULL )
  {
    printf( "Failed to load texture image!\n" );
    success = false;
  }

  return success;
}

void close()
{
  //Free loaded image
  SDL_DestroyTexture( gTexture );
  gTexture = NULL;

  //Destroy window  
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;
  gRenderer = NULL;

  //Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}

SDL_Texture* loadTexture( std::string path )
{
  //The final texture
  SDL_Texture* newTexture = NULL;

  //Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
  if( loadedSurface == NULL )
  {
    printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
  }
  else
  {
    //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
    if( newTexture == NULL )
    {
      printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
    }

    //Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
  }

  return newTexture;
}


int main( int argc, char* args[] )
{
  //Start up SDL and create window
  if( !init() )
  {
    printf( "Failed to initialize!\n" );
  }
  else
  {
    //Load media
    if( !loadMedia() )
    {
      printf( "Failed to load media!\n" );
    }
    else
    {  
      //Main loop flag
      bool quit = false;

      //Event handler
      SDL_Event e;

      //While application is running
      while( !quit )
      {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
          //User requests quit
          if( e.type == SDL_QUIT )
          {
            quit = true;
          }
          else if (e.type == SDL_KEYDOWN)
          {
            switch (e.key.keysym.sym)
            {
            case SDLK_ESCAPE:
              quit = true;
              break;
            case SDLK_f:
              SDL_MaximizeWindow(gWindow);
              break;
            }
          }
        }

        //Clear screen
        SDL_RenderClear( gRenderer );

        //Render texture to screen
        SDL_RenderCopy( gRenderer, gTexture, NULL, NULL );

        //Update screen
        SDL_RenderPresent( gRenderer );
      }
    }
  }

  //Free resources and close SDL
  close();

  return 0;
}