/////////////////////////////////////////////////////////////////////////////
// Name:        text.cpp
// Author:      Laurent Pugin
// Created:     2015
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "text.h"

//----------------------------------------------------------------------------

#include <cassert>

//----------------------------------------------------------------------------

#include "editorial.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// Text
//----------------------------------------------------------------------------

Text::Text() : TextElement(TEXT, "text-")
{
    this->Reset();
}

Text::~Text() {}

void Text::Reset()
{
    TextElement::Reset();
}

} // namespace vrv
