// block_model.h -- FrameModel with context.
// 
// Copyright 2005, 2009 Per Abrahamsen and KVL.
//
// This file is part of Daisy.
// 
// Daisy is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// Daisy is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
// 
// You should have received a copy of the GNU Lesser Public License
// along with Daisy; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ifndef BLOCK_MODEL_H
#define BLOCK_MODEL_H

#include "object_model/block_nested.h"
#include "object_model/frame_model.h"

class BlockModel : public BlockNested
{
  const FrameModel& frame_;
public:
  const FrameModel& frame () const;
  BlockModel (const Block&, const FrameModel&, symbol scope_tag);
  ~BlockModel ();
};

#endif // BLOCK_MODEL_H
