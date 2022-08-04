/////////////////////////////////////////////////////////////////////////////
// Name:        mrpt.cpp
// Author:      Klaus Rettinghaus
// Created:     2018
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "mrpt.h"

//----------------------------------------------------------------------------

#include <cassert>
#include <math.h>

//----------------------------------------------------------------------------

#include "chord.h"
#include "editorial.h"
#include "functorparams.h"
#include "layer.h"
#include "note.h"
#include "staff.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// MRpt
//----------------------------------------------------------------------------

static const ClassRegistrar<MRpt> s_factory("mRpt", MRPT);

MRpt::MRpt() : LayerElement(MRPT, "mrpt-"), AttColor(), AttNumbered(), AttNumberPlacement()
{
    this->RegisterAttClass(ATT_COLOR);
    this->RegisterAttClass(ATT_NUMBERED);
    this->RegisterAttClass(ATT_NUMBERPLACEMENT);

    this->Reset();
}

MRpt::~MRpt() {}

void MRpt::Reset()
{
    LayerElement::Reset();
    this->ResetColor();
    this->ResetNumbered();
    this->ResetNumberPlacement();

    m_drawingMeasureCount = 0;
}

//----------------------------------------------------------------------------
// MRpt functor methods
//----------------------------------------------------------------------------

int MRpt::GenerateMIDI(FunctorParams *functorParams)
{
    // GenerateMIDIParams *params = vrv_params_cast<GenerateMIDIParams *>(functorParams);
    // assert(params);

    LogWarning("MRpt produces empty MIDI output");

    return FUNCTOR_CONTINUE;
}

int MRpt::PrepareRpt(FunctorParams *functorParams)
{
    PrepareRptParams *params = vrv_params_cast<PrepareRptParams *>(functorParams);
    assert(params);

    // If multiNumber is not true, nothing needs to be done
    if (params->m_multiNumber != BOOLEAN_true) {
        return FUNCTOR_CONTINUE;
    }

    // If this is the first one, number has to be 2
    if (params->m_currentMRpt == NULL) {
        m_drawingMeasureCount = 2;
    }
    // Otherwise increment it
    else {
        m_drawingMeasureCount = params->m_currentMRpt->m_drawingMeasureCount + 1;
    }
    params->m_currentMRpt = this;
    return FUNCTOR_CONTINUE;
}

} // namespace vrv
