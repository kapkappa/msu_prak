#pragma once

dense_matrix matrix_multiplication(const dense_matrix&, const dense_matrix&);

std::vector<double> matvec_multiplication(const dense_matrix&, const std::vector<double>&);

double get_norm (const std::vector<double>&);

dense_matrix create_reflection_matrix(const std::vector<double>&, uint64_t);

std::vector<double> create_householder_vector(const std::vector<double>&, uint64_t);

void print(const std::vector<double> &);

std::vector<double> solve_gauss(const dense_matrix&, const std::vector<double>&);

std::vector<double> generate_vector(uint64_t);
std::vector<double> generate_vector(const dense_matrix&, uint64_t);

void householder_multiplication(dense_matrix&, std::vector<double>&, const std::vector<double>&);

double get_discrepancy(const dense_matrix&, const std::vector<double>&, const std::vector<double>&);