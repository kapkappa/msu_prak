#pragma once

dense_matrix matrix_multiplication(const dense_matrix&, const dense_matrix&);

std::vector<double> matvec_multiplication(const dense_matrix&, double *);

dense_matrix create_reflection_matrix(const std::vector<double>&, uint32_t);

std::vector<double> create_householder_vector(const std::vector<double>& );

void print(const std::vector<double> &);

std::vector<double> solve_gauss(const dense_matrix&, const std::vector<double>&);

std::vector<double> generate_vector(uint32_t);
std::vector<double> generate_vector(const dense_matrix&, uint32_t);

void householder_multiplication(dense_matrix&, std::vector<double>&, const std::vector<double>&);

double get_discrepancy(const dense_matrix&, double *, const std::vector<double>&);

double get_manhattan_norm(const std::vector<double>& );

double get_error_norm(std::vector<double>);
double get_error_norm(double *, uint32_t);
double get_norm(double *, uint32_t);
