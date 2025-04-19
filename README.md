# YuriWidget

A widget engine that renders HTML, CSS, and JavaScript.

Using GTK-4, it is composed of a client (`yuriwidget_client`) and a server (`yuriwidget`), allowing the user to run commands that change the app status in real-time.

The use of sockets means that it is not bound by the OS, particularly supporting the infamous window compositor
Hyprland.

---
## Installation

### Dependencies

WIP

### Installation

- Clone this repo
- Move to the repo folder
- Use command `make`
- Use command `make install`
- Use command `make clean` (optional)
---

## Usage
### Create a widget
Commands:
`yuriwidget [options]`

| Option          | Description                                                                 |
|-----------------|-----------------------------------------------------------------------------|
| `--title`       | Window title, it will be used by any other system to as ID                  |
| `--config-file` | Path to your html file to show or remote URL                                |
| `--width`       | If the window is not set like floating, this value will be overwritten      |
| `--height`      | If the window is not set like floating, this value will be overwritten      |
| `--x`           | If the window is not set like floating, this value will be overwritten      |
| `--y`           | If the window is not set like floating, this value will be overwritten      |

### Manage widgets
