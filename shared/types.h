#pragma once

/**
* Random number setting current game platform id
* to prevent connections from different games/mods, in future
*/
#ifndef NETWORK_PLATFORM_ID
#define NETWORK_PLATFORM_ID 1
#endif

/**
* Version of protocol, should be changed if major, incompatible changes are introduced
*/
#ifndef NETWORK_PROTOCOL_VERSION
#define NETWORK_PROTOCOL_VERSION 1
#endif

/**
* version of build. server owner can decide,
* should clients should be refused or allowed joining the game
*/
#ifndef NETWORK_BUILD_VERSION
#define NETWORK_BUILD_VERSION 1
#endif

enum {
    TYPE_PLAYER,
    TYPE_BOMB,
    TYPE_ENEMY,
};

struct bomb_t {
    bomb_t(float timeLeft) : timeLeft(timeLeft), startTime(timeLeft) {}
    bomb_t() : timeLeft(0), startTime(0) {}
    float timeLeft;
    float startTime;
};

struct hero_t {
    hero_t()          : maxHP(100), HP(maxHP), cooldown(0), accel() {}
    hero_t(int maxHP) : maxHP(maxHP), HP(maxHP), cooldown(0), accel() {}
    int maxHP;
    int HP;
    float cooldown;
    hmm_vec3 accel;

    // enemy stuff
    float panicCooldown   = 0.0;
    hmm_vec3 bombLocation = {};

    float walkTime        = 0.0;
    float decayLevel      = 1.0;
};

struct building_t {
    building_t() {}

    // TODO: what should the buildings have?
};

