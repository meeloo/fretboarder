//
//  OnExecutePreviewEventHandler.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#include "Fretboarder.h"

static void AddLine(std::vector<double>& vecCoords, const Point& _p0, const Point& _p1) {
    auto p0 = create_point(_p0);
    auto p1 = create_point(_p1);
    vecCoords.push_back(p0->x());
    vecCoords.push_back(p0->y());
    vecCoords.push_back(p0->z());
    vecCoords.push_back(p1->x());
    vecCoords.push_back(p1->y());
    vecCoords.push_back(p1->z());
}

static void AddLines(std::vector<double>& vecCoords, const Quad& q) {
    for (size_t i = 0; i < 4; i++) {
        AddLine(vecCoords, q.points[i % 4], q.points[(i + 1) % 4]);
    }
}

void OnExecutePreviewEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    // Interpret instrument inputs
    auto command = eventArgs->command();
    Ptr<CommandInputs> inputs = command->commandInputs();
    if (!inputs)
        return;

    auto instrument = InstrumentFromInputs(inputs);
    fretboarder::Fretboard fretboard(instrument);

    //  get selection entity first since it's fragile and any creation/edit operations will clear the selection.
    if (!Fretboarder::cgGroups)
        return;
    Ptr<CustomGraphicsGroup> cgGroup = Fretboarder::cgGroups->add();
    if (!cgGroup)
        return;

    Ptr<CustomGraphicsEntity> cgEnt = nullptr;

    std::vector<double> vecCoords;

    std::vector<int> vertexIndexList;
    std::vector<int> vecStripLen;

    // Draw the strings:
    if (instrument.draw_strings) {
        for (auto s : fretboard.strings()) {
            AddLine(vecCoords, s.point_at_nut(), s.point_at_bridge());
        }
    }

    {
        // Draw bridge line
        AddLine(vecCoords, fretboard.strings().front().point_at_bridge(), fretboard.strings().back().point_at_bridge());
    }

    // Draw the fret positions
    for (auto s : fretboard.fret_lines()) {
        AddLine(vecCoords, s.point1, s.point2);
    }

    // Draw the frettboard shape
    AddLines(vecCoords, fretboard.board_shape());

    // Draw the nut slot
    if (instrument.carve_nut_slot) {
        AddLines(vecCoords, fretboard.nut_shape());
    }

    Ptr<CustomGraphicsCoordinates> coordinates = CustomGraphicsCoordinates::create(vecCoords);
    if (!coordinates)
        return;

    Ptr<CustomGraphicsLines> cgLines = cgGroup->addLines(coordinates, vertexIndexList, false, vecStripLen);
    applyLinesProperties(cgLines);
}

void OnExecutePreviewEventHandler::applyLinesProperties(Ptr<CustomGraphicsLines> cgLines)
{
    if (!cgLines)
        return;

    cgLines->lineStylePattern(continuousLineStylePattern);
}

