#include <vector>
#include <math.h>
#include <iostream>
#include "random_numbers.h"

// input data
std::vector<double> temperatures = {8.5, 10.9, 17.0, 22.0, 25.4, 30.0, 35.0, 40.0, 45.0, 50.0};
std::vector<double> resistances = {21000.00, 18735.96, 14127.36, 11312.50, 9749.03, 8010.56, 6446.95, 5245.90, 4307.69, 3567.64};
size_t iterations = 1.5 * pow(10, 9); // number of times to run. More times, more better estimate
size_t print = iterations / 20;
double kelvin = 273.15; // to convert to kelvin degrees
// reused several times, so having them saved is faster
double pow_3 = pow(10.0, -3.0);
double pow_4 = pow(10.0, -4.0);
double pow_7 = pow(10.0, -7.0);

// log transforming resistances to spare calculations
std::vector<double> log_resist;

// turn measurements to Kelvin degrees
std::vector<double> temperatures_kelvin;

// transform ohms into temperature (Kelvin)
double calc(double &a, double &b, double &c, double &R)
{
    return (1 / (a + b * R + c * pow(R, 3.0))); //a * exp(-b * T) + c;
}

int main()
{
    for (size_t j = 0; j < resistances.size(); j++)
    {
        log_resist.push_back(log(resistances[j]));
    }

    for (size_t j = 0; j < temperatures.size(); j++)
    {
        temperatures_kelvin.push_back(temperatures[j] + kelvin);
    }

    // set seed
    randomize();

    // random starting values in the approximate scale
    double a = normal(0, 10) * pow_3;
    double b = normal(0, 10) * pow_4;
    double c = normal(0, 10) * pow_7;

    // starting least squares
    double sum_start = 0.0;
    for (size_t j = 0; j < temperatures_kelvin.size(); j++)
    {
        sum_start += pow(calc(a, b, c, log_resist[j]) - temperatures_kelvin[j], 2.0);
    }

    std::cout << "Starting: a = " << a
              << ", b = " << b
              << ", c = " << c
              << ", SLS = " << sum_start
              << std::endl;

    double sum_end;
    for (int i = 0; i < iterations; i++)
    {
        // new values
        double new_a = a + normal(0, pow_3);
        double new_b = b + normal(0, pow_4);
        double new_c = c + normal(0, pow_7);
        sum_end = 0.0;
        // least squares
        for (size_t j = 0; j < temperatures.size(); j++)
        {
            sum_end += pow(calc(new_a, new_b, new_c, log_resist[j]) - temperatures_kelvin[j], 2.0);
        }

        // keep old or replace with new
        if (sum_end < sum_start)
        {
            a = new_a;
            b = new_b;
            c = new_c;
            sum_start = sum_end;
        }

        // output progress
        if (i % print == 0)
        {
            //Print on the screen some message
            std::cout << "Done: " << (static_cast<double>(i) / static_cast<double>(iterations)) * 100.0 << "%" << std::endl;
        }
    }

    std::cout << "Ending: a = " << a
              << ", b = " << b
              << ", c = " << c
              << ", SLS = " << sum_end
              << std::endl;
    return 0;
}