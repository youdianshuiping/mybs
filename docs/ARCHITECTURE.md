# mybs 项目逻辑文档

## 1. 项目定位

mybs 是一个基于 Cocos2d-x 3.17.2 的简化卡牌匹配示例。

- 玩法核心：将“可操作牌”替换到 Top 牌位。
- 牌来源：Playfield（场上牌）与 Stack（牌堆）。
- 规则核心：场上牌只有在与 Top 点数差值为 1 时可点击；牌堆顶牌始终可点击。
- 支持撤销：每次替换 Top 操作都会记录 UndoRecord，并可回滚。

## 2. 架构分层（MVC + Service）

- configs
	- `LevelConfig`：关卡静态数据结构。
	- `LevelConfigLoader`：从 `Resources/levels/level_{id}.json` 加载并解析配置。
- models
	- `GameModel`：运行时全局状态（卡牌容器、Top、Stack 游标等）。
	- `CardModel`：单卡运行时状态（face/suit/source/zone/原始位置）。
	- `UndoRecord`：一次 Top 替换行为的回滚数据。
- services
	- `GameModelFromLevelGenerator`：静态配置转运行时模型。
	- `CardRuleService`：无状态规则服务（当前仅点数差值为 1）。
- managers
	- `UndoManager`：LIFO 撤销栈。
- views
	- `GameView`：渲染区域、卡牌节点、按钮与提示文案；只负责转发交互。
	- `CardView`：单卡可视化与点击命中。
- controllers
	- `GameController`：唯一业务编排点，负责规则校验、模型变更、动画时序、视图刷新。

## 3. 启动与初始化链路

1. `AppDelegate::applicationDidFinishLaunching`
2. 创建并运行 `GameScene`
3. `GameScene::init`
4. 创建 `GameView`
5. 创建 `GameController` 并调用 `startGame(1, gameView)`
6. `startGame` 完成以下动作：
	 - 读取 `level_1.json`
	 - 生成 `GameModel`
	 - 绑定视图回调（场上牌点击 / 牌堆牌点击 / Undo）
	 - 设置初始 Top、Stack Peek、Undo 状态
	 - 执行 `_refreshViewState()`

## 4. 运行时状态模型

`GameModel` 关键字段：

- `cards`：`id -> CardModel` 的全量索引。
- `playfieldCardIds`：场上牌 id 集合。
- `stackCardIds`：牌堆牌 id 集合。
- `topCardId`：当前 Top 牌 id。
- `stackDrawIndex`：当前牌堆可抽位置索引。

状态约束：

- 关卡必须有 Stack 数据，否则加载失败。
- 初始时 `topCardId = stackCardIds.front()`，并把该牌标记为 `CZT_TOP`。
- 初始时 `stackDrawIndex = 1`，表示第 0 张已成为 Top，peek 指向下一张。
- `_isAnimating = true` 期间，所有点击与撤销都会被忽略，防止并发状态写入。

## 5. 核心交互流程

### 5.1 点击场上牌

1. `GameView` 把点击转发给 `GameController::_handlePlayfieldCardClick(cardId)`。
2. 控制器校验：
	 - 当前不在动画中。
	 - 牌存在且处于 `CZT_PLAYFIELD`。
	 - 与当前 Top 满足 `CardRuleService::canMatch`（点数差绝对值为 1）。
3. 校验成功后走统一替换流程 `_replaceTopWithCard(cardId, UMS_PLAYFIELD)`。
4. 校验失败会更新提示文案，不变更模型。

### 5.2 点击牌堆牌

1. `GameView` 只允许当前 peek 牌触发回调。
2. `GameController::_handleStackCardClick(cardId)` 仅接受 `cardId == _getStackPeekCardId()`。
3. 成功后走 `_replaceTopWithCard(cardId, UMS_STACK)`。

### 5.3 统一替换 Top 流程（关键）

`_replaceTopWithCard` 执行顺序：

1. 生成并压入 `UndoRecord`：
	 - `movedCardId`
	 - `previousTopCardId`
	 - `previousStackDrawIndex`
	 - `movedFromPos`
	 - `source`
2. 更新模型：
	 - incoming 牌 `zone -> CZT_TOP`
	 - old top 牌 `zone -> CZT_HIDDEN`
	 - `topCardId = incoming.id`
	 - 若来源为 Stack，则 `stackDrawIndex += 1`
3. 播放动画：incoming 移动到 Top 槽位。
4. 动画完成后刷新：
	 - 隐藏旧 Top
	 - 更新 Top 与 Stack Peek
	 - `_refreshViewState()`

### 5.4 Undo 回滚流程

1. `UndoManager.popRecord` 获取最近一次操作。
2. 将 moved 牌先移动回 `movedFromPos`。
3. 动画回调内恢复模型：
	 - `previousTop.zone = CZT_TOP`
	 - `topCardId = previousTopCardId`
	 - 若来源是 Playfield：moved 牌回到 `CZT_PLAYFIELD`
	 - 若来源是 Stack：moved 牌回到 `CZT_STACK` 且 `stackDrawIndex` 回滚
4. 更新可见性、Top、Stack Peek、提示和 Undo 可用状态。

## 6. 视图刷新策略

`_refreshViewState()` 是界面一致性的核心兜底：

- 遍历 `playfieldCardIds`：
	- 仅当卡牌在 `CZT_PLAYFIELD` 且与 Top 可匹配时可点击。
	- 仅 `CZT_PLAYFIELD`（以及当前 Top）应可见。
- 牌堆 peek（若存在）永远可点击。
- Undo 按钮跟随 `UndoManager.canUndo()`。
- 提示文案优先级：
	1. 有可匹配场上牌
	2. 无匹配但有牌堆可抽
	3. 牌堆为空

## 7. 关卡配置与数据映射

配置文件：`Resources/levels/level_1.json`

- `Playfield[]`：场上牌初始 face/suit/position。
- `Stack[]`：牌堆顺序，`Stack[0]` 会直接成为开局 Top。

生成器会给每张牌分配自增 id：先 Playfield，再 Stack。

## 8. 当前已知边界与限制

- 规则仅支持“点数差 1”，未包含环形规则（如 K 与 A 互通）。
- 没有显式“胜利/失败”终局判定逻辑。
- 撤销仅覆盖“替换 Top”动作。
- 场上牌无覆盖关系或解锁机制（全部按静态位置直接可见）。

## 9. 扩展建议

1. 增加终局判定（清空场上牌即胜利；无匹配且无 Stack 即失败）。
2. 为 `CardRuleService` 增加可配置规则（是否允许 A-K 相邻等）。
3. 在 `GameModel` 中引入覆盖/依赖图，支持“翻牌解锁”玩法。
4. 将槽位坐标、动画时长、提示文案迁移到配置层。
5. 增加序列化（存档/读档）与回放能力。
