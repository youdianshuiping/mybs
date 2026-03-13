#include "GameScene.h"

#include "controllers/GameController.h"
#include "views/GameView.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

GameScene::~GameScene() = default;

bool GameScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto gameView = GameView::create();
    if (gameView == nullptr)
    {
        return false;
    }
    addChild(gameView);

    _gameController.reset(new GameController());
    if (!_gameController->startGame(1, gameView))
    {
        return false;
    }

    return true;
}
