#pragma once

#include "cocos2d.h"
#include "json/document.h"

/**
 * @brief 撤销记录中的移动来源。
 *
 * 标记替换顶部牌时，新牌来自场上还是牌堆。
 */
enum UndoMoveSource
{
    UMS_PLAYFIELD,
    UMS_STACK
};

/**
 * @brief 一次替换顶部牌操作的回滚快照。
 *
 * 记录回退所需的最小状态集，支持序列化与反序列化，
 * 供撤销管理和存档恢复使用。
 */
struct UndoRecord
{
    /** 本次移动到顶部的卡牌 id。 */
    int movedCardId = -1;
    /** 被替换前的顶部牌 id。 */
    int previousTopCardId = -1;
    /** 操作前牌堆游标。 */
    int previousStackDrawIndex = 1;
    /** 被移动卡牌的原始位置。 */
    cocos2d::Vec2 movedFromPos = cocos2d::Vec2::ZERO;
    /** 新顶部牌来源。 */
    UndoMoveSource source = UMS_PLAYFIELD;

    /**
     * @brief 序列化为 JSON 对象。
     * @param allocator RapidJSON 分配器。
     * @return 对应的 JSON 值对象。
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType& allocator) const
    {
        rapidjson::Value obj(rapidjson::kObjectType);
        obj.AddMember("movedCardId", movedCardId, allocator);
        obj.AddMember("previousTopCardId", previousTopCardId, allocator);
        obj.AddMember("previousStackDrawIndex", previousStackDrawIndex, allocator);
        obj.AddMember("source", static_cast<int>(source), allocator);

        rapidjson::Value pos(rapidjson::kObjectType);
        pos.AddMember("x", movedFromPos.x, allocator);
        pos.AddMember("y", movedFromPos.y, allocator);
        obj.AddMember("movedFromPos", pos, allocator);

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

        movedCardId = obj.HasMember("movedCardId") && obj["movedCardId"].IsInt() ? obj["movedCardId"].GetInt() : -1;
        previousTopCardId = obj.HasMember("previousTopCardId") && obj["previousTopCardId"].IsInt() ? obj["previousTopCardId"].GetInt() : -1;
        previousStackDrawIndex = obj.HasMember("previousStackDrawIndex") && obj["previousStackDrawIndex"].IsInt()
                                     ? obj["previousStackDrawIndex"].GetInt()
                                     : 1;
        source = static_cast<UndoMoveSource>(obj.HasMember("source") && obj["source"].IsInt() ? obj["source"].GetInt() : UMS_PLAYFIELD);

        movedFromPos = cocos2d::Vec2::ZERO;
        if (obj.HasMember("movedFromPos") && obj["movedFromPos"].IsObject())
        {
            const rapidjson::Value& pos = obj["movedFromPos"];
            if (pos.HasMember("x") && pos["x"].IsNumber())
            {
                movedFromPos.x = pos["x"].GetFloat();
            }
            if (pos.HasMember("y") && pos["y"].IsNumber())
            {
                movedFromPos.y = pos["y"].GetFloat();
            }
        }

        return true;
    }
};
