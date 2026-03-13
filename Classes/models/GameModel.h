#pragma once

#include <map>
#include <string>
#include <vector>

#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "models/CardModel.h"

/**
 * GameModel stores complete runtime state of one level.
 */
struct GameModel
{
    int levelId = 0;
    std::map<int, CardModel> cards;
    std::vector<int> playfieldCardIds;
    std::vector<int> stackCardIds;
    int topCardId = -1;
    int stackDrawIndex = 1;

    rapidjson::Value toJson(rapidjson::Document::AllocatorType& allocator) const
    {
        rapidjson::Value obj(rapidjson::kObjectType);
        obj.AddMember("levelId", levelId, allocator);
        obj.AddMember("topCardId", topCardId, allocator);
        obj.AddMember("stackDrawIndex", stackDrawIndex, allocator);

        rapidjson::Value cardsArray(rapidjson::kArrayType);
        for (const auto& pair : cards)
        {
            cardsArray.PushBack(pair.second.toJson(allocator), allocator);
        }
        obj.AddMember("cards", cardsArray, allocator);

        rapidjson::Value playfieldArray(rapidjson::kArrayType);
        for (int id : playfieldCardIds)
        {
            playfieldArray.PushBack(id, allocator);
        }
        obj.AddMember("playfieldCardIds", playfieldArray, allocator);

        rapidjson::Value stackArray(rapidjson::kArrayType);
        for (int id : stackCardIds)
        {
            stackArray.PushBack(id, allocator);
        }
        obj.AddMember("stackCardIds", stackArray, allocator);

        return obj;
    }

    bool fromJson(const rapidjson::Value& obj)
    {
        if (!obj.IsObject())
        {
            return false;
        }

        cards.clear();
        playfieldCardIds.clear();
        stackCardIds.clear();

        levelId = obj.HasMember("levelId") && obj["levelId"].IsInt() ? obj["levelId"].GetInt() : 0;
        topCardId = obj.HasMember("topCardId") && obj["topCardId"].IsInt() ? obj["topCardId"].GetInt() : -1;
        stackDrawIndex = obj.HasMember("stackDrawIndex") && obj["stackDrawIndex"].IsInt() ? obj["stackDrawIndex"].GetInt() : 1;

        if (obj.HasMember("cards") && obj["cards"].IsArray())
        {
            for (const auto& item : obj["cards"].GetArray())
            {
                CardModel card;
                if (!card.fromJson(item))
                {
                    continue;
                }
                cards[card.id] = card;
            }
        }

        if (obj.HasMember("playfieldCardIds") && obj["playfieldCardIds"].IsArray())
        {
            for (const auto& item : obj["playfieldCardIds"].GetArray())
            {
                if (item.IsInt())
                {
                    playfieldCardIds.push_back(item.GetInt());
                }
            }
        }

        if (obj.HasMember("stackCardIds") && obj["stackCardIds"].IsArray())
        {
            for (const auto& item : obj["stackCardIds"].GetArray())
            {
                if (item.IsInt())
                {
                    stackCardIds.push_back(item.GetInt());
                }
            }
        }

        return true;
    }

    std::string toJsonString() const
    {
        rapidjson::Document doc;
        doc.SetObject();
        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
        doc.CopyFrom(toJson(allocator), allocator);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        return std::string(buffer.GetString(), buffer.GetSize());
    }

    bool fromJsonString(const std::string& json)
    {
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        if (doc.HasParseError() || !doc.IsObject())
        {
            return false;
        }

        return fromJson(doc);
    }
};
