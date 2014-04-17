#ifndef SSLDA_INFERENCE_H
#define SSLDA_INFERENCE_H

#include "utils.h"
#include "model.h"
#include "unistd.h"
#include "stdlib.h"
#include "twtm-estimate.h"

void inference_gamma(Document* doc, twtm_model* model);
void inference_xi(Document* doc, twtm_model* model,Config* config);
void do_inference(Document* doc, twtm_model* model, Config* config);

#endif
