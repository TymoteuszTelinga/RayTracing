#pragma once

#include <glm/glm.hpp>
#include <vector>

struct KeyFrame
{
	uint32_t frame;
	glm::vec3 value;
};

class Track
{
public:
	Track();
	~Track();

	glm::vec3 GetValue(uint32_t frame) const;

	void AddKeyFrame(const KeyFrame& keyFrame);
	bool IsValid() const { return !m_Frames.empty(); };
	std::vector<KeyFrame>& GetTrackData() { return m_Frames; };

private:
	std::vector<KeyFrame> m_Frames;
};
