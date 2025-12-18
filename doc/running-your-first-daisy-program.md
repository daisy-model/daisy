# Running your first Daisy program
Several sample programs are included in the Daisy distribution. These are located in the directory `sample` in the directory where you installed Daisy.

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


## Running your first spawn program
Daisy has builtin support for simulating multiple scenarios in parallel using the spawn program . To test it

1. Create a new folder named `daisy` on your Desktop
2. Copy the file `test-spawn.dai` from the `sample` folder to the newly created `daisy` folder
3. Open the `test-spawn.dai` file from the `daisy` folder in your editor
4. Run the `test-spawn.dai` file. Depending on you hardware, this might take some time. Once it is done you should see the following directories, each containing the output from a single scenario.
```
N-A-P		N-F-S		N-J-W		W-F-P		W-J-S
N-A-S		N-F-W		W-A-P		W-F-S		W-J-W
N-A-W		N-J-P		W-A-S		W-F-W
N-F-P		N-J-S		W-A-W		W-J-P
```

See more info on [spawn usage](spawn-usage.md) for how to control the `spawn` program.
