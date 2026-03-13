#pragma once

#include "cocos2d.h"
#include "json/document.h"

/**
 * UndoMoveSource marks where incoming top card came from.
 */
enum UndoMoveSource
{
    UMS_PLAYFIELD,
    UMS_STACK
};

/**
 * UndoRecord stores one replace-top operation for rollback.
 */
struct UndoRecord
{
    int movedCardId = -1;
    int previousTopCardId = -1;
    int previousStackDrawIndex = 1;
    cocos2d::Vec2 movedFromPos = cocos2d::Vec2::ZERO;
    UndoMoveSource source = UMS_PLAYFIELD;

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
