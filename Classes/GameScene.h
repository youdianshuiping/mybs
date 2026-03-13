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
    /**
     * @brief 创建场景实例。
     * @return 场景对象指针。
     */
    static cocos2d::Scene* createScene();

    /** 析构并释放控制器资源。 */
    ~GameScene() override;

    /**
     * @brief 初始化场景内容。
     * @return true 表示初始化成功；false 表示失败。
     */
    bool init() override;

    CREATE_FUNC(GameScene);

private:
    /** 场景对应的业务控制器。 */
    std::unique_ptr<GameController> _gameController;
};
