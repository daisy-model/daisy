#ifndef BLOCK_MODEL_H
#define BLOCK_MODEL_H

#include "symbol.h"
#include "model.h"

struct BlockModel : public Model {
  symbol type;
  BlockModel() {}

  symbol type_name() const {
    return this->type;
  }
};
#endif
