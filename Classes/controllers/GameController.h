#pragma once

#include <memory>

#include "managers/UndoManager.h"
#include "models/GameModel.h"

class GameView;

/**
 * GameController 负责协调模型与视图，处理卡牌点击、匹配与撤销。
 */
class GameController
{
public:
    bool clearSave();
    bool startNewGame(int levelId);
    bool startGame(int levelId, GameView* gameView);

private:
    bool _tryLoadSavedGame(int levelId);
    void _saveGameProgress() const;

    void _handlePlayfieldCardClick(int cardId);
    void _handleStackCardClick(int cardId);
    void _handleUndoClick();

    bool _replaceTopWithCard(int cardId, UndoMoveSource source);
    int _getStackPeekCardId() const;
    void _refreshViewState();

    std::unique_ptr<GameModel> _gameModel;
    GameView* _gameView = nullptr;
    UndoManager _undoManager;
    bool _isAnimating = false;
};
