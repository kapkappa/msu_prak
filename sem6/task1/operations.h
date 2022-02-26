#pragma once

dense_matrix matrix_multiplication(const dense_matrix&, const dense_matrix&);

std::vector<double> matvec_multiplication(const dense_matrix&, const std::vector<double>&);

uint64_t get_norm (const std::vector<double>&);
