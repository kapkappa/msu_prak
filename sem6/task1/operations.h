#pragma once

dense_matrix matrix_multiplication(const dense_matrix&, const dense_matrix&);

std::vector<double> matvec_multiplication(const dense_matrix&, const std::vector<double>&);

double get_norm (const std::vector<double>&);

dense_matrix create_reflection_matrix(const std::vector<double>&, uint64_t);

std::vector<double> create_householder_vector(const std::vector<double>&, uint64_t);

void print(const std::vector<double> &);
