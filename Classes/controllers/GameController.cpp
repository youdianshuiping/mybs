#include "controllers/GameController.h"

#include "configs/loaders/LevelConfigLoader.h"
#include "managers/SaveGameManager.h"
#include "services/CardRuleService.h"
#include "services/GameModelFromLevelGenerator.h"
#include "views/GameView.h"

using namespace cocos2d;

namespace
{
// Development default: apply level config changes immediately on next launch.
const bool kEnableAutoLoadSnapshot = false;
}

bool GameController::clearSave()
{
    return SaveGameManager::clearSnapshot();
}

bool GameController::startNewGame(int levelId)
{
    if (_gameView == nullptr)
    {
        return false;
    }

    SaveGameManager::clearSnapshot();

    LevelConfig levelConfig;
    if (!LevelConfigLoader::loadLevelConfig(levelId, levelConfig))
    {
        return false;
    }

    _gameModel = GameModelFromLevelGenerator::generate(levelConfig);
    if (!_gameModel || _gameModel->topCardId < 0)
    {
        return false;
    }

    _undoManager.clear();
    _isAnimating = false;

    _gameView->setup(*_gameModel);
    _gameView->setTopCard(_gameModel->topCardId);
    _gameView->setStackPeekCard(_getStackPeekCardId());
    _gameView->setUndoEnabled(false);

    _refreshViewState();
    _saveGameProgress();
    return true;
}

bool GameController::startGame(int levelId, GameView* gameView)
{
    if (gameView == nullptr)
    {
        return false;
    }

    _gameView = gameView;

    if (!kEnableAutoLoadSnapshot || !_tryLoadSavedGame(levelId))
    {
        LevelConfig levelConfig;
        if (!LevelConfigLoader::loadLevelConfig(levelId, levelConfig))
        {
            return false;
        }

        _gameModel = GameModelFromLevelGenerator::generate(levelConfig);
        if (!_gameModel || _gameModel->topCardId < 0)
        {
            return false;
        }

        _undoManager.clear();
    }

    // View only renders; controller binds user intents here.
    _gameView->setup(*_gameModel);
    _gameView->setOnPlayfieldCardClick(CC_CALLBACK_1(GameController::_handlePlayfieldCardClick, this));
    _gameView->setOnStackCardClick(CC_CALLBACK_1(GameController::_handleStackCardClick, this));
    _gameView->setOnUndoClick(CC_CALLBACK_0(GameController::_handleUndoClick, this));

    _gameView->setTopCard(_gameModel->topCardId);
    _gameView->setStackPeekCard(_getStackPeekCardId());
    _gameView->setUndoEnabled(_undoManager.canUndo());

    _refreshViewState();
    _saveGameProgress();
    return true;
}

bool GameController::_tryLoadSavedGame(int levelId)
{
    SaveSnapshot snapshot;
    if (!SaveGameManager::loadSnapshot(snapshot))
    {
        return false;
    }

    if (snapshot.model.levelId != levelId)
    {
        return false;
    }

    _gameModel.reset(new GameModel(snapshot.model));
    _undoManager.setRecords(snapshot.undoRecords);
    return _gameModel != nullptr;
}

void GameController::_saveGameProgress() const
{
    if (!_gameModel)
    {
        return;
    }

    SaveSnapshot snapshot;
    snapshot.model = *_gameModel;
    snapshot.undoRecords = _undoManager.getRecords();
    SaveGameManager::saveSnapshot(snapshot);
}

void GameController::_handlePlayfieldCardClick(int cardId)
{
    // Ignore input during animation to avoid interleaved transitions.
    if (_isAnimating || !_gameModel)
    {
        return;
    }

    const auto cardIt = _gameModel->cards.find(cardId);
    const auto topIt = _gameModel->cards.find(_gameModel->topCardId);
    if (cardIt == _gameModel->cards.end() || topIt == _gameModel->cards.end())
    {
        return;
    }

    if (cardIt->second.zone != CZT_PLAYFIELD)
    {
        return;
    }

    if (!CardRuleService::canMatch(cardIt->second.face, topIt->second.face))
    {
        _gameView->setTipText("Playfield card must differ by 1 from top card");
        return;
    }

    // Playfield and stack clicks share the same replace-top pipeline.
    _replaceTopWithCard(cardId, UMS_PLAYFIELD);
}

void GameController::_handleStackCardClick(int cardId)
{
    if (_isAnimating || !_gameModel)
    {
        return;
    }

    if (cardId != _getStackPeekCardId())
    {
        return;
    }

    _replaceTopWithCard(cardId, UMS_STACK);
}

void GameController::_handleUndoClick()
{
    if (_isAnimating || !_gameModel)
    {
        return;
    }

    UndoRecord record;
    if (!_undoManager.popRecord(record))
    {
        _gameView->setTipText("No undo history");
        _gameView->setUndoEnabled(false);
        return;
    }

    const int movedCardId = record.movedCardId;
    const int previousTopCardId = record.previousTopCardId;

    auto movedIt = _gameModel->cards.find(movedCardId);
    auto previousTopIt = _gameModel->cards.find(previousTopCardId);
    if (movedIt == _gameModel->cards.end() || previousTopIt == _gameModel->cards.end())
    {
        return;
    }

    _isAnimating = true;

    // Replay reverse movement first, then restore model state in callback.
    _gameView->setCardVisible(movedCardId, true);
    _gameView->moveCardToPosition(movedCardId, record.movedFromPos, [this, record]() {
        CardModel& movedCard = _gameModel->cards[record.movedCardId];
        CardModel& previousTop = _gameModel->cards[record.previousTopCardId];

        previousTop.zone = CZT_TOP;
        _gameModel->topCardId = record.previousTopCardId;

        if (record.source == UMS_PLAYFIELD)
        {
            movedCard.zone = CZT_PLAYFIELD;
            _gameView->setCardVisible(record.movedCardId, true);
            _gameView->setCardPosition(record.movedCardId, record.movedFromPos);
            _gameView->setCardZOrder(record.movedCardId, 10);
        }
        else
        {
            movedCard.zone = CZT_STACK;
            _gameModel->stackDrawIndex = record.previousStackDrawIndex;
            _gameView->setCardPosition(record.movedCardId, _gameView->getStackPeekSlotPosition());
        }

        _gameView->setCardVisible(record.previousTopCardId, true);
        _gameView->setTopCard(record.previousTopCardId);
        _gameView->setStackPeekCard(_getStackPeekCardId());

        _refreshViewState();
        _saveGameProgress();
        _isAnimating = false;
    });
}

bool GameController::_replaceTopWithCard(int cardId, UndoMoveSource source)
{
    if (!_gameModel || !_gameView)
    {
        return false;
    }

    const int previousTopCardId = _gameModel->topCardId;
    if (previousTopCardId < 0 || previousTopCardId == cardId)
    {
        return false;
    }

    // Capture undo snapshot before mutating runtime state.
    UndoRecord record;
    record.movedCardId = cardId;
    record.previousTopCardId = previousTopCardId;
    record.previousStackDrawIndex = _gameModel->stackDrawIndex;
    record.movedFromPos = _gameView->getCardPosition(cardId);
    record.source = source;

    _undoManager.pushRecord(record);

    CardModel& incoming = _gameModel->cards[cardId];
    CardModel& previousTop = _gameModel->cards[previousTopCardId];

    incoming.zone = CZT_TOP;
    previousTop.zone = CZT_HIDDEN;
    _gameModel->topCardId = cardId;

    if (source == UMS_STACK)
    {
        // Consuming stack peek advances draw pointer.
        _gameModel->stackDrawIndex += 1;
    }

    // State is updated first; animation then syncs the view.
    _isAnimating = true;
    _gameView->moveCardToTop(cardId, [this, previousTopCardId, source]() {
        _gameView->setCardVisible(previousTopCardId, false);
        _gameView->setTopCard(_gameModel->topCardId);
        if (source == UMS_STACK)
        {
            _gameView->setStackPeekCard(_getStackPeekCardId());
        }

        _refreshViewState();
        _saveGameProgress();
        _isAnimating = false;
    });

    return true;
}

int GameController::_getStackPeekCardId() const
{
    if (!_gameModel)
    {
        return -1;
    }

    if (_gameModel->stackDrawIndex < 0 || _gameModel->stackDrawIndex >= static_cast<int>(_gameModel->stackCardIds.size()))
    {
        return -1;
    }

    return _gameModel->stackCardIds[_gameModel->stackDrawIndex];
}

void GameController::_refreshViewState()
{
    if (!_gameModel || !_gameView)
    {
        return;
    }

    const auto topIt = _gameModel->cards.find(_gameModel->topCardId);
    if (topIt == _gameModel->cards.end())
    {
        return;
    }

    bool hasMatch = false;
    for (int cardId : _gameModel->playfieldCardIds)
    {
        auto it = _gameModel->cards.find(cardId);
        if (it == _gameModel->cards.end())
        {
            continue;
        }

        // Playfield cards are clickable only when visible and rule-compatible.
        const bool canClick = (it->second.zone == CZT_PLAYFIELD) && CardRuleService::canMatch(it->second.face, topIt->second.face);
        _gameView->setCardClickable(cardId, canClick);
        _gameView->setCardVisible(cardId, it->second.zone == CZT_PLAYFIELD || cardId == _gameModel->topCardId);
        if (canClick)
        {
            hasMatch = true;
        }
    }

    const int stackPeekId = _getStackPeekCardId();
    if (stackPeekId >= 0)
    {
        _gameView->setCardClickable(stackPeekId, true);
    }

    _gameView->setUndoEnabled(_undoManager.canUndo());
    if (hasMatch)
    {
        _gameView->setTipText("Click playfield card (diff 1) or stack card to replace top");
    }
    else if (stackPeekId >= 0)
    {
        _gameView->setTipText("No playfield match, click stack card to replace top");
    }
    else
    {
        _gameView->setTipText("No more stack cards");
    }
}
