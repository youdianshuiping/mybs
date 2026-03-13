#pragma once

#include <vector>

#include "cocos2d.h"
#include "models/CardTypes.h"

/**
 * CardSpawnConfig 定义关卡配置中的单张卡牌条目。
 */
struct CardSpawnConfig
{
    CardFaceType face = CFT_NONE;
    CardSuitType suit = CST_NONE;
    cocos2d::Vec2 position = cocos2d::Vec2::ZERO;
};

/**
 * LevelConfig 包含静态卡牌布局与牌堆配置。
 */
struct LevelConfig
{
    int levelId = 0;
    std::vector<CardSpawnConfig> playfield;
    std::vector<CardSpawnConfig> stack;
};
