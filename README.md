# YuriWidget

A widget engine that renders HTML, CSS, and JavaScript.

Using GTK-4, it is composed of a client (`yuriwidgetctl`) and a server (`yuriwidget`), allowing the user to run commands that change the app status in real-time.

The use of sockets means that it is not bound by the OS, particularly supporting the infamous window compositor
Hyprland.


## Installation

### Dependencies

webkit2gtk

### Installation

- Clone this repo
- Move to the repo folder
- To build: `make`
- To install: `make install`
- (Optional) To clean (remove bin file from *build* directory) `make clean`


## Usage

### Create a widget

Commands:

`yuriwidget -c [path to configuration file (config.json)]`

────────────────────────


### Configuration file

This file is to be saved in ~/.config/yuriwidget/ with the main html, css and js files too, if they exists.
After `-c` it expects relative path (relaive to ~/.config/yuriwidget/) to config.json. It is meant to have several sub-directory, each for every widget with theirs config.json.

<pre> ```json
{
  "title": "[widget-title]",
  "url": "[path-to-html-file]",
  "width": [window-width],
  "height": [window-height],
  "x": [window-x-position],
  "y": [window-y-position],
  "transparent": [transparent-bool]
}

``` </pre>

| Option                | Data type | Description                                                               |
|-----------------------|-----------|---------------------------------------------------------------------------|
| `widget-title`        | String    | Window title, it will be used by any other system to as ID                |
| `path-to-html-file`   | String    | Path to your html file to show or remote URL                              |
| `window-width`        | Integer   | If the window is not set like floating, this value will be overwritten    |
| `window-height`       | Integer   | If the window is not set like floating, this value will be overwritten    |
| `window-x-position`   | Integer   | If the window is not set like floating, this value will be overwritten    |
| `window-y-position`   | Integer   | If the window is not set like floating, this value will be overwritten    |
| `transparent-bool`    | Boolean   | If true it uses the opacity value of your WM or your DE                            |

────────────────────────

### Manage widgets

`yuriwidgetctl [command]`

| Command                   | Description                                                                                        |
|---------------------------|----------------------------------------------------------------------------------------------------|
| `hide <window-title>`     | Make the widget whose window has title <window-title> invisible by view, selection and inputs      |
| `show <window-title>`     | Make the hidden widget whose window has title <window-title> visible by view, selection and inputs |
| `reload <window-title>`   | Re-create the widget whose window has title <window-title>                                         |

## Little advices

Every widget is an instance of yuriwidget software and has his own PID, `pgrep yuriwidget` show every one of theme.
If you want to kill only one widget, use `kill <PID>`. Alternatively use your system method to kill the window with the right title-name.
