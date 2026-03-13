# mybs 代码规范巡检清单（Classes）

本清单基于 `Classes/` 目录自动扫描 + 人工复核，按 `docs/DEVELOPMENT_STANDARDS.md` 执行。

## 1. 巡检范围

- 扫描文件数：26（`.h/.cpp`）
- 目录范围：`Classes/`（不含 `cocos2d/` 第三方引擎代码）

## 2. 总体结论

- 类体量：未发现超过 500 行的类（通过文件规模和类定义复核，暂无超限）。
- 命名：类名/文件名整体符合首字母大写；函数与变量大多符合小驼峰。
- 私有成员前缀：业务类基本符合 `_` 前缀要求。
- 主要缺口：
  - 公共方法注释覆盖不足。
  - 成员变量注释覆盖不足。
  - 存在超过 50 行函数，需拆分重构。

## 3. P0（优先整改）

### 3.1 超过 50 行函数

1. `Classes/controllers/GameController.cpp`
   - `_handleUndoClick`：约 59 行（起始约第 173 行）
   - `_replaceTopWithCard`：约 53 行（起始约第 233 行）
2. `Classes/views/CardView.cpp`
   - `initWithCardId`：约 61 行（起始约第 103 行）
   - `updateVisual`：约 51 行（起始约第 165 行）
3. `Classes/views/GameView.cpp`
   - `_layoutStackCards`：约 52 行（起始约第 285 行）

整改建议：

- 将“参数校验/模型更新/动画回调/视图同步”拆成私有小函数。
- 按“单一职责 + 可测试性”拆分，避免一个函数同时处理状态变更和 UI 更新。

## 4. P1（高优先级）

### 4.1 类注释缺失

1. `Classes/HelloWorldScene.h`：`HelloWorld` 类声明处缺少规范化类注释。

### 4.2 公共方法注释不足（抽样）

1. `Classes/controllers/GameController.h`
   - `clearSave`
   - `startNewGame`
   - `startGame`
2. `Classes/views/GameView.h`
   - `setup`
   - `setTopCard`
   - `moveCardToTop`
   - `setTipText`
3. `Classes/views/CardView.h`
   - `create`
   - `initWithCardId`
   - `updateVisual`
4. `Classes/managers/UndoManager.h`
   - `clear`
   - `pushRecord`
   - `popRecord`

说明：自动扫描统计显示公共方法注释缺口较多（约 45 处），建议分模块补齐。

### 4.3 成员变量注释不足

1. `Classes/views/GameView.h` 多个成员未注明用途。
2. `Classes/models/GameModel.h` 与 `Classes/models/CardModel.h` 字段注释不足。

说明：自动扫描统计显示成员注释缺口较多（约 80+），建议优先补充头文件中的结构体/类字段注释。

## 5. 分层职责检查

### 5.1 models 层

- 现状：`CardModel`、`GameModel` 已具备序列化/反序列化能力。
- 结论：符合“数据层 + 支持存档恢复”的要求。

### 5.2 views 层

- 现状：`GameView`、`CardView` 负责 UI 展示与回调转发。
- 结论：符合“视图层 + 回调交互”要求。

### 5.3 controllers 层

- 现状：`GameController` 负责协同 model/view 与业务流程。
- 结论：职责基本符合。

### 5.4 managers 层

- 现状：`UndoManager` 作为 controller 成员使用；`SaveGameManager` 为静态工具式管理。
- 风险：`SaveGameManager` 更接近 service 风格，后续可评估迁移到 services 层以增强语义一致性。

### 5.5 services 层

- 现状：`CardRuleService`、`GameModelFromLevelGenerator` 为无状态处理。
- 结论：符合“处理逻辑，不持有生命周期数据”的要求。

## 6. 推荐整改顺序

1. 先拆分 5 个超长函数（P0）。
2. 补齐 `HelloWorld` 类注释（P1）。
3. 按模块批量补齐头文件中公共方法与成员变量注释（P1）。
4. 评估 `SaveGameManager` 是否迁移为 `services/SaveGameService`（P2，可选）。

## 7. 备注

- 本轮自动扫描对 C++ 语法做了近似匹配，可能存在少量误报；行号已对关键问题做人工复核。