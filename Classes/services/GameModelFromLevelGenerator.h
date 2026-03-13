#pragma once

#include <memory>

#include "configs/models/LevelConfig.h"
#include "models/GameModel.h"

/**
 * @brief 关卡配置到运行时模型的转换器。
 *
 * 负责将静态 LevelConfig 映射为 GameModel，
 * 不持有业务状态，仅提供数据加工能力。
 */
class GameModelFromLevelGenerator
{
public:
    /**
     * @brief 生成运行时模型。
     * @param levelConfig 关卡静态配置。
     * @return GameModel 智能指针，失败时返回空指针。
     */
    static std::unique_ptr<GameModel> generate(const LevelConfig& levelConfig);
};
