# GTK Web Browser

This repository contains a personal project: a minimalist web browser built using **GTK+ 3** and **WebKit2GTK**, developed in **C**. The goal is to emulate the core user interface and experience of Safari on Linux.

## Requirements

<div>
<img src="https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&amp;logo=C&amp;logoColor=white" alt="C">
<img src="https://img.shields.io/badge/GTK-76B900?style=for-the-badge&amp;logo=GTK&amp;logoColor=white" alt="GTK">
</div>

- GCC (GNU Compiler Collection)
- GTK+ 3 Development Libraries
- WebKit2GTK 4.1 Development Libraries
- `make` utility

## Installation

> **Note:** These instructions are intended for Debian-based systems (e.g., Ubuntu).

### Install Build Dependencies

```bash
sudo apt update
sudo apt install build-essential libgtk-3-dev libwebkit2gtk-4.1-dev
```
## Build & Run

```bash
make
./tux
```
## Features
- Basic tabbed browsing interface
- Custom navigation bar
- WebKit2GTK rendering engine integration

## Roadmap
- [x] Basic rendering using WebKit2GTK
- [x] Basic navigation (back, forward, reload)
- [ ] History and bookmarks system
- [ ] Download manager integration
- [ ] UI/UX enhancements (Safari-inspired)
- [ ] Codebase modularization and cleanup

## Project Status
**Development Phase:** 🧪 Active Development

The project is currently in its early stages, focusing on achieving a stable and extensible architecture.

## Author
Matheus N Silva  
GitHub: [@matheuznsilva](https://github.com/matheuznsilva)


**Contributions, suggestions, and feedback are welcome.**