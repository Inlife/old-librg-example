#include <stdio.h>
#include <string.h>
#include <iostream>

#include <uv.h>
#include <SDL.h>
#include <librg/librg.h>
#include <librg/components/transform.h>

#include <messages.h>
#include <types.h>
#undef main

using namespace librg;

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
entity_t playerEntity = {};

struct explosion_t {
    hmm_vec3 position;
    float impact;
};

std::vector<explosion_t> explosions;

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

    librg::entities->each<librg::transform_t, hero_t>([](Entity entity, librg::transform_t& transform, hero_t& hero) {
        SDL_Rect position;

        position.x = (int)transform.position.X - 10;
        position.y = (int)transform.position.Y - 10;
        position.w = 20;
        position.h = 20;

        SDL_RenderFillRect( renderer, &position );

        if (hero.HP > 0) {
            position.h = 5;
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
            SDL_RenderFillRect(renderer, &position);

            position.w = 20 * (hero.HP / (float)hero.maxHP);

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 150);
            SDL_RenderFillRect(renderer, &position);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
            SDL_RenderFillRect(renderer, &position);
        }
    });

    librg::entities->each<bomb_t, librg::transform_t>([](Entity entity, bomb_t& bomb, librg::transform_t& transform) {
        SDL_Rect position;

        position.x = (int)transform.position.X - 10;
        position.y = (int)transform.position.Y - 10;
        position.w = 20;
        position.h = 20;

        SDL_SetRenderDrawColor(renderer, 255*(1 - bomb.timeLeft/ bomb.startTime), 0, 0, 255);

        SDL_RenderFillRect(renderer, &position);
    });

    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);

    SDL_Rect position;

    position.x = playerPos.x - 10;
    position.y = playerPos.y - 10;
    position.w = 20;
    position.h = 20;

    SDL_RenderFillRect( renderer, &position );

    // Health bar
    if (playerEntity) {
        auto hero = playerEntity.component<hero_t>();
        if (hero->HP > 0) {
            position.h = 5;
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
            SDL_RenderFillRect(renderer, &position);

            position.w = 20 * (hero->HP / (float)hero->maxHP);

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 150);
            SDL_RenderFillRect(renderer, &position);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
            SDL_RenderFillRect(renderer, &position);
        }
    }

    for (auto &explosion : explosions) {
        if (explosion.impact < 0) continue;

        position.x = explosion.position.X - explosion.impact / 2.f;
        position.y = explosion.position.Y - explosion.impact / 2.f;
        position.w = explosion.impact;
        position.h = explosion.impact;

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150);
        SDL_RenderFillRect(renderer, &position);

        position.x += explosion.impact / 4.f;
        position.y += explosion.impact / 4.f;
        position.w = explosion.impact  / 2.f;
        position.h = explosion.impact  / 2.f;

        SDL_SetRenderDrawColor(renderer, 255, 148, 0, 150);
        SDL_RenderFillRect(renderer, &position);
    }

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
bool keysHeld[323] = { false };

void RunGame()
{
    bool loop = true;

    while ( loop )
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                loop = false;

            if (event.type == SDL_KEYDOWN)
            {
                keysHeld[event.key.keysym.sym] = true;

                switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    shooting = true;
                    break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                keysHeld[event.key.keysym.sym] = false;
            }
        }
         
        if (playerEntity) {
            auto hero = playerEntity.component<hero_t>();

            if (hero->HP > 0) {
                if (keysHeld[SDLK_a]) {
                    if (hero->accel.X > -5) hero->accel.X -= 0.5f;
                }
                if (keysHeld[SDLK_d]) {
                    if (hero->accel.X <  5) hero->accel.X += 0.5f;
                }
                if (keysHeld[SDLK_w]) {
                    if (hero->accel.Y > -5) hero->accel.Y -= 0.5f;
                }
                if (keysHeld[SDLK_s]) {
                    if (hero->accel.Y <  5) hero->accel.Y += 0.5f;
                }
            }
        }

        librg::core::tick();

        Render();

        //SDL_Delay( 16 );
    }
}

/**
 * Entity add to streamer
 */
void entity_create(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_create_t*) evt;
    librg::core::log("entity_create called");

    switch (event->type) {
        case TYPE_PLAYER:
        {
            int HP, maxHP;
            event->data->Read(maxHP);
            event->data->Read(HP);

            auto hero = event->entity.assign<hero_t>(maxHP);
            hero->HP = HP;
        }break;

        case TYPE_BOMB:
        {
            float timeLeft, startTime;
            event->data->Read(startTime);
            event->data->Read(timeLeft);

            auto bomb = event->entity.assign<bomb_t>(timeLeft);
            bomb->startTime = startTime;
        } break;
    }
}

/**
 * Entity update in streamer
 */
void entity_update(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_update_t*) evt;

    // librg::core::log("entity_create update");
}

/**
 * Entity remove from streamer
 */
void entity_remove(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_remove_t*) evt;
    librg::core::log("entity_remove called, type: %d", event->type);

    switch (event->type) {
        case TYPE_BOMB:
        {
            librg::core::log("EXPLOSION!!!");
            auto transform = event->entity.component<transform_t>();

            bool needsNew = true;
            explosion_t explosion = { transform->position, 150.f };

            for (auto &exp : explosions) {
                if (exp.impact < 0) {
                    exp = explosion;
                    needsNew = false;
                    break;
                }
            }

            if (needsNew) explosions.push_back(explosion);
        }break;
    }
}

/**
 * Entity interpolate callback
 */
void entity_inter(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_inter_t*) evt;
    // librg::core::log("entity_interpolate called");
}

void ontick(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_tick_t*) evt;

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

    librg::entities->each<bomb_t>([event](Entity entity, bomb_t& bomb) {
        bomb.timeLeft -= event->dt;
    });

    for (auto &exp : explosions) {
        exp.impact -= event->dt * 40;
    }

    if (playerEntity) {
        auto hero = playerEntity.component<hero_t>();

        if (hero->accel.X > 0.012f || hero->accel.X < -0.012f) playerPos.x += hero->accel.X;
        if (hero->accel.Y > 0.012f || hero->accel.Y < -0.012f) playerPos.y += hero->accel.Y;

        hero->accel.X = HMM_Lerp(hero->accel.X, 0.95 * event->dt, 0.f);
        hero->accel.Y = HMM_Lerp(hero->accel.Y, 0.95 * event->dt, 0.f);
    }
}


int main(int argc, char *args[])
{
    std::string test = "";
    test.append("==========          CLIENT         ===============\n");
    test.append("==                                              ==\n");
    test.append("==                 ¯\\_(ツ)_/¯                   ==\n");
    test.append("==                                              ==\n");
    test.append("==================================================\n");
    printf("%s\n\n", test.c_str());

    // setup manual client mode
    librg::core_initialize(librg::mode_client_manual);

    // setup callbacks
    librg::callbacks::set(librg::callbacks::tick, ontick);
    // librg::callbacks::set(librg::callbacks::inter, entity_inter);
    librg::callbacks::set(librg::callbacks::create, entity_create);
    librg::callbacks::set(librg::callbacks::update, entity_update);
    librg::callbacks::set(librg::callbacks::remove, entity_remove);

    librg::network::add(GAME_NEW_LOCAL_PLAYER, [](network::bitstream_t *data, network::packet_t *packet) {
        int maxHP;
        data->Read(maxHP);

        playerEntity = librg::entities->create();
        playerEntity.assign<hero_t>(maxHP);
    });

    librg::network::add(GAME_HIT_LOCAL_PLAYER, [](network::bitstream_t *data, network::packet_t *packet) {
        int HP;
        data->Read(HP);

        auto hero = playerEntity.component<hero_t>();
        hero->HP = HP;

        if (hero->HP < 0) {
            core::log("Oh you're dead!");
        }
    });

    librg::network::add(GAME_HIT_PLAYER, [](network::bitstream_t *data, network::packet_t *packet) {
        network::guid_t guid;
        int HP;

        data->Read(guid);
        data->Read(HP);

        auto hero = streamer::client_cache[guid].component<hero_t>();
        hero->HP = HP;
    });

    if (!InitEverything()) {
        return -1;
    }

    // start the client (network connection)
    librg::core::start("localhost", 7750);

    // Initlaize our player
    playerPos.x = 20;
    playerPos.y = 20;
    playerPos.w = 20;
    playerPos.h = 20;

    RunGame();

    librg::core_terminate();
    return 0;
}
