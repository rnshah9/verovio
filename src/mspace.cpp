/////////////////////////////////////////////////////////////////////////////
// Name:        mspace.cpp
// Author:      Klaus Rettinghaus
// Created:     2019
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "mspace.h"
//----------------------------------------------------------------------------

#include <cassert>

//----------------------------------------------------------------------------

#include "functorparams.h"

namespace vrv {

//----------------------------------------------------------------------------
// MSpace
//----------------------------------------------------------------------------

static const ClassRegistrar<MSpace> s_factory("mSpace", MSPACE);

MSpace::MSpace() : LayerElement(MSPACE, "mSpace-")
{
    this->Reset();
}

MSpace::~MSpace() {}

void MSpace::Reset()
{
    LayerElement::Reset();
}

//----------------------------------------------------------------------------
// Functors methods
//----------------------------------------------------------------------------

} // namespace vrv
