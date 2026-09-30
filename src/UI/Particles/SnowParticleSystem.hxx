#pragma once

#include "imgui.h"
#include <vector>
#include <random>

struct ImDrawList;

namespace UI
{
    struct SnowParticle
    {
        float X = 0.0f;
        float Y = 0.0f;
        float Size = 0.0f;
        float FallSpeed = 0.0f;
        float DriftPhase = 0.0f;
        float DriftSpeed = 0.0f;
        float Opacity = 0.0f;
        bool Blurred = false;
    };

    class SnowParticleSystem
    {
    public:
        SnowParticleSystem();

        void Reset(ImVec2 bounds);
        void Update(float deltaSeconds);
        void Render(ImDrawList* drawList) const;
        void RenderInRoundedRect(ImDrawList* drawList, ImVec2 min, ImVec2 max, float radius) const;

    private:
        enum class SpawnRegion
        {
            Anywhere,
            AboveTop
        };

        void Respawn(SnowParticle& particle, SpawnRegion region);

        std::vector<SnowParticle> m_Particles;
        std::mt19937 m_Random;
        ImVec2 m_Bounds;
    };
}
