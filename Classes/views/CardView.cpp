#include "views/CardView.h"

#include <array>

using namespace cocos2d;

namespace
{
const Size kCardSize(150.0f, 210.0f);
const char* kCardBgPath = "res/res/card_general.png";

const char* faceToText(CardFaceType face)
{
    static const std::array<const char*, CFT_NUM_CARD_FACE_TYPES> kFaces = {
        "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"
    };
    if (face < CFT_ACE || face >= CFT_NUM_CARD_FACE_TYPES)
    {
        return "?";
    }
    return kFaces[static_cast<int>(face)];
}

const char* suitToText(CardSuitType suit)
{
    switch (suit)
    {
    case CST_CLUBS:
        return "C";
    case CST_DIAMONDS:
        return "D";
    case CST_HEARTS:
        return "H";
    case CST_SPADES:
        return "S";
    default:
        return "?";
    }
}

const char* suitToToken(CardSuitType suit)
{
    switch (suit)
    {
    case CST_CLUBS:
        return "club";
    case CST_DIAMONDS:
        return "diamond";
    case CST_HEARTS:
        return "heart";
    case CST_SPADES:
        return "spade";
    default:
        return "club";
    }
}

const char* suitToColorToken(CardSuitType suit)
{
    return (suit == CST_DIAMONDS || suit == CST_HEARTS) ? "red" : "black";
}

Color4B suitToColor(CardSuitType suit)
{
    if (suit == CST_DIAMONDS || suit == CST_HEARTS)
    {
        return Color4B(217, 51, 51, 255);
    }
    return Color4B(38, 38, 38, 255);
}

bool setSpriteTextureIfExists(Sprite* sprite, const std::string& path)
{
    if (sprite == nullptr)
    {
        return false;
    }

    if (!FileUtils::getInstance()->isFileExist(path))
    {
        return false;
    }

    sprite->setTexture(path);
    sprite->setVisible(true);
    return true;
}
}

CardView* CardView::create(int cardId)
{
    CardView* ret = new (std::nothrow) CardView();
    if (ret != nullptr && ret->initWithCardId(cardId))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool CardView::initWithCardId(int cardId)
{
    if (!Node::init())
    {
        return false;
    }

    _cardId = cardId;
    setContentSize(kCardSize);
    setAnchorPoint(Vec2(0.5f, 0.5f));

    _createRenderNodes();
    _setupTouchListener();

    return true;
}

void CardView::_createRenderNodes()
{
    _background = DrawNode::create();
    addChild(_background);

    _label = Label::createWithSystemFont("", "Arial", 44);
    _label->setPosition(Vec2(kCardSize.width * 0.5f, kCardSize.height * 0.5f));
    addChild(_label, 1);

    _cardBgSprite = Sprite::create();
    _cardBgSprite->setPosition(Vec2::ZERO);
    _cardBgSprite->setVisible(false);
    addChild(_cardBgSprite, 0);

    _bigNumberSprite = Sprite::create();
    _bigNumberSprite->setPosition(Vec2::ZERO);
    _bigNumberSprite->setVisible(false);
    addChild(_bigNumberSprite, 1);

    _smallNumberSprite = Sprite::create();
    _smallNumberSprite->setPosition(Vec2(-kCardSize.width * 0.28f, kCardSize.height * 0.33f));
    _smallNumberSprite->setVisible(false);
    addChild(_smallNumberSprite, 1);

    _smallSuitSprite = Sprite::create();
    _smallSuitSprite->setPosition(Vec2(kCardSize.width * 0.30f, kCardSize.height * 0.33f));
    _smallSuitSprite->setVisible(false);
    addChild(_smallSuitSprite, 1);
}

void CardView::_setupTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    listener->onTouchBegan = [this](Touch* touch, Event*) {
        if (!_clickable)
        {
            return false;
        }

        const Vec2 local = this->convertToNodeSpace(touch->getLocation());
        Rect rect(0.0f, 0.0f, getContentSize().width, getContentSize().height);
        rect.origin = Vec2(-getContentSize().width * 0.5f, -getContentSize().height * 0.5f);
        return rect.containsPoint(local);
    };
    listener->onTouchEnded = [this](Touch*, Event*) {
        if (_clickCallback)
        {
            _clickCallback(_cardId);
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CardView::updateVisual(CardFaceType face, CardSuitType suit)
{
    const std::string faceToken = faceToText(face);
    const std::string colorToken = suitToColorToken(suit);
    const std::string bigNumberPath = StringUtils::format("res/res/number/big_%s_%s.png", colorToken.c_str(), faceToken.c_str());
    const std::string smallNumberPath = StringUtils::format("res/res/number/small_%s_%s.png", colorToken.c_str(), faceToken.c_str());
    const std::string suitPath = StringUtils::format("res/res/suits/%s.png", suitToToken(suit));

    if (_applyImageVisual(bigNumberPath, smallNumberPath, suitPath))
    {
        return;
    }

    _applyFallbackVisual(faceToken, suit);
}

bool CardView::_applyImageVisual(const std::string& bigNumberPath, const std::string& smallNumberPath, const std::string& suitPath)
{
    const bool hasImageAssets =
        setSpriteTextureIfExists(_cardBgSprite, kCardBgPath) &&
        setSpriteTextureIfExists(_bigNumberSprite, bigNumberPath) &&
        setSpriteTextureIfExists(_smallNumberSprite, smallNumberPath) &&
        setSpriteTextureIfExists(_smallSuitSprite, suitPath);

    if (!hasImageAssets)
    {
        return false;
    }

    _background->clear();
    _label->setString("");

    const Size bgSize = _cardBgSprite->getContentSize();
    if (bgSize.width > 0.0f && bgSize.height > 0.0f)
    {
        _cardBgSprite->setScaleX(kCardSize.width / bgSize.width);
        _cardBgSprite->setScaleY(kCardSize.height / bgSize.height);
    }

    _bigNumberSprite->setScale(0.45f);
    _smallNumberSprite->setScale(0.6f);
    _smallSuitSprite->setScale(0.7f);
    return true;
}

void CardView::_applyFallbackVisual(const std::string& faceToken, CardSuitType suit)
{
    _cardBgSprite->setVisible(false);
    _bigNumberSprite->setVisible(false);
    _smallNumberSprite->setVisible(false);
    _smallSuitSprite->setVisible(false);

    _background->clear();
    Vec2 rect[4] = {
        Vec2(-kCardSize.width * 0.5f, -kCardSize.height * 0.5f),
        Vec2(kCardSize.width * 0.5f, -kCardSize.height * 0.5f),
        Vec2(kCardSize.width * 0.5f, kCardSize.height * 0.5f),
        Vec2(-kCardSize.width * 0.5f, kCardSize.height * 0.5f)
    };

    _background->drawSolidPoly(rect, 4, Color4F(0.97f, 0.97f, 0.97f, 1.0f));
    _background->drawPoly(rect, 4, true, Color4F(0.2f, 0.2f, 0.2f, 1.0f));

    _label->setString(StringUtils::format("%s%s", suitToText(suit), faceToken.c_str()));
    _label->setTextColor(suitToColor(suit));
}

void CardView::setCardClickCallback(const std::function<void(int)>& callback)
{
    _clickCallback = callback;
}

void CardView::setClickable(bool clickable)
{
    _clickable = clickable;
    setOpacity(clickable ? 255 : 215);
}

int CardView::getCardId() const
{
    return _cardId;
}
