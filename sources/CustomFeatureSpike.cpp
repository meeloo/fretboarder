//
//  CustomFeatureSpike.cpp
//  Fretboarder
//
//  Throwaway spike proving the editable-custom-feature lifecycle (create ->
//  timeline node -> double-click edit -> recompute) on a trivial parametric
//  box, following the Autodesk "Custom Pocket" pattern. See the header.
//

#include "Fretboarder.h"
#include "CustomFeatureSpike.hpp"

#include <Fusion/Features/CustomFeature.h>
#include <Fusion/Features/CustomFeatureDefinition.h>
#include <Fusion/Features/CustomFeatures.h>
#include <Fusion/Features/CustomFeatureInput.h>
#include <Fusion/Features/CustomFeatureEvents.h>
#include <Fusion/Features/CustomFeatureParameters.h>
#include <Fusion/Features/CustomFeatureParameter.h>
#include <Fusion/Features/BaseFeature.h>
#include <Fusion/Features/BaseFeatures.h>
#include <Fusion/BRep/TemporaryBRepManager.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepBodies.h>
#include <Core/Geometry/OrientedBoundingBox3D.h>

namespace {

const char* kBoxDefId    = "Fretboarder.SpikeBox";
const char* kCreateCmdId = "spikeBoxCreate";
const char* kEditCmdId   = "spikeBoxEdit";
const char* kParamLength = "boxLength";
const char* kParamWidth  = "boxWidth";
const char* kParamHeight = "boxHeight";

Ptr<CustomFeatureDefinition> g_boxDef;
Ptr<CustomFeature>           g_editedCF;
Ptr<TimelineObject>          g_restoreObject;

// Build a temporary box body sitting on the XY plane: L along X, W along Y,
// H along Z. All dimensions in cm (Fusion internal units).
Ptr<BRepBody> make_box_body(double L, double W, double H) {
    auto center = Point3D::create(0, 0, H / 2.0);
    auto xdir = Vector3D::create(1, 0, 0);
    auto ydir = Vector3D::create(0, 1, 0);
    if (!center || !xdir || !ydir) return nullptr;
    auto obb = OrientedBoundingBox3D::create(center, xdir, ydir, L, W, H);
    if (!obb) return nullptr;
    auto mgr = TemporaryBRepManager::get();
    if (!mgr) return nullptr;
    return mgr->createBox(obb);
}

Ptr<BaseFeature> find_base_feature(const Ptr<CustomFeature>& cf) {
    if (!cf) return nullptr;
    for (const auto& f : cf->features()) {
        Ptr<BaseFeature> bf = f->cast<BaseFeature>();
        if (bf) return bf;
    }
    return nullptr;
}

// ---- compute: rebuild the box from parameters and swap it in via updateBody --
class BoxComputeHandler : public CustomFeatureEventHandler {
public:
    void notify(const Ptr<CustomFeatureEventArgs>& args) override {
        if (!args) return;
        auto cf = args->customFeature();
        if (!cf) return;
        auto params = cf->parameters();
        if (!params) return;
        auto pl = params->itemById(kParamLength);
        auto pw = params->itemById(kParamWidth);
        auto ph = params->itemById(kParamHeight);
        if (!pl || !pw || !ph) return;
        auto body = make_box_body(pl->value(), pw->value(), ph->value());
        if (!body) return;
        auto bf = find_base_feature(cf);
        if (!bf) return;
        // Source bodies are only readable while the base feature is in edit mode.
        bf->startEdit();
        auto srcs = bf->sourceBodies();
        if (!srcs.empty() && srcs[0]) {
            bf->updateBody(srcs[0], body);
        }
        bf->finishEdit();
    }
};
BoxComputeHandler g_computeHandler;

// ---- create --------------------------------------------------------------
class BoxCreateExecuteHandler : public CommandEventHandler {
public:
    void notify(const Ptr<CommandEventArgs>& args) override {
        CHECK2(args);
        auto cmd = args->command();
        CHECK2(cmd);
        auto inputs = cmd->commandInputs();
        CHECK2(inputs);
        Ptr<ValueCommandInput> li = inputs->itemById(kParamLength);
        Ptr<ValueCommandInput> wi = inputs->itemById(kParamWidth);
        Ptr<ValueCommandInput> hi = inputs->itemById(kParamHeight);
        CHECK2(li); CHECK2(wi); CHECK2(hi);

        auto design = Fretboarder::app->activeProduct()->cast<Design>();
        CHECK2(design);
        design->designType(ParametricDesignType);
        auto rootComp = design->rootComponent();
        CHECK2(rootComp);
        auto features = rootComp->features();
        CHECK2(features);

        auto body = make_box_body(li->value(), wi->value(), hi->value());
        CHECK2(body);

        auto baseFeat = features->baseFeatures()->add();
        CHECK2(baseFeat);
        baseFeat->startEdit();
        rootComp->bRepBodies()->add(body, baseFeat);
        baseFeat->finishEdit();

        CHECK2(g_boxDef);
        auto cfInput = features->customFeatures()->createInput(g_boxDef);
        CHECK2(cfInput);
        cfInput->addCustomParameter(kParamLength, "Length", ValueInput::createByString(li->expression()), "cm", true);
        cfInput->addCustomParameter(kParamWidth,  "Width",  ValueInput::createByString(wi->expression()), "cm", true);
        cfInput->addCustomParameter(kParamHeight, "Height", ValueInput::createByString(hi->expression()), "cm", true);
        cfInput->setStartAndEndFeatures(baseFeat, baseFeat);
        auto cf = features->customFeatures()->add(cfInput);
        CHECK2(cf);
    }
};
BoxCreateExecuteHandler g_createExecuteHandler;

class BoxCreateCreatedHandler : public CommandCreatedEventHandler {
public:
    void notify(const Ptr<CommandCreatedEventArgs>& args) override {
        CHECK2(args);
        auto cmd = args->command();
        CHECK2(cmd);
        auto inputs = cmd->commandInputs();
        CHECK2(inputs);
        inputs->addValueInput(kParamLength, "Length", "cm", ValueInput::createByReal(5.0));
        inputs->addValueInput(kParamWidth,  "Width",  "cm", ValueInput::createByReal(3.0));
        inputs->addValueInput(kParamHeight, "Height", "cm", ValueInput::createByReal(1.0));
        auto exec = cmd->execute();
        CHECK2(exec);
        exec->add(&g_createExecuteHandler);
    }
};
BoxCreateCreatedHandler g_createCreatedHandler;

// ---- edit ----------------------------------------------------------------
class BoxEditActivateHandler : public CommandEventHandler {
public:
    void notify(const Ptr<CommandEventArgs>& args) override {
        if (!args || !g_editedCF) return;
        auto design = Fretboarder::app->activeProduct()->cast<Design>();
        if (!design) return;
        auto timeline = design->timeline();
        if (!timeline) return;
        int pos = timeline->markerPosition();
        if (pos > 0) g_restoreObject = timeline->item(pos - 1);
        auto tlo = g_editedCF->timelineObject();
        if (tlo) tlo->rollTo(true);
    }
};
BoxEditActivateHandler g_editActivateHandler;

class BoxEditExecuteHandler : public CommandEventHandler {
public:
    void notify(const Ptr<CommandEventArgs>& args) override {
        CHECK2(args);
        if (!g_editedCF) return;
        auto cmd = args->command();
        CHECK2(cmd);
        auto inputs = cmd->commandInputs();
        CHECK2(inputs);
        Ptr<ValueCommandInput> li = inputs->itemById(kParamLength);
        Ptr<ValueCommandInput> wi = inputs->itemById(kParamWidth);
        Ptr<ValueCommandInput> hi = inputs->itemById(kParamHeight);
        CHECK2(li); CHECK2(wi); CHECK2(hi);
        auto params = g_editedCF->parameters();
        CHECK2(params);
        auto pl = params->itemById(kParamLength);
        auto pw = params->itemById(kParamWidth);
        auto ph = params->itemById(kParamHeight);
        CHECK2(pl); CHECK2(pw); CHECK2(ph);
        // Writing the expressions marks the feature dirty -> Fusion fires the
        // compute event, which rebuilds the geometry. The edit command itself
        // never touches geometry.
        pl->expression(li->expression());
        pw->expression(wi->expression());
        ph->expression(hi->expression());
        if (g_restoreObject) g_restoreObject->rollTo(false);
    }
};
BoxEditExecuteHandler g_editExecuteHandler;

class BoxEditCreatedHandler : public CommandCreatedEventHandler {
public:
    void notify(const Ptr<CommandCreatedEventArgs>& args) override {
        CHECK2(args);
        auto cmd = args->command();
        CHECK2(cmd);
        // Fusion runs the edit command with the feature being edited as the
        // active selection.
        g_editedCF = nullptr;
        auto sels = Fretboarder::ui->activeSelections();
        if (sels) {
            for (size_t i = 0; i < sels->count(); ++i) {
                auto ent = sels->item(i)->entity();
                Ptr<CustomFeature> cf = ent ? ent->cast<CustomFeature>() : nullptr;
                if (cf && cf->definition() && cf->definition()->id() == std::string(kBoxDefId)) {
                    g_editedCF = cf;
                    break;
                }
            }
        }
        CHECK2(g_editedCF);
        auto params = g_editedCF->parameters();
        CHECK2(params);
        auto pl = params->itemById(kParamLength);
        auto pw = params->itemById(kParamWidth);
        auto ph = params->itemById(kParamHeight);
        CHECK2(pl); CHECK2(pw); CHECK2(ph);
        auto inputs = cmd->commandInputs();
        CHECK2(inputs);
        inputs->addValueInput(kParamLength, "Length", "cm", ValueInput::createByString(pl->expression()));
        inputs->addValueInput(kParamWidth,  "Width",  "cm", ValueInput::createByString(pw->expression()));
        inputs->addValueInput(kParamHeight, "Height", "cm", ValueInput::createByString(ph->expression()));
        auto act = cmd->activate();
        if (act) act->add(&g_editActivateHandler);
        auto exec = cmd->execute();
        CHECK2(exec);
        exec->add(&g_editExecuteHandler);
    }
};
BoxEditCreatedHandler g_editCreatedHandler;

} // namespace

bool register_custom_feature_spike() {
    auto cmdDefs = Fretboarder::ui->commandDefinitions();
    CHECK(cmdDefs, false);

    // Create the edit command FIRST so editCommandId can bind to a live command.
    auto editCmd = cmdDefs->itemById(kEditCmdId);
    if (!editCmd)
        editCmd = cmdDefs->addButtonDefinition(kEditCmdId, "Edit Spike Box", "Edit the spike box.");
    CHECK(editCmd, false);
    editCmd->commandCreated()->add(&g_editCreatedHandler);

    // Create command + a button in the Solid > Create panel.
    auto createCmd = cmdDefs->itemById(kCreateCmdId);
    if (!createCmd)
        createCmd = cmdDefs->addButtonDefinition(kCreateCmdId, "Spike Box", "Create an editable spike box.");
    CHECK(createCmd, false);
    createCmd->commandCreated()->add(&g_createCreatedHandler);
    auto panel = Fretboarder::ui->allToolbarPanels()->itemById("SolidCreatePanel");
    if (panel) {
        auto ctrl = panel->controls()->addCommand(createCmd);
        if (ctrl) { ctrl->isPromoted(true); ctrl->isPromotedByDefault(true); }
    }

    // Custom feature definition + edit binding + compute handler.
    g_boxDef = CustomFeatureDefinition::create(kBoxDefId, "Spike Box", "");
    CHECK(g_boxDef, false);
    bool eidOk = g_boxDef->editCommandId(kEditCmdId);
    auto computeEvent = g_boxDef->customFeatureCompute();
    if (computeEvent) computeEvent->add(&g_computeHandler);

    // The linchpin diagnostic: did editCommandId actually bind?
    Fretboarder::ui->messageBox(std::string("Spike registered.\neditCommandId(\"")
                                + kEditCmdId + "\") returned: " + (eidOk ? "TRUE" : "FALSE"));
    return true;
}

void unregister_custom_feature_spike() {
    if (!Fretboarder::ui) return;
    auto panel = Fretboarder::ui->allToolbarPanels()->itemById("SolidCreatePanel");
    if (panel) {
        auto c = panel->controls()->itemById(kCreateCmdId);
        if (c) c->deleteMe();
    }
    auto cmdDefs = Fretboarder::ui->commandDefinitions();
    if (cmdDefs) {
        auto cc = cmdDefs->itemById(kCreateCmdId);
        if (cc) cc->deleteMe();
        auto ec = cmdDefs->itemById(kEditCmdId);
        if (ec) ec->deleteMe();
    }
}
