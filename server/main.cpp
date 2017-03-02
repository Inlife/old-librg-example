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
    // ...
};

entity_id create_cube(int res, float size, bool hollow) {
    auto data = new CubeData {res, wallSize, hollow };

    auto entity = Entity::Manager::Instance()->Create(EntityType_Cube);

    Entity::Manager::Instance()->AttachData(entity, data);

    return entity;
}

using namespace Server;

int main(int argc, char** argv) {

    Start(argc, argv);
}
