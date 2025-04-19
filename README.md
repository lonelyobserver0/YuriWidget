# YuriWidget

A widget engine that renders HTML, CSS, and JavaScript.

Using GTK-4, it is composed of a client (`yuriwidget_client`) and a server (`yuriwidget`), allowing the user to run commands that change the app status in real-time.

The use of sockets means that it is not bound by the OS, particularly supporting the infamous window compositor
Hyprland.


## Installation
### Dependencies

WIP

### Installation

- Clone this repo
- Move to the repo folder
- Use command `make`
- Use command `make install`
- Use command `make clean` (optional)


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

`yuriwidgetctl [command]`

| Command                   | Description                                                                                        |
|---------------------------|----------------------------------------------------------------------------------------------------|
| `hide <window-title>`     | Make the widget whose window has title <window-title> invisible by view, selection and inputs      |
| `show <window-title>`     | Make the hidden widget whose window has title <window-title> visible by view, selection and inputs |
| `reload <window-title>`   | Re-create the widget whose window has title <window-title>                                         |

## Little advice

Every widget is an instance of yuriwidget software and has his own PID, `pgrep yuriwidget` show every one of theme.
If you want to kill only one widget, use `kill <PID>`
