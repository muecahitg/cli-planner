# CLI-Planner

A lightweight command-line planner written in C, using SQLite for local data storage.

## Features

- Add events with a title and date
- View all events or filter by year, month, or day
- Delete events by title
- No server or configuration needed — data is stored in a local `planner.db` file

## Requirements

- GCC
- SQLite3

```bash
# Ubuntu/Debian
sudo apt install libsqlite3-dev

# Fedora/RHEL
sudo dnf install sqlite-devel
```

## Build

```bash
gcc main.c -o planner -lsqlite3
```

## Usage

```bash
./planner
```

### Commands

| Command | Description |
|---|---|
| `view all` | View all events |
| `view YYYY` | View events in a year, e.g. `view 2026` |
| `view YYYY-MM` | View events in a month, e.g. `view 2026-05` |
| `view YYYY-MM-DD` | View events on a day, e.g. `view 2026-05-13` |
| `add YYYY-MM-DD title` | Add a new event, e.g. `add 2026-05-13 Buy milk` |
| `delete title` | Delete an event by title, e.g. `delete Buy milk` |
| `help` | Show available commands |
| `quit` / `q` | Exit the program |

## Data Storage

Events are stored locally in `planner.db` in the same directory as the binary. This file is not included in the repository — each user has their own local database.

## Todo

- improve ui
