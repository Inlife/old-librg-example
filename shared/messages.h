#include <librg/network.h>

enum {
    GAME_ON_SHOOT = librg::network::last_packet_number,
    GAME_SYNC_PACKET,
    GAME_NEW_LOCAL_PLAYER,
    GAME_LOCAL_PLAYER_SETHP,
    GAME_PLAYER_SETHP,
    GAME_BOMB_EXPLODE,
};
