"""Fretboarder - a cross-platform Fusion add-in that generates parametric,
natively-editable fretboards for luthiers.

Editable-custom-feature recipe (hard-won; see project memory):
  1. CustomFeatureDefinition.create needs a VALID icon folder (real 16/32 png).
  2. The document must be Hybrid design intent (Part Design blocks native edit).
Editing rebuilds the geometry from the stored Instrument (rebuild-on-edit); the
whole Instrument is persisted as JSON in the feature's custom named values.
"""
import os
import json
import sys
import traceback

import adsk.core
import adsk.fusion

# Fresh-import our own modules so Stop/Run picks up edits during development.
for _m in [m for m in list(sys.modules) if m in ("generate", "ui") or m == "luthier" or m.startswith("luthier.")]:
    del sys.modules[_m]

import ui as fbui          # noqa: E402
import generate            # noqa: E402
from luthier import Instrument, presets  # noqa: E402

ADDIN_DIR = os.path.dirname(os.path.realpath(__file__))
ICON = os.path.join(ADDIN_DIR, "icons")

DEFID = "Fretboarder.Fretboard"
CREATE_ID = "fretboarderCreate"
EDIT_ID = "fretboarderEdit"
NAMED_VALUE = "instrument"

app = None
ui = None
handlers = []
feature_def = None
edited_cf = None
_last_frt_dir = None


def _default_instrument():
    for name, inst in presets():
        if name == "Stratocaster":
            return _clone(inst)
    return Instrument()


def _clone(inst):
    return Instrument.from_dict(inst.to_dict())


def _build_feature(design, inst_cm):
    """Generate the fretboard and wrap it in a custom feature. Returns the feature."""
    design.designType = adsk.fusion.DesignTypes.ParametricDesignType
    if design.designIntent != adsk.fusion.DesignIntentTypes.HybridDesignIntentType:
        design.designIntent = adsk.fusion.DesignIntentTypes.HybridDesignIntentType

    root = design.rootComponent
    timeline = design.timeline
    start = timeline.markerPosition

    inst_mm = _clone(inst_cm)
    inst_mm.scale(10)  # cm -> mm (the generator works in mm)
    generate.create_fretboard(design, inst_mm, root)

    end = timeline.markerPosition
    cf_input = root.features.customFeatures.createInput(feature_def)
    first_feature = timeline.item(start).entity
    last_feature = timeline.item(end - 1).entity
    cf_input.setStartAndEndFeatures(first_feature, last_feature)
    cf = root.features.customFeatures.add(cf_input)
    cf.customNamedValues.addOrSetValue(NAMED_VALUE, json.dumps(inst_cm.to_dict()))
    return cf


def _instrument_of(cf):
    raw = cf.customNamedValues.value(NAMED_VALUE)
    if raw:
        return Instrument.from_dict(json.loads(raw))
    return _default_instrument()


# ---- shared input handling (presets / load / save) ----
class InputChangedHandler(adsk.core.InputChangedEventHandler):
    def notify(self, args):
        global _last_frt_dir
        try:
            changed = args.input
            inputs = args.inputs
            cid = changed.id
            if cid == fbui.PRESET:
                sel = changed.selectedItem
                if sel and sel.name != "Custom":
                    for name, inst in presets():
                        if name == sel.name:
                            fbui.instrument_to_inputs(inputs, inst)
                            break
            elif cid == fbui.LOAD and changed.value:
                changed.value = False
                dlg = ui.createFileDialog()
                dlg.filter = "Fretboard (*.frt);;All (*.*)"
                dlg.title = "Load fretboard"
                if _last_frt_dir:
                    dlg.initialDirectory = _last_frt_dir
                if dlg.showOpen() == adsk.core.DialogResults.DialogOK:
                    _last_frt_dir = os.path.dirname(dlg.filename)
                    fbui.instrument_to_inputs(inputs, Instrument.load(dlg.filename))
            elif cid == fbui.SAVE and changed.value:
                changed.value = False
                dlg = ui.createFileDialog()
                dlg.filter = "Fretboard (*.frt);;All (*.*)"
                dlg.title = "Save fretboard"
                dlg.isMultiSelectEnabled = False
                if _last_frt_dir:
                    dlg.initialDirectory = _last_frt_dir
                if dlg.showSave() == adsk.core.DialogResults.DialogOK:
                    _last_frt_dir = os.path.dirname(dlg.filename)
                    fbui.instrument_from_inputs(inputs).save(dlg.filename)
        except:
            if ui:
                ui.messageBox("Input change failed:\n{}".format(traceback.format_exc()))


# ---- create ----
class CreateExecuteHandler(adsk.core.CommandEventHandler):
    def notify(self, args):
        try:
            design = adsk.fusion.Design.cast(app.activeProduct)
            inst = fbui.instrument_from_inputs(args.command.commandInputs)
            _build_feature(design, inst)
        except:
            if ui:
                ui.messageBox("Create failed:\n{}".format(traceback.format_exc()))


class CreateCreatedHandler(adsk.core.CommandCreatedEventHandler):
    def notify(self, args):
        try:
            cmd = args.command
            fbui.build_dialog(cmd, _default_instrument())
            on_exec = CreateExecuteHandler()
            cmd.execute.add(on_exec)
            handlers.append(on_exec)
            on_input = InputChangedHandler()
            cmd.inputChanged.add(on_input)
            handlers.append(on_input)
        except:
            if ui:
                ui.messageBox("Create dialog failed:\n{}".format(traceback.format_exc()))


# ---- native edit ----
class EditExecuteHandler(adsk.core.CommandEventHandler):
    def notify(self, args):
        global edited_cf
        try:
            if not edited_cf:
                return
            design = adsk.fusion.Design.cast(app.activeProduct)
            inst = fbui.instrument_from_inputs(args.command.commandInputs)
            edited_cf.deleteMe()   # removes the feature and its grouped geometry
            edited_cf = None
            _build_feature(design, inst)
        except:
            if ui:
                ui.messageBox("Edit failed:\n{}".format(traceback.format_exc()))


class EditCreatedHandler(adsk.core.CommandCreatedEventHandler):
    def notify(self, args):
        global edited_cf
        try:
            cmd = args.command
            edited_cf = None
            sels = ui.activeSelections
            for i in range(sels.count):
                cf = adsk.fusion.CustomFeature.cast(sels.item(i).entity)
                if cf and cf.definition and cf.definition.id == DEFID:
                    edited_cf = cf
                    break
            if not edited_cf:
                return
            fbui.build_dialog(cmd, _instrument_of(edited_cf))
            on_exec = EditExecuteHandler()
            cmd.execute.add(on_exec)
            handlers.append(on_exec)
            on_input = InputChangedHandler()
            cmd.inputChanged.add(on_input)
            handlers.append(on_input)
        except:
            if ui:
                ui.messageBox("Edit dialog failed:\n{}".format(traceback.format_exc()))


class ComputeHandler(adsk.fusion.CustomFeatureEventHandler):
    def notify(self, args):
        # Rebuild-on-edit: the geometry is real features that persist; nothing to
        # recompute here. (Kept so the definition has a compute handler.)
        pass


def run(context):
    global app, ui, feature_def
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        cds = ui.commandDefinitions

        edit_cmd = cds.itemById(EDIT_ID)
        if not edit_cmd:
            edit_cmd = cds.addButtonDefinition(EDIT_ID, "Edit Fretboard", "Edit the fretboard.", ICON)
        on_edit = EditCreatedHandler()
        edit_cmd.commandCreated.add(on_edit)
        handlers.append(on_edit)

        create_cmd = cds.itemById(CREATE_ID)
        if not create_cmd:
            create_cmd = cds.addButtonDefinition(CREATE_ID, "Fretboard", "Create a parametric fretboard.", ICON)
        on_create = CreateCreatedHandler()
        create_cmd.commandCreated.add(on_create)
        handlers.append(on_create)
        panel = ui.allToolbarPanels.itemById("SolidCreatePanel")
        if panel and not panel.controls.itemById(CREATE_ID):
            ctrl = panel.controls.addCommand(create_cmd)
            ctrl.isPromoted = True
            ctrl.isPromotedByDefault = True

        feature_def = adsk.fusion.CustomFeatureDefinition.create(DEFID, "Fretboard", ICON)
        feature_def.editCommandId = EDIT_ID
        on_compute = ComputeHandler()
        feature_def.customFeatureCompute.add(on_compute)
        handlers.append(on_compute)
    except:
        if ui:
            ui.messageBox("Fretboarder run() failed:\n{}".format(traceback.format_exc()))


def stop(context):
    try:
        panel = ui.allToolbarPanels.itemById("SolidCreatePanel")
        if panel:
            c = panel.controls.itemById(CREATE_ID)
            if c:
                c.deleteMe()
        for cid in (CREATE_ID, EDIT_ID):
            d = ui.commandDefinitions.itemById(cid)
            if d:
                d.deleteMe()
    except:
        if ui:
            ui.messageBox("Fretboarder stop() failed:\n{}".format(traceback.format_exc()))
