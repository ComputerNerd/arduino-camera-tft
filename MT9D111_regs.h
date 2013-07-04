#pragma once
#include "config.h"
#ifdef MT9D111
extern const struct regval_list MT9D111_RGB565[];
extern const struct regval_list MT9D111_QVGA[];
extern const struct regval_list MT9D111_VGA[];
extern const struct regval_listP MT9D111_init[];
extern const struct regval_list default_size_a_list[];
extern const struct regval_list default_size_b_list[];
extern const struct regval_listP MT9D111_refresh[];
#endif
