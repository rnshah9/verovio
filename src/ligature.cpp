/////////////////////////////////////////////////////////////////////////////
// Name:        ligature.cpp
// Author:      Don Byrd
// Created:     2016
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "ligature.h"

//----------------------------------------------------------------------------

#include <cassert>
#include <iostream>

//----------------------------------------------------------------------------

#include "doc.h"
#include "dot.h"
#include "editorial.h"
#include "functorparams.h"
#include "note.h"
#include "staff.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// Ligature
//----------------------------------------------------------------------------

static const ClassRegistrar<Ligature> s_factory("ligature", LIGATURE);

Ligature::Ligature() : LayerElement(LIGATURE, "ligature-"), ObjectListInterface(), AttLigatureVis()
{
    this->RegisterAttClass(ATT_LIGATUREVIS);

    this->Reset();
}

Ligature::~Ligature() {}

void Ligature::Reset()
{
    LayerElement::Reset();
    this->ResetLigatureVis();
}

bool Ligature::IsSupportedChild(Object *child)
{
    if (child->Is(DOT)) {
        assert(dynamic_cast<Dot *>(child));
    }
    else if (child->Is(NOTE)) {
        assert(dynamic_cast<Note *>(child));
    }
    else if (child->IsEditorialElement()) {
        assert(dynamic_cast<EditorialElement *>(child));
    }
    else {
        return false;
    }
    return true;
}

Note *Ligature::GetFirstNote()
{
    return const_cast<Note *>(std::as_const(*this).GetFirstNote());
}

const Note *Ligature::GetFirstNote() const
{
    const Note *firstNote = vrv_cast<const Note *>(this->GetListFront(this));
    assert(firstNote);
    return firstNote;
}

Note *Ligature::GetLastNote()
{
    return const_cast<Note *>(std::as_const(*this).GetLastNote());
}

const Note *Ligature::GetLastNote() const
{
    // The first note is the bottom
    const Note *lastNote = vrv_cast<const Note *>(this->GetListBack(this));
    assert(lastNote);
    return lastNote;
}

void Ligature::FilterList(ListOfConstObjects &childList) const
{
    // Retain only note children of ligatures
    ListOfConstObjects::iterator iter = childList.begin();

    while (iter != childList.end()) {
        if (!(*iter)->Is(NOTE)) {
            // remove anything that is not an LayerElement
            iter = childList.erase(iter);
        }
        else {
            // assert that we keep only notes
            assert(dynamic_cast<const Note *>(*iter));
            ++iter;
        }
    }
}

int Ligature::GetDrawingNoteShape(const Note *note) const
{
    assert(note);
    int position = this->GetListIndex(note);
    if (position == -1) return -1;

    // no check because m_drawingShapes will have been filled by then
    return m_drawingShapes.at(position);
}

//----------------------------------------------------------------------------
// Functors methods
//----------------------------------------------------------------------------

int Ligature::CalcLigatureNotePos(FunctorParams *functorParams)
{
    FunctorDocParams *params = vrv_params_cast<FunctorDocParams *>(functorParams);
    assert(params);

    if (params->m_doc->GetOptions()->m_ligatureAsBracket.GetValue()) return FUNCTOR_CONTINUE;

    m_drawingShapes.clear();

    const ListOfObjects &notes = this->GetList(this);
    Note *lastNote = dynamic_cast<Note *>(notes.back());
    Staff *staff = this->GetAncestorStaff();

    if (notes.size() < 2) return FUNCTOR_SIBLINGS;

    Note *previousNote = NULL;
    bool previousUp = false;
    int n1 = 0;
    int n2 = 1;

    bool isMensuralBlack = (staff->m_drawingNotationType == NOTATIONTYPE_mensural_black);
    bool oblique = false;
    if ((notes.size() == 2) && this->GetForm() == LIGATUREFORM_obliqua) oblique = true;

    // For better clarify, we loop withing the Ligature::CalcLigatureNotePos instead of
    // implementing Note::CalcLigatureNotePos.

    for (auto &iter : notes) {

        Note *note = vrv_cast<Note *>(iter);
        assert(note);

        m_drawingShapes.push_back(LIGATURE_DEFAULT);

        if (!previousNote) {
            previousNote = note;
            continue;
        }

        // Look at the @lig attribute on the previous note
        if (previousNote->GetLig() == LIGATUREFORM_obliqua) oblique = true;
        int dur1 = previousNote->GetActualDur();
        int dur2 = note->GetActualDur();
        // Same treatment for Mx and LG execpt for positioning, which is done above
        // We still need to avoid oblique, so keep a flag.
        bool isMaxima = false;
        if (dur1 == DUR_MX) {
            dur1 = DUR_LG;
            isMaxima = true;
        }
        if (dur2 == DUR_MX) dur2 = DUR_LG;

        int diatonicStep = note->GetDiatonicPitch() - previousNote->GetDiatonicPitch();
        bool up = (diatonicStep > 0);
        bool isLastNote = (note == lastNote);

        // L - L
        if ((dur1 == DUR_LG) && (dur2 == DUR_LG)) {
            if (up) {
                m_drawingShapes.at(n1) = LIGATURE_STEM_RIGHT_DOWN;
                m_drawingShapes.at(n2) = LIGATURE_STEM_RIGHT_DOWN;
            }
            else {
                // nothing to change
            }
        }
        // L - B
        else if ((dur1 == DUR_LG) && (dur2 == DUR_BR)) {
            if (up) {
                m_drawingShapes.at(n1) = LIGATURE_STEM_RIGHT_DOWN;
            }
            // automatically set oblique on B, but not with Mx and only at the beginning and end
            else if (!isMaxima && ((n1 == 0) || (isLastNote))) {
                m_drawingShapes.at(n1) = LIGATURE_OBLIQUE;
                // make sure we previous one is not oblique
                if (n1 > 0) {
                    m_drawingShapes.at(n1 - 1) &= ~LIGATURE_OBLIQUE;
                }
            }
        }
        // B - B
        else if ((dur1 == DUR_BR) && (dur2 == DUR_BR)) {
            if (up) {
                // nothing to change
            }
            // automatically set oblique on B only at the beginning and end
            else if ((n1 == 0) || (isLastNote)) {
                m_drawingShapes.at(n1) = LIGATURE_OBLIQUE;
                // make sure we previous one is not oblique
                if (n1 > 0) {
                    m_drawingShapes.at(n1 - 1) &= ~LIGATURE_OBLIQUE;
                }
                else {
                    m_drawingShapes.at(n1) |= LIGATURE_STEM_LEFT_DOWN;
                }
            }
        }
        // B - L
        else if ((dur1 == DUR_BR) && (dur2 == DUR_LG)) {
            if (up) {
                m_drawingShapes.at(n2) = LIGATURE_STEM_RIGHT_DOWN;
            }
            else {
                if (!isLastNote) {
                    m_drawingShapes.at(n2) = LIGATURE_STEM_RIGHT_DOWN;
                }
                if (n1 == 0) {
                    m_drawingShapes.at(n1) = LIGATURE_STEM_LEFT_DOWN;
                }
            }
        }
        // SB - SB
        else if ((dur1 == DUR_1) && (dur2 == DUR_1)) {
            m_drawingShapes.at(n1) = LIGATURE_STEM_LEFT_UP;
        }
        // SB - L (this should not happen on the first two notes, but this is an encoding problem)
        else if ((dur1 == DUR_1) && (dur2 == DUR_LG)) {
            if (up) {
                m_drawingShapes.at(n2) = LIGATURE_STEM_RIGHT_DOWN;
            }
            else {
                // nothing to change
            }
        }
        // SB - B (this should not happen on the first two notes, but this is an encoding problem)
        else if ((dur1 == DUR_1) && (dur2 == DUR_BR)) {
            if (up) {
                // nothing to change
            }
            // only set the oblique with the SB if the following B is not the start of an oblique
            else if (note->GetLig() != LIGATUREFORM_obliqua) {
                m_drawingShapes.at(n1) = LIGATURE_OBLIQUE;
                if (n1 > 0) {
                    m_drawingShapes.at(n1 - 1) &= ~LIGATURE_OBLIQUE;
                }
            }
        }

        // Blindly set the oblique shape without trying to deal with encoding problems
        if (oblique) {
            m_drawingShapes.at(n1) |= LIGATURE_OBLIQUE;
            if (n1 > 0) {
                m_drawingShapes.at(n1 - 1) &= ~LIGATURE_OBLIQUE;
            }
        }

        // With mensural black notation, stack longa going up
        if (isLastNote && isMensuralBlack && (dur2 == DUR_LG) && up) {
            // Stack only if a least a third
            int stackThreshold = 1;
            // If the previous was going down, adjust the threshold
            if ((n1 > 0) && !previousUp) {
                // For oblique, stack but only from a fourth, for recta, never stack them
                stackThreshold = (m_drawingShapes.at(n1 - 1) & LIGATURE_OBLIQUE) ? 2 : -VRV_UNSET;
            }
            if (diatonicStep > stackThreshold) m_drawingShapes.at(n2) = LIGATURE_STACKED;
        }

        oblique = false;
        previousNote = note;
        previousUp = up;
        n1++;
        n2++;
    }

    /**** Set the xRel position for each note ****/

    int previousRight = 0;
    previousNote = NULL;
    n1 = 0;

    for (auto &iter : notes) {

        Note *note = vrv_cast<Note *>(iter);
        assert(note);

        // previousRight is 0 for the first note
        int width = (note->GetDrawingRadius(params->m_doc, true) * 2)
            - params->m_doc->GetDrawingStemWidth(staff->m_drawingStaffSize);
        // With stacked notes, back-track the position
        if (m_drawingShapes.at(n1 + 1) & LIGATURE_STACKED) previousRight -= width;
        note->SetDrawingXRel(previousRight);
        previousRight += width;

        if (!previousNote) {
            previousNote = note;
            continue;
        }

        int diatonicStep = note->GetDiatonicPitch() - previousNote->GetDiatonicPitch();

        // For large interval and oblique, adjust the x position to limit the angle
        if ((m_drawingShapes.at(n1) & LIGATURE_OBLIQUE) && (abs(diatonicStep) > 2)) {
            // angle stay the same from third onward (2 / 3 or a brevis per diatonic step)
            int shift = (abs(diatonicStep) - 2) * width * 2 / 3;
            note->SetDrawingXRel(note->GetDrawingXRel() + shift);
            previousRight += shift;
        }
        previousNote = note;
        n1++;
    }

    return FUNCTOR_SIBLINGS;
}

int Ligature::ResetData(FunctorParams *functorParams)
{
    // Call parent one too
    LayerElement::ResetData(functorParams);

    m_drawingShapes.clear();

    // We want the list of the ObjectListInterface to be re-generated
    this->Modify();
    return FUNCTOR_CONTINUE;
}

} // namespace vrv
