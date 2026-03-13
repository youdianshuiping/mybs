#pragma once

#include <memory>

#include "cocos2d.h"
#include "controllers/GameController.h"

/**
 * GameScene 是简易卡牌匹配示例的应用入口场景。
 */
class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    ~GameScene() override;

    bool init() override;

    CREATE_FUNC(GameScene);

private:
    std::unique_ptr<GameController> _gameController;
};
