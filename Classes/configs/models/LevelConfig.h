#pragma once

#include <vector>

#include "cocos2d.h"
#include "models/CardTypes.h"

/**
 * @brief 关卡中的单张卡牌生成配置。
 *
 * 用于描述卡牌初始点数、花色和布局坐标，
 * 常作为静态配置输入，后续由生成器转换为 CardModel。
 */
struct CardSpawnConfig
{
    /** 卡牌点数。 */
    CardFaceType face = CFT_NONE;
    /** 卡牌花色。 */
    CardSuitType suit = CST_NONE;
    /** 卡牌在场景中的初始坐标。 */
    cocos2d::Vec2 position = cocos2d::Vec2::ZERO;
};

/**
 * @brief 单关卡静态配置。
 *
 * 描述场上牌、牌堆牌以及关卡编号，不包含运行时可变状态。
 */
struct LevelConfig
{
    /** 关卡编号。 */
    int levelId = 0;
    /** 场上牌初始配置列表。 */
    std::vector<CardSpawnConfig> playfield;
    /** 牌堆牌初始配置列表。 */
    std::vector<CardSpawnConfig> stack;
};
