#pragma once

#include <string>

#include "configs/models/LevelConfig.h"

/**
 * @brief 关卡配置加载器。
 *
 * 负责从 Resources 下的关卡 JSON 文件读取并解析出 LevelConfig，
 * 主要用于开局初始化或切关时构建运行时模型输入。
 */
class LevelConfigLoader
{
public:
    /**
     * @brief 通过关卡 id 加载关卡配置。
     * @param levelId 关卡编号。
     * @param outConfig 输出的关卡配置对象。
     * @return true 表示加载并解析成功；false 表示失败。
     */
    static bool loadLevelConfig(int levelId, LevelConfig& outConfig);

private:
    /**
     * @brief 解析 JSON 字符串为关卡配置。
     * @param json 关卡 JSON 文本。
     * @param outConfig 输出的关卡配置对象。
     * @return true 表示解析成功；false 表示解析失败。
     */
    static bool _parseConfig(const std::string& json, LevelConfig& outConfig);
};
