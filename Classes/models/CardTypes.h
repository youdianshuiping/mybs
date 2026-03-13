#pragma once

/**
 * CardSuitType 定义支持的花色。
 */
enum CardSuitType //花色
{
    CST_NONE = -1,
    CST_CLUBS, //梅花
    CST_DIAMONDS, //方块
    CST_HEARTS, //红桃
    CST_SPADES, //黑桃
    CST_NUM_CARD_SUIT_TYPES
};

/**
 * CardFaceType 定义牌面点数。
 */
enum CardFaceType //数值
{
    CFT_NONE = -1,
    CFT_ACE,
    CFT_TWO,
    CFT_THREE,
    CFT_FOUR,
    CFT_FIVE,
    CFT_SIX,
    CFT_SEVEN,
    CFT_EIGHT,
    CFT_NINE,
    CFT_TEN,
    CFT_JACK,
    CFT_QUEEN,
    CFT_KING,
    CFT_NUM_CARD_FACE_TYPES
};

/**
 * CardSourceType 标记卡牌生成来源。
 */
enum CardSourceType //牌的来源
{
    CST_PLAYFIELD, //场上
    CST_STACK //牌堆
};

/**
 * CardZoneType 标记卡牌当前所在区域。
 */
enum CardZoneType //牌当前所处的区域
{
    CZT_PLAYFIELD, //场上
    CZT_STACK, //手牌
    CZT_TOP, //顶部
    CZT_HIDDEN //隐藏（已被替换但动画未完成）
};
