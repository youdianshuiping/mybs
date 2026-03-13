#pragma once

#include <cstdlib>

#include "models/CardTypes.h"

/**
 * @brief 卡牌规则服务。
 *
 * 提供无状态规则判断能力，不持有模型数据生命周期，
 * 适用于控制器在交互过程中进行即时规则校验。
 */
class CardRuleService
{
public:
    /**
     * @brief 判断两张牌是否可匹配。
     * @param a 第一张牌面点数。
     * @param b 第二张牌面点数。
     * @return true 表示可匹配；false 表示不可匹配。
     */
    static bool canMatch(CardFaceType a, CardFaceType b)
    {
        return (std::abs(static_cast<int>(a) - static_cast<int>(b)) == 1)||(a == CFT_ACE && b == CFT_KING) || (a == CFT_KING && b == CFT_ACE);
    }
};
