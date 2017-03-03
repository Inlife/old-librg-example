#include <librg/server.h>
#include <librg/Core.h>

struct CubeData
{
    int res;
    float wallSize;
    bool hollow;
};

using namespace Server;

enum
{
    EntityType_Cube = EntityType_FirstFree,
    EntityType_Vehicle,
    // ...
};

entity_id create_cube(int res, float size, bool hollow) {
    auto data = new CubeData {res, size, hollow };

    auto entity = Entity::Manager::Instance()->Create(EntityType_Cube);

    Entity::Manager::Instance()->AttachData(entity, data);

    return entity;
}


using ResourceManager = Server::Resource::Manager;
using EntityManager = Server::Resource::Manager;



struct Vehicle {
    byte wheels[2];
    int64_t color[2];
};

bool test() {
    return ResourceManager::Instance()->IsRunning("default");
}

int64_t createVehicle(int model, float x, float y, float z, float rx, float ry, float rz, Function callback) {
    entity_id entity = Server::Entity::Manager::Create(EntityType_Vehicle);

    return entity;
}

void sqinstall(Sqrat::Table& native)
{
    native.Func("test",  &test);
    native.Func("createVehicle", &createVehicle);
}

int main(int argc, char** argv)
{
    Server::AddLogger(printer);
    Server::AddLogger([](int type, std::string message) -> void {
        printf(message.c_str());
    });

    Server::SetResourceFolder("resources");

    Server::Scripting::Registry::listeners.insert(sqinstall);

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
