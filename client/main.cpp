#include <uv.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <librg/core/client.h>
#include <librg/entities.h>
#include <librg/events.h>
#include <librg/network.h>
#include <librg/resources.h>
#include <librg/streamer.h>
#include <librg/components/transform.h>
#include <SDL.h>
#include <BitStream.h>
#include <messages.h>
#include <types.h>
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
        std::string ip;
        std::cin >> ip;
        librg::network::client(ip.c_str(), 7750);
    }
}


int posX = 100;
int posY = 200;
int sizeX = 800;
int sizeY = 600;

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
SDL_Rect playerRange;

void Render()
{
    // Clear the window and make it all green
    SDL_RenderClear( renderer );

    SDL_SetRenderDrawColor( renderer, 75, 75, 76, 10 );

    playerRange.x = playerPos.x - 250;
    playerRange.y = playerPos.y - 250;
    playerRange.w = 500;
    playerRange.h = 500;

    // Render our "player"
    SDL_RenderFillRect( renderer, &playerRange );

    // Change color to blue
    SDL_SetRenderDrawColor( renderer, 150, 150, 150, 255 );

    librg::entities->each<librg::transform_t>([](Entity entity, librg::transform_t &transform) {
        SDL_Rect position;

        position.x = (int)transform.position.x();
        position.y = (int)transform.position.y();
        position.w = 20;
        position.h = 20;

        SDL_RenderFillRect( renderer, &position );
    });

    librg::entities->each<bomb_t, librg::transform_t>([](Entity entity, bomb_t& bomb, librg::transform_t& transform) {
        SDL_Rect position;

        position.x = (int)transform.position.x();
        position.y = (int)transform.position.y();
        position.w = 20;
        position.h = 20;

        SDL_SetRenderDrawColor(renderer, 255*(1 - bomb.timeLeft/ bomb.startTime), 0, 0, 255);

        SDL_RenderFillRect(renderer, &position);
    });

    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);

    SDL_RenderFillRect( renderer, &playerPos );

    // Change color to green
    SDL_SetRenderDrawColor( renderer, 39, 40, 34, 150 );

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
    window = SDL_CreateWindow( "client", posX, posY, sizeX, sizeY, 0 );

    if ( window == nullptr )
    {
        std::cout << "Failed to create window : " << SDL_GetError();
        return false;
    }

    return true;
}

bool CreateRenderer()
{
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_PRESENTVSYNC );

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
    SDL_SetRenderDrawColor( renderer, 39, 40, 34, 150 );
}

bool shooting = false;

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
                        playerPos.x += 5;
                        break;
                    case SDLK_LEFT:
                        playerPos.x -= 5;
                        break;
                }
                switch ( event.key.keysym.sym )
                {
                        // Remeber 0,0 in SDL is left-top. So when the user pressus down, the y need to increase
                    case SDLK_DOWN:
                        playerPos.y += 5;
                        break;
                    case SDLK_UP:
                        playerPos.y -= 5;
                        break;
                }
                switch (event.key.keysym.sym)
                {
                    // Remeber 0,0 in SDL is left-top. So when the user pressus down, the y need to increase
                case SDLK_SPACE:
                    shooting = 1;
                    break;
                }
            }
        }

        librg::core::client_tick();
        
        Render();

        //SDL_Delay( 16 );
    }
}

void entity_create(uint64_t guid, uint8_t type, Entity entity, void* data)
{
    auto packet = (RakNet::BitStream*)data;
    librg::core::log("entity_create called");
}

void entity_update(uint64_t guid, uint8_t type, Entity entity, void* data)
{
    auto packet = (RakNet::BitStream*)data;
    //librg::core::log("entity_update called");
}

void entity_remove(uint64_t guid, uint8_t type, Entity entity, void* data)
{
    auto packet = (RakNet::BitStream*)data;
    librg::core::log("entity_remove called");
}

void entity_interpolate(uint64_t guid, uint8_t type, Entity entity, void* data)
{
    auto transform = (librg::transform_t*)data;
    // librg::core::log("entity_interpolate called");
}

void ontick(double dt)
{
    using namespace librg;

    network::msg(GAME_SYNC_PACKET, [](network::bitstream_t* data) {
        data->Write((float) playerPos.x);
        data->Write((float) playerPos.y);
    });

    if (shooting) {
        network::msg(GAME_ON_SHOOT, [](network::bitstream_t* data) {
            // No stuff here yet. welp...
        });
        shooting = false;
    }

    librg::entities->each<bomb_t>([dt](Entity entity, bomb_t& bomb) {
        bomb.timeLeft -= dt;
    });
}


int main(int argc, char *args[])
{
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

    librg::streamer_callbacks::set(librg::streamer_callbacks::create, entity_create);
    librg::streamer_callbacks::set(librg::streamer_callbacks::update, entity_update);
    librg::streamer_callbacks::set(librg::streamer_callbacks::remove, entity_remove);
    librg::streamer_callbacks::set(librg::streamer_callbacks::interpolate, entity_interpolate);

    librg::network::set_sync_cb(librg::core::rgmode::mode_client, [](librg::network::bitstream_t *data, Entity entity, int type) {
        switch (type) {
        case TYPE_BOMB:
        {
            float timeLeft, startTime;
            data->Read(startTime);
            data->Read(timeLeft);
            auto bomb = entity.assign<bomb_t>(timeLeft);
            bomb->startTime = startTime;
        }break;

        }
    });

    librg::entities_initialize();
    librg::events_initialize();
    librg::network_initialize();
    librg::resources_initialize();

    // a game ticker
    if (!InitEverything())
     return -1;

    librg::network::client("localhost", 7750);

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
