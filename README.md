# Arena calibration

Program to calibrate the restricted cubic splines used in the temperature arena.

## Getting Started

### Windows

(haven't tested yet :))

To run this program on Windows you can download the executable file on the releases page.

### Linux

(not working right now, missing one unpublished file :))

To run it on Linux it might be easier to compile it yourself with the following commands

```bash
git clone https://github.com/marioasmira/arena_calibration.git

cd arena_calibration

g++ -c src/*.cpp; g++ -O2 -o calibrate main.o random_numbers.o natural_cubic_spline.o

rm *.o
```

This will create the `arena_calibration/` folder with the executable file `calibrate` inside.

## Requirements

Once the program is ready, a file named `export_data.csv` need to be placed in the same folder as the executable.
This file contains the data to be used in the calibration formatted as:

```csv
read,measure,tile
2921,18,L
2934,18,L
```

Where the first column is the readings from the arena debug output, the second column is the measured temperature, and the third column is the tile (codified as "L", "M", or "R").
The program will not check the headers so make sure the columns are in this order.

### Executing program

To run the program you can use the following command on Linux

```text
./calibrate ITERATIONS
```

or on Windows

```text
calibrate.exe ITERATIONS
```

Where `ITERATIONS` should be a positive whole number. The larger the number the more accurate the result will be and the longer it will take to run. The default value for `ITERATIONS` is 1000.

### Output

The program outputs 2 files:

1) `SLS.csv` lists the values of the sum of least squares for the 3 tiles for every single iteration in case you want to track how it is converging;
2) `spline_values.txt` prints the final values for the spline vectors.
These can be copied over to the Arduino source file to upload.

## Authors

* Mário Artur Mira

## Version History

* 0.1
  * Initial Release
