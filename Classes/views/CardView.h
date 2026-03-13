#pragma once

#include <functional>

#include "cocos2d.h"
#include "models/CardModel.h"

/**
 * CardView renders one card and forwards click events.
 */
class CardView : public cocos2d::Node
{
public:
    static CardView* create(int cardId);

    bool initWithCardId(int cardId);

    void updateVisual(CardFaceType face, CardSuitType suit);
    void setCardClickCallback(const std::function<void(int)>& callback);
    void setClickable(bool clickable);

    int getCardId() const;

private:
    int _cardId = -1;
    bool _clickable = true;

    // Fallback rendering path when image assets are missing.
    cocos2d::DrawNode* _background = nullptr;
    cocos2d::Label* _label = nullptr;

    // Preferred rendering path using card image assets in Resources/res/res.
    cocos2d::Sprite* _cardBgSprite = nullptr;
    cocos2d::Sprite* _bigNumberSprite = nullptr;
    cocos2d::Sprite* _smallNumberSprite = nullptr;
    cocos2d::Sprite* _smallSuitSprite = nullptr;

    std::function<void(int)> _clickCallback;
};
