#include <librg/server.h>
#include <librg/Core.h>

#include <entityx/entityx.h>

struct Position {
    Position(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    float x, y, z;
};

struct Rotation {
    Rotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    float x, y, z;
};

int main(int argc, char** argv)
{
    // Server::AddLogger(printer);
    // Server::AddLogger([](int type, std::string message) -> void {
    //     printf(message.c_str());
    // });

    // Server::SetResourceFolder("resources");

    // Server::Scripting::Registry::listeners.insert(sqinstall);

    entityx::Entity entity = Server::Entity::Manager::Instance()->create();

    entity.assign<Position>(1.0f, 2.0f, 2.0f);
    entity.assign<Rotation>(1.0f, 2.0f, 2.0f);

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
