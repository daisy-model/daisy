# Getting started on MacOS
Download the latest release of Daisy from [https://github.com/daisy-model/daisy/releases/latest](https://github.com/daisy-model/daisy/releases/latest) and run the installer.

In the following we will refer to `<daisy-dir>`. This is the folder where you install Daisy, which is most likely `/Applications/Daisy`.


## Setup Python on MacOS
To use python with Daisy on MacOS it is necesary to install python manually. Daisy will only work with python 3.13 that can be downloaded from [https://www.python.org/ftp/python/3.13.2/python-3.13.2-macos11.pkg](https://www.python.org/ftp/python/3.13.2/python-3.13.2-macos11.pkg).

Once downloaded, launch the installer and follow the instructions to install python.


## Setting up an environment for running Daisy
You can run Daisy from the commandline, but we recommend that you use an editor like [TextMate](https://macromates.com/). The Daisy distribution includes a support file for TextMate, `Daisy.tmbundle`. This file is located in the folder `TextMate` under `<daisy-dir>`.


## Running your first Daisy program
Several sample programs are included in the Daisy distribution. These are located in the folder `sample` under `<daisy-dir>`.

1. Create a new folder named `daisy` on your Desktop
2. Copy the file `test.dai` from the `sample` folder to the newly created `daisy` folder
3. Open the `test.dai` file from the `daisy` folder in your editor
4. Run the `test.dai` file. This should produce the following files in the `daisy` folder
  - `checkpoint-1987-8-7+6.dai`.
     the state of the simulation. Can be used to hot start the simulation.
  - `daisy.log`
     Log of the simulation that was just run
  - `field_nitrogen.dlf`
  - `field_water.dlf`
  - `harvest.dlf`
  - `sbarley.dlf`
  - `soil_nitrogen.dlf`
  - `soil_water.dlf`

The `.dlf` files are the output of the simulation. These are tab separated files with a custom header that can be read into spreadsheet applications or with you favorite programming language. We recommend using RStudio with the [daisyrVis](https://github.com/daisy-model/daisyrVis) package or Python with the [daispy-vis](https://github.com/daisy-model/daisypy-vis) package.
