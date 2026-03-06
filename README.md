<div align="center">

# 🚄 Railway Ticket Booking System

### Qt6 C++ Desktop App with SQLite

An intelligent desktop railway ticket booking system with Dijkstra-based shortest-path route planning, real-time seat management, and native Qt Widgets GUI

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Qt](https://img.shields.io/badge/Qt-6.9.1-green.svg)](https://www.qt.io/)
[![SQLite](https://img.shields.io/badge/Database-SQLite-003B57?logo=sqlite&logoColor=white)](https://www.sqlite.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows&logoColor=white)](https://github.com/kent234535/railway-booking-system-localized-db)

[Features](#features) · [Tech Stack](#tech-stack) · [Project Structure](#project-structure) · [Quick Start](#quick-start) · [Usage](#usage) · [Database](#database-schema) · [License](#license)

[![ENGLISH](https://img.shields.io/badge/ENGLISH-blue?style=for-the-badge)](README.md)
[![简体中文](https://img.shields.io/badge/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-gray?style=for-the-badge)](README.zh-CN.md)

</div>

---

> A full-featured **railway ticket booking desktop application** built with **Qt6**, **C++17**, and **SQLite**. Models **real Chinese railway station networks**, supports **Dijkstra-based shortest-path routing**, real-time seat management, user authentication, admin dashboard, and a complete booking workflow — all in a native Qt Widgets GUI.

---

## Features

#### 🧑‍💼 For Passengers
- **User Registration & Login** — Secure authentication with phone number & password validation
- **Ticket Booking** — Search and book tickets between any two stations
- **Account Management** — View booking history and manage account balance
- **Real-time Availability** — Check available seats for specific routes and times
- **Balance Recharge** — Add funds to account for ticket purchases

#### 🔧 For Administrators
- **Train Management** — Add, modify, and suspend train services
- **Route Configuration** — Set up station routes, schedules, and pricing
- **System Monitoring** — Overview of all bookings and system status
- **User Management** — Administrative oversight of user accounts

## Tech Stack

| Layer | Technology |
|-------|-----------|
| **Language** | C++17 |
| **GUI Framework** | Qt 6.9.1 (Widgets) |
| **Database** | SQLite |
| **Build System** | qmake |
| **Compiler** | MinGW64 (GCC) |
| **Platform** | Windows (primary), cross-platform compatible |

## Project Structure

```
railway-booking-system-localized-db/
├── README.md                         # English README
├── README.zh-CN.md                   # Chinese README
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

## Quick Start

> Steps marked with 🔑 require **Administrator privileges** — right-click → **Run as Administrator**

### Prerequisites

1. 🔑 Install **Qt 6.9.1** or later (the installer requires admin privileges)
2. 🔑 Install **MinGW64** compiler (bundled with the Qt installer)
3. 🔑 Add Qt and MinGW to the system `PATH` (run **PowerShell as Administrator**):
   ```powershell
   # PowerShell (Administrator)
   [Environment]::SetEnvironmentVariable(
       "Path",
       $env:Path + ";C:\Qt\6.9.1\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin",
       "Machine"
   )
   ```
   > Adjust the paths above to match your Qt installation directory.
4. SQLite support is bundled with Qt — no extra setup needed

### Build & Run

No administrator privileges needed for building and running.

```cmd
git clone https://github.com/kent234535/railway-booking-system-localized-db.git
cd railway-booking-system-localized-db\src

:: Build with qmake + MinGW
qmake railway.pro
mingw32-make

:: Run
railway.exe
```

Or open `railway.pro` in **Qt Creator** and click Build.

## Usage

### For Passengers
1. **Register** — Create account with phone number, password, name, and ID
2. **Login** — Use phone number and password
3. **Search** — Select origin and destination stations
4. **Book** — Choose train and confirm booking
5. **Manage** — View trips, recharge balance

### For Administrators
1. **Admin Login** — Use administrator credentials
2. **Manage Trains** — Add routes, modify schedules, suspend services
3. **Monitor** — View all bookings and system statistics

## Database Schema

| Table | Description |
|-------|-------------|
| `users` | User accounts, balances, booking history |
| `admins` | Administrator accounts |
| `trains` | Schedules, routes, pricing, seat counts |
| `suspended_trains` | Suspended train services |

## Security
- Password validation: min 8 chars, mixed case, numbers
- Phone number format validation
- Encrypted credential storage (`.enc` files)

## Contributing
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing`)
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

## License

MIT License — see [LICENSE](LICENSE) for details.

---

<div align="center">

`railway booking system` · `train ticket` · `Qt6` · `C++17` · `SQLite` · `desktop application` · `GUI` · `Dijkstra algorithm` · `route planning` · `铁路售票系统`

⭐ **Star this repo if you find it useful!** ⭐

</div>
