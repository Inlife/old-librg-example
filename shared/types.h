
#pragma once

enum {
    TYPE_PLAYER,
    TYPE_BOMB,
};

struct bomb_t {
    bomb_t(float timeLeft) : timeLeft(timeLeft), startTime(timeLeft) {}
    bomb_t() : timeLeft(0), startTime(0) {}
    float timeLeft;
    float startTime;
};

struct hero_t {
    hero_t()          : maxHP(100), HP(maxHP) {}
    hero_t(int maxHP) : maxHP(maxHP), HP(maxHP) {}
    int maxHP;
    int HP;
};