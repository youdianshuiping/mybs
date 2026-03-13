#pragma once

#include <cstdlib>

#include "models/CardTypes.h"

/**
 * CardRuleService 提供无状态的卡牌匹配规则。
 */
class CardRuleService
{
public:
    /**
     * 当两张牌面点数恰好相差 1 时返回 true。
     */
    static bool canMatch(CardFaceType a, CardFaceType b)
    {
        return (std::abs(static_cast<int>(a) - static_cast<int>(b)) == 1)||(a == CFT_ACE && b == CFT_KING) || (a == CFT_KING && b == CFT_ACE);
    }
};
