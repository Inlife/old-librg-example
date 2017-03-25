#include <uv.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <librg/core/client.h>
#include <librg/entities.h>
#include <librg/events.h>
#include <librg/network.h>
#include <librg/resources.h>
#include <SDL.h>
#undef main

/**
 * Alloc callback for allocating input memory
 * @param handle         tty handle
 * @param suggested_size suggensted size by uv (65536 in most cases)
 * @param buf            buffer, where data will be written to, and read from by us
 */
static void tty_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    buf->base = new char[1024];
    buf->len = 1024;
}

/**
 * On user console message
 * @param stream tty handle
 * @param nread  size of string
 * @param buf    buffer with data
 */
void on_console_message(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    buf->base[nread] = '\0';

    if (strncmp(buf->base, "conn", 4) == 0) {
        librg::network::client("127.0.0.1", 27010);
    }
}

void ontick(double dt)
{
    // printf("tick?\n");
}

int posX = 100;
int posY = 200;
int sizeX = 300;
int sizeY = 400;

SDL_Window* window;
SDL_Renderer* renderer;

bool InitEverything();
bool InitSDL();
bool mehCreateWindow();
bool CreateRenderer();
void SetupRenderer();

void Render();
void RunGame();

SDL_Rect playerPos;

void Render()
{
    // Clear the window and make it all green
    SDL_RenderClear( renderer );

    // Change color to blue
    SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );

    // Render our "player"
    SDL_RenderFillRect( renderer, &playerPos );

    // Change color to green
    SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );

    // Render the changes above
    SDL_RenderPresent( renderer);
}

bool InitEverything()
{
    if ( !InitSDL() )
        return false;

    if ( !mehCreateWindow() )
        return false;

    if ( !CreateRenderer() )
        return false;

    SetupRenderer();

    return true;
}
bool InitSDL()
{
    if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

bool mehCreateWindow()
{
    window = SDL_CreateWindow( "Server", posX, posY, sizeX, sizeY, 0 );

    if ( window == nullptr )
    {
        std::cout << "Failed to create window : " << SDL_GetError();
        return false;
    }

    return true;
}

bool CreateRenderer()
{
    renderer = SDL_CreateRenderer( window, -1, 0 );

    if ( renderer == nullptr )
    {
        std::cout << "Failed to create renderer : " << SDL_GetError();
        return false;
    }

    return true;
}

void SetupRenderer()
{
    // Set size of renderer to the same as window
    SDL_RenderSetLogicalSize( renderer, sizeX, sizeY );

    // Set color of renderer to green
    SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );
}

void RunGame()
{
    bool loop = true;

    while ( loop )
    {
        SDL_Event event;
        while ( SDL_PollEvent( &event ) )
        {
            if ( event.type == SDL_QUIT )
                loop = false;
            else if ( event.type == SDL_KEYDOWN )
            {
                switch ( event.key.keysym.sym )
                {
                    case SDLK_RIGHT:
                        ++playerPos.x;
                        break;
                    case SDLK_LEFT:
                        --playerPos.x;
                        break;
                        // Remeber 0,0 in SDL is left-top. So when the user pressus down, the y need to increase
                    case SDLK_DOWN:
                        ++playerPos.y;
                        break;
                    case SDLK_UP:
                        --playerPos.y;
                        break;
                    default :
                        break;
                }
            }
        }

        Render();

        librg::core::client_tick();

        // Add a 16msec delay to make our game run at ~60 fps
        SDL_Delay( 16 );
    }
}

int main(int argc, char *args[]) {
    uv_tty_t tty;

    uv_tty_init(uv_default_loop(), &tty, 0, 1);
    uv_tty_set_mode(&tty, UV_TTY_MODE_NORMAL);

    // setup reading callback
    uv_read_start((uv_stream_t*)&tty, tty_alloc, on_console_message);

    std::string test = "";

    test.append("==========          CLIENT         ===============\n");
    test.append("==                                              ==\n");
    test.append("==                 ¯\\_(ツ)_/¯                   ==\n");
    test.append("==                                              ==\n");
    test.append("==================================================\n");

    printf("%s\n\n", test.c_str());

    librg::core::set_mode(librg::core::mode_client);
    librg::core::set_tick_cb(ontick);

    librg::entities_initialize();
    librg::events_initialize();
    librg::network_initialize();
    librg::resources_initialize();

    // a game ticker
    if (!InitEverything())
     return -1;


    // Initlaize our playe
    playerPos.x = 20;
    playerPos.y = 20;
    playerPos.w = 20;
    playerPos.h = 20;

    RunGame();

    librg::entities_terminate();
    librg::events_terminate();
    librg::network_terminate();
    librg::resources_terminate();

    librg::core::client_terminate();

    return 0;
}
