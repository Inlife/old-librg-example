#include <librg/server.h>
#include <librg/Core.h>

#include <entityx/entityx.h>
#include <entityx/deps/Dependencies.h>


struct Position {
    Position(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    float x, y, z;
};

struct Rotation {
    Rotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    float x, y, z;
};

struct Vehicle {
    Vehicle(float fuel = 0.0f) : fuel(fuel) {}

    float fuel;
};

int main(int argc, char** argv)
{
    // Server::AddLogger(printer);
    // Server::AddLogger([](int type, std::string message) -> void {
    //     printf(message.c_str());
    // });

    // Server::SetResourceFolder("resources");

    // Server::Scripting::Registry::listeners.insert(sqinstall);

    //NOTE(zaklaus): We can make vehicle dependant on Pos, Rot easily!
    Server::Entity::Manager::GetSystems()->add<entityx::deps::Dependency<Vehicle, Position, Rotation>>();

    // now let's assume we have native.vehicleCreate(fuel)

    entityx::Entity entity = Server::Entity::Manager::Instance()->create();

    entity.assign<Vehicle>(80.f);

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

    printf("created entity: %zd\n", entity.id().index());

    Server::Start(argc, argv);
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
