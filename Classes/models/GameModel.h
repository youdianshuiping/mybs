#pragma once

#include <map>
#include <string>
#include <vector>

#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "models/CardModel.h"

/**
 * @brief 单局游戏运行时数据模型。
 *
 * 保存局内卡牌索引、区域分组、顶部牌与牌堆游标等状态，
 * 支持序列化与反序列化，用于存档恢复。
 */
struct GameModel
{
    /** 当前关卡编号。 */
    int levelId = 0;
    /** 全量卡牌索引：cardId -> CardModel。 */
    std::map<int, CardModel> cards;
    /** 场上牌 id 列表。 */
    std::vector<int> playfieldCardIds;
    /** 牌堆牌 id 列表。 */
    std::vector<int> stackCardIds;
    /** 当前顶部牌 id。 */
    int topCardId = -1;
    /** 牌堆可抽位置游标。 */
    int stackDrawIndex = 1;

    /**
     * @brief 序列化为 JSON 对象。
     * @param allocator RapidJSON 分配器。
     * @return 对应的 JSON 值对象。
     */
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

    /**
     * @brief 序列化为 JSON 字符串。
     * @return JSON 文本。
     */
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

    /**
     * @brief 从 JSON 字符串反序列化。
     * @param json JSON 文本。
     * @return true 表示解析成功；false 表示解析失败。
     */
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
