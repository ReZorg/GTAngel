    // Component-specific Z++ pickup CParticleObject contract binding (from objects/ZppContracts.hpp)
    static constexpr zpp_objects_spec::CParticleObject_ZppContract makePickupParticleContract() {
        return zpp_objects_spec::apply_CParticleObject_RemoveObject(zpp_objects_spec::CParticleObject_ZppContract{});
    }
    static_assert(zpp_objects_spec::op_CParticleObject_RemoveObject(zpp_objects_spec::CParticleObject_ZppContract{}, makePickupParticleContract()), "Pickups CParticleObject Z++ contract must hold");
    void buildPickups(const avatar3d::GameWorld& world) {
        for (const auto& pk : world.pickups) {
            if (pk.collected) continue;
            const Vec3 c = pk.pos;
            const float r = 0.22f;
            const float sa = world.trafficTime * 2.0f + pk.phase;
            const Vec3 rx{r * cosf(sa), 0.0f, r * sinf(sa)};
            const Vec3 rz{r * sinf(sa), 0.0f, -r * cosf(sa)};
            const Vec3 top = c + Vec3{0, r*1.4f, 0};
            const Vec3 bot = c + Vec3{0, -r*1.4f, 0};
            addTriangle(top, c + rx, c + rz, Colour{255,220,80,255}, ObjectPickup);
            addTriangle(top, c + rz, c - rx, Colour{255,220,80,255}, ObjectPickup);
            addTriangle(bot, c + rx, c + rz, Colour{255,200,60,255}, ObjectPickup);
            addTriangle(bot, c + rz, c - rx, Colour{255,200,60,255}, ObjectPickup);
            const float glow = 0.45f + 0.1f * sinf(world.trafficTime * 3.0f + pk.phase);
            addTriangle(c + Vec3{glow, 0.02f, 0}, c + Vec3{0, 0.02f, glow}, c + Vec3{-glow, 0.02f, 0}, Colour{120,100,40,255}, ObjectPickup);
            addTriangle(c + Vec3{glow, 0.02f, 0}, c + Vec3{-glow, 0.02f, 0}, c + Vec3{0, 0.02f, -glow}, Colour{120,100,40,255}, ObjectPickup);
        }
    }
    // re3 CDummy props: street lamps (pole + lit head).
