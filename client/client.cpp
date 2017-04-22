#include <stdio.h>
#include <string.h>
#include <iostream>

#include <uv.h>
#include <SDL.h>
#include <librg/librg.h>
#include <librg/components/transform.h>
#include <librg/components/interpolable.h>

#include <messages.h>
#include <types.h>
#undef main

using namespace librg;

int posX = 100;
int posY = 200;
int sizeX = 800;
int sizeY = 600;

bool isAccelerated = true;

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

SDL_Rect default_position() {
    SDL_Rect position;

    position.x = sizeX/2 - playerPos.x;
    position.y = sizeY/2 - playerPos.y;

    return position;
}

void Render()
{
    // Clear the window and make it all green
    SDL_RenderClear( renderer );

    SDL_SetRenderDrawColor( renderer, 75, 75, 76, 10 );

    playerRange.x = sizeX/2 - 250;
    playerRange.y = sizeY/2 - 250;
    playerRange.w = 500;
    playerRange.h = 500;

    // Render our "player"
    SDL_RenderFillRect( renderer, &playerRange );

    // Change color to gray
    SDL_SetRenderDrawColor( renderer, 150, 150, 150, 255 );

    librg::entities->each<librg::transform_t, hero_t, librg::streamable_t>([](Entity entity, librg::transform_t& transform, hero_t& hero, librg::streamable_t& stream) {
        SDL_Rect position = default_position();

        position.x += (int)transform.position.X - 10;
        position.y += (int)transform.position.Y - 10;
        position.w = 20;
        position.h = 20;

        if (stream.type == TYPE_ENEMY) {
            SDL_SetRenderDrawColor(renderer, 25, 255, 34, 255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        }

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
            SDL_SetRenderDrawColor(renderer, HMM_Lerp(255, hero.decayLevel, 150.f), HMM_Lerp(0.0f, hero.decayLevel, 150.f), HMM_Lerp(0.0f, hero.decayLevel, 150.f), 255);
            SDL_RenderFillRect(renderer, &position);
        }
    });

    librg::entities->each<bomb_t, librg::transform_t>([](Entity entity, bomb_t& bomb, librg::transform_t& transform) {
        SDL_Rect position = default_position();

        position.x += (int)transform.position.X - 10;
        position.y += (int)transform.position.Y - 10;
        position.w = 20;
        position.h = 20;

        SDL_SetRenderDrawColor(renderer, 255*(1 - bomb.timeLeft/ bomb.startTime), 0, 0, 255);

        SDL_RenderFillRect(renderer, &position);
    });

    SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255);

    SDL_Rect position;

    position.x = sizeX/2 - 10;
    position.y = sizeY/2 - 10;
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
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150.f);
            SDL_RenderFillRect(renderer, &position);
        }
    }

    for (auto &explosion : explosions) {
        if (explosion.impact < 0) continue;

        position = default_position();

        position.x += explosion.position.X - explosion.impact / 2.f;
        position.y += explosion.position.Y - explosion.impact / 2.f;
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
    renderer = SDL_CreateRenderer( window, -1, isAccelerated ? SDL_RENDERER_PRESENTVSYNC : SDL_RENDERER_SOFTWARE);

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
                if (event.key.keysym.sym < 323) keysHeld[event.key.keysym.sym] = true;

                switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    shooting = true;
                    break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym < 323) keysHeld[event.key.keysym.sym] = false;
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

        SDL_Delay( 16 );
    }
}

/**
 * Entity add to streamer
 */
void entity_create(events::event_t* evt)
{
    auto event = (events::event_create_t*) evt;

    switch (event->type) {
        case TYPE_ENEMY:
        case TYPE_PLAYER:
        {
            int HP, maxHP;
            event->data->read(maxHP);
            event->data->read(HP);

            auto hero  = event->entity.assign<hero_t>(maxHP);
            auto tran  = event->entity.component<transform_t>();
            auto inter = event->entity.assign<interpolable_t>(*tran);
            hero->HP = HP;
        }break;

        case TYPE_BOMB:
        {
            float timeLeft, startTime;
            event->data->read(startTime);
            event->data->read(timeLeft);

            auto bomb = event->entity.assign<bomb_t>(timeLeft);
            bomb->startTime = startTime;
        } break;
    }
}

/**
 * Entity update in streamer
 */
void entity_update(events::event_t* evt)
{
    auto event = (events::event_update_t*) evt;

    switch (event->type) {
        case TYPE_ENEMY:
        case TYPE_PLAYER:
        {
            int HP, maxHP;


            float decayLevel = 1.f;
            event->data->read(maxHP);
            event->data->read(HP);

            if (HP <= 0) event->data->read(decayLevel);

            auto hero = event->entity.component<hero_t>();
            auto tran = event->entity.component<transform_t>();
            hero->maxHP = maxHP;
            hero->HP = HP;
            hero->decayLevel = decayLevel;
        }break;

        case TYPE_BOMB:
        {
            float timeLeft;
            event->data->read(timeLeft);
            auto bomb = event->entity.component<bomb_t>();
            bomb->timeLeft = timeLeft;
        }break;
    }
}

/**
 * Entity remove from streamer
 */
void entity_remove(events::event_t* evt)
{
    auto event = (events::event_remove_t*) evt;

    switch (event->type) {
        case TYPE_BOMB:
        {
            auto bomb = event->entity.component<bomb_t>();
            if (bomb->timeLeft >= 0.2) break;
            auto tran = event->entity.component<transform_t>();
            hmm_vec3 position = tran->position;

            bool needsNew = true;
            explosion_t explosion = { position, 150.f };

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
void entity_inter(events::event_t* evt)
{
    auto event = (events::event_inter_t*) evt;
    // librg::core::log("entity_interpolate called");

    auto tran = event->entity.component<transform_t>();

    // tran->position = event->data.position;
    *tran = event->data;
}

void ontick(events::event_t* evt)
{
    auto event = (events::event_tick_t*) evt;

    network::msg(GAME_SYNC_PACKET, [](network::bitstream_t* data) {
        // core::log("sending packet with id: %d", GAME_SYNC_PACKET);
        data->write((float) playerPos.x);
        data->write((float) playerPos.y);
    });

    if (shooting) {
        network::msg(GAME_ON_SHOOT, nullptr);
        shooting = false;
    }

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

    // if (argc > 1 ) {//&& !strcpy(args[1], "sw")) {
    //     isAccelerated = false;
    // }

    // setup manual client mode
    librg::core_initialize(librg::mode_client_manual);
    // librg::core_initialize(librg::mode_client);

    // setup callbacks
    librg::events::add(librg::events::on_tick, ontick);
    librg::events::add(librg::events::on_inter, entity_inter);
    librg::events::add(librg::events::on_create, entity_create);
    librg::events::add(librg::events::on_update, entity_update);
    librg::events::add(librg::events::on_remove, entity_remove);

    librg::network::set(GAME_NEW_LOCAL_PLAYER, [](network::peer_t* peer, network::packet_t* packet, network::bitstream_t* data) {
        int maxHP;
        data->read(maxHP);

        playerEntity = librg::entities->create();
        playerEntity.assign<hero_t>(maxHP);
    });

    librg::network::set(GAME_LOCAL_PLAYER_SETHP, [](network::peer_t* peer, network::packet_t* packet, network::bitstream_t* data) {
        int HP;
        data->read(HP);

        auto hero = playerEntity.component<hero_t>();
        hero->HP = HP;

        if (hero->HP < 0) {
            core::log("Oh you're dead!");
        }
    });

    librg::network::set(GAME_PLAYER_SETHP, [](network::peer_t* peer, network::packet_t* packet, network::bitstream_t* data) {
        uint64_t guid;
        int HP;

        data->read(guid);
        data->read(HP);

        auto hero = streamer::entity_pool[guid].component<hero_t>();
        hero->HP = HP;
    });

    if (!InitEverything()) {
        return -1;
    }

    auto cfg = librg::config_t{};
    cfg.ip = "localhost";
    cfg.port = 7750;
    cfg.world_size = HMM_Vec3(5000, 5000, 5000);
    cfg.tick_delay = 32;
    cfg.max_connections = 8;
    cfg.platform_id = NETWORK_PLATFORM_ID;
    cfg.proto_version = NETWORK_PROTOCOL_VERSION;
    cfg.build_version = NETWORK_BUILD_VERSION;

    events::add(events::on_log, [](events::event_t* evt) {
        auto event = (events::event_log_t*) evt;
        std::cout << event->output;
    });

    events::add(events::on_start, [](events::event_t* evt) {
        core::log("me started !");
    });

    // start the client (network connection)
    librg::core::start(cfg);
    librg::network::start(cfg);

    // Initlaize our player
    playerPos.x = 20;
    playerPos.y = 20;
    playerPos.w = 20;
    playerPos.h = 20;

    RunGame();

    librg::core_terminate();
    return 0;
}
