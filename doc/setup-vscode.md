# Setup Visual Studio Code for Daisy

0. [Before you begin](#before-you-begin)
1. [Install VSCode](#install-vscode)
2. [Install Daisy extension](#install-daisy-extension)
3. [Switching between Daisy versions](#switching-between-daisy-versions)
4. [Example `settings.json`](#example-settingsjson)

Visual Studio Code (VSCode) is an IDE and text editor that can be used to edit and run Daisy setup files (`.dai`) using the [Daisy extension](https://marketplace.visualstudio.com/items?itemName=daisy-model.daisy) developed by the Daisy team.

## Before you begin
If you already have VSCode installed, then you should ensure that old Daisy related configurations are deleted from

 * `settings.json`
 * `tasks.json`
 * `keybindings.json`

### Install VSCode
Download and install VSCode from https://code.visualstudio.com/

## Install Daisy extension
Follow the instructions at https://marketplace.visualstudio.com/items?itemName=daisy-model.daisy

### Basic usage

1. Open the `sample.dai` file from the `sample` directory in your Daisy installation.
2. Press `F5` to run the simulation (or click on the tiny Daisy icon in the top right of VSCode).

This should prompt you first for the path to your Daisy installation directory, then for the path to the Daisy executable you want to use.

* On Windows Daisy is typically installed to `C:/Program Files/daisy <version-number>`.
* On MacOS Daisy is typically installed to `/Applications/Daisy`

Once you have provided these paths a terminal should open and display the output of the simulation. If not, check that you have deleted all Daisy related settings from your `settings.json` file. If this does not fix the problem, please open an issue at https://github.com/daisy-model/daisy-vscode/issues


## Switching between Daisy versions
You can switch between Daisy version by adding multiple entries to the "Daisy.executable" settings. Each entry is defined by a user defined name and path to the executable. See [Example `settings.json`](#example-settingsjson).

When you have multiple entries in your "Daisy.executable" settings, you will be asked to pick which to use whenever you run Daisy.


## Example `settings.json`
```{json}
{
    "Daisy.executable": [
    {
        "name": "daisy-v7.1.0",
        "path": "C:/Program Files/daisy 7.1.0/bin/daisy.exe"
    },
    {
        "name": "daisy-v7.1.0_no-python",
        "path": "C:/Program Files/daisy 7.1.0_no-python/bin/daisy.exe"
    }
    "Daisy.home": "C:/Program Files/daisy 7.1.0"
}
```
