#define RGSERVER

#include <librg/core/server.h>
#include <librg/events.h>
#include <librg/entities.h>
#include <librg/events.h>
#include <librg/network.h>
#include <librg/resources.h>
#include <librg/streamer.h>

#include <messages.h>
#include <types.h>

// #include <entityx/entityx.h>
// #include <entityx/deps/Dependencies.h>


// struct Position {
//     Position(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

//     float x, y, z;
// };

// struct Rotation {
//     Rotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

//     float x, y, z;
// };

// struct Vehicle {
//     Vehicle(float fuel = 0.0f) : fuel(fuel) {}

//     float fuel;
// };

void ontick(double dt)
{
    librg::entities->each<bomb_t>([dt](Entity entity, bomb_t& bomb) {
        if (bomb.timeLeft < 0) {
            librg::streamer::remove(entity);
            entity.destroy();
        }
        else {
            bomb.timeLeft -= dt;
        }
    });
}

struct hero_t {
    hero_t() : attackSpeed(1.f), ammo(42) { }
    hero_t(float attackSpeed, int ammo) : attackSpeed(attackSpeed), ammo(ammo) { }

    float attackSpeed;
    int ammo;
};

void* on_client_connected_proxy(const void* data, Sqrat::Array *array)
{
    // NOTE(zaklaus): Expect only native calls!
    return (void*)data;
}

void on_client_connected_cb(const void* data, void* /* blob */)
{
    auto entity = librg::entities->get((Entity::Id)*(uint64_t*)data);
    entity.assign<hero_t>();
    librg::core::log("New hero came!");
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

    using namespace librg;

    librg::entities_initialize();
    librg::events_initialize();
    librg::network_initialize();
    librg::resources_initialize();
    librg::streamer_initialize(4000, 4000);

    librg::events::add("onClientConnect", on_client_connected_cb, on_client_connected_proxy);

    librg::network::set_sync_cb(librg::core::rgmode::mode_server, [](network::bitstream_t* data, Entity entity, int type) {
        switch (type) {
        
        case TYPE_BOMB:
        {
            auto bomb = entity.component<bomb_t>();
            data->Write(bomb->startTime);
            data->Write(bomb->timeLeft);
        }break;
        }
    });

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

        transform->position = vectorial::vec3f(x, y, transform->position.z());
    });

    librg::network::server(7750);
    librg::core::set_tick_cb(ontick);
    librg::core::server(argc, argv);

    librg::streamer_terminate();
    librg::entities_terminate();
    librg::events_terminate();
    librg::network_terminate();
    librg::resources_terminate();

    // Server::AddLogger(printer);
    // Server::AddLogger([](int type, std::string message) -> void {
    //     printf(message.c_str());
    // });

    // Server::SetResourceFolder("resources");

    // Server::Scripting::Registry::listeners.insert(sqinstall);

    //NOTE(zaklaus): We can make vehicle dependant on Pos, Rot easily!
    // Server::Entity::Manager::GetSystems()->add<entityx::deps::Dependency<Vehicle, Position, Rotation>>();

    // now let's assume we have native.vehicleCreate(fuel)

    // entityx::Entity entity = Server::Entity::Manager::Instance()->create();

    // entity.assign<Vehicle>(80.f);

    // Now, the entity also contains Pos and Rot, we just have to set it using let's say:
    /*
        native.movableSetPosition(x,y,z)
        native.movableSetRotation(x,y,z)

        We can also have native.movableCreate, which would add Position, Rotation and a Scale to the entity, but since Vehicle depends on them, they exist already

        SQ process to create vehicle and move to 120, 40, 8:
        - local fastcar = native.entityCreate()
        - native.vehicleCreate(fastcar, 42.0f // fuel //)
        - native.movableSetPosition(fastcar, [120, 40, 8])

        which can be encapsulated in our SQ framework as:
        - local fastcar = Vehicle(42.0f, [120, 40, 8])
        - fastcar.Spawn()

        -- zaklaus
    */

    // printf("created entity: %zd\n", entity.id().index());


    // librg::core::client(argc, argv);

    // while (true) {
    //     librg::core::client_tick();
    // }

    return 0;
}

// function createVehicle(modelid, x,y,z) {
//     local entity = null;

//     native.createEntity("type", [], function(entity) {
//         entity = entity;
//     });

//     while (!entity) {
//         // do nothing
//     }

//     return entity.index;
// }
