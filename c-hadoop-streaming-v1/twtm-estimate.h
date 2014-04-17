#ifndef SSLDA_ESTIMATE_H
#define SSLDA_ESTIMATE_H

#include "utils.h"
#include "model.h"
#include <algorithm>

double compute_doc_likehood(Document* doc, twtm_model* model);

#endif
