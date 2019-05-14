#ifndef POINTLIGHTCLASS_HPP
#define POINTLIGHTCLASS_HPP

#include <DirectXMath.h>


class PointLightClass {
private:
	DirectX::XMFLOAT3 worldPos;


public:
	PointLightClass();
	~PointLightClass();

	DirectX::XMFLOAT3 GetWorldPos();

	void SetWorldPos(DirectX::XMFLOAT3 WorldPosition);


};




#endif