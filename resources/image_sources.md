# Image Sources

本批卡牌和敌人图由 image2 生成一张 5x3 美术表后裁切得到。后续如果要替换图片，覆盖对应 `resources/cards/` 或 `resources/enemies/` 文件即可，`GameText.h` 里的路径不用变。

原始美术表保存在：

`resources/image2_card_enemy_sheet.png`

裁切预览保存在：

`build-codex-verify/image2_assets_contact.jpg`

| 文件 | 用途 |
| --- | --- |
| `resources/cards/card_strike_homework_battle.png` | 与作业战斗！ |
| `resources/cards/card_defend_video_break.png` | 刷会儿b站 |
| `resources/cards/card_bash_seen_problem.png` | 这题我见过！ |
| `resources/cards/card_inflame_idea_open.png` | 思路打开 |
| `resources/cards/card_heavy_blade_flow.png` | 这就是心流？ |
| `resources/cards/card_pommel_i_get_it.png` | 我觉得我会了 |
| `resources/cards/card_shrug_game_break.png` | 原神，启动！ |
| `resources/cards/card_anger_flash_inspiration.png` | 灵感，稍纵即逝！ |
| `resources/cards/card_flex_energy_bar.png` | 来根士力架！ |
| `resources/cards/card_cleave_tonight_clear.png` | 今晚必拿下 |
| `resources/enemies/enemy_gaoshu_homework.png` | 高数作业 |
| `resources/enemies/enemy_poj_problem.png` | POJ习题 |
| `resources/enemies/enemy_easy_course_slime.png` | 水课作业 |
| `resources/enemies/enemy_group_presentation.png` | 小组汇报 |
| `resources/enemies/enemy_final_exam.png` | 期末考试 |
| `resources/cards/genshin_impact_wordmark.png` | 原神卡牌备用 logo 资源 |
| `resources/events/library_night.png` | 事件：图书馆深夜 |
| `resources/events/dorm_power_outage.png` | 事件：宿舍停电 |
| `resources/events/science_building_elevator.png` | 事件：理科楼电梯 |
| `resources/events/weiming_lake_walk.png` | 事件：未名湖夜行 |
| `resources/events/student_affairs_new_task.png` | 事件：学工群新锅 |
| `resources/events/open_day.png` | 事件：开放日 |
| `resources/events/ideology_pre.png` | 事件：思政课 Pre |

## 事件图约定

新增事件时，事件 id 使用 `snake_case`，事件图保存为 `resources/events/<event_id>.png`。`EventLibrary::makeEvent()` 会在未手动填写 `imagePath` 时自动使用这个路径。后续新增事件时同步用 image2 生成对应事件图，质量优先，可以按事件气质自由选择写实、玄幻或喜剧化风格。
