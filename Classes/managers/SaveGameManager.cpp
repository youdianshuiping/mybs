#include "managers/SaveGameManager.h"

#include "cocos2d.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

using namespace cocos2d;

bool SaveGameManager::saveSnapshot(const SaveSnapshot& snapshot)
{
    const std::string json = _toJsonString(snapshot);
    if (json.empty())
    {
        return false;
    }

    return FileUtils::getInstance()->writeStringToFile(json, getSaveFilePath());
}

bool SaveGameManager::loadSnapshot(SaveSnapshot& outSnapshot)
{
    const std::string path = getSaveFilePath();
    if (!FileUtils::getInstance()->isFileExist(path))
    {
        return false;
    }

    const std::string content = FileUtils::getInstance()->getStringFromFile(path);
    if (content.empty())
    {
        return false;
    }

    return _fromJsonString(content, outSnapshot);
}

bool SaveGameManager::clearSnapshot()
{
    const std::string path = getSaveFilePath();
    if (!FileUtils::getInstance()->isFileExist(path))
    {
        return true;
    }

    return FileUtils::getInstance()->removeFile(path);
}

std::string SaveGameManager::getSaveFilePath()
{
    return FileUtils::getInstance()->getWritablePath() + "mybs_save.json";
}

std::string SaveGameManager::_toJsonString(const SaveSnapshot& snapshot)
{
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    doc.AddMember("model", snapshot.model.toJson(allocator), allocator);

    rapidjson::Value undoArray(rapidjson::kArrayType);
    for (const auto& record : snapshot.undoRecords)
    {
        undoArray.PushBack(record.toJson(allocator), allocator);
    }
    doc.AddMember("undoRecords", undoArray, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    if (!doc.Accept(writer))
    {
        return std::string();
    }

    return std::string(buffer.GetString(), buffer.GetSize());
}

bool SaveGameManager::_fromJsonString(const std::string& json, SaveSnapshot& outSnapshot)
{
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError() || !doc.IsObject())
    {
        return false;
    }

    if (!doc.HasMember("model") || !doc["model"].IsObject())
    {
        return false;
    }

    GameModel model;
    if (!model.fromJson(doc["model"]))
    {
        return false;
    }

    std::vector<UndoRecord> undoRecords;
    if (doc.HasMember("undoRecords") && doc["undoRecords"].IsArray())
    {
        for (const auto& item : doc["undoRecords"].GetArray())
        {
            UndoRecord record;
            if (!record.fromJson(item))
            {
                continue;
            }
            undoRecords.push_back(record);
        }
    }

    outSnapshot.model = model;
    outSnapshot.undoRecords = undoRecords;
    return true;
}
