#include "managers/UndoManager.h"

void UndoManager::clear()
{
    _records.clear();
}

void UndoManager::pushRecord(const UndoRecord& record)
{
    _records.push_back(record);
}

bool UndoManager::canUndo() const
{
    return !_records.empty();
}

bool UndoManager::popRecord(UndoRecord& outRecord)
{
    if (_records.empty())
    {
        return false;
    }

    outRecord = _records.back();
    _records.pop_back();
    return true;
}

const std::vector<UndoRecord>& UndoManager::getRecords() const
{
    return _records;
}

void UndoManager::setRecords(const std::vector<UndoRecord>& records)
{
    _records = records;
}
