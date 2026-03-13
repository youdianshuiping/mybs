#pragma once

#include <memory>

#include "managers/UndoManager.h"
#include "models/GameModel.h"
#include "models/UndoModel.h"

class GameView;

/**
 * @brief 游戏主控制器。
 *
 * 负责协调 GameModel 与 GameView 的交互，处理用户输入、匹配规则、
 * 牌堆推进、撤销回滚和存档同步，是单局业务流程的编排入口。
 */
class GameController
{
public:
    /**
     * @brief 清除当前存档。
     * @return true 表示清除成功；false 表示清除失败。
     */
    bool clearSave();

    /**
     * @brief 强制从关卡配置开始新局。
     * @param levelId 关卡编号。
     * @return true 表示新局初始化成功；false 表示失败。
     */
    bool startNewGame(int levelId);

    /**
     * @brief 启动游戏流程并绑定视图回调。
     * @param levelId 关卡编号。
     * @param gameView 游戏视图实例。
     * @return true 表示启动成功；false 表示失败。
     */
    bool startGame(int levelId, GameView* gameView);

private:
    bool _tryLoadSavedGame(int levelId);
    void _saveGameProgress() const;

    void _handlePlayfieldCardClick(int cardId);
    void _handleStackCardClick(int cardId);
    void _handleUndoClick();
    bool _tryPopUndoRecord(UndoRecord& outRecord);
    bool _hasUndoRecordCards(const UndoRecord& record) const;
    void _restoreStateFromUndoRecord(const UndoRecord& record);

    bool _replaceTopWithCard(int cardId, UndoMoveSource source);
    UndoRecord _buildUndoRecord(int cardId, int previousTopCardId, UndoMoveSource source) const;
    void _applyTopReplacementState(int cardId, int previousTopCardId, UndoMoveSource source);
    void _finalizeTopReplacement(int previousTopCardId, UndoMoveSource source);
    int _getStackPeekCardId() const;
    void _refreshViewState();

    /** 当前局运行时模型。 */
    std::unique_ptr<GameModel> _gameModel;
    /** 当前绑定的视图对象，不负责其生命周期。 */
    GameView* _gameView = nullptr;
    /** 撤销记录管理器。 */
    UndoManager _undoManager;
    /** 是否处于关键动画流程，用于输入防抖。 */
    bool _isAnimating = false;
};
