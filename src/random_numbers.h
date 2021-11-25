#ifndef RANDOM_NUMBERS_H
#define RANDOM_NUMBERS_H

#include <random>
#include <chrono>
#include <iostream>
#include <stdexcept>

long randomize();

// random integer [0,n)
int random_integer(const int&);

// random integer [0,n)
int random_integer(const int &, const int&);

// binomial distribution
int binomial(const int &draws, const double &prob = 0.5);

// random uniform [0,1)
double uniform();

// random standard normal
double normal(const double &mean, const double &stddev);

// random binary
bool bernoulli(const double &prob = 0.5);

// random Poisson
int poisson(const double &lambda);

// random exponential
double exponential(const double&lambda);

// generate random index based on vector of int weights
// may template this later if I knew how...
//int rindex(const std::vector<double> &weights);

extern std::mt19937 rng;

template <typename T>
int rindex(const std::vector<T> &weights){
    std::discrete_distribution<int> ri(weights.begin(), weights.end());
    return(ri(rng));
}


// functor version
template <typename T>
class random_index
{
public:
    random_index(const std::vector<T> &w) : weights(w) {}
    int operator()() const {
        std::discrete_distribution<> d(weights.begin(), weights.end());
        return d(rng);
    }
private:
    const std::vector<T> &weights;
};

#endif // RANDOM_NUMBERS_H
