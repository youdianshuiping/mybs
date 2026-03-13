#include "configs/loaders/LevelConfigLoader.h"

#include "cocos2d.h"
#include "json/document.h"

using namespace cocos2d;

bool LevelConfigLoader::loadLevelConfig(int levelId, LevelConfig& outConfig)
{
    // 约定：每个关卡存放在 Resources/levels/level_{id}.json。
    const std::string filePath = StringUtils::format("levels/level_%d.json", levelId);
    const std::string content = FileUtils::getInstance()->getStringFromFile(filePath);
    if (content.empty())
    {
        CCLOG("LevelConfigLoader: failed to load %s", filePath.c_str());
        return false;
    }

    outConfig.levelId = levelId;
    return _parseConfig(content, outConfig);
}

bool LevelConfigLoader::_parseConfig(const std::string& json, LevelConfig& outConfig)
{
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError() || !doc.IsObject())
    {
        CCLOG("LevelConfigLoader: invalid json");
        return false;
    }

    outConfig.playfield.clear();
    outConfig.stack.clear();

    // 解析场上卡牌；对不完整字段使用宽容默认值。
    if (doc.HasMember("Playfield") && doc["Playfield"].IsArray())
    {
        for (const auto& entry : doc["Playfield"].GetArray())
        {
            if (!entry.IsObject())
            {
                continue;
            }
            CardSpawnConfig config;
            config.face = static_cast<CardFaceType>(entry.HasMember("CardFace") ? entry["CardFace"].GetInt() : CFT_NONE);
            config.suit = static_cast<CardSuitType>(entry.HasMember("CardSuit") ? entry["CardSuit"].GetInt() : CST_NONE);
            if (entry.HasMember("Position") && entry["Position"].IsObject())
            {
                const auto& pos = entry["Position"];
                config.position.x = pos.HasMember("x") ? pos["x"].GetFloat() : 0.0f;
                config.position.y = pos.HasMember("y") ? pos["y"].GetFloat() : 0.0f;
            }
            outConfig.playfield.push_back(config);
        }
    }

    // 按声明顺序解析牌堆；生成器依赖该顺序确定顶部牌与抽牌索引。
    if (doc.HasMember("Stack") && doc["Stack"].IsArray())
    {
        for (const auto& entry : doc["Stack"].GetArray())
        {
            if (!entry.IsObject())
            {
                continue;
            }
            CardSpawnConfig config;
            config.face = static_cast<CardFaceType>(entry.HasMember("CardFace") ? entry["CardFace"].GetInt() : CFT_NONE);
            config.suit = static_cast<CardSuitType>(entry.HasMember("CardSuit") ? entry["CardSuit"].GetInt() : CST_NONE);
            if (entry.HasMember("Position") && entry["Position"].IsObject())
            {
                const auto& pos = entry["Position"];
                config.position.x = pos.HasMember("x") ? pos["x"].GetFloat() : 0.0f;
                config.position.y = pos.HasMember("y") ? pos["y"].GetFloat() : 0.0f;
            }
            outConfig.stack.push_back(config);
        }
    }

    // 当前玩法要求至少有一张牌堆卡牌作为初始顶部牌。
    return !outConfig.stack.empty();
}
