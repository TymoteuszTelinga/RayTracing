#pragma once

class Layer
{
public:
	virtual ~Layer() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}

	virtual void OnUpdate(float DeltaTime) {}
	virtual void OnUIRender() {}
};