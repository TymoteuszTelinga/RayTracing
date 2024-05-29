
#include "Timer.h"
#include <GL/glew.h>

Timer::Timer()
{
	m_Start = std::chrono::high_resolution_clock::now();
	glGenQueries(1, &m_QueryId);
	glBeginQuery(GL_TIME_ELAPSED, m_QueryId);
}

Timer::~Timer()
{
	m_End = std::chrono::high_resolution_clock::now();
	glDeleteQueries(1, &m_QueryId);
}

float Timer::ElapsedMiliseconds()
{
	m_End = std::chrono::high_resolution_clock::now();
	glEndQuery(GL_TIME_ELAPSED);

	int32_t Done = 0;
	while (!Done)
	{
		glGetQueryObjectiv(m_QueryId,GL_QUERY_RESULT_AVAILABLE,&Done);
	}

	uint32_t GPUTime = 0;
	glGetQueryObjectuiv(m_QueryId, GL_QUERY_RESULT, &GPUTime);

	//m_Duration = m_End - m_Start;
	//float time = std::chrono::duration_cast<std::chrono::milliseconds>(m_Duration).count();
	return (float)GPUTime / 1000000.0f;
}

float Timer::GetCPUTime()
{
	glEndQuery(GL_TIME_ELAPSED);
	m_End = std::chrono::high_resolution_clock::now();
	m_Duration = m_End - m_Start;
	return std::chrono::duration_cast<std::chrono::milliseconds>(m_Duration).count();
}
