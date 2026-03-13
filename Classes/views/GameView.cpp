#include "views/GameView.h"

using namespace cocos2d;

namespace
{
const float kMoveDuration = 0.18f;
const Vec2 kTopSlotPos(760.0f, 250.0f);
const Vec2 kStackPeekPos(200.0f, 250.0f);
const float kStackFanOffsetX = 34.0f;
const int kMaxVisibleStackCards = 4;
const int kPlayfieldCardZOrder = 10;
const Rect kPlayfieldRect(0.0f, 580.0f, 1080.0f, 1500.0f);
const Rect kStackRect(0.0f, 0.0f, 1080.0f, 580.0f);
}

bool GameView::init()
{
    if (!Node::init())
    {
        return false;
    }

    setContentSize(Size(1080.0f, 2080.0f));

    _createRegions();
    _createUndoButton();

    // _tipLabel = Label::createWithSystemFont("", "Arial", 30);
    // _tipLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
    // _tipLabel->setPosition(Vec2(540.0f, 530.0f));
    // addChild(_tipLabel, 20);

    return true;
}

void GameView::setup(const GameModel& model)
{
    for (auto& pair : _cardViews)
    {
        if (pair.second)
        {
            pair.second->removeFromParent();
        }
    }
    _cardViews.clear();
    _cardSources.clear();

    _stackCardIds = model.stackCardIds;

    for (const auto& pair : model.cards)
    {
        const CardModel& card = pair.second;
        auto view = CardView::create(card.id);
        view->updateVisual(card.face, card.suit);
        view->setCardClickCallback(CC_CALLBACK_1(GameView::_handleCardClicked, this));
        addChild(view, 10);

        if (card.source == CST_PLAYFIELD)
        {
            view->setPosition(card.originalPosition);
            view->setVisible(true);
            view->setLocalZOrder(kPlayfieldCardZOrder);
        }
        else
        {
            view->setPosition(kStackPeekPos);
            view->setVisible(false);
        }

        _cardViews[card.id] = view;
        _cardSources[card.id] = card.source;
    }

    _layoutStackCards();
}

void GameView::setOnPlayfieldCardClick(const CardClickCallback& callback)
{
    _onPlayfieldCardClick = callback;
}

void GameView::setOnStackCardClick(const CardClickCallback& callback)
{
    _onStackCardClick = callback;
}

void GameView::setOnUndoClick(const std::function<void()>& callback)
{
    _onUndoClick = callback;
}

void GameView::setTopCard(int cardId)
{
    _topCardId = cardId;
    auto it = _cardViews.find(cardId);
    if (it == _cardViews.end())
    {
        return;
    }

    it->second->setVisible(true);
    it->second->setPosition(kTopSlotPos);
    // Keep match card below stack cards to make stack-to-match movement feel smoother.
    it->second->setLocalZOrder(14);
    it->second->setClickable(false);

    _layoutStackCards();
}

void GameView::setStackPeekCard(int cardId)
{
    _stackPeekCardId = cardId;
    _layoutStackCards();
}

void GameView::setCardVisible(int cardId, bool visible)
{
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end())
    {
        it->second->setVisible(visible);
    }
}

void GameView::setCardClickable(int cardId, bool clickable)
{
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end())
    {
        it->second->setClickable(clickable);
    }
}

void GameView::setCardPosition(int cardId, const Vec2& position)
{
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end())
    {
        it->second->setPosition(position);
    }
}

void GameView::setCardZOrder(int cardId, int zOrder)
{
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end())
    {
        it->second->setLocalZOrder(zOrder);
    }
}

void GameView::moveCardToTop(int cardId, const std::function<void()>& onComplete)
{
    moveCardToPosition(cardId, kTopSlotPos, onComplete);
}

void GameView::moveCardToPosition(int cardId, const Vec2& target, const std::function<void()>& onComplete)
{
    auto it = _cardViews.find(cardId);
    if (it == _cardViews.end())
    {
        if (onComplete)
        {
            onComplete();
        }
        return;
    }

    it->second->stopAllActions();
    it->second->runAction(Sequence::create(
        MoveTo::create(kMoveDuration, target),
        CallFunc::create([onComplete]() {
            if (onComplete)
            {
                onComplete();
            }
        }),
        nullptr));
}

Vec2 GameView::getCardPosition(int cardId) const
{
    auto it = _cardViews.find(cardId);
    if (it == _cardViews.end())
    {
        return Vec2::ZERO;
    }
    return it->second->getPosition();
}

Vec2 GameView::getTopSlotPosition() const
{
    return kTopSlotPos;
}

Vec2 GameView::getStackPeekSlotPosition() const
{
    return kStackPeekPos;
}

void GameView::setUndoEnabled(bool enabled)
{
    if (_undoItem)
    {
        _undoItem->setEnabled(enabled);
        _undoItem->setOpacity(enabled ? 255 : 120);
    }
}

void GameView::setTipText(const std::string& text)
{
    if (_tipLabel)
    {
        _tipLabel->setString(text);
    }
}

void GameView::_createRegions()
{
    auto playfieldBg = DrawNode::create();
    Vec2 playfield[4] = {
        Vec2(kPlayfieldRect.getMinX(), kPlayfieldRect.getMinY()),
        Vec2(kPlayfieldRect.getMaxX(), kPlayfieldRect.getMinY()),
        Vec2(kPlayfieldRect.getMaxX(), kPlayfieldRect.getMaxY()),
        Vec2(kPlayfieldRect.getMinX(), kPlayfieldRect.getMaxY())
    };
    playfieldBg->drawSolidPoly(playfield, 4, Color4F(0.12f, 0.45f, 0.27f, 1.0f));
    addChild(playfieldBg, 0);

    auto stackBg = DrawNode::create();
    Vec2 stack[4] = {
        Vec2(kStackRect.getMinX(), kStackRect.getMinY()),
        Vec2(kStackRect.getMaxX(), kStackRect.getMinY()),
        Vec2(kStackRect.getMaxX(), kStackRect.getMaxY()),
        Vec2(kStackRect.getMinX(), kStackRect.getMaxY())
    };
    stackBg->drawSolidPoly(stack, 4, Color4F(0.16f, 0.23f, 0.36f, 1.0f));
    addChild(stackBg, -1);

    // auto title = Label::createWithSystemFont("", "Arial", 34);
    // title->setPosition(Vec2(540.0f, 2020.0f));
    // addChild(title, 10);
}

void GameView::_createUndoButton()
{
    auto label = Label::createWithSystemFont("回退", "Arial", 42);
    _undoItem = MenuItemLabel::create(label, [this](Ref*) {
        if (_onUndoClick)
        {
            _onUndoClick();
        }
    });
    _undoItem->setPosition(Vec2(920.0f, 250.0f));

    auto menu = Menu::create(_undoItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 20);
}

void GameView::_handleCardClicked(int cardId)
{
    const auto sourceIt = _cardSources.find(cardId);
    if (sourceIt == _cardSources.end())
    {
        return;
    }

    if (sourceIt->second == CST_PLAYFIELD)
    {
        if (_onPlayfieldCardClick)
        {
            _onPlayfieldCardClick(cardId);
        }
        return;
    }

    if (cardId == _stackPeekCardId && _onStackCardClick)
    {
        _onStackCardClick(cardId);
    }
}

void GameView::_layoutStackCards()
{
    _hideAllStackCardsExceptTop();

    if (_stackPeekCardId < 0 || _stackPeekCardId == _topCardId)
    {
        return;
    }

    const int peekIndex = _findStackPeekIndex();

    if (peekIndex < 0)
    {
        return;
    }

    _layoutVisibleStackCards(peekIndex);
}

void GameView::_hideAllStackCardsExceptTop()
{
    for (int cardId : _stackCardIds)
    {
        auto it = _cardViews.find(cardId);
        if (it == _cardViews.end())
        {
            continue;
        }

        it->second->setClickable(false);
        if (cardId != _topCardId)
        {
            it->second->setVisible(false);
        }
    }
}

int GameView::_findStackPeekIndex() const
{
    for (int index = 0; index < static_cast<int>(_stackCardIds.size()); ++index)
    {
        if (_stackCardIds[index] == _stackPeekCardId)
        {
            return index;
        }
    }

    return -1;
}

void GameView::_layoutVisibleStackCards(int peekIndex)
{
    if (peekIndex < 0 || peekIndex >= static_cast<int>(_stackCardIds.size()))
    {
        return;
    }

    const int remainingCount = static_cast<int>(_stackCardIds.size()) - peekIndex;
    const int visibleCount = std::min(remainingCount, kMaxVisibleStackCards);

    for (int visibleIndex = 0; visibleIndex < visibleCount; ++visibleIndex)
    {
        const int cardId = _stackCardIds[peekIndex + visibleIndex];
        auto it = _cardViews.find(cardId);
        if (it == _cardViews.end())
        {
            continue;
        }

        const float offset = static_cast<float>(visibleIndex) * kStackFanOffsetX;
        it->second->setVisible(true);
        it->second->setPosition(Vec2(kStackPeekPos.x - offset, kStackPeekPos.y));
        it->second->setLocalZOrder(20 + (visibleCount - visibleIndex));
        it->second->setClickable(visibleIndex == 0);
    }
}
