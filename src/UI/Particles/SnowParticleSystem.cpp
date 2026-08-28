#include "SnowParticleSystem.h"
#include "UI/Theme/Theme.h"

#include "imgui.h"
#include <cmath>

namespace UI
{
    namespace
    {
        constexpr float kTwoPi = 6.28318530718f;

        bool InsideRoundedRect(ImVec2 point, ImVec2 size, float radius)
        {
            const float nearestX = std::max(radius, std::min(point.x, size.x - radius));
            const float nearestY = std::max(radius, std::min(point.y, size.y - radius));
            const float dx = point.x - nearestX;
            const float dy = point.y - nearestY;
            return dx * dx + dy * dy <= radius * radius;
        }
    }

    SnowParticleSystem::SnowParticleSystem()
        : m_Random(std::random_device{}())
        , m_Bounds(0.0f, 0.0f)
    {
    }

    void SnowParticleSystem::Reset(ImVec2 bounds)
    {
        m_Bounds = bounds;
        m_Particles.assign(Theme::Get().Snow().ParticleCount, SnowParticle{});
        for (SnowParticle& particle : m_Particles)
            Respawn(particle, SpawnRegion::Anywhere);
    }

    void SnowParticleSystem::Respawn(SnowParticle& particle, SpawnRegion region)
    {
        const ThemeSnow& snow = Theme::Get().Snow();

        std::uniform_real_distribution<float> unit(0.0f, 1.0f);
        std::uniform_real_distribution<float> sizeDist(snow.MinSize, snow.MaxSize);

        particle.Size = sizeDist(m_Random);
        particle.X = unit(m_Random) * m_Bounds.x;
        particle.Y = (region == SpawnRegion::Anywhere)
            ? unit(m_Random) * m_Bounds.y
            : -particle.Size;
        particle.FallSpeed = snow.MinFallSpeed + unit(m_Random) * (snow.MaxFallSpeed - snow.MinFallSpeed);
        particle.DriftPhase = unit(m_Random) * kTwoPi;
        particle.DriftSpeed = snow.DriftSpeed * (0.5f + unit(m_Random));
        particle.Opacity = snow.MinOpacity + unit(m_Random) * (snow.MaxOpacity - snow.MinOpacity);
        particle.Blurred = unit(m_Random) < snow.BlurredFraction;
    }

    void SnowParticleSystem::Update(float deltaSeconds)
    {
        if (m_Bounds.x <= 0.0f || m_Bounds.y <= 0.0f)
            return;

        for (SnowParticle& particle : m_Particles)
        {
            particle.Y += particle.FallSpeed * deltaSeconds;
            particle.DriftPhase += particle.DriftSpeed * deltaSeconds;

            if (particle.Y - particle.Size > m_Bounds.y)
                Respawn(particle, SpawnRegion::AboveTop);
        }
    }

    void SnowParticleSystem::Render(ImDrawList* drawList) const
    {
        const ThemeSnow& snow = Theme::Get().Snow();
        const ImVec4& base = Theme::Get().Colors().SnowParticle;

        for (const SnowParticle& particle : m_Particles)
        {
            const float drift = std::sin(particle.DriftPhase) * snow.DriftAmount;
            const ImVec2 position(particle.X + drift, particle.Y);
            const float radius = particle.Size * 0.5f;

            if (particle.Blurred)
            {
                const ImU32 halo = ImGui::ColorConvertFloat4ToU32(
                    ImVec4(base.x, base.y, base.z, particle.Opacity * 0.35f));
                drawList->AddCircleFilled(position, radius * 2.1f, halo, 10);
            }

            const ImU32 core = ImGui::ColorConvertFloat4ToU32(
                ImVec4(base.x, base.y, base.z, particle.Opacity));
            drawList->AddCircleFilled(position, radius, core, 8);
        }
    }

    void SnowParticleSystem::RenderInRoundedRect(ImDrawList* drawList, ImVec2 min, ImVec2 max, float radius) const
    {
        const ThemeSnow& snow = Theme::Get().Snow();
        const ImVec4& base = Theme::Get().Colors().SnowParticle;
        const ImVec2 size(max.x - min.x, max.y - min.y);
        drawList->PushClipRect(min, max, true);

        for (const SnowParticle& particle : m_Particles)
        {
            const float drift = std::sin(particle.DriftPhase) * snow.DriftAmount;
            const ImVec2 position(particle.X + drift, particle.Y);
            const ImVec2 local(position.x - min.x, position.y - min.y);
            if (!InsideRoundedRect(local, size, radius))
                continue;

            const float particleRadius = particle.Size * 0.5f;
            if (particle.Blurred)
            {
                const ImU32 halo = ImGui::ColorConvertFloat4ToU32(
                    ImVec4(base.x, base.y, base.z, particle.Opacity * 0.35f));
                drawList->AddCircleFilled(position, particleRadius * 2.1f, halo, 10);
            }

            const ImU32 core = ImGui::ColorConvertFloat4ToU32(
                ImVec4(base.x, base.y, base.z, particle.Opacity));
            drawList->AddCircleFilled(position, particleRadius, core, 8);
        }

        drawList->PopClipRect();
    }
}
