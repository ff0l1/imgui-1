#pragma once

namespace UI
{
    namespace Ease
    {
        float OutCubic(float t);
        float InOutCubic(float t);
    }

    class AnimatedFloat
    {
    public:
        explicit AnimatedFloat(float value = 0.0f, float smoothingSeconds = 0.15f);

        void SetSmoothing(float seconds) { m_SmoothingSeconds = seconds; }
        void SetTarget(float target) { m_Target = target; }
        void SnapTo(float value);
        void Update(float deltaSeconds);

        float Value() const { return m_Current; }

    private:
        float m_Current;
        float m_Target;
        float m_SmoothingSeconds;
    };

    class Transition
    {
    public:
        explicit Transition(float durationSeconds);

        void Start(bool forward);
        void Update(float deltaSeconds);

        float Progress() const { return m_Progress; }
        bool IsActive() const { return m_Active; }
        bool IsForward() const { return m_Forward; }

    private:
        float m_DurationSeconds;
        float m_Progress;
        bool m_Forward;
        bool m_Active;
    };
}
