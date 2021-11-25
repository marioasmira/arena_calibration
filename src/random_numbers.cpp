#include "random_numbers.h"

std::mt19937 rng;

long randomize()
{
    unsigned int seed =
        static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::cout << "Used seed: " << seed << std::endl;
    rng.seed(seed);
    return seed;
}

// random integer {0,...,n} (not including n)
int random_integer(const int &n)
{
    if (n <= 0)
        throw std::runtime_error("Single argument of random_integer needs to be higher than 0.\n");
    std::uniform_int_distribution<> d{};
    using parm_t = decltype(d)::param_type;
    return d(rng, parm_t{0, n - 1});
}

// random integer {i,...,n} (including n)
int random_integer(const int &i, const int &n)
{
    if (n <= i)
        throw std::runtime_error("In random_integer second argument needs to be higher than the first one.\n");
    std::uniform_int_distribution<> d{};
    using parm_t = decltype(d)::param_type;
    return d(rng, parm_t{i, n});
}

int binomial(const int &draws, const double &prob)
{
    if (draws <= 0)
        throw std::runtime_error("Number of draws needs to be a positive integer.\n");

    std::binomial_distribution<> d{draws, prob};
    return d(rng);
}

// random double [0,1)
double uniform()
{
    std::uniform_real_distribution<> d{};
    return d(rng);
}

// random standard normal
double normal(const double &mean, const double &stddev)
{
    if (stddev <= 0)
        throw std::runtime_error("Standard deviation equal or smaller than zero for Normal distribution.\n");
    std::normal_distribution<> d{mean, stddev};
    return d(rng);
}

// random bernoulli {0,1}
bool bernoulli(const double &prob)
{
    if (prob < 0.0 || prob > 1.0)
        throw std::runtime_error("Probability outside boundaries for Bernoulli distribution.\n");
    std::bernoulli_distribution d(prob);
    return d(rng);
}

int poisson(const double &lambda)
{
    if (lambda <= 0)
        throw std::runtime_error("Lambda equal or smaller than zero for Poisson distribution.\n");
    std::poisson_distribution<> d{};
    using parm_t = decltype(d)::param_type;
    return d(rng, parm_t{lambda});
}

double exponential(const double &lambda)
{
    if (lambda <= 0)
        throw std::runtime_error("Lambda equal or smaller than zero for Exponential distribution.\n");
    std::exponential_distribution<> d{};
    using parm_t = decltype(d)::param_type;
    return d(rng, parm_t{lambda});
}

int rindex(const std::vector<double> &weights)
{
    std::discrete_distribution<int> ri(weights.begin(), weights.end());
    return (ri(rng));
}
