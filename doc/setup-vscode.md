# Setup Visual Studio Code for Daisy
Visual Studio Code is an ide and text editor that provides syntax highlighting and autocompletion for `.dai` files and can be used to run Daisy.

1. Download and install Visual Studio Code from https://code.visualstudio.com/
2. Install a lisp extension from [Visual Studio Marketplace](https://marketplace.visualstudio.com/search?term=tag%3A%22Lisp%22&target=VSCode&category=All%20categories&sortBy=Relevance)
    - If you only want syntax highlighting have a look at https://marketplace.visualstudio.com/items?itemName=mattn.Lisp
    - If you also want completion have a look at https://marketplace.visualstudio.com/items?itemName=qingpeng.common-lisp
3. Set the language mode for .dai files
    1. Open a .dai file
    2. Press `Ctrl + Shift + p`
    3. Type "change language mode" and select it
    4. Select "Configure File Association for '.dai'..."
    5. Type "lisp" and select the extension you want to use
4. (a) Open the tasks file
    1. Press `Ctrl + Shift + p`
    2. Type "configure task" and select it
    3. Select "Create tasks.json file from template". This should open a json file that looks something like below
```{json}
{
    // See https://go.microsoft.com/fwlink/?LinkId=733558
    // for the documentation about the tasks.json format
    "version": "2.0.0",
    "tasks": []
}
```
4. (b) Add a task that runs daisy. You need the path to the daisy program. The example below is for windows using daisy 7.1.0
```{json}
{
    // See https://go.microsoft.com/fwlink/?LinkId=733558
    // for the documentation about the tasks.json format
    "version": "2.0.0",
    "tasks": [
        {
            "label": "daisy-7.1.0",
            "type": "shell",
            "command": "C:\\Program Files\\daisy 7.1.0\\bin\\daisy.exe",
            "args": ["${file}"],
        }
    ]
}
```
5. (a) Open the keybindings file
    1. Press `Ctrl + Shift + p`
    2. Type "keyboard shortcuts" and select "Open Keyboard Shortcuts (JSON)". This should open a json file that looks something like the one below
```{json}
// Place your key bindings in this file to override the defaults
[]
```
5. (b) Add a keyboard shortcut. The example below assumes you used the label "daisy-7.1.0" for the task.

```{json}
// Place your key bindings in this file to override the defaults
[
    {
        "key": "ctrl+r 1",
        "command": "workbench.action.tasks.runTask",
        "args" : "daisy-7.1.0"
    },
]
```
6. Test by opening  dai file. Then press and release `Ctrl + r` followed by `1`

## Swithing between multiple Daisy installations
You can easily switch between Daisy version by adding another entry to the tasks and keybinding files. The example below use `Ctrl + r` `1` for version 7.1.0 and `Ctrl + r` `2` for version 7.0.7

`tasks.json`
```{json}
{
    // See https://go.microsoft.com/fwlink/?LinkId=733558
    // for the documentation about the tasks.json format
    "version": "2.0.0",
    "tasks": [
        {
            "label": "daisy-7.0.7",
            "type": "shell",
            "command": "C:\\Program Files\\daisy 7.0.7\\bin\\daisy.exe",
            "args" :["${file}"],
        },
        {
            "label": "daisy-7.1.0",
            "type": "shell",
            "command": "C:\\Program Files\\daisy 7.1.0\\bin\\daisy.exe",
            "args": ["${file}"],
        }
    ]
}
```

`keybindings.json`
```{json}
// Place your key bindings in this file to override the defaults
[
    {
        "key": "ctrl+r 1",
        "command": "workbench.action.tasks.runTask",
        "args" : "daisy-7.1.0"
    },
    {
        "key": "ctrl+r 2",
        "command": "workbench.action.tasks.runTask",
        "args" : "daisy-7.0.7"
    }
]
```