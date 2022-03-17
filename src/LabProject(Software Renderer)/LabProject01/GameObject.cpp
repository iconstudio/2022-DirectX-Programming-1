#include "stdafx.h"
#include "GameObject.h"
#include "GraphicsPipeline.h"

CGameObject::~CGameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
}

CPoint3D CGameObject::WorldTransform(CPoint3D& f3Model)
{
	float fPitch = DegreeToRadian(m_fxRotation);
	float fYaw = DegreeToRadian(m_fyRotation);
	float fRoll = DegreeToRadian(m_fzRotation);

	f3Model.x *= m_fxScale;
	f3Model.y *= m_fyScale;
	f3Model.z *= m_fzScale;

	CPoint3D f3World = f3Model;
	CPoint3D f3Rotated = f3Model;

	if (fPitch != 0.0f)
	{
		f3Rotated.y = float(f3World.y * cos(fPitch) - f3World.z * sin(fPitch));
		f3Rotated.z = float(f3World.y * sin(fPitch) + f3World.z * cos(fPitch));
		f3World.y = f3Rotated.y;
		f3World.z = f3Rotated.z;
	}
	if (fYaw != 0.0f)
	{
		f3Rotated.x = float(f3World.x * cos(fYaw) + f3World.z * sin(fYaw));
		f3Rotated.z = float(-f3World.x * sin(fYaw) + f3World.z * cos(fYaw));
		f3World.x = f3Rotated.x;
		f3World.z = f3Rotated.z;
	}
	if (fRoll != 0.0f)
	{
		f3Rotated.x = float(f3World.x * cos(fRoll) - f3World.y * sin(fRoll));
		f3Rotated.y = float(f3World.x * sin(fRoll) + f3World.y * cos(fRoll));
		f3World.x = f3Rotated.x;
		f3World.y = f3Rotated.y;
	}

	f3World.x += m_fxPosition;
	f3World.y += m_fyPosition;
	f3World.z += m_fzPosition;

	return(f3World);
}

void CGameObject::Animate(float fElapsedTime)
{
	Rotate(m_fxRotationSpeed * fElapsedTime, m_fyRotationSpeed * fElapsedTime, m_fzRotationSpeed * fElapsedTime);
}

void CGameObject::Render(HDC hDCFrameBuffer)
{
	CGraphicsPipeline::SetGameObject(this);

	HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

	if (m_pMesh) m_pMesh->Render(hDCFrameBuffer);

	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}
