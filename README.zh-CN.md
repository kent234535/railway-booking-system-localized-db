<div align="center">

# 🚄 铁路售票系统

### 基于 Qt6 C++ 和 SQLite 的桌面应用

一个智能铁路售票桌面系统，支持 Dijkstra 最短路径路线规划、实时座位管理和原生 Qt Widgets 图形界面

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Qt](https://img.shields.io/badge/Qt-6.9.1-green.svg)](https://www.qt.io/)
[![SQLite](https://img.shields.io/badge/Database-SQLite-003B57?logo=sqlite&logoColor=white)](https://www.sqlite.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows&logoColor=white)](https://github.com/kent234535/railway-booking-system-localized-db)

[功能特性](#功能特性) · [技术栈](#技术栈) · [项目结构](#项目结构) · [快速开始](#快速开始) · [使用指南](#使用指南) · [数据库](#数据库架构) · [许可证](#许可证)

[![ENGLISH](https://img.shields.io/badge/ENGLISH-gray?style=for-the-badge)](README.md)
[![简体中文](https://img.shields.io/badge/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-blue?style=for-the-badge)](README.zh-CN.md)

</div>

---

> 基于 **Qt6** 和 **C++17** 开发的综合性**铁路售票桌面系统**，使用**真实中国铁路站点网络**数据建模。系统支持 **Dijkstra 最短路径算法**路线规划、实时座位管理、用户认证、管理员后台，以及完整的购票流程 —— 全部基于原生 Qt Widgets 图形界面，后端集成 **SQLite** 数据库。

---

## 功能特性

#### 🧑‍💼 乘客功能
- **用户注册与登录** — 安全的手机号+密码验证系统
- **车票预订** — 搜索并预订任意两站间的车票
- **账户管理** — 查看订票历史、管理余额
- **实时查询** — 检查特定路线和时间的可用座位
- **余额充值** — 向账户充值以购票

#### 🔧 管理员功能
- **列车管理** — 添加、修改和停运列车服务
- **路线配置** — 设置站点路线、时刻表和定价
- **系统监控** — 查看所有预订和系统状态
- **用户管理** — 管理员账户监督

## 技术栈

| 层级 | 技术 |
|------|------|
| **语言** | C++17 |
| **GUI 框架** | Qt 6.9.1 (Widgets) |
| **数据库** | SQLite |
| **构建系统** | qmake |
| **编译器** | MinGW64 (GCC) |
| **平台** | Windows（主要），可跨平台兼容 |

## 项目结构

```
railway-booking-system-localized-db/
├── README.md                         # 英文 README
├── README.zh-CN.md                   # 中文 README
├── LICENSE
└── src/                              # 源码 & 运行时目录
    ├── kent.cpp                      # 主程序源代码
    ├── railway.pro                   # Qt 项目文件
    ├── db_viewer.cpp                 # 数据库查看工具
    ├── railway.exe                   # 编译后的可执行文件
    ├── railway_system.db             # SQLite 数据库
    ├── Makefile                      # 构建配置
    ├── Qt6*.dll                      # Qt 运行时库
    ├── platforms/                    # Qt 平台插件
    │   └── qwindows.dll
    ├── plugins/sqldrivers/           # SQLite 驱动插件
    │   └── qsqlite.dll
    └── data/                         # 明文数据文件
        ├── admins.txt                # 管理员数据
        ├── users.txt                 # 用户数据
        ├── map.txt                   # 站点网络地图
        ├── new_trains.txt            # 新增列车
        └── suspended_trains.txt      # 停运列车
```

## 快速开始

### 前置要求
1. 安装 **Qt 6.9.1** 或更高版本
2. 安装 **MinGW64** 编译器
3. SQLite 已随 Qt 捆绑

### 构建 & 运行
```bash
git clone https://github.com/kent234535/railway-booking-system-localized-db.git
cd railway-booking-system-localized-db/src

# 使用 qmake 构建
qmake railway.pro
make

# 运行
./railway.exe
```

或者在 **Qt Creator** 中打开 `railway.pro` 直接构建运行。

## 使用指南

### 乘客用户
1. **注册** — 使用手机号、密码、姓名和身份证创建账户
2. **登录** — 使用手机号和密码
3. **搜索** — 选择出发站和到达站
4. **预订** — 选择列车并确认
5. **管理** — 查看行程、充值余额

### 管理员用户
1. **管理员登录** — 使用管理员凭据
2. **管理列车** — 添加新路线、修改时刻表、停运服务
3. **系统监控** — 查看所有预订和系统统计

## 数据库架构

| 数据表 | 说明 |
|--------|------|
| `users` | 用户账户、余额、购票记录 |
| `admins` | 管理员账户 |
| `trains` | 时刻表、路线、定价、座位数 |
| `suspended_trains` | 停运列车列表 |

## 安全性
- 密码验证：最少 8 字符，大小写混合，包含数字
- 手机号格式验证
- 凭据加密存储（`.enc` 文件）

## 贡献指南
1. Fork 此仓库
2. 创建功能分支 (`git checkout -b feature/amazing`)
3. 提交更改
4. 推送到分支
5. 创建 Pull Request

## 许可证

MIT 许可证 — 详见 [LICENSE](LICENSE)。

---

<div align="center">

`铁路售票系统` · `火车票` · `桌面应用` · `Qt6` · `C++17` · `SQLite` · `GUI` · `Dijkstra 算法` · `路线规划` · `railway booking system`

⭐ **如果觉得有用，请给个 Star！** ⭐

</div>
