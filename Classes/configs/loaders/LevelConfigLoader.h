#pragma once

#include <string>

#include "configs/models/LevelConfig.h"

/**
 * LevelConfigLoader 将静态关卡 JSON 加载为 LevelConfig。
 */
class LevelConfigLoader
{
public:
    /**
     * 通过关卡 id 加载关卡配置。
     */
    static bool loadLevelConfig(int levelId, LevelConfig& outConfig);

private:
    static bool _parseConfig(const std::string& json, LevelConfig& outConfig);
};
