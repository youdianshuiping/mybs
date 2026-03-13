#pragma once

#include <functional>
#include <string>

#include "cocos2d.h"
#include "models/CardModel.h"

/**
 * @brief 单张卡牌视图组件。
 *
 * 负责卡牌视觉展示和点击命中转发，可由控制器设置可点击状态与回调。
 */
class CardView : public cocos2d::Node
{
public:
    /**
     * @brief 创建卡牌视图。
     * @param cardId 卡牌唯一 id。
     * @return 创建成功返回 CardView 指针，失败返回 nullptr。
     */
    static CardView* create(int cardId);

    /**
     * @brief 初始化卡牌视图。
     * @param cardId 卡牌唯一 id。
     * @return true 表示初始化成功；false 表示失败。
     */
    bool initWithCardId(int cardId);

    /**
     * @brief 根据点数和花色刷新显示内容。
     * @param face 卡牌点数。
     * @param suit 卡牌花色。
     */
    void updateVisual(CardFaceType face, CardSuitType suit);

    /**
     * @brief 设置点击回调。
     * @param callback 点击后回调，参数为 cardId。
     */
    void setCardClickCallback(const std::function<void(int)>& callback);

    /**
     * @brief 设置是否可点击。
     * @param clickable true 为可点击，false 为不可点击。
     */
    void setClickable(bool clickable);

    /**
     * @brief 获取当前卡牌 id。
     * @return 卡牌唯一 id。
     */
    int getCardId() const;

private:
    void _createRenderNodes();
    void _setupTouchListener();
    bool _applyImageVisual(const std::string& bigNumberPath, const std::string& smallNumberPath, const std::string& suitPath);
    void _applyFallbackVisual(const std::string& faceToken, CardSuitType suit);

    /** 当前绑定的卡牌 id。 */
    int _cardId = -1;
    /** 当前是否允许点击。 */
    bool _clickable = true;

    // Fallback rendering path when image assets are missing.
    cocos2d::DrawNode* _background = nullptr;
    cocos2d::Label* _label = nullptr;

    // Preferred rendering path using card image assets in Resources/res/res.
    cocos2d::Sprite* _cardBgSprite = nullptr;
    cocos2d::Sprite* _bigNumberSprite = nullptr;
    cocos2d::Sprite* _smallNumberSprite = nullptr;
    cocos2d::Sprite* _smallSuitSprite = nullptr;

    /** 点击事件回调。 */
    std::function<void(int)> _clickCallback;
};
