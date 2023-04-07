#include "solver.hpp"

#include <cmath>
#include <iostream>
#include <vector>
#include <utility>
#include <cstdint>
#include <string>

#define MAX_ITERS 50000

template<typename T>
void print_vector(const std::vector<T>& vec, const std::string& str) {
    std::cout << str;
    for (const auto& el : vec) {
        std::cout << el << ' ';
    }
    std::cout << std::endl;
}

void print_system(int **W, int *P, int *C, size_t n, size_t m) {
    for (size_t j = 0; j < m; j++) {
        for (size_t i = 0; i < n; i++) {
            std::cout << W[i][j] << ' ';
        }
        std::cout << C[j] << std::endl;
    }

    for (size_t i = 0; i < n; i++) {
        std::cout << P[i] << ' ';
    }
    std::cout << "-> max" << std::endl;
}

void update_prices(std::vector<double>& price, const std::vector<double>& lambda, int *P, int **W) {
    for (size_t i = 0; i < price.size(); i++) {
        price[i] = (double)P[i];
        for (size_t j = 0; j < lambda.size(); j++) {
            price[i] -= (double)W[i][j] * lambda[j];
        }
    }
}

int get_C(const std::vector<double>& lambda, int *C) {
    double sum = 0.0;
    for (size_t i = 0; i < lambda.size(); i++) {
        sum += (double)lambda[i] * C[i];
    }
    return static_cast<int>(sum);
}

void update_lambdas(std::vector<double>& lambda, const std::vector<int>& X, int **W, int *C, int iter) {
    size_t size = lambda.size();
    int G[size];
//    std::cout << "Subgradient: ";
    for (size_t i = 0; i < size; i++) {
        G[i] = C[i];
        for (const auto& pos : X) {
            G[i] -= W[pos][i];
        }
//        std::cout << G[i] << ' ';
    }
//    std::cout << std::endl;
    double g_norm = 0.0;
    for (size_t i = 0; i < size; i++) {
        g_norm += G[i] * G[i];
    }

    g_norm = sqrt(g_norm) + 1e-7;

    for (size_t i = 0; i < size; i++) {
        lambda[i] -= (double) 1 / (iter+1) * G[i] / g_norm;
        lambda[i] = std::max(lambda[i], 0.0);
    }
}

int main () {

    size_t n, m;
    std::cin >> n >> m;

    int C[m];
    int P[n];

    for (size_t i = 0; i < m; i++)
        std::cin >> C[i];

    for (size_t i = 0; i < n; i++)
        std::cin >> P[i];

    int **W = new int * [n];

    for (size_t i = 0; i < n; i++) {
        W[i] = new int [m];
        for (size_t j = 0; j < m; j++) {
            std::cin >> W[i][j];
        }
    }

//    print_system(W, P, C, n, m);

    std::vector<double> price(n);
    std::vector<double> weight(n);
    for (size_t i = 0; i < n; i++) {
        price[i] = P[i];
        weight[i] = W[i][m-1];
    }

    std::vector<double> lambda(m-1);
    for (size_t i = 0; i < m-1; i++)
        lambda[i] = 1;

    double prev_loss = 0.0, cur_loss = 0.0;

    ListSolver solver(n, C[m-1]);

    for (int iter = 0; iter < MAX_ITERS; iter++) {
        update_prices(price, lambda, P, W);

//        print_vector(lambda, "lambda: ");
//        print_vector(price, "price: ");

//        auto solution = solve(price, weight, n, C[m-1]);
        auto solution = solver.solve(price, weight);

//        print_vector(solution.second, "solution: ");

        prev_loss = cur_loss;
        cur_loss = solution.first + get_C(lambda, C);
        if (std::abs(cur_loss - prev_loss) < 1e-7) {
//            std::cout << "iter: " << iter << std::endl;
            break;
        }
//        std::cout << "cur_loss: " << cur_loss << std::endl;
        update_lambdas(lambda, solution.second, W, C, iter);
    }

    std::cout << cur_loss << std::endl;
    for (const auto& l : lambda) {
        std::cout << l << std::endl;
    }

    for (size_t i = 0; i < n; i++) {
        delete [] W[i];
    }
    delete [] W;

    return 0;
}
