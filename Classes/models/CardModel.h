#pragma once

#include <string>

#include "cocos2d.h"
#include "json/document.h"
#include "models/CardTypes.h"

/**
 * @brief 单张卡牌运行时模型。
 *
 * 仅描述卡牌状态数据，不承载业务流程。支持 JSON 序列化与反序列化，
 * 用于局内存档恢复。
 */
struct CardModel
{
    /** 卡牌唯一 id。 */
    int id = -1;
    /** 卡牌点数。 */
    CardFaceType face = CFT_NONE;
    /** 卡牌花色。 */
    CardSuitType suit = CST_NONE;
    /** 卡牌来源（场上或牌堆）。 */
    CardSourceType source = CST_PLAYFIELD;
    /** 卡牌当前所在区域。 */
    CardZoneType zone = CZT_HIDDEN;
    /** 卡牌原始布局坐标。 */
    cocos2d::Vec2 originalPosition = cocos2d::Vec2::ZERO;

    /**
     * @brief 序列化为 JSON 对象。
     * @param allocator RapidJSON 分配器。
     * @return 对应的 JSON 值对象。
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType& allocator) const
    {
        rapidjson::Value obj(rapidjson::kObjectType);
        obj.AddMember("id", id, allocator);
        obj.AddMember("face", static_cast<int>(face), allocator);
        obj.AddMember("suit", static_cast<int>(suit), allocator);
        obj.AddMember("source", static_cast<int>(source), allocator);
        obj.AddMember("zone", static_cast<int>(zone), allocator);

        rapidjson::Value pos(rapidjson::kObjectType);
        pos.AddMember("x", originalPosition.x, allocator);
        pos.AddMember("y", originalPosition.y, allocator);
        obj.AddMember("originalPosition", pos, allocator);

        return obj;
    }

    /**
     * @brief 从 JSON 对象反序列化。
     * @param obj 输入 JSON 对象。
     * @return true 表示解析成功；false 表示解析失败。
     */
    bool fromJson(const rapidjson::Value& obj)
    {
        if (!obj.IsObject())
        {
            return false;
        }

        if (!obj.HasMember("id") || !obj["id"].IsInt())
        {
            return false;
        }

        id = obj["id"].GetInt();
        face = static_cast<CardFaceType>(obj.HasMember("face") && obj["face"].IsInt() ? obj["face"].GetInt() : CFT_NONE);
        suit = static_cast<CardSuitType>(obj.HasMember("suit") && obj["suit"].IsInt() ? obj["suit"].GetInt() : CST_NONE);
        source = static_cast<CardSourceType>(obj.HasMember("source") && obj["source"].IsInt() ? obj["source"].GetInt() : CST_PLAYFIELD);
        zone = static_cast<CardZoneType>(obj.HasMember("zone") && obj["zone"].IsInt() ? obj["zone"].GetInt() : CZT_HIDDEN);

        originalPosition = cocos2d::Vec2::ZERO;
        if (obj.HasMember("originalPosition") && obj["originalPosition"].IsObject())
        {
            const rapidjson::Value& pos = obj["originalPosition"];
            if (pos.HasMember("x") && pos["x"].IsNumber())
            {
                originalPosition.x = pos["x"].GetFloat();
            }
            if (pos.HasMember("y") && pos["y"].IsNumber())
            {
                originalPosition.y = pos["y"].GetFloat();
            }
        }

        return true;
    }
};
