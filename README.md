# 亚马逊棋 · Game of the Amazons (Qt)

北京大学《计算概论(A)》2025 年秋季大作业：10×10 亚马逊棋，Qt Widgets 图形界面，支持人机对弈 / 双人对弈、悔棋、存档与读档。电脑对手为 **minimax + alpha-beta 剪枝**，评估函数按 QueenMove / KingMove 领地计算。

## 规则

- 双方各 4 枚棋子（Amazon），黑方在上、白方在下，白方先手。
- 每回合先像国际象棋的皇后一样移动一枚棋子，再由该棋子以同样的走法射出一支箭；箭落点永久成为障碍。
- 轮到某方时无子可动，即判负。

## 目录

| 文件 | 内容 |
|---|---|
| `Amazon_AI/logic.h / logic.cpp` | 走法合法性、走法生成（`getAllMoves`）、是否有子可动 |
| `Amazon_AI/ai.h / ai.cpp` | 搜索（minimax + alpha-beta、迭代加深、候选裁剪）与评估函数；旧的一层贪心保留为 `greedyMove` 作对照 |
| `Amazon_AI/mainwindow.*` | Qt 界面：落子、射箭、悔棋、存读档、调用 AI |
| `tests/selfplay.cpp` | 不依赖 Qt 的检查：走法还原、一步陷阱、与贪心版自对弈 |

## AI 设计

**搜索** `searchBestMove()`
1. 第 1 层：对当前所有合法走法逐一走出并用完整评估函数打分，得到深度 1 的答案，同时作为更深搜索的走法排序。
2. 第 2 层起：minimax + alpha-beta 剪枝，迭代加深，时间预算 1.5 s、最多 6 层；某一层没在预算内搜完就作废，沿用上一层的结果。
3. 候选裁剪：亚马逊棋开局分支因子约 2000，全宽搜索两层以上不可行。每个结点先把走法按「己方机动性 − 对方机动性」排序，只展开前 40 / 24 / 16 / 12 / 8 个（按层数递减）。
4. 无子可动即为终局，胜负值按层数微调，让更早的胜利优先。

**评估** `evaluatePosition()`，从根方视角
- 用 BFS 分别算双方到每个空格的 QueenMove 距离（一步可走任意格）与 KingMove 距离（一步只走一格）；某个空格谁先到就归谁，同距离不计。
- 分值 = 10 × 皇后领地差 + 3 × 国王领地差 + 机动性差。
- 参考：Lieberum, *An evaluation function for the game of Amazons*, Theoretical Computer Science, 2005。

**对照结果**（`tests/selfplay.cpp`，Apple Silicon，`-O2`）
- 开局 300 ms 内搜到第 3 层（约 3,800 个局面）。
- 与课程原版的一层贪心（只看机动性）自对弈，随机开局两步、双方轮流执黑白：**10 局 10 胜**（300 ms/步，平均深度 3.9，约 16,000 局面/步）。

## 构建与运行

- Qt 6（qmake）：`qmake Amazon_AI/Amazon_AI.pro && make`；Windows 下用 Qt Creator 打开 `Amazon_AI/Amazon_AI.pro` 直接构建。
- 自对弈检查（无需 Qt）：

```sh
clang++ -O2 -std=c++17 -I Amazon_AI tests/selfplay.cpp Amazon_AI/ai.cpp Amazon_AI/logic.cpp -o selfplay
./selfplay 10 300      # 局数、每步毫秒
```

## 版本

- 2025-11：课程提交版本，电脑对手为一层贪心（机动性打分）。
- 2026-09：改为 minimax + alpha-beta 剪枝与领地评估，新增自对弈测试；构建产物移出版本库。

---

**English.** Course project for *Introduction to Computation (A)*, Peking University, autumn 2025: the Game of the Amazons with a Qt GUI (human vs computer, two players, undo, save/load). The computer opponent uses minimax with alpha-beta pruning, iterative deepening under a 1.5 s budget, and per-ply candidate pruning ordered by mobility; positions are evaluated by queen-move and king-move territory (BFS distance maps) plus mobility. Against the original one-ply greedy opponent it scores 10–0 in self-play at 300 ms per move (average depth 3.9). `tests/selfplay.cpp` reproduces these numbers without Qt.
