这个就是把文件读写换成SQLite管理数据的版本，其他的和另外一个项目没区别
# Railway Ticket System / 铁路售票系统

[English](#english) | [中文](#中文)

## English

### Overview
A comprehensive railway ticket booking system built with Qt6 and C++, featuring a graphical user interface for both passengers and administrators. The system supports real-time ticket booking, user management, and train administration with SQLite database integration.

### Features

#### For Passengers
- **User Registration & Login**: Secure authentication with phone number and password validation
- **Ticket Booking**: Search and book tickets between different stations
- **Account Management**: View booking history and manage account balance
- **Real-time Availability**: Check available seats for specific routes and times
- **Balance Recharge**: Add funds to account for ticket purchases

#### For Administrators
- **Train Management**: Add, modify, and suspend train services
- **Route Configuration**: Set up station routes, schedules, and pricing
- **System Monitoring**: Overview of all bookings and system status
- **User Management**: Administrative oversight of user accounts

### Technical Specifications

#### Built With
- **Framework**: Qt 6.9.1
- **Language**: C++17
- **Database**: SQLite
- **GUI**: Qt Widgets
- **Build System**: qmake

#### System Requirements
- **OS**: Windows (primary), Cross-platform compatible
- **Qt Version**: 6.9.1 or higher
- **Compiler**: MinGW64 (GCC)
- **Database**: SQLite (bundled)

### Project Structure
```
railway-system/
├── kent.cpp                    # Main application source code
├── railway.pro                 # Qt project file
├── railway.exe                 # Compiled executable
├── railway_system.db           # SQLite database file
├── Makefile                    # Build configuration
├── Qt6*.dll                    # Qt runtime libraries
├── platforms/                  # Qt platform plugins
│   └── qwindows.dll
├── plugins/                    # Qt plugins
│   └── sqldrivers/
│       └── qsqlite.dll
└── 未加密txt文件/              # Unencrypted data files
    ├── admins.txt
    ├── users.txt
    ├── map.txt                 # Station network map
    ├── new_trains.txt
    └── suspended_trains.txt
```

### Installation & Setup

#### Prerequisites
1. Install Qt 6.9.1 or later
2. Install MinGW64 compiler
3. Ensure SQLite support is available

#### Building from Source
```bash
# Clone the repository
git clone <repository-url>
cd railway-system

# Build with qmake
qmake railway.pro
make

# Or use Qt Creator
# Open railway.pro in Qt Creator and build
```

#### Running the Application
```bash
# Run the executable
./railway.exe

# Or on Windows
railway.exe
```

### Usage Guide

#### First Time Setup
1. Launch the application
2. The system will automatically initialize the SQLite database
3. Default data will be migrated from text files if available

#### For Passengers
1. **Registration**: Create account with phone number, password, name, and ID
2. **Login**: Use phone number and password
3. **Search Tickets**: Select origin and destination stations
4. **Book Tickets**: Choose train and confirm booking
5. **Manage Account**: View trips and recharge balance

#### For Administrators
1. **Admin Login**: Use administrator credentials
2. **Manage Trains**: Add new routes, modify schedules, suspend services
3. **Monitor System**: View all bookings and system statistics

### Database Schema

#### Tables
- **users**: User account information and balance
- **admins**: Administrator accounts
- **trains**: Train schedules, routes, and pricing
- **suspended_trains**: List of suspended train services

### Key Features Detailed

#### Security
- Password validation (minimum 8 characters, mixed case, numbers)
- Phone number format validation
- Secure user authentication

#### Booking System
- Real-time seat availability checking
- Dynamic pricing based on distance
- Booking history tracking
- Balance management

#### Administrative Tools
- Complete train schedule management
- Route configuration with station mapping
- Service suspension capabilities
- System oversight and monitoring

### Contributing
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

### License
This project is available under the MIT License.

---

## 中文

### 项目概述
这是一个基于Qt6和C++开发的综合性铁路售票系统，为乘客和管理员提供图形化用户界面。系统支持实时购票、用户管理和列车管理，集成SQLite数据库。

### 功能特性

#### 乘客功能
- **用户注册登录**: 安全的手机号和密码验证系统
- **车票预订**: 搜索并预订不同站点间的车票
- **账户管理**: 查看订票历史和管理账户余额
- **实时查询**: 检查特定路线和时间的可用座位
- **余额充值**: 为购票向账户添加资金

#### 管理员功能
- **列车管理**: 添加、修改和停运列车服务
- **路线配置**: 设置站点路线、时刻表和定价
- **系统监控**: 查看所有预订和系统状态
- **用户管理**: 管理员用户账户监督

### 技术规格

#### 开发技术
- **框架**: Qt 6.9.1
- **编程语言**: C++17
- **数据库**: SQLite
- **图形界面**: Qt Widgets
- **构建系统**: qmake

#### 系统要求
- **操作系统**: Windows（主要）, 跨平台兼容
- **Qt版本**: 6.9.1或更高
- **编译器**: MinGW64 (GCC)
- **数据库**: SQLite（内置）

### 项目结构
```
railway-system/
├── kent.cpp                    # 主程序源代码
├── railway.pro                 # Qt项目文件
├── railway.exe                 # 编译后的可执行文件
├── railway_system.db           # SQLite数据库文件
├── Makefile                    # 构建配置文件
├── Qt6*.dll                    # Qt运行时库
├── platforms/                  # Qt平台插件
│   └── qwindows.dll
├── plugins/                    # Qt插件
│   └── sqldrivers/
│       └── qsqlite.dll
└── 未加密txt文件/              # 未加密数据文件
    ├── admins.txt              # 管理员数据
    ├── users.txt               # 用户数据
    ├── map.txt                 # 站点网络地图
    ├── new_trains.txt          # 新增列车
    └── suspended_trains.txt    # 停运列车
```

### 安装和设置

#### 前置要求
1. 安装Qt 6.9.1或更高版本
2. 安装MinGW64编译器
3. 确保SQLite支持可用

#### 从源码构建
```bash
# 克隆仓库
git clone <repository-url>
cd railway-system

# 使用qmake构建
qmake railway.pro
make

# 或使用Qt Creator
# 在Qt Creator中打开railway.pro并构建
```

#### 运行应用程序
```bash
# 运行可执行文件
./railway.exe

# 或在Windows上
railway.exe
```

### 使用指南

#### 首次设置
1. 启动应用程序
2. 系统将自动初始化SQLite数据库
3. 如果可用，默认数据将从文本文件迁移

#### 乘客用户
1. **注册**: 使用手机号、密码、姓名和身份证创建账户
2. **登录**: 使用手机号和密码登录
3. **搜索车票**: 选择出发和到达站点
4. **预订车票**: 选择列车并确认预订
5. **管理账户**: 查看行程和充值余额

#### 管理员用户
1. **管理员登录**: 使用管理员凭据登录
2. **管理列车**: 添加新路线、修改时刻表、停运服务
3. **监控系统**: 查看所有预订和系统统计

### 数据库架构

#### 数据表
- **users**: 用户账户信息和余额
- **admins**: 管理员账户
- **trains**: 列车时刻表、路线和定价
- **suspended_trains**: 停运列车服务列表

### 主要功能详解

#### 安全性
- 密码验证（最少8个字符，大小写混合，包含数字）
- 手机号格式验证
- 安全的用户身份验证

#### 订票系统
- 实时座位可用性检查
- 基于距离的动态定价
- 订票历史跟踪
- 余额管理

#### 管理工具
- 完整的列车时刻表管理
- 站点映射的路线配置
- 服务停运功能
- 系统监督和监控

### 贡献指南
1. Fork此仓库
2. 创建功能分支
3. 提交您的更改
4. 推送到分支
5. 创建Pull Request

### 许可证
此项目基于MIT许可证提供。

### 联系方式
如有问题或建议，请通过以下方式联系：
- 提交Issue到GitHub仓库
- 发送邮件到项目维护者

### 版本历史
- **v1.0.0**: 初始版本，包含基本的购票和管理功能
- 数据库集成和Qt界面实现
- 支持多站点路线规划

### 致谢
感谢Qt框架提供的强大GUI开发工具，以及SQLite提供的轻量级数据库解决方案。
