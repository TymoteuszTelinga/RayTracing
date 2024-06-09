
#include "Track.h"

Track::Track()
{
}

Track::~Track()
{
}

glm::vec3 Track::GetValue(uint32_t frame) const
{
	if (frame <= m_Frames.front().frame)
	{
		return m_Frames.front().value;
	}

	if (frame >= m_Frames.back().frame)
	{
		return m_Frames.back().value;
	}

	uint32_t upperKey = 0;
	for (; upperKey < m_Frames.size(); upperKey++)
	{
		if (frame <= m_Frames[upperKey].frame)
		{
			break;
		}
	}

	uint32_t lowerKey = upperKey - 1;

	uint32_t startFrame = m_Frames[lowerKey].frame;
	uint32_t endFrame = m_Frames[upperKey].frame;

	float alpha = float(frame - startFrame) / float(endFrame - startFrame);

	return glm::mix(m_Frames[lowerKey].value, m_Frames[upperKey].value, alpha);
}

void Track::AddKeyFrame(const KeyFrame& keyFrame)
{
	for(auto it = m_Frames.begin(); it != m_Frames.end(); it++)
	{
		if (keyFrame.frame == (*it).frame)
		{
			(*it) = keyFrame;
			return;
		}

		if (keyFrame.frame < (*it).frame)
		{
			m_Frames.insert(it, keyFrame);
			return;
		}
	}

	m_Frames.emplace_back(keyFrame);
}
