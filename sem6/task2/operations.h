#pragma once

std::vector<double> matvec_multiplication(const dense_matrix&, double *);

void print(const std::vector<double> &);
void print(double *, uint32_t);

std::vector<double> generate_vector(uint32_t);
std::vector<double> generate_vector(const dense_matrix&, uint32_t);
void generate_vector(const dense_matrix&, double *, uint32_t);

double get_discrepancy(const dense_matrix&, double *, double *);

double get_manhattan_norm(const std::vector<double>& );
double get_error_norm(std::vector<double>);
double get_error_norm(double *, uint32_t);
double get_norm(double *, uint32_t);
