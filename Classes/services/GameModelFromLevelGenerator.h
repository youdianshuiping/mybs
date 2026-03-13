#pragma once

#include <memory>

#include "configs/models/LevelConfig.h"
#include "models/GameModel.h"

/**
 * GameModelFromLevelGenerator 将静态关卡配置转换为运行时模型。
 */
class GameModelFromLevelGenerator
{
public:
    static std::unique_ptr<GameModel> generate(const LevelConfig& levelConfig);
};
