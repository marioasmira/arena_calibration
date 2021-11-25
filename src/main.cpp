#include <vector>
#include <math.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "random_numbers.h"
#include "natural_cubic_spline.h"

// input data
const int bit_resolution = pow(2, 12);
const Natural_cubic_spline M(5, 500, 1, 1);
const double random_chance = 0.2;
const int n_tiles = 3;

double calc(const double &value)
{
    return (value / (bit_resolution - 1));
}

// from https://stackoverflow.com/questions/29248585/c-checking-command-line-argument-is-integer-or-not
bool isNumber(char number[])
{
    int i = 0;

    //checking for negative numbers
    if (number[0] == '-')
        return false;
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}

void read_csv(std::vector<std::vector<double>> &temperatures, std::vector<std::vector<double>> &resistances){
    std::ifstream in("export_data.csv");
    if (!in.is_open())
        throw std::runtime_error("Cannot open input file.");
    std::string line, field;
    std::vector<std::string> v; // array of values for one line only

    bool header_line = true;
    while (getline(in, line)) // get next line in file
    {
        if (header_line)
        {
            header_line = false;
            continue;
        }
        else
        {
            v.clear();
            std::stringstream ss(line);

            while (getline(ss, field, ',')) // break line into comma delimitted fields
            {
                v.push_back(field); // add each field to the 1D array
            }
            if (v[2] == "L")
            {
                resistances[0].push_back(stod(v[0]));
                temperatures[0].push_back(stod(v[1]));
            }
            else if (v[2] == "M")
            {
                resistances[1].push_back(stod(v[0]));
                temperatures[1].push_back(stod(v[1]));
            }
            else if (v[2] == "R")
            {
                resistances[2].push_back(stod(v[0]));
                temperatures[2].push_back(stod(v[1]));
            }
        }
    }
    in.close();


}


int main(int argc, char **argv)
{
    size_t iterations = 1 * pow(10, 3); // number of times to run. More times, more better estimate
    try{
        if (argc >= 2){
            // when more than one argument ignore all but the first
            if( argc > 2){
                std::cout << "Ignoring all arguments after the first.\n";
            }
            if(isNumber(argv[1])){
                iterations = atoi(argv[1]);
                if(iterations < 100){
                    throw std::invalid_argument("Please use a value larger than 100.");
                }
            }
            // if the first argument is not a number throw error
            else {
                throw std::invalid_argument(
                    "The first argument must be a positive number. It defines the number of iterations."
                    );
            }
        }
    }
    catch(std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    const size_t print = static_cast<size_t>(iterations * 0.05);
    
    
    // fill data with input csv file
    // needs to be called "export_data.csv"
    std::vector<std::vector<double>> temperatures(3, std::vector<double>(0));
    std::vector<std::vector<double>> resistances(3, std::vector<double>(0));
    try{
        read_csv(temperatures, resistances);
    }
    catch(std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return -2;
    }

    std::ofstream myFile("SLS.csv");
    myFile << "tileL,tileM,tileR\n";

    std::ofstream final_vals("spline_values.txt");
    final_vals << "These are the final values for the spline\n";

    // set seed
    randomize();

    // nov 24 lighting calibration longer
    std::vector<std::vector<double>> values{{90.0, -100.0, 0.0, 0.0, 0.0},
                                            {90.0, -100.0, 0.0, 0.0, 0.0},
                                            {90.0, -100.0, 0.0, 0.0, 0.0}}; 

    // starting least squares
    std::vector<double> sum_start = {0.0, 0.0, 0.0};
    for (int t = 0; t < n_tiles; t++)
    {
        for (size_t j = 0; j < temperatures[t].size(); j++)
        {
            // sum_start += pow(calc(a, b, c, log_resist[j]) - temperatures_kelvin[j], 2.0);
            double val = M({calc(resistances[t][j])}, values[t]);
            sum_start[t] += pow(val - temperatures[t][j], 2.0);
        }
        sum_start[t] /= temperatures[t].size();
    }

    std::cout << "Starting: ";
    for (int t = 0; t < n_tiles; t++)
    {
        std::cout << "\t";
        for (size_t j = 0; j < values[t].size(); j++)
        {
            std::cout << values[t][j] << ", ";
        }
        std::cout << "SLS = " << sum_start[t]
                  << std::endl;
    }

    std::vector<double> sum_end = {0.0, 0.0, 0.0};
    for (int i = 0; i < iterations; i++)
    {
        std::vector<std::vector<double>> new_values = values;
        for (int t = 0; t < n_tiles; t++)
        {
            for (size_t j = 0; j < new_values[t].size(); j++)
            {
                if (bernoulli(random_chance))
                    new_values[t][j] += normal(0, 0.1);
            }
        }

        sum_end = {0.0, 0.0, 0.0};
        // least squares
        for (int t = 0; t < n_tiles; t++)
        {
            for (size_t j = 0; j < temperatures[t].size(); j++)
            {
                // sum_start += pow(calc(a, b, c, log_resist[j]) - temperatures_kelvin[j], 2.0);
                sum_end[t] += pow(M({calc(resistances[t][j])}, new_values[t]) - temperatures[t][j], 2.0);
            }
            sum_end[t] /= temperatures[t].size();
        }

        for (int t = 0; t < n_tiles; t++)
        {
            if (sum_end[t] < sum_start[t])
            {
                values[t] = new_values[t];
                sum_start[t] = sum_end[t];
            }
        }

        // keep old or replace with new
        for (int t = 0; t < n_tiles; t++)
        {
            myFile << sum_start[t] << ((t < n_tiles - 1) ? ",": "");
        }
        myFile << "\n";

        // output progress
        if (i % print == 0)
        {
            // Print on the screen some message
            std::cout << "Done: "
                      << round((static_cast<double>(i) / static_cast<double>(iterations)) * 100.0)
                      << "%, SLS = " << sum_start[0];
            for (int t = 1; t < n_tiles; t++)
            {
                std::cout << ", " << sum_start[t];
            }
            std::cout << std::endl;
        }
    }
    myFile.close();

    std::cout << "Ending: ";
    for (int t = 0; t < n_tiles; t++)
    {
        std::cout << "\t";
        final_vals << "tile " << t + 1 << ": ";
        for (size_t j = 0; j < values[t].size(); j++)
        {
            std::cout << values[t][j] << ", ";
            final_vals << values[t][j] << ", ";
        }
        std::cout << "SLS = " << sum_start[t]
                  << std::endl;
        final_vals << "SLS = " << sum_start[t]
                  << std::endl;
    }
    
    return 0;
}