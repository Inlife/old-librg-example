#include <librg/librg.h>

#include <messages.h>
#include <types.h>

using namespace librg;

void client_connect(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_connect_t*)evt;

    auto entity = event->entity;
    entity.assign<hero_t>(100);

    auto client = entity.component<client_t>();
    
    network::msg(GAME_NEW_LOCAL_PLAYER, client->address, [](network::bitstream_t *data) {
        data->Write(100);
    });

    librg::core::log("New hero came!");
}

/**
 * Entity add to streamer
 */
void entity_create_forplayer(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_create_t*) evt;

    switch (event->type) {
        case TYPE_ENEMY:
        case TYPE_PLAYER:
        {
            auto hero = event->entity.component<hero_t>();
            event->data->Write(hero->maxHP);
            event->data->Write(hero->HP);
        }break;

        case TYPE_BOMB:
        {
            auto bomb = event->entity.component<bomb_t>();
            event->data->Write(bomb->startTime);
            event->data->Write(bomb->timeLeft);
        }break;
    }
}

/**
 * Entity update in streamer
 */
void entity_update_forplayers(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_update_t*) evt;

    switch (event->type) {
        case TYPE_ENEMY:
        case TYPE_PLAYER:
        {
            auto hero = event->entity.component<hero_t>();
            event->data->Write(hero->maxHP);
            event->data->Write(hero->HP);
        }break;

        case TYPE_BOMB:
        {
            auto bomb = event->entity.component<bomb_t>();
            event->data->Write(bomb->timeLeft);
        }break;
    }
}

/**
 * Entity remove from streamer
 */
void entity_remove_forplayers(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_remove_t*) evt;
}

void ontick(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_tick_t*) evt;

    librg::entities->each<bomb_t, transform_t>([event](entity_t bombEntity, bomb_t& bomb, transform_t& bombTransform) {
        if (bomb.timeLeft < 0) {

            auto victims = streamer::query(bombEntity);

            for (auto victim : victims) {
                if (!victim.component<hero_t>()) continue;

                float blastRadius = 5;
                auto transform = victim.component<transform_t>();
                auto hero = victim.component<hero_t>();

                hero->panicCooldown = 20;
                hero->walkTime = 0;
                hero->accel = HMM_NormalizeVec3(HMM_SubtractVec3(bombTransform.position, transform->position));
                hero->accel.X *= -4;
                hero->accel.Y *= -4;

                auto v = (HMM_SubtractVec3(transform->position, bombTransform.position));
                auto d = HMM_LengthSquaredVec3(v);

                if (d <= 150*150) { // NOTE: optimization
                    hero->HP -= d / 150*150 - 20; // deal 10 HP damage!
                    auto client = victim.component<client_t>();

                    if (client) {
                        network::msg(GAME_LOCAL_PLAYER_SETHP, client->address, [hero](network::bitstream_t *data) {
                            data->Write(hero->HP);
                        });
                    }
                }
            }

            librg::streamer::remove(bombEntity);
        }
        else {
            bomb.timeLeft -= event->dt;
        }
    });

    librg::entities->each<hero_t, client_t>([event](entity_t entity, hero_t& hero, client_t& client) {
        if (hero.HP <= 0) {
            if (hero.cooldown == 0) {
                hero.cooldown = 1;
            }
            else if (hero.cooldown < 0) {
                hero.HP = 100;
                hero.cooldown = 0;

                network::msg(GAME_LOCAL_PLAYER_SETHP, client.address, [entity](network::bitstream_t *data) {
                    data->Write(100);
                });
            }
            else
            {
                hero.cooldown -= event->dt / 25.f;
            }
        }
    });

    librg::entities->each<hero_t, transform_t, streamable_t>([event](entity_t npc, hero_t& hero, transform_t& tran, streamable_t& stream) {
        if (stream.type == TYPE_ENEMY && hero.HP > 0) {
            if (hero.walkTime == 0) {

                if (hero.panicCooldown > 0) {
                    hero.walkTime = 0.33;
                    hero.accel.X += (rand() % 5 - 10.0) / 10.0;
                    hero.accel.Y += (rand() % 5 - 10.0) / 10.0;
                    hero.panicCooldown -= event->dt;
                }
                else {
                    hero.walkTime = 2;
                    hero.accel.X += (rand() % 3 - 1.0) / 10.0;
                    hero.accel.Y += (rand() % 3 - 1.0) / 10.0;
                }

                hero.accel.X = (hero.accel.X > -1.0) ? ((hero.accel.X < 1.0) ? hero.accel.X : 1.0) : -1.0;
                hero.accel.X = (hero.accel.Y > -1.0) ? ((hero.accel.Y < 1.0) ? hero.accel.Y : 1.0) : -1.0;
            }
            else {
                auto curpos = tran.position;

                curpos.X += hero.accel.X;
                curpos.Y += hero.accel.Y;

                if (curpos.X < 0 || curpos.X >= 800) {
                    curpos.X += hero.accel.X * -2;
                    hero.accel.X *= -1;
                }

                if (curpos.Y < 0 || curpos.Y >= 600) {
                    curpos.Y += hero.accel.Y * -2;
                    hero.accel.Y *= -1;
                }

                tran.position = curpos;

                hero.walkTime -= event->dt;

                if (hero.walkTime < 0) {
                    hero.walkTime = 0;
                }
            }
        }
    });
}




int main(int argc, char** argv)
{
    std::string test = "";
    test.append("===============      SERVER      =================\n");
    test.append("==                                              ==\n");
    test.append("==                 ¯\\_(ツ)_/¯                   ==\n");
    test.append("==                                              ==\n");
    test.append("==================================================\n");
    printf("%s\n\n", test.c_str());

    librg::core_initialize(librg::mode_server);
    librg::callbacks::set(librg::callbacks::tick, ontick);
    librg::callbacks::set(librg::callbacks::create, entity_create_forplayer);
    librg::callbacks::set(librg::callbacks::update, entity_update_forplayers);
    librg::callbacks::set(librg::callbacks::remove, entity_remove_forplayers);
    librg::callbacks::set(librg::callbacks::connect, client_connect);

    //librg::events::add("onClientConnect", on_client_connected_cb, on_client_connected_proxy);

    librg::network::add(GAME_ON_SHOOT, [](network::bitstream_t *data, network::packet_t *packet) {
        librg::core::log("Player shoots! BANG BANG!");

        auto player = network::clients[packet->guid];
        auto plTransform = player.component<transform_t>();

        auto entity = entities->create();
        entity.assign<transform_t>(*plTransform);
        entity.assign<bomb_t>(4);

        auto streamable = entity.assign<streamable_t>(hmm_vec3{250, 250, 250});
        streamable->type = TYPE_BOMB;
    });

    librg::network::add(GAME_SYNC_PACKET, [](network::bitstream_t *data, network::packet_t *packet) {
        float x, y;

        data->Read(x);
        data->Read(y);

        auto transform = network::clients[packet->guid].component<transform_t>();

        transform->position = hmm_vec3{ x, y, transform->position.Z };
    });

    auto cfg = librg::core::config_t{};
    cfg.ip = "localhost";
    cfg.port = 7750;
    cfg.worldSize = HMM_Vec3(5000, 5000, 5000);
    cfg.tickRate = 256;

    cfg.platformId   = NETWORK_PLATFORM_ID;
    cfg.protoVersion = NETWORK_PROTOCOL_VERSION;
    cfg.buildVersion = NETWORK_BUILD_VERSION;

    callbacks::set(callbacks::log, [](callbacks::evt_t* evt) {
        auto event = (callbacks::evt_log_t*)evt;
        std::cout << event->output;
    });

    callbacks::set(callbacks::start, [](callbacks::evt_t* evt) {
        for (int i = 0; i < 50; i++) {
            auto entity = entities->create();
            auto tran   = entity.assign<transform_t>();
            auto stream = entity.assign<streamable_t>();
            auto hero   = entity.assign<hero_t>(100);

            hero->accel.X = (rand() % 3 - 1.0);
            hero->accel.Y = (rand() % 3 - 1.0);

            stream->type = TYPE_ENEMY;
            srand(time(0) + i);

            tran->position = hmm_vec3{ (float)(rand() % 800), (float)(rand() % 600), 0.0f };
        }
    });

    librg::core::start(cfg);
    librg::core_terminate();

    return 0;
}
