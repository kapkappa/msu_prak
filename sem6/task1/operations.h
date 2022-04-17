#pragma once

std::vector<double> matvec_multiplication(const dense_matrix&, const std::vector<double>&);

void print(const std::vector<double> &);

std::vector<double> generate_vector(uint32_t);
void generate_vector(const dense_matrix&, double *, uint32_t);

double get_discrepancy(const dense_matrix&, const std::vector<double>&, const std::vector<double>&);

double get_norm(double *, uint32_t);
double get_manhattan_norm(const std::vector<double>& );
double get_error_norm(std::vector<double>);
