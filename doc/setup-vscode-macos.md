# Setup Visual Studio Code for Daisy
<!-- vscode-markdown-toc -->
1. [Before you begin](#before-you-begin)
    * 1.1. [Install VSCode](#install-vscode)
2. [Enable syntax highlighting](#enable-syntax-highlighting)
3. [Setup VSCode to run Daisy](#setup-vscode-to-run-daisy)
	* 3.1. [Configure a launcher to run Daisy when pressing `F5`](#configure-a-launcher-to-run-daisy-when-pressing-f5)
4. [Example configurations](#example-configurations)
	* 4.1. [`settings.json`](#settingsjson)

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
    2. Press `F1` or `Cmd + Shift + p` to bring up the "Command Palette"
    3. Start typing "Change Language Mode" and select it when it appears in the menu
    4. Select "Configure File Association for '.dai'..."
    5. Type "lisp" and select the extension you want to use

## Setup VSCode to run Daisy
### Configure a launcher to run Daisy when pressing `F5`

1. Open your settings file
    1. Press `F1` or `Cmd + Shift + p` to bring up the "Command Palette"
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
                "command": "/Applications/Daisy/bin/daisy \"${file}\""
            }
        ]
    }
// ...
```
3. Test by opening `.dai` file. Then press and release `F5`.


## Example configurations
### `settings.json`
```{json}
{
    "files.associations": {
        "*.dai": "lisp"
    },
    "launch": {
        "configurations": [
           {
                "name" : "Run Daisy",
                "request": "launch",
                "type": "node-terminal",
                "cwd" : "${workspaceFolder}",
                "command": "/Applications/Daisy/bin/daisy \"${file}\""
            }
        ]
    }
}
```
