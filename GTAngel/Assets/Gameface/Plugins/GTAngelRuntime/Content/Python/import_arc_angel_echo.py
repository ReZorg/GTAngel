"""Reproducibly import the Arc Angel Echo source package into Gameface.

Run inside Unreal Editor:
  py "<project>/Plugins/GTAngelRuntime/Content/Python/import_arc_angel_echo.py"

The script deliberately treats the LFS source files and JSON manifest as the
source of truth. Generated .uasset files remain build products.
"""

from __future__ import annotations

import json
import hashlib
from pathlib import Path
import unreal

DESTINATION = "/Game/GTAngel/Avatars/ArcAngelEcho"


def _package_root() -> Path:
    project_dir = Path(unreal.Paths.project_dir()).resolve()
    return (project_dir.parent / "Avatars" / "ArcAngelEcho" / "v1").resolve()


def _load_manifest(root: Path) -> dict:
    with (root / "arc-angel-echo.avatar.json").open("r", encoding="utf-8") as handle:
        return json.load(handle)


def _asset_path(manifest: dict, root: Path, key: str) -> str:
    entry = next(asset for asset in manifest["assets"] if asset["key"] == key)
    path = (root / entry["path"]).resolve()
    path.relative_to(root)
    if path.stat().st_size != entry["sizeBytes"]:
        raise RuntimeError(f"{key} byte length does not match the signed manifest")
    digest = hashlib.sha256(path.read_bytes()).hexdigest()
    if digest.lower() != entry["sha256"].lower():
        raise RuntimeError(f"{key} SHA-256 does not match the signed manifest")
    return str(path)


def _import_skeletal_mesh(source_file: str) -> unreal.SkeletalMesh:
    options = unreal.FbxImportUI()
    options.import_mesh = True
    options.import_animations = False
    options.import_as_skeletal = True
    options.mesh_type_to_import = unreal.FBXImportType.FBXIT_SKELETAL_MESH
    options.create_physics_asset = True
    options.skeletal_mesh_import_data.import_morph_targets = False
    options.skeletal_mesh_import_data.preserve_smoothing_groups = True
    options.skeletal_mesh_import_data.use_t0_as_ref_pose = True
    options.skeletal_mesh_import_data.convert_scene_unit = True

    task = unreal.AssetImportTask()
    task.filename = source_file
    task.destination_path = DESTINATION
    task.destination_name = "SK_ArcAngelEcho"
    task.automated = True
    task.save = True
    task.replace_existing = True
    task.options = options
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

    mesh = unreal.load_asset(f"{DESTINATION}/SK_ArcAngelEcho")
    if not isinstance(mesh, unreal.SkeletalMesh):
        raise RuntimeError("Arc Angel skeletal mesh import did not produce SK_ArcAngelEcho")
    return mesh


def _import_animation(source_file: str, name: str, skeleton: unreal.Skeleton) -> None:
    options = unreal.FbxImportUI()
    options.import_mesh = False
    options.import_animations = True
    options.import_as_skeletal = True
    options.mesh_type_to_import = unreal.FBXImportType.FBXIT_ANIMATION
    options.skeleton = skeleton
    options.anim_sequence_import_data.import_bone_tracks = True
    options.anim_sequence_import_data.preserve_local_transform = True
    options.anim_sequence_import_data.import_custom_attribute = True

    task = unreal.AssetImportTask()
    task.filename = source_file
    task.destination_path = DESTINATION
    task.destination_name = name
    task.automated = True
    task.save = True
    task.replace_existing = True
    task.options = options
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])


def _import_texture(source_file: str, name: str) -> None:
    task = unreal.AssetImportTask()
    task.filename = source_file
    task.destination_path = DESTINATION
    task.destination_name = name
    task.automated = True
    task.save = True
    task.replace_existing = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

    texture = unreal.load_asset(f"{DESTINATION}/{name}")
    if isinstance(texture, unreal.Texture2D) and name.endswith("_Normal"):
        texture.set_editor_property("srgb", False)
        texture.set_editor_property(
            "compression_settings", unreal.TextureCompressionSettings.TC_NORMALMAP
        )
        unreal.EditorAssetLibrary.save_loaded_asset(texture, only_if_is_dirty=False)
    elif isinstance(texture, unreal.Texture2D) and (
        name.endswith("_Metallic") or name.endswith("_Roughness")
    ):
        texture.set_editor_property("srgb", False)
        texture.set_editor_property(
            "compression_settings", unreal.TextureCompressionSettings.TC_MASKS
        )
        unreal.EditorAssetLibrary.save_loaded_asset(texture, only_if_is_dirty=False)


def _create_aura_material(mesh: unreal.SkeletalMesh) -> None:
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    material = unreal.load_asset(f"{DESTINATION}/M_ArcAngelEcho")
    if material is None:
        material = asset_tools.create_asset(
            "M_ArcAngelEcho", DESTINATION, unreal.Material, unreal.MaterialFactoryNew()
        )
    if not isinstance(material, unreal.Material):
        raise RuntimeError("M_ArcAngelEcho is not a Material asset")

    editing = unreal.MaterialEditingLibrary
    editing.delete_all_material_expressions(material)
    material.set_editor_property("two_sided", True)

    def texture_parameter(name: str, asset_name: str, x: int, y: int):
        expression = editing.create_material_expression(
            material, unreal.MaterialExpressionTextureSampleParameter2D, x, y
        )
        expression.set_editor_property("parameter_name", name)
        expression.set_editor_property("texture", unreal.load_asset(f"{DESTINATION}/{asset_name}"))
        return expression

    base = texture_parameter("BaseColor", "T_ArcAngelEcho_BaseColor", -900, -250)
    normal = texture_parameter("Normal", "T_ArcAngelEcho_Normal", -900, 50)
    metallic = texture_parameter("Metallic", "T_ArcAngelEcho_Metallic", -900, 250)
    roughness = texture_parameter("Roughness", "T_ArcAngelEcho_Roughness", -900, 450)
    normal.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)

    aura_intensity = editing.create_material_expression(
        material, unreal.MaterialExpressionScalarParameter, -650, -500
    )
    aura_intensity.set_editor_property("parameter_name", "AuraIntensity")
    aura_intensity.set_editor_property("default_value", 0.35)
    aura_color = editing.create_material_expression(
        material, unreal.MaterialExpressionVectorParameter, -650, -650
    )
    aura_color.set_editor_property("parameter_name", "AuraColor")
    aura_color.set_editor_property("default_value", unreal.LinearColor(0.45, 0.65, 1.0, 1.0))

    aura_multiply = editing.create_material_expression(
        material, unreal.MaterialExpressionMultiply, -350, -575
    )
    editing.connect_material_expressions(aura_color, "", aura_multiply, "A")
    editing.connect_material_expressions(aura_intensity, "", aura_multiply, "B")
    aura_mask = editing.create_material_expression(
        material, unreal.MaterialExpressionMultiply, -100, -500
    )
    editing.connect_material_expressions(base, "RGB", aura_mask, "A")
    editing.connect_material_expressions(aura_multiply, "", aura_mask, "B")

    editing.connect_material_property(base, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)
    editing.connect_material_property(normal, "RGB", unreal.MaterialProperty.MP_NORMAL)
    editing.connect_material_property(metallic, "R", unreal.MaterialProperty.MP_METALLIC)
    editing.connect_material_property(roughness, "R", unreal.MaterialProperty.MP_ROUGHNESS)
    editing.connect_material_property(aura_mask, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    editing.set_material_usage(material, unreal.MaterialUsage.MATUSAGE_SKELETAL_MESH)
    editing.recompile_material(material)
    unreal.EditorAssetLibrary.save_loaded_asset(material, only_if_is_dirty=False)

    material_slots = list(mesh.get_editor_property("materials"))
    if not material_slots:
        slot = unreal.SkeletalMaterial()
        slot.set_editor_property("material_interface", material)
        slot.set_editor_property("material_slot_name", "ArcAngelEcho")
        material_slots.append(slot)
    else:
        for slot in material_slots:
            slot.set_editor_property("material_interface", material)
    mesh.set_editor_property("materials", material_slots)
    unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False)


def main() -> None:
    root = _package_root()
    manifest = _load_manifest(root)
    unreal.log(f"Importing {manifest['name']} {manifest['version']} from {root}")

    mesh = _import_skeletal_mesh(_asset_path(manifest, root, "Mesh"))
    skeleton = mesh.get_editor_property("skeleton")
    if skeleton is None:
        raise RuntimeError("SK_ArcAngelEcho has no generated skeleton")

    _import_animation(_asset_path(manifest, root, "Walk"), "A_ArcAngelEcho_Walk", skeleton)
    _import_animation(_asset_path(manifest, root, "Run"), "A_ArcAngelEcho_Run", skeleton)
    _import_texture(_asset_path(manifest, root, "BaseColor"), "T_ArcAngelEcho_BaseColor")
    _import_texture(_asset_path(manifest, root, "Normal"), "T_ArcAngelEcho_Normal")
    _import_texture(_asset_path(manifest, root, "Metallic"), "T_ArcAngelEcho_Metallic")
    _import_texture(_asset_path(manifest, root, "Roughness"), "T_ArcAngelEcho_Roughness")
    _create_aura_material(mesh)

    unreal.EditorAssetLibrary.save_directory(DESTINATION, only_if_is_dirty=False, recursive=True)
    unreal.log("Arc Angel Echo import complete. Generate production LODs before cooking.")


if __name__ == "__main__":
    main()
