/////////////////////////////////////////////////////////////////////////////
// Name:        rend.cpp
// Author:      Laurent Pugin
// Created:     2017
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "rend.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "editorial.h"
#include "lb.h"
#include "text.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// Rend
//----------------------------------------------------------------------------

Rend::Rend() : TextElement("rend-"), AttColor(), AttHorizontalAlign(), AttLang(), AttTypography(), AttWhitespace()

{
    RegisterAttClass(ATT_COLOR);
    RegisterAttClass(ATT_HORIZONTALALIGN);
    RegisterAttClass(ATT_LANG);
    RegisterAttClass(ATT_TYPOGRAPHY);
    RegisterAttClass(ATT_WHITESPACE);

    Reset();
}

Rend::~Rend()
{
}

void Rend::Reset()
{
    TextElement::Reset();
    ResetColor();
    ResetHorizontalAlign();
    ResetLang();
    ResetTypography();
    ResetWhitespace();
}

void Rend::AddChild(Object *child)
{
    if (child->Is(LB)) {
        assert(dynamic_cast<Lb *>(child));
    }
    else if (child->Is(REND)) {
        assert(dynamic_cast<Rend *>(child));
    }
    else if (child->Is(TEXT)) {
        assert(dynamic_cast<Text *>(child));
    }
    else if (child->IsEditorialElement()) {
        assert(dynamic_cast<EditorialElement *>(child));
    }
    else {
        LogError("Adding '%s' to a '%s'", child->GetClassName().c_str(), this->GetClassName().c_str());
        assert(false);
    }

    child->SetParent(this);
    m_children.push_back(child);
    Modify();
}

} // namespace vrv
