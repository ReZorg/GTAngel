#pragma once
    // Component-specific Z++ quest shard CObjectInfo contract binding (from objects/ZppContracts.hpp)
    static constexpr zpp_objects_spec::CObjectInfo_ZppContract makeQuestShardObjectContract() {
        return zpp_objects_spec::CObjectInfo_ZppContract{};
    }
    [[nodiscard]] bool verifyLiveWorld_questShard_zpp(const avatar3d::GameWorld& world) const {
        zpp_objects_spec::CObjectInfo_ZppContract c{};
        const bool shardFinite = std::isfinite(world.questShard.x) && std::isfinite(world.questShard.y) && std::isfinite(world.questShard.z);
        c.m_nCollisionDamageEffect = shardFinite ? 1 : -1;
        return c.checkInvariant();
    }
    static_assert(makeQuestShardObjectContract().checkInvariant(), "QuestShard CObjectInfo Z++ contract must hold");
    void buildQuestShard(const avatar3d::GameWorld& world) {
        if (!verifyLiveWorld_questShard_zpp(world)) { noteValidationFailure("questShard"); return; }
        if (!world.questShardVisible) return;
        const auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        const float bob = 0.62f + 0.12f * sinf((float)(ticks % 1000000) * 0.005f);
        const float radius = 0.28f;
        const Vec3 c = world.questShard + Vec3{0.0f, bob, 0.0f};
        const Vec3 top = c + Vec3{0, radius * 1.5f, 0};
        const Vec3 bottom = c + Vec3{0, -radius * 1.5f, 0};
        const std::array<Vec3, 4> ring{{
            c + Vec3{radius, 0, 0}, c + Vec3{0, 0, radius},
            c + Vec3{-radius, 0, 0}, c + Vec3{0, 0, -radius}}};
        const Colour shard{72, 224, 244, 255};
            const Colour halo{26, 66, 74, 255};
            for (int i = 0; i < 8; ++i) {
                const float a0 = (float)i * 0.7854f, a1 = (float)(i + 1) * 0.7854f;
                const float hr = radius * 1.9f;
                const Vec3 q0 = {c.x + hr * cosf(a0), c.y + hr * sinf(a0), c.z};
                const Vec3 q1 = {c.x + hr * cosf(a1), c.y + hr * sinf(a1), c.z};
                addTriangle(c, q0, q1, halo, ObjectQuestItem);
            }
        for (int i = 0; i < 4; ++i) {
            const int next = (i + 1) % 4;
            addTriangle(top, ring[i], ring[next], shard, ObjectQuestItem);
            addTriangle(bottom, ring[next], ring[i], Colour{34, 154, 225, 255}, ObjectQuestItem);
        }
    }

