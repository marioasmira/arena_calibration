#include <vector>
#include <math.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "random_numbers.h"
#include "natural_cubic_spline.h"

// input data
int bit_resolution = pow(2, 12);
Natural_cubic_spline M(5, 500, 1, 1);
size_t iterations = 1 * pow(10, 5); // number of times to run. More times, more better estimate
size_t print = iterations / 20;
double random_chance = 0.2;
int n_tiles = 3;

double calc(const double &value)
{
    return (value / (bit_resolution - 1));
}

int main()
{

    std::vector<std::vector<double>> temperatures(3, std::vector<double>(0));
    std::vector<std::vector<double>> resistances(3, std::vector<double>(0));
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
    std::ofstream myFile("SS.csv");
    myFile << "Sum of squared diff\n";

    // set seed
    randomize();

    /* std::vector<std::vector<double>> values{{93.7487, -104.413, 6.54582, -33.6192, -100.089},
                                            {94.5345, -108.241, 3.45671, 28.6225, -305.973},
                                            {99.6374, -116.446, -4.51146, 61.102, -271.405}}; */
    // nov 3 morning measurements
    /* std::vector<std::vector<double>> values{{80.8773, -93.5178, 0.418203, 25.94, 23.1023},
                                            {83.1577, -97.4657, -0.041277, 31.2212, 21.8266},
                                            {83.628, -98.3216, 0.0269609, 29.0856, 26.5983}}; */
    // nov 3 morning measurements rerun
    /* std::vector<std::vector<double>> values{{86.4084, -107.371, -3.69321, 127.656, -393.183},
                                            {89.0889, -111.768, -7.23342, 144.546, -411.263},
                                            {90.8136, -116.597, -3.05034, 154.133, -501.815}}; */
    // nov 12 lighting calibration
    std::vector<std::vector<double>> values{{90.5645, -110.825, -11.1511, 138.974, -380.393},
                                            {90.6486, -112.128, -10.443, 145.702, -432.13},
                                            {91.4241, -115.468, -2.89886, 145.405, -511.608}};
    // nov 12 lighting calibration longer
    /* std::vector<std::vector<double>> values{{92.343, -114.878, -15.8376, 183.041, -530.145},
                                            {91.6528, -114.506, -11.794, 164.843, -501.396},
                                            {90.7346, -114.074, 0.349853, 123.888, -446.572}}; */
    // std::vector<double> values{62.65304, -62.09515, 0.0, 0.0, 0.0};

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
            myFile << sum_start[t] << ",";
        }
        myFile << "\n";
        // output progress
        if (i % print == 0)
        {
            // Print on the screen some message
            std::cout << "Done: "
                      << (static_cast<double>(i) / static_cast<double>(iterations)) * 100.0
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
        for (size_t j = 0; j < values[t].size(); j++)
        {
            std::cout << values[t][j] << ", ";
        }
        std::cout << "SLS = " << sum_start[t]
                  << std::endl;
    }
    return 0;
}