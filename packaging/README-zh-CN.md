# 时之笛 Switch 简体中文补丁

> **声明：本项目包含 AI 生成代码。** 相关代码经过项目维护者整理、审阅与测试。

本汉化移植工作特别感谢 [wonderfulnx/Shipwright-CN](https://github.com/wonderfulnx/Shipwright-CN)、[HarbourMasters/Shipwright](https://github.com/HarbourMasters/Shipwright) 以及 [HarbourMasters/Shipwright-Switch](https://github.com/HarbourMasters/Shipwright-Switch) 的无私付出与贡献，让在 Nintendo Switch 上游玩原生中文《塞尔达传说：时之笛》成为可能。

此目录是由个人合法美版 Rev 2 ROM 生成的本地安装包。将整个 `switch/soh/` 目录复制到 SD 卡根目录的 `switch/soh/`，启动 `soh.nro`。`oot.otr` 是按 ROM 提取的原版资源，`soh.otr` 包含中文消息、字库、界面贴图和菜单字体；两者必须来自同一次构建。

首次启动默认简体中文。游戏内语言选择位于存档选项，增强菜单语言选择位于增强设置。已有配置中的 `gLanguages` 会保留原值；中文消息缺失或随机模式动态消息会显示英文。复制或升级前请备份 `sd:/switch/soh/` 下的配置和存档文件。

本包使用 Source Han Sans 简体中文字体子集，授权文本在 `licenses/SourceHanSans-OFL.txt`。字体只用于增强菜单，用户昵称、路径、种子和技术标识不会被翻译。

2026-09-22 r5 修复了美版 Rev 2 缺少 `0xFFFC` 字体顺序表时的回退控制码：命名键盘会完整加载数字、大小写字母和标点，不再出现条纹方块。同时补齐美版 Rev 2 过场动画的通用命令导入；旧版本会在林克家开场资源的 `0x3D` 命令处截断缓冲区，进入新存档后黑屏、长时间假死或崩溃。r5 还补上了“选择游戏模式”和“首领连战设置”标题。该版本已通过 Eden CLI 自动创建/打开存档并进入林克家开场画面。

Switch 默认 A/B 映射已按实体按键交换；已有配置会在首次启动时迁移旧的默认映射。升级时必须完整替换 `switch/soh/`，尤其是 `soh.nro`；不要把旧版 NRO 与本包资源混用。

构建需要 Windows devkitPro/MSYS2、WSL Ubuntu、CMake/Ninja，以及个人 ROM：

```powershell
python scripts/chinese/build_package.py "D:\Download\Legend of Zelda, The - Ocarina of Time (USA) (Rev 2)\Legend of Zelda, The - Ocarina of Time (USA) (Rev 2).z64"
```

脚本会先核验 ROM SHA-1 `41b3bdc48d98c48529219919015a1af22f5057c2`，使用新建构建目录，构建宿主导出器和 Switch Release，生成并读取 OTR，最后写出 `switch/soh/manifest.json` 和 zip。硬件验证仍需检查标题画面、存档、开场剧情、暂停菜单、增强菜单、掌机/底座分辨率、换页和手柄操作。
