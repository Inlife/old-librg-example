#include <librg/core/server.h>
#include <librg/events.h>

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
    // librg::core::log("ticking\n");
}

int main(int argc, char** argv)
{
    std::string test = "";

    test.append("==================================================\n");
    test.append("==                                              ==\n");
    test.append("==                 ¯\\_(ツ)_/¯                   ==\n");
    test.append("==                                              ==\n");
    test.append("==================================================\n");

    printf("%s\n\n", test.c_str());

    librg::entities_initialize();
    librg::events_initialize();
    librg::network_initialize();
    librg::resources_initialize();

    librg::core::set_tick_cb(ontick);
    librg::core::server(argc, argv);

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
