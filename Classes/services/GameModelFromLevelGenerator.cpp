#include "services/GameModelFromLevelGenerator.h"

std::unique_ptr<GameModel> GameModelFromLevelGenerator::generate(const LevelConfig& levelConfig)
{
    std::unique_ptr<GameModel> model(new GameModel());
    model->levelId = levelConfig.levelId;

    // 按稳定顺序分配 ID，便于按配置顺序调试。
    int nextId = 1;
    for (const auto& entry : levelConfig.playfield)
    {
        CardModel card;
        card.id = nextId++;
        card.face = entry.face;
        card.suit = entry.suit;
        card.source = CST_PLAYFIELD;
        card.zone = CZT_PLAYFIELD;
        card.originalPosition = entry.position;

        model->playfieldCardIds.push_back(card.id);
        model->cards[card.id] = card;
    }

    // 牌堆卡牌同样会实例化为普通卡牌；运行时通过区域控制可见性与交互。
    for (const auto& entry : levelConfig.stack)
    {
        CardModel card;
        card.id = nextId++;
        card.face = entry.face;
        card.suit = entry.suit;
        card.source = CST_STACK;
        card.zone = CZT_STACK;
        card.originalPosition = entry.position;

        model->stackCardIds.push_back(card.id);
        model->cards[card.id] = card;
    }

    if (!model->stackCardIds.empty())
    {
        // 第一张牌堆卡牌会被消耗为初始顶部牌。
        model->topCardId = model->stackCardIds.front();
        model->cards[model->topCardId].zone = CZT_TOP;
        // 抽牌索引指向下一张可用的牌堆卡牌。
        model->stackDrawIndex = 1;
    }

    return model;
}
