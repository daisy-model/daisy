# Setup Visual Studio Code for Daisy
<!-- vscode-markdown-toc -->
1. [Before you begin](#before-you-begin)
    * 1.1. [Install VSCode](#install-vscode)
2. [Enable syntax highlighting](#enable-syntax-highlighting)
3. [Setup VSCode to run Daisy](#setup-vscode-to-run-daisy)
	* 3.1. [Configure a launcher to run Daisy when pressing `F5`](#configure-a-launcher-to-run-daisy-when-pressing-f5)
	* 3.2. [Configure tasks to run Daisy when a user defined keyboard shortcut is pressed](#configure-tasks-to-run-daisy-when-a-user-defined-keyboard-shortcut-is-pressed)
	* 3.3. [Switching between multiple Daisy installations](#switching-between-multiple-daisy-installations)
4. [Example configurations](#example-configurations)
	* 4.1. [`settings.json`](#settingsjson)
	* 4.2. [`tasks.json`](#tasksjson)
	* 4.3. [`keybindings.json`](#keybindingsjson)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->

Visual Studio Code (VSCode) is an ide and text editor that provides syntax highlighting and autocompletion for `.dai` files and can be used to run Daisy.

VSCode can be configured in many ways. Some choices have unexpected consequences, so these instructions might not work for you. In that case, please report it as an [issue](https://github.com/daisy-model/daisy/issues) so we can cover your case as well.

## Before you begin
If you already have VSCode installed, then you should ensure that old Daisy related configurations are deleted from your settings.json file.

### Install VSCode
Download and install VSCode from https://code.visualstudio.com/

## Enable syntax highlighting
1. Install a lisp extension from [Visual Studio Marketplace](https://marketplace.visualstudio.com/search?term=tag%3A%22Lisp%22&target=VSCode&category=All%20categories&sortBy=Relevance)
    - If you only want syntax highlighting have a look at https://marketplace.visualstudio.com/items?itemName=mattn.Lisp
    - If you also want completion have a look at https://marketplace.visualstudio.com/items?itemName=qingpeng.common-lisp
2. Set the language mode for .dai files
    1. Open a .dai file
    2. Press `F1` or `Ctrl + Shift + p` to bring up the "Command Palette"
    3. Start typing "Change Language Mode" and select it when it appears in the menu
    4. Select "Configure File Association for '.dai'..."
    5. Type "lisp" and select the extension you want to use

## Setup VSCode to run Daisy
There are two different approaches that complement each other

1. Configure a launcher to run Daisy when pressing `F5`
2. Configure tasks to run Daisy when a user defined keyboard shortcut is pressed

The first option is the most relevant if you just want to run a single version of Daisy. The second option is more relevant if you want to be able to choose between multiple versions of Daisy.

### Configure a launcher to run Daisy when pressing `F5`
There are several ways of running a program from VSCode. In order for the Daisy spawn program to work correctly, it is important that "CommandPrompt" (cmd.exe) is used. 

#### Set default terminal
1. Press `F1`or `Ctrl + Shift + p` to bring up the "Command Palette"
2. Start typing "Terminal: Select Default Profile" and select it when it appears in the menu
3. Choose "Command Prompt".

#### Configure the launcher

1. Open your settings file
    1. Press `F1` or `Ctrl + Shift + p` to bring up the "Command Palette"
    2. Start typing the commannd "Preferences: Open User settings (JSON)" and select it when it appears in the menu.
    3. This should open the file `settings.json`
2. Add a new configuration under "configurations" under "launch".
```{json}
// ...
    "launch": {
        "configurations": [
            // ...
            // You could already have a bunch of other configurations here
            {
                "name" : "Run Daisy",
                "request": "launch",
                "type": "node-terminal",
                "cwd" : "${workspaceFolder}",
                "command": "\"C:/Program Files/daisy 7.0.7/bin/daisy.exe\" \"${file}\""
            }
        ]
    }
// ...
```
Notice that you have to choose between a PowerShell version and a CommandPrompt version. Look at [CommandPrompt or PowerShell](#commandprompt-or-powershell) if you are in doubt.


### Configure tasks to run Daisy when a user defined keyboard shortcut is pressed
1. Open the tasks file
    1. Press `F1` or `Ctrl + Shift + p` to bring up the "Command Palette"
    2. Start typing the command "Tasks: Configure Task" and select it when it appears in the menu
    3. Select "Create tasks.json file from template" (others).
    4. This should open the file `tasks.json`
2. Add a new task under "tasks"
```{json}
{
    // See https://go.microsoft.com/fwlink/?LinkId=733558
    // for the documentation about the tasks.json format
    "version": "2.0.0",
    "tasks": [
        // You could already have a bunch of other tasks here
        {
            "label": "daisy-7.0.7",
            "type": "process",
            "command": "C:/Program Files/daisy 7.1.0/bin/daisy.exe",
            "args": ["${file}"],
        }
    ]
}
```
3. Open the keybindings file
    1. Press `F1` or `Ctrl + Shift + p` to bring up the "Command Palette"
    2. Start typing the command "Preferences: Open Keyboard Shortcuts (JSON)" and select it when it appears in the menu
    4. This should open the file `keybindings.json`
4. Add a new key binding that runs the task you just defined
```{json}
// Place your key bindings in this file to override the defaults
[
    // You could already have a bunch of other keybindings here.
    {
        "key": "ctrl+r 1",
        "command": "workbench.action.tasks.runTask",
        "args" : "daisy-7.0.7"
    },
]
```
5. Test by opening `.dai` file. Then press and release `Ctrl + r` followed by `1`.

### Switching between multiple Daisy installations
You can easily switch between Daisy version by adding another entry to the tasks and keybinding files. Just remember to have unique names for the tasks


## Example configurations
### `settings.json`
```{json}
{
    "files.associations": {
        "*.dai": "commonlisp"
    },
    "launch": {
        "configurations": [
            {
                "name" : "Run Daisy",
                "request": "launch",
                "type": "node-terminal",
                "command": "\"C:/Program Files/daisy 7.0.7/bin/daisy.exe\" \"${file}\"",
                "cwd" : "${workspaceFolder}"

            }
        ]
    },
    "terminal.integrated.defaultProfile.windows": "Command Prompt"
}
```

### `tasks.json`
```{json}
{
    // See https://go.microsoft.com/fwlink/?LinkId=733558
    // for the documentation about the tasks.json format
    "version": "2.0.0",
    "tasks": [
        {
            "label": "daisy-7.0.7",
            "type": "process",
            "command": "C:/Program Files/daisy 7.0.7/bin/daisy.exe",
            "args": ["${file}"]
        },
        {
            "label": "daisy-7.1.0",
            "type": "process",
            "command": "C:/Program Files/daisy 7.1.0/bin/daisy.exe",
            "args": ["${file}"]
        },
    ]
}
```

### `keybindings.json`
```{json}
// Place your key bindings in this file to override the defaults
[
    {
        "key": "ctrl+r 1",
        "command": "workbench.action.tasks.runTask",
        "args" : "daisy-7.0.7"
    },
    {
        "key": "ctrl+r 2",
        "command": "workbench.action.tasks.runTask",
        "args" : "daisy-7.1.0"
    }
]
```
