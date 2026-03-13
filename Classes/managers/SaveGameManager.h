#pragma once

#include <string>
#include <vector>

#include "models/GameModel.h"
#include "models/UndoModel.h"

/**
 * SaveSnapshot groups runtime model and undo records for persistence.
 */
struct SaveSnapshot
{
    GameModel model;
    std::vector<UndoRecord> undoRecords;
};

/**
 * SaveGameManager provides stateless save/load helpers for game progress.
 */
class SaveGameManager
{
public:
    static bool saveSnapshot(const SaveSnapshot& snapshot);
    static bool loadSnapshot(SaveSnapshot& outSnapshot);
    static bool clearSnapshot();
    static std::string getSaveFilePath();

private:
    static std::string _toJsonString(const SaveSnapshot& snapshot);
    static bool _fromJsonString(const std::string& json, SaveSnapshot& outSnapshot);
};
