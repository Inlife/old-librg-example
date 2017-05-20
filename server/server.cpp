#include <librg/librg.h>
#include <librg/components/transform.h>
#include <librg/components/interpolable.h>
#include <librg/components/client.h>

#include <messages.h>
#include <types.h>

#include <librg/utils/fs.h>

using namespace librg;

void client_connect(events::event_t* evt)
{
    auto event  = (events::event_connect_t*)evt;
    auto entity = event->entity;

    entity.assign<streamable_t>();
    entity.assign<transform_t>();
    entity.assign<hero_t>(100);

    auto client = entity.component<client_t>();
    client->active = true;

    network::msg(GAME_NEW_LOCAL_PLAYER, client->peer, [](network::bitstream_t *data) {
        data->write(100);
    });

    librg::core::log("New hero came!");
}

void client_disconnect(events::event_t* evt)
{
    auto event  = (events::event_connect_t*)evt;
    auto entity = event->entity;

    streamer::remove(entity, false);
}

/**
 * Entity add to streamer
 */
void entity_create_forplayer(events::event_t* evt)
{
    auto event = (events::event_create_t*) evt;

    switch (event->type) {
        case TYPE_ENEMY:
        case TYPE_PLAYER:
        {
            auto hero = event->entity.component<hero_t>();
            event->data->write(hero->maxHP);
            event->data->write(hero->HP);
        }break;

        case TYPE_BOMB:
        {
            auto bomb = event->entity.component<bomb_t>();
            event->data->write(bomb->startTime);
            event->data->write(bomb->timeLeft);
        }break;
    }
}

/**
 * Entity update in streamer
 */
void entity_update_forplayers(events::event_t* evt)
{
    auto event = (events::event_update_t*) evt;

    switch (event->type) {
        case TYPE_ENEMY:
        case TYPE_PLAYER:
        {
            auto hero = event->entity.component<hero_t>();

            event->data->write(hero->maxHP);
            event->data->write(hero->HP);

            if (hero->HP <= 0) event->data->write(hero->decayLevel);
        }break;

        case TYPE_BOMB:
        {
            auto bomb = event->entity.component<bomb_t>();
            event->data->write(bomb->timeLeft);
        }break;
    }
}

/**
 * Entity remove from streamer
 */
void entity_remove_forplayers(events::event_t* evt)
{
    auto event = (events::event_remove_t*) evt;
}

void ontick(events::event_t* evt)
{
    auto event = (events::event_tick_t*) evt;

    librg::entities->each<bomb_t, transform_t>([event](entity_t bombEntity, bomb_t& bomb, transform_t& bombTransform) {
        if (bomb.timeLeft < 0) {

            auto victims = streamer::query(bombEntity);

            for (auto victim : victims) {
                if (!victim.component<hero_t>()) continue;

#define pp(x) x*x
                constexpr float blastRadius = pp(250);
#undef pp
                auto transform = victim.component<transform_t>();
                auto hero = victim.component<hero_t>();

                hero->panicCooldown = 20;
                hero->walkTime = 0;
                hero->accel = HMM_NormalizeVec3(HMM_SubtractVec3(bombTransform.position, transform->position));
                hero->accel.X *= -4;
                hero->accel.Y *= -4;

                auto v = (HMM_SubtractVec3(transform->position, bombTransform.position));
                auto d = HMM_LengthSquaredVec3(v);

                if (d <= blastRadius) {
                    hero->HP -= (blastRadius - d) / blastRadius * 250;

                    if (hero->HP < 0) hero->HP = 0;

                    auto client = victim.component<client_t>();

                    if (client) {
                        network::msg(GAME_LOCAL_PLAYER_SETHP, client->peer, [hero](network::bitstream_t *data) {
                            data->write(hero->HP);
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

    librg::entities->each<hero_t>([event](entity_t entity, hero_t& hero) {
        if (hero.HP <= 0) {
            if (hero.cooldown == 0) {
                hero.cooldown = 1;
            }
            else if (hero.cooldown < 0) {
                hero.HP = 100;
                hero.cooldown = 0;

                auto client = entity.component<client_t>();

                if (client) {
                    network::msg(GAME_LOCAL_PLAYER_SETHP, client->peer, [entity](network::bitstream_t *data) {
                        data->write(100);
                    });
                }
                else {
                    streamer::remove(entity);
                }
            }
            else
            {
                hero.cooldown -= event->dt / 25.f;
                hero.decayLevel = 1 - HMM_Lerp(0.f, HMM_MAX(hero.cooldown, 0.0f), 1.0f);
            }
        }
    });

    librg::entities->each<hero_t, transform_t, streamable_t>([event](entity_t npc, hero_t& hero, transform_t& tran, streamable_t& stream) {
        if (stream.type == TYPE_ENEMY && hero.HP > 0) {
            if (hero.walkTime == 0) {

                if (hero.panicCooldown > 0) {
                    hero.walkTime = 0.33f;
                    hero.accel.X += (rand() % 5 - 10.0) / 10.0;
                    hero.accel.Y += (rand() % 5 - 10.0) / 10.0;
                    hero.panicCooldown -= event->dt;
                }
                else {
                    hero.walkTime = 2;
                    hero.accel.X += (rand() % 3 - 1.0) / 10.0;
                    hero.accel.Y += (rand() % 3 - 1.0) / 10.0;

                    hero.accel.X = (hero.accel.X > -1.0) ? ((hero.accel.X < 1.0) ? hero.accel.X : 1.0) : -1.0;
                    hero.accel.X = (hero.accel.Y > -1.0) ? ((hero.accel.Y < 1.0) ? hero.accel.Y : 1.0) : -1.0;
                }
            }
            else {
                auto curpos = tran.position;

                curpos.X += hero.accel.X * event->dt * 100;
                curpos.Y += hero.accel.Y * event->dt * 100;

                if (curpos.X < 0 || curpos.X >= 5000) {
                    curpos.X += hero.accel.X * -2;
                    hero.accel.X *= -1;
                }

                if (curpos.Y < 0 || curpos.Y >= 5000) {
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

void generate_world()
{
    // TODO: Add some world variety...
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

    events::add(events::on_log, [](events::event_t* evt) {
        auto event = (events::event_log_t*) evt;
        std::cout << event->output;
    });

    events::set(events::on_tick, ontick);
    events::set(events::on_create, entity_create_forplayer);
    events::set(events::on_update, entity_update_forplayers);
    events::set(events::on_remove, entity_remove_forplayers);
    events::set(events::on_connect, client_connect);
    events::set(events::on_disconnect, client_disconnect);

    // //librg::events::add("onClientConnect", on_client_connected_cb, on_client_connected_proxy);

    librg::network::set(GAME_ON_SHOOT, [](network::peer_t* peer, network::packet_t* packet, network::bitstream_t* data) {
        librg::core::log("Player shoots! BANG BANG!");

        auto player = network::connected_peers[peer];
        auto plTransform = player.component<transform_t>();

        auto entity = entities->create();
        entity.assign<transform_t>(*plTransform);
        entity.assign<bomb_t>(4);

        auto streamable = entity.assign<streamable_t>(hmm_vec3{250, 250, 250});
        streamable->type = TYPE_BOMB;
    });

    librg::network::set(GAME_SYNC_PACKET, [](network::peer_t* peer, network::packet_t* packet, network::bitstream_t* data) {
        float x, y;

        data->read(x);
        data->read(y);

        auto transform = network::connected_peers[peer].component<transform_t>();

        transform->position = hmm_vec3{ x, y, transform->position.Z };
    });

    auto cfg = librg::config_t{};
    cfg.ip = "localhost";
    cfg.port = 7750;
    cfg.world_size = HMM_Vec3(5000, 5000, 5000);
    cfg.tick_delay = 32;
    cfg.max_connections = 32;
    cfg.platform_id   = NETWORK_PLATFORM_ID;
    cfg.proto_version = NETWORK_PROTOCOL_VERSION;
    cfg.build_version = NETWORK_BUILD_VERSION;

    events::add(events::on_connect, [](events::event_t* evt) {
        core::log("client connected yay!!!!!!");
    });

    network::set(GAME_VEHICLE_ENTER, [](network::peer_t* peer, network::packet_t* packet, network::bitstream_t* data) {
        auto id = entity_t::Id(data->read_uint64());

        if (entities->valid(id)) {
            streamer::client::set(entities->get(id), peer);
        }
    });

    network::set(GAME_VEHICLE_EXIT, [](network::peer_t* peer, network::packet_t* packet, network::bitstream_t* data) {
        auto id = entity_t::Id(data->read_uint64());

        if (entities->valid(id)) {
            streamer::client::remove(entities->get(id));
        }
    });

    events::set(events::on_start, [](events::event_t* evt) {
        for (int i = 0; i < 450; i++) {

            auto entity = entities->create();
            auto tran   = entity.assign<transform_t>();
            auto stream = entity.assign<streamable_t>();
            auto hero   = entity.assign<hero_t>(100);

            hero->accel.X = (rand() % 3 - 1.0);
            hero->accel.Y = (rand() % 3 - 1.0);

            stream->type = TYPE_ENEMY;
            srand(time(0) + i);

            tran->position = hmm_vec3{ (float)(rand() % 5000), (float)(rand() % 5000), 0.0f };
        }
    });

    librg::core::start(cfg);
    librg::core_terminate();

    return 0;
}
