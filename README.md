# 时之笛 Switch 简体中文移植

基于 [HarbourMasters/Shipwright-Switch](https://github.com/HarbourMasters/Shipwright-Switch) 的 Nintendo Switch 简体中文移植。项目加入游戏内中文、简体中文字体、增强菜单翻译和适配 Switch 的操作修正。

本仓库及公开 Release 不包含游戏 ROM，也不发布从 ROM 提取的 `oot.otr` / `soh.otr`。请使用自己合法拥有的美版 Rev 2 ROM，在本机生成资源包。Release 提供程序文件和源码；完整安装目录由构建脚本在本机生成。

## 下载与安装

查看[最新发布](https://github.com/Electr0Whale/Shipwright-Switch-CN/releases/latest)。构建完整安装目录需要 Windows、devkitPro/MSYS2、WSL Ubuntu、CMake/Ninja 和个人 ROM。按[中文构建与安装说明](packaging/README-zh-CN.md)操作，完成后将生成的 `switch/soh/` 复制到 SD 卡根目录。升级前请备份存档和设置。

构建脚本只接受 SHA-1 为 `41b3bdc48d98c48529219919015a1af22f5057c2` 的美版 Rev 2 ROM；ROM 文件本身不会被复制进仓库或安装目录。脚本会生成配套的 `oot.otr`、`soh.otr`、`soh.nro`、菜单字体和校验清单。

首次启动默认简体中文。已有配置中的语言选择会保留；缺少中文翻译的消息和随机模式动态对话会回退到英文。可在存档选项切换游戏语言。

## 当前功能与修复

- 移植中文消息表、双字节消息解码与字形加载，覆盖剧情对白、标题和存档界面、暂停菜单、物品、地点、首领名称及结尾内容。
- 加入约 2,180 个中文游戏字形，以及菜单所需的简体中文字体和界面贴图；字体授权文件随本机安装包提供。
- 翻译增强设置、控制器配置、随机模式设置、追踪器和调试工具菜单；配置键、保存值和控件 ID 保持兼容，用户昵称、文件路径、种子和技术标识保留原样。
- 修复美版 Rev 2 命名键盘的字体顺序回退，避免数字、字母和标点显示为条纹方块。
- 修复存档创建后进入新游戏时的过场资源导入问题，避免在开场黑屏、假死或崩溃。
- 修复 Switch 增强菜单操作：实体 A 确认、B 返回、X 切换菜单栏焦点，方向键或左摇杆移动，减号键显示或隐藏菜单栏。
- 修复增强菜单中文字体在 Switch / Eden 启动目录变化时无法加载的问题。
- 支持 Shipwright-CN 发布的高清中文字库：将 `chinese_font_hd.o2r` 放到 SD 卡根目录的 `mods/`，并开启“使用替代资源”。高清字体文件由用户从[原仓库发布页](https://github.com/wonderfulnx/Shipwright-CN/releases/)获取，本项目不重新分发。

## 构建

使用递归克隆以取回固定版本的子模块：

```powershell
git clone --recurse-submodules https://github.com/Electr0Whale/Shipwright-Switch-CN.git
cd Shipwright-Switch-CN
python scripts/chinese/build_package.py "D:\path\to\Legend of Zelda, The - Ocarina of Time (USA) (Rev 2).z64"
```

脚本创建新的宿主及 Switch Release 构建目录，校验 ROM，运行中文资源检查，生成配套 OTR 和 NRO，并输出本机安装包、源码归档及构建日志。完整要求、手动安装步骤和存档备份建议见[构建说明](packaging/README-zh-CN.md)。

当前移植基线记录在 [`packaging/BASELINES.json`](packaging/BASELINES.json)：Switch 主线 `2976c182`、Shipwright-CN `9749ffb8`、PC 主仓库 `2bc50f60`；资源导出器、ZAPDTR 和 libultraship 子模块版本也一并固定。

## 验证状态

中文资源检查、Switch Release 构建和 Eden CLI 开场对白验证已通过。高清字库开启时，对话可以正常显示；未安装高清字库时，普通中文字库也能正常显示。真实 Switch 上的掌机/底座画面、长时间运行和性能仍需实机确认。

## 致谢与授权

感谢 [wonderfulnx/Shipwright-CN](https://github.com/wonderfulnx/Shipwright-CN)、[HarbourMasters/Shipwright](https://github.com/HarbourMasters/Shipwright) 和 [HarbourMasters/Shipwright-Switch](https://github.com/HarbourMasters/Shipwright-Switch) 的工作。

菜单字体使用 Source Han Sans 子集，随本机安装包提供 OFL 授权文本。代码和各上游组件沿用其各自许可证。仓库包含 AI 辅助生成的代码；相关改动已由维护者整理并纳入构建及模拟器检查。
