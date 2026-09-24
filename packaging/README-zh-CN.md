# 时之笛 Switch 简体中文移植：构建与安装

本项目基于 Shipwright-Switch `2976c182`，移植 Shipwright-CN 的美版 Rev 2 中文实现，并修复 Switch 使用中的输入、字体和新游戏开场问题。此文档说明如何使用个人 ROM 生成完整安装目录。

## 准备

- Windows 10/11、devkitPro/MSYS2、WSL Ubuntu、CMake、Ninja 和 Python 3。
- 自己合法拥有的《时之笛》美版 Rev 2 `.z64` ROM。构建脚本只接受 SHA-1 `41b3bdc48d98c48529219919015a1af22f5057c2`；不会上传或复制 ROM 文件。
- 通过递归克隆获取固定的 Git 子模块：

```powershell
git clone --recurse-submodules https://github.com/Electr0Whale/Shipwright-Switch-CN.git
cd Shipwright-Switch-CN
```

## 生成本机安装包

```powershell
python scripts/chinese/build_package.py "D:\path\to\Legend of Zelda, The - Ocarina of Time (USA) (Rev 2).z64"
```

脚本会在新的构建目录中配置并构建宿主资源工具和 Switch Release，运行中文资源检查，导出并检查 OTR，最后生成 `switch/soh/` 安装目录。构建日志和校验结果保存在输出目录的 `checks/` 中。完整安装目录包含 `soh.nro`、同次构建生成的 `oot.otr` / `soh.otr`、菜单字体和授权文本；这些本机资源包不要上传到公开 Release，因为其中包含从 ROM 导出的游戏资源。

## 安装与升级

备份 `sd:/switch/soh/` 中的设置和存档，然后将本机生成的 `switch/soh/` 目录完整复制到 SD 卡根目录，覆盖旧文件。使用 Atmosphere 的 `Game+R` 启动。必须配套使用同一次构建生成的 NRO 和 OTR，不要只替换程序文件。

首次启动默认简体中文。已有配置中的 `gLanguages` 选择会保留；可从存档选项切换游戏语言。没有中文译文的消息，以及随机模式的动态消息，会安全显示为英文。

## 高清中文字体

本程序兼容 [Shipwright-CN 发布页](https://github.com/wonderfulnx/Shipwright-CN/releases/)提供的 `chinese_font_hd.o2r.zip`：

1. 解压下载的 ZIP，取出里面的 `chinese_font_hd.o2r`。
2. 将该文件放到 SD 卡根目录的 `mods/`，例如 `sd:/mods/chinese_font_hd.o2r`。
3. 在增强设置中开启“使用替代资源”（Use Alternate Assets）。

本项目不重新分发高清字体包。关闭高清字体或删除该 mod 时，游戏继续使用安装包内的标准中文字库。

## Switch 操作

增强菜单的实体 A 为确认，B 为返回，X 将焦点切换到菜单栏；方向键或左摇杆移动。实体减号键显示或隐藏增强菜单栏。游戏内默认简体中文，游戏按键仍可在原设置界面调整。

## 已包含的移植与修复

- 简体中文剧情消息、双字节解码、中文游戏字形、标题/存档/暂停菜单贴图、物品和地点等界面文字。
- 增强设置、控制器配置、随机模式、追踪器、调试工具的中文菜单；配置键、序列化值和 ImGui 控件 ID 保持兼容。
- 中文菜单字体及 Source Han Sans OFL 授权文本；用户昵称、文件路径、种子和技术标识保留用户输入。
- 美版 Rev 2 命名键盘字符回退、创建存档后开场过场资源导入、Switch A/B 与减号菜单输入、菜单字体加载路径等修复。
- 高清中文字库按资源路径直接进入纹理绘制，避免把高分辨率像素写进固定大小的字形缓冲区。

## 验证

资源脚本会检查消息表、控制码、字形和贴图引用。当前 Switch Release 构建以及 Eden CLI 开场对白检查通过；标准和高清中文字库均能显示。真实 Switch 的掌机/底座显示与性能仍需用户实机验证。首次游玩前请备份存档；升级时保留原存档文件。
