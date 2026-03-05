# 🚄 Railway Ticket Booking System — Qt6 C++ Desktop App with SQLite

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Qt](https://img.shields.io/badge/Qt-6.9.1-green.svg)](https://www.qt.io/)
[![SQLite](https://img.shields.io/badge/Database-SQLite-blue)](https://www.sqlite.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

<div align="center">

**🌐 [📖 简体中文版](README.zh-CN.md)**

</div>

---

### Overview

A full-featured **railway ticket booking desktop application** built with **Qt6**, **C++17**, and **SQLite**. The system models **real Chinese railway station networks**, supports **Dijkstra-based shortest-path routing**, real-time seat management, user authentication, admin dashboard, and a complete booking workflow — all in a native Qt Widgets GUI.

### Features

#### For Passengers
- **User Registration & Login** — Secure authentication with phone number & password validation
- **Ticket Booking** — Search and book tickets between any two stations
- **Account Management** — View booking history and manage account balance
- **Real-time Availability** — Check available seats for specific routes and times
- **Balance Recharge** — Add funds to account for ticket purchases

#### For Administrators
- **Train Management** — Add, modify, and suspend train services
- **Route Configuration** — Set up station routes, schedules, and pricing
- **System Monitoring** — Overview of all bookings and system status
- **User Management** — Administrative oversight of user accounts

### Tech Stack

| Layer | Technology |
|-------|-----------|
| **Language** | C++17 |
| **GUI Framework** | Qt 6.9.1 (Widgets) |
| **Database** | SQLite |
| **Build System** | qmake |
| **Compiler** | MinGW64 (GCC) |
| **Platform** | Windows (primary), cross-platform compatible |

### Project Structure

```
railway-booking-system-localized-db/
├── README.md
├── README.zh-CN.md
├── LICENSE
└── src/                              # Source & runtime directory
    ├── kent.cpp                      # Main application source code
    ├── railway.pro                   # Qt project file
    ├── db_viewer.cpp                 # Database viewer utility
    ├── railway.exe                   # Compiled executable (Windows)
    ├── railway_system.db             # SQLite database
    ├── Makefile                      # Build configuration
    ├── Qt6*.dll                      # Qt runtime libraries
    ├── platforms/                    # Qt platform plugins
    │   └── qwindows.dll
    ├── plugins/sqldrivers/           # SQLite driver plugin
    │   └── qsqlite.dll
    └── data/                         # Plain-text data files
        ├── admins.txt
        ├── users.txt
        ├── map.txt                   # Station network map
        ├── new_trains.txt
        └── suspended_trains.txt
```

### Quick Start

#### Prerequisites
1. Install **Qt 6.9.1** or later
2. Install **MinGW64** compiler
3. SQLite support is bundled with Qt

#### Build & Run
```bash
git clone https://github.com/kent234535/railway-booking-system-localized-db.git
cd railway-booking-system-localized-db/src

# Build with qmake
qmake railway.pro
make

# Run
./railway.exe
```

Or open `railway.pro` in **Qt Creator** and click Build.

### Usage

#### For Passengers
1. **Register** — Create account with phone number, password, name, and ID
2. **Login** — Use phone number and password
3. **Search** — Select origin and destination stations
4. **Book** — Choose train and confirm booking
5. **Manage** — View trips, recharge balance

#### For Administrators
1. **Admin Login** — Use administrator credentials
2. **Manage Trains** — Add routes, modify schedules, suspend services
3. **Monitor** — View all bookings and system statistics

### Database Schema

| Table | Description |
|-------|-------------|
| `users` | User accounts, balances, booking history |
| `admins` | Administrator accounts |
| `trains` | Schedules, routes, pricing, seat counts |
| `suspended_trains` | Suspended train services |

### Security
- Password validation: min 8 chars, mixed case, numbers
- Phone number format validation
- Encrypted credential storage (`.enc` files)

### Contributing
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing`)
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

### License

MIT License — see [LICENSE](LICENSE) for details.

---

## 🏷️ Keywords

`railway booking system` · `train ticket` · `Qt6` · `C++17` · `SQLite` · `desktop application` · `GUI` · `Dijkstra algorithm` · `route planning` · `铁路售票系统` · `火车票` · `桌面应用`

---

<div align="center">

⭐ **Star this repo if you find it useful!** ⭐

</div>
