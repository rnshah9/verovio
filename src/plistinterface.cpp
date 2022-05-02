/////////////////////////////////////////////////////////////////////////////
// Name:        plistinterface.cpp
// Author:      Laurent Pugin
// Created:     2017
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "plistinterface.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "comparison.h"
#include "functorparams.h"
#include "layerelement.h"
#include "measure.h"
#include "staff.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// PlistInterface
//----------------------------------------------------------------------------

PlistInterface::PlistInterface() : Interface(), AttPlist()
{
    this->RegisterInterfaceAttClass(ATT_PLIST);

    this->Reset();
}

PlistInterface::~PlistInterface() {}

void PlistInterface::Reset()
{
    this->ResetPlist();
}

void PlistInterface::AddRef(const std::string &ref)
{
    xsdAnyURI_List references = this->GetPlist();
    if (std::find(references.begin(), references.end(), ref) == references.end()) {
        references.push_back(ref);
        this->SetPlist(references);
    }
}

void PlistInterface::AddRefAllowDuplicate(const std::string &ref)
{
    xsdAnyURI_List references = this->GetPlist();
    references.push_back(ref);
    this->SetPlist(references);
}

void PlistInterface::SetRef(const Object *ref)
{
    if (!IsValidRef(ref)) {
        return;
    }

    if (std::find(m_references.begin(), m_references.end(), ref) == m_references.end()) {
        m_references.push_back(ref);
    }
}

ArrayOfObjects PlistInterface::GetRefs()
{
    ArrayOfObjects result;
    std::transform(m_references.begin(), m_references.end(), std::back_inserter(result),
        [](const Object *obj) { return const_cast<Object *>(obj); });
    return result;
}

void PlistInterface::SetUuidStrs()
{
    assert(m_uuids.empty() && m_references.empty());

    xsdAnyURI_List list = this->GetPlist();
    xsdAnyURI_List::iterator iter;
    for (iter = list.begin(); iter != list.end(); ++iter) {
        std::string uuid = ExtractUuidFragment(*iter);
        if (!uuid.empty()) {
            m_uuids.push_back(uuid);
        }
        else {
            LogError("Cannot parse the anyURI '%s'", (*iter).c_str());
        }
    }
}

//----------------------------------------------------------------------------
// Interface pseudo functor (redirected)
//----------------------------------------------------------------------------

int PlistInterface::InterfacePreparePlist(FunctorParams *functorParams, Object *object)
{
    PreparePlistParams *params = vrv_params_cast<PreparePlistParams *>(functorParams);
    assert(params);

    // This should not happen?
    if (params->m_fillList == false) {
        return FUNCTOR_CONTINUE;
    }

    this->SetUuidStrs();

    std::vector<std::string>::iterator iter;
    for (iter = m_uuids.begin(); iter != m_uuids.end(); ++iter) {
        params->m_interfaceUuidTuples.push_back(std::make_tuple(this, *iter, (Object *)NULL));
    }

    return FUNCTOR_CONTINUE;
}

int PlistInterface::InterfaceResetData(FunctorParams *functorParams, Object *object)
{
    m_uuids.clear();
    m_references.clear();

    return FUNCTOR_CONTINUE;
}

} // namespace vrv
