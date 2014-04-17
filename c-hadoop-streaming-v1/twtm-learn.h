#ifndef SSLDA_LEARN_H
#define SSLDA_LEARN_H

#include "utils.h"
#include "model.h"

void learn_pi(Document** corpus, twtm_model* model, Config* config);
void learn_theta_phi(Document** corpus, twtm_model* model);
void normalize_log_matrix_rows(double* log_mat, int rows, int cols);

#endif
