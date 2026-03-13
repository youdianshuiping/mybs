#pragma once

#include <functional>
#include <map>
#include <vector>

#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/CardView.h"

/**
 * @brief 游戏主视图。
 *
 * 负责区域背景、卡牌节点、撤销按钮等 UI 展示，
 * 并将用户操作通过回调接口上抛给控制器。
 */
class GameView : public cocos2d::Node
{
public:
    using CardClickCallback = std::function<void(int)>;

    CREATE_FUNC(GameView);

    /**
     * @brief 初始化视图节点。
     * @return true 表示初始化成功；false 表示失败。
     */
    bool init() override;

    /**
     * @brief 根据模型创建并布局卡牌视图。
     * @param model 运行时模型快照。
     */
    void setup(const GameModel& model);

    /** 设置场上牌点击回调。 */
    void setOnPlayfieldCardClick(const CardClickCallback& callback);
    /** 设置牌堆牌点击回调。 */
    void setOnStackCardClick(const CardClickCallback& callback);
    /** 设置撤销按钮点击回调。 */
    void setOnUndoClick(const std::function<void()>& callback);

    /** 设置顶部牌 id 并更新位置。 */
    void setTopCard(int cardId);
    /** 设置牌堆可抽牌（peek）id。 */
    void setStackPeekCard(int cardId);
    /** 设置卡牌可见性。 */
    void setCardVisible(int cardId, bool visible);
    /** 设置卡牌可点击状态。 */
    void setCardClickable(int cardId, bool clickable);
    /** 设置卡牌坐标。 */
    void setCardPosition(int cardId, const cocos2d::Vec2& position);
    /** 设置卡牌渲染层级。 */
    void setCardZOrder(int cardId, int zOrder);

    /** 将卡牌移动到顶部槽位。 */
    void moveCardToTop(int cardId, const std::function<void()>& onComplete);
    /** 将卡牌移动到指定位置。 */
    void moveCardToPosition(int cardId, const cocos2d::Vec2& target, const std::function<void()>& onComplete);

    /** 获取指定卡牌当前位置。 */
    cocos2d::Vec2 getCardPosition(int cardId) const;
    /** 获取顶部槽位坐标。 */
    cocos2d::Vec2 getTopSlotPosition() const;
    /** 获取牌堆 peek 槽位坐标。 */
    cocos2d::Vec2 getStackPeekSlotPosition() const;

    /** 设置撤销按钮可用状态。 */
    void setUndoEnabled(bool enabled);
    /** 设置底部提示文案。 */
    void setTipText(const std::string& text);

private:
    void _createRegions();
    void _createUndoButton();
    void _handleCardClicked(int cardId);
    void _layoutStackCards();
    void _hideAllStackCardsExceptTop();
    int _findStackPeekIndex() const;
    void _layoutVisibleStackCards(int peekIndex);

    /** 场上区域根节点。 */
    cocos2d::Node* _playfieldRoot = nullptr;
    /** 牌堆区域根节点。 */
    cocos2d::Node* _stackRoot = nullptr;
    /** 提示文本节点。 */
    cocos2d::Label* _tipLabel = nullptr;
    /** 撤销按钮节点。 */
    cocos2d::MenuItemLabel* _undoItem = nullptr;

    /** 所有卡牌视图索引：cardId -> CardView。 */
    std::map<int, CardView*> _cardViews;
    /** 卡牌来源索引：cardId -> 来源区域。 */
    std::map<int, CardSourceType> _cardSources;
    /** 牌堆卡牌 id 顺序。 */
    std::vector<int> _stackCardIds;

    /** 当前顶部牌 id。 */
    int _topCardId = -1;
    /** 当前牌堆可抽牌 id。 */
    int _stackPeekCardId = -1;

    /** 场上牌点击回调。 */
    CardClickCallback _onPlayfieldCardClick;
    /** 牌堆牌点击回调。 */
    CardClickCallback _onStackCardClick;
    /** 撤销按钮点击回调。 */
    std::function<void()> _onUndoClick;
};
