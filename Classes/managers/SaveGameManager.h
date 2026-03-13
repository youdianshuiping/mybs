#pragma once

#include <string>
#include <vector>

#include "models/GameModel.h"
#include "models/UndoModel.h"

/**
 * @brief 存档快照数据。
 *
 * 聚合单局运行时模型与撤销记录，作为存档读写的传输对象。
 */
struct SaveSnapshot
{
    /** 运行时模型快照。 */
    GameModel model;
    /** 撤销记录快照。 */
    std::vector<UndoRecord> undoRecords;
};

/**
 * @brief 存档管理工具。
 *
 * 提供无状态的存档读写能力，负责 SaveSnapshot 与 JSON 文本之间转换。
 * 适用于局内进度持久化与恢复。
 */
class SaveGameManager
{
public:
    /**
     * @brief 保存快照到存档文件。
     * @param snapshot 待保存快照。
     * @return true 表示保存成功；false 表示失败。
     */
    static bool saveSnapshot(const SaveSnapshot& snapshot);

    /**
     * @brief 从存档文件加载快照。
     * @param outSnapshot 输出快照。
     * @return true 表示读取并解析成功；false 表示失败。
     */
    static bool loadSnapshot(SaveSnapshot& outSnapshot);

    /**
     * @brief 清理存档文件。
     * @return true 表示清理成功；false 表示失败。
     */
    static bool clearSnapshot();

    /**
     * @brief 获取存档文件路径。
     * @return 存档文件绝对路径。
     */
    static std::string getSaveFilePath();

private:
    static std::string _toJsonString(const SaveSnapshot& snapshot);
    static bool _fromJsonString(const std::string& json, SaveSnapshot& outSnapshot);
};
