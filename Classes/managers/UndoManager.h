#pragma once

#include <vector>

#include "models/UndoModel.h"

/**
 * UndoManager 按后进先出（LIFO）顺序存储回滚记录。
 */
class UndoManager
{
public:
    void clear();
    void pushRecord(const UndoRecord& record);
    bool canUndo() const;
    bool popRecord(UndoRecord& outRecord);
    const std::vector<UndoRecord>& getRecords() const;
    void setRecords(const std::vector<UndoRecord>& records);

private:
    std::vector<UndoRecord> _records;
};
