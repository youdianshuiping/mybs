#pragma once

#include <functional>
#include <map>
#include <vector>

#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/CardView.h"

/**
 * GameView 负责绘制棋盘区域与卡牌节点，并发出点击回调。
 */
class GameView : public cocos2d::Node
{
public:
    using CardClickCallback = std::function<void(int)>;

    CREATE_FUNC(GameView);

    bool init() override;

    void setup(const GameModel& model);

    void setOnPlayfieldCardClick(const CardClickCallback& callback);
    void setOnStackCardClick(const CardClickCallback& callback);
    void setOnUndoClick(const std::function<void()>& callback);

    void setTopCard(int cardId);
    void setStackPeekCard(int cardId);
    void setCardVisible(int cardId, bool visible);
    void setCardClickable(int cardId, bool clickable);
    void setCardPosition(int cardId, const cocos2d::Vec2& position);
    void setCardZOrder(int cardId, int zOrder);

    void moveCardToTop(int cardId, const std::function<void()>& onComplete);
    void moveCardToPosition(int cardId, const cocos2d::Vec2& target, const std::function<void()>& onComplete);

    cocos2d::Vec2 getCardPosition(int cardId) const;
    cocos2d::Vec2 getTopSlotPosition() const;
    cocos2d::Vec2 getStackPeekSlotPosition() const;

    void setUndoEnabled(bool enabled);
    void setTipText(const std::string& text);

private:
    void _createRegions();
    void _createUndoButton();
    void _handleCardClicked(int cardId);
    void _layoutStackCards();

    cocos2d::Node* _playfieldRoot = nullptr;
    cocos2d::Node* _stackRoot = nullptr;
    cocos2d::Label* _tipLabel = nullptr;
    cocos2d::MenuItemLabel* _undoItem = nullptr;

    std::map<int, CardView*> _cardViews;
    std::map<int, CardSourceType> _cardSources;
    std::vector<int> _stackCardIds;

    int _topCardId = -1;
    int _stackPeekCardId = -1;

    CardClickCallback _onPlayfieldCardClick;
    CardClickCallback _onStackCardClick;
    std::function<void()> _onUndoClick;
};
