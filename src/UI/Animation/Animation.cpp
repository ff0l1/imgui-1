#include "Animation.h"

#include <cmath>

namespace UI
{
    namespace Ease
    {
        float OutCubic(float t)
        {
            const float f = t - 1.0f;
            return f * f * f + 1.0f;
        }

        float InOutCubic(float t)
        {
            if (t < 0.5f)
                return 4.0f * t * t * t;
            const float f = -2.0f * t + 2.0f;
            return 1.0f - (f * f * f) / 2.0f;
        }

    }

    AnimatedFloat::AnimatedFloat(float value, float smoothingSeconds)
        : m_Current(value)
        , m_Target(value)
        , m_SmoothingSeconds(smoothingSeconds)
    {
    }

    void AnimatedFloat::SnapTo(float value)
    {
        m_Current = value;
        m_Target = value;
    }

    void AnimatedFloat::Update(float deltaSeconds)
    {
        if (m_SmoothingSeconds <= 0.0f)
        {
            m_Current = m_Target;
            return;
        }

        const float alpha = 1.0f - std::exp(-deltaSeconds / m_SmoothingSeconds);
        m_Current += (m_Target - m_Current) * alpha;

        if (std::fabs(m_Target - m_Current) < 0.0005f)
            m_Current = m_Target;
    }

    Transition::Transition(float durationSeconds)
        : m_DurationSeconds(durationSeconds)
        , m_Progress(1.0f)
        , m_Forward(true)
        , m_Active(false)
    {
    }

    void Transition::Start(bool forward)
    {
        m_Forward = forward;
        m_Progress = 0.0f;
        m_Active = true;
    }

    void Transition::Update(float deltaSeconds)
    {
        if (!m_Active)
            return;

        if (m_DurationSeconds <= 0.0f)
        {
            m_Progress = 1.0f;
            m_Active = false;
            return;
        }

        m_Progress += deltaSeconds / m_DurationSeconds;
        if (m_Progress >= 1.0f)
        {
            m_Progress = 1.0f;
            m_Active = false;
        }
    }
}
