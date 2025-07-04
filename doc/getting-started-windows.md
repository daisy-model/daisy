# Getting started on Windows
Download the latest release of Daisy from [https://github.com/daisy-model/daisy/releases/latest](https://github.com/daisy-model/daisy/releases/latest)

We provide both a classical Windows installer and a zip file. The content is the same so pick whichever you prefer.

The installer requires administrator privileges, so pick the zip file if this a problem.
The zip file just needs to be unzipped to whatever location you prefer.

In the following we will refer to `<daisy-dir>`. This is the folder where you install Daisy, which is most likely `C:\Program Files\daisy 7.1.0` if you use the installer.


## Setting up an environment for running Daisy
You can run Daisy from the commandline, but we recommend that you use an editor like [VSCode](setup-vscode.md) or [TextPad](setup-textpad.md).


## Running your first Daisy program
Several sample programs are included in the Daisy distribution. These are located in the directory `sample` under `<daisy-dir>`

1. Create a new directory named `daisy` on your Desktop
2. Copy the file `test.dai` from the `sample` directory to the newly created `daisy` directory
3. Open the `test.dai` file from the `daisy` directory in your editor
4. Run the `test.dai` file. This should produce the following files in the `daisy` directory
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
