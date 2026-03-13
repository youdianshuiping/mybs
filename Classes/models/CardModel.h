#pragma once

#include <string>

#include "cocos2d.h"
#include "json/document.h"
#include "models/CardTypes.h"

/**
 * CardModel runtime data for one card.
 */
struct CardModel
{
    int id = -1;
    CardFaceType face = CFT_NONE;
    CardSuitType suit = CST_NONE;
    CardSourceType source = CST_PLAYFIELD;
    CardZoneType zone = CZT_HIDDEN;
    cocos2d::Vec2 originalPosition = cocos2d::Vec2::ZERO;

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
