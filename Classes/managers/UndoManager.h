#pragma once

#include <vector>

#include "models/UndoModel.h"

/**
 * @brief 撤销记录管理器。
 *
 * 维护后进先出（LIFO）的 UndoRecord 栈，供控制器执行回退逻辑。
 */
class UndoManager
{
public:
    /** 清空所有撤销记录。 */
    void clear();
    /**
     * @brief 压入一条撤销记录。
     * @param record 待保存的撤销记录。
     */
    void pushRecord(const UndoRecord& record);
    /**
     * @brief 判断是否可执行撤销。
     * @return true 表示存在可撤销记录。
     */
    bool canUndo() const;
    /**
     * @brief 弹出最近一条撤销记录。
     * @param outRecord 输出记录。
     * @return true 表示弹出成功；false 表示记录为空。
     */
    bool popRecord(UndoRecord& outRecord);
    /**
     * @brief 获取当前全部撤销记录。
     * @return 记录只读引用。
     */
    const std::vector<UndoRecord>& getRecords() const;
    /**
     * @brief 用外部记录覆盖当前撤销栈。
     * @param records 新的撤销记录序列。
     */
    void setRecords(const std::vector<UndoRecord>& records);

private:
    /** 按时间顺序保存的撤销记录栈。 */
    std::vector<UndoRecord> _records;
};
