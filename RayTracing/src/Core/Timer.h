#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	~Timer();

	float ElapsedMiliseconds();
	float GetCPUTime();

private:
	std::chrono::time_point<std::chrono::steady_clock> m_Start, m_End;
	std::chrono::duration<float> m_Duration;

	uint32_t m_QueryId;
};