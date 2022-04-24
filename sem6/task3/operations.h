#pragma once

void spmv(const sparse_matrix&, const std::vector<double>&, std::vector<double>&);

double dot(const std::vector<double>& , const std::vector<double>&);

void axpby(double, const std::vector<double>&, double, std::vector<double>&);

void precond(std::vector<double>&, const std::vector<double>&);
void precond(std::vector<double>&, const std::vector<double>&, const std::vector<double>&);

void print(const std::vector<double> &);

std::vector<double> generate_vector(const sparse_matrix&);

double get_discrepancy(const sparse_matrix&, const std::vector<double>&, const std::vector<double>&);

double get_error_norm(std::vector<double>);
double get_norm(const std::vector<double>&);

void set_const(std::vector<double>&, double);

bool check_symmetry(const sparse_matrix&);

#include "operations.inl"
