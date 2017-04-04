
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