#pragma once
// Master Z++ Formal Specification Aggregator across all 22 GTAngel src/ subsystems
#include "../animation/ZppContracts.hpp"
#include "../audio/ZppContracts.hpp"
#include "../buildings/ZppContracts.hpp"
#include "../collision/ZppContracts.hpp"
#include "../control/ZppContracts.hpp"
#include "../core/ZppContracts.hpp"
#include "../docs/ZppContracts.hpp"
#include "../entities/ZppContracts.hpp"
#include "../extras/ZppContracts.hpp"
#include "../fakerw/ZppContracts.hpp"
#include "../leeds/ZppContracts.hpp"
#include "../math/ZppContracts.hpp"
#include "../modelinfo/ZppContracts.hpp"
#include "../objects/ZppContracts.hpp"
#include "../peds/ZppContracts.hpp"
#include "../renderer/ZppContracts.hpp"
#include "../rw/ZppContracts.hpp"
#include "../save/ZppContracts.hpp"
#include "../skel/ZppContracts.hpp"
#include "../text/ZppContracts.hpp"
#include "../vehicles/ZppContracts.hpp"
#include "../weapons/ZppContracts.hpp"
namespace zpp_master_spec {
constexpr int totalZppModules() {
    return zpp_animation_spec::kZppModuleCount + zpp_audio_spec::kZppModuleCount + zpp_buildings_spec::kZppModuleCount + zpp_collision_spec::kZppModuleCount + zpp_control_spec::kZppModuleCount + zpp_core_spec::kZppModuleCount + zpp_docs_spec::kZppModuleCount + zpp_entities_spec::kZppModuleCount + zpp_extras_spec::kZppModuleCount + zpp_fakerw_spec::kZppModuleCount + zpp_leeds_spec::kZppModuleCount + zpp_math_spec::kZppModuleCount + zpp_modelinfo_spec::kZppModuleCount + zpp_objects_spec::kZppModuleCount + zpp_peds_spec::kZppModuleCount + zpp_renderer_spec::kZppModuleCount + zpp_rw_spec::kZppModuleCount + zpp_save_spec::kZppModuleCount + zpp_skel_spec::kZppModuleCount + zpp_text_spec::kZppModuleCount + zpp_vehicles_spec::kZppModuleCount + zpp_weapons_spec::kZppModuleCount;
}
constexpr int totalZppSchemas() {
    return zpp_animation_spec::kZppSchemaCount + zpp_audio_spec::kZppSchemaCount + zpp_buildings_spec::kZppSchemaCount + zpp_collision_spec::kZppSchemaCount + zpp_control_spec::kZppSchemaCount + zpp_core_spec::kZppSchemaCount + zpp_docs_spec::kZppSchemaCount + zpp_entities_spec::kZppSchemaCount + zpp_extras_spec::kZppSchemaCount + zpp_fakerw_spec::kZppSchemaCount + zpp_leeds_spec::kZppSchemaCount + zpp_math_spec::kZppSchemaCount + zpp_modelinfo_spec::kZppSchemaCount + zpp_objects_spec::kZppSchemaCount + zpp_peds_spec::kZppSchemaCount + zpp_renderer_spec::kZppSchemaCount + zpp_rw_spec::kZppSchemaCount + zpp_save_spec::kZppSchemaCount + zpp_skel_spec::kZppSchemaCount + zpp_text_spec::kZppSchemaCount + zpp_vehicles_spec::kZppSchemaCount + zpp_weapons_spec::kZppSchemaCount;
}
constexpr int totalZppEnums() {
    return zpp_animation_spec::kZppEnumCount + zpp_audio_spec::kZppEnumCount + zpp_buildings_spec::kZppEnumCount + zpp_collision_spec::kZppEnumCount + zpp_control_spec::kZppEnumCount + zpp_core_spec::kZppEnumCount + zpp_docs_spec::kZppEnumCount + zpp_entities_spec::kZppEnumCount + zpp_extras_spec::kZppEnumCount + zpp_fakerw_spec::kZppEnumCount + zpp_leeds_spec::kZppEnumCount + zpp_math_spec::kZppEnumCount + zpp_modelinfo_spec::kZppEnumCount + zpp_objects_spec::kZppEnumCount + zpp_peds_spec::kZppEnumCount + zpp_renderer_spec::kZppEnumCount + zpp_rw_spec::kZppEnumCount + zpp_save_spec::kZppEnumCount + zpp_skel_spec::kZppEnumCount + zpp_text_spec::kZppEnumCount + zpp_vehicles_spec::kZppEnumCount + zpp_weapons_spec::kZppEnumCount;
}
constexpr bool verifyAll22Subsystems() {
    return zpp_animation_spec::verify_animation_zpp_invariants() && zpp_audio_spec::verify_audio_zpp_invariants() && zpp_buildings_spec::verify_buildings_zpp_invariants() && zpp_collision_spec::verify_collision_zpp_invariants() && zpp_control_spec::verify_control_zpp_invariants() && zpp_core_spec::verify_core_zpp_invariants() && zpp_docs_spec::verify_docs_zpp_invariants() && zpp_entities_spec::verify_entities_zpp_invariants() && zpp_extras_spec::verify_extras_zpp_invariants() && zpp_fakerw_spec::verify_fakerw_zpp_invariants() && zpp_leeds_spec::verify_leeds_zpp_invariants() && zpp_math_spec::verify_math_zpp_invariants() && zpp_modelinfo_spec::verify_modelinfo_zpp_invariants() && zpp_objects_spec::verify_objects_zpp_invariants() && zpp_peds_spec::verify_peds_zpp_invariants() && zpp_renderer_spec::verify_renderer_zpp_invariants() && zpp_rw_spec::verify_rw_zpp_invariants() && zpp_save_spec::verify_save_zpp_invariants() && zpp_skel_spec::verify_skel_zpp_invariants() && zpp_text_spec::verify_text_zpp_invariants() && zpp_vehicles_spec::verify_vehicles_zpp_invariants() && zpp_weapons_spec::verify_weapons_zpp_invariants();
}
static_assert(verifyAll22Subsystems(), "All 22 Z++ subsystem formal specifications (positive + negative cases + operations) must hold");
} // namespace zpp_master_spec
