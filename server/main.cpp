#include <librg/librg.h>

#include <messages.h>
#include <types.h>

using namespace librg;

void* on_client_connected_proxy(const void* data, Sqrat::Array *array)
{
    // NOTE(zaklaus): Expect only native calls!
    return (void*)data;
}

void on_client_connected_cb(const void* data, void* /* blob */)
{
    auto entity = librg::entities->get((Entity::Id)*(uint64_t*)data);
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
    librg::core::log("entity_create called with type: %d", event->type);

    switch (event->type) {
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

}

/**
 * Entity remove from streamer
 */
void entity_remove_forplayers(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_remove_t*) evt;
    librg::core::log("entity_remove called");
}





void ontick(callbacks::evt_t* evt)
{
    auto event = (callbacks::evt_tick_t*) evt;

    librg::entities->each<bomb_t, transform_t>([event](entity_t bombEntity, bomb_t& bomb, transform_t& bombTransform) {
        if (bomb.timeLeft < 0) {

            librg::entities->each<client_t, transform_t>([&](entity_t playerEntity, client_t& client, transform_t& transform) {
                float blastRadius = 5;

                auto v = (HMM_SubtractVec3(transform.position, bombTransform.position));
                auto d = sqrtf(v.X*v.X + v.Y*v.Y + v.Z*v.Z);
                core::log("Player: %d distance: %f", playerEntity.id().id(), d);
                if (d < 150) {
                    core::log("We've hit hero: %d hard!", client.address.systemIndex);

                    auto hero = playerEntity.component<hero_t>();
                    hero->HP -= 10; // deal 10 HP damage!

                    network::msg(GAME_PLAYER_SETHP, playerEntity, [hero, playerEntity](network::bitstream_t *data) {
                        data->Write(playerEntity.id().id());
                        data->Write(hero->HP);
                    });

                    network::msg(GAME_LOCAL_PLAYER_SETHP, client.address, [hero](network::bitstream_t *data) {
                        data->Write(hero->HP); 
                    });

                }
            });

            auto transform = bombEntity.component<transform_t>();
            auto position = transform->position;
            librg::entities->each<client_t>([bombEntity, position](entity_t entity, client_t& client){
                network::msg(GAME_BOMB_EXPLODE, bombEntity, [position](network::bitstream_t *data) {
                    data->Write(position);
                });
            });
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

                network::msg(GAME_PLAYER_SETHP, entity, [entity](network::bitstream_t *data) {
                    data->Write(entity.id().id());
                    data->Write(100);
                });

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

    librg::events::add("onClientConnect", on_client_connected_cb, on_client_connected_proxy);

    librg::network::add(GAME_ON_SHOOT, [](network::bitstream_t *data, network::packet_t *packet) {
        librg::core::log("Player shoots! BANG BANG!");

        auto player = network::clients[packet->guid];
        auto plTransform = player.component<transform_t>();

        auto entity = entities->create();
        entity.assign<transform_t>(*plTransform);
        entity.assign<bomb_t>(4);

        auto streamable = entity.assign<streamable_t>();
        streamable->type = TYPE_BOMB;
    });

    librg::network::add(GAME_SYNC_PACKET, [](network::bitstream_t *data, network::packet_t *packet) {
        float x, y;

        data->Read(x);
        data->Read(y);

        auto transform = network::clients[packet->guid].component<transform_t>();

        transform->position = hmm_vec3{ x, y, transform->position.Z };
    });

    librg::core::start("localhost", 7750);
    librg::core_terminate();

    return 0;
}
