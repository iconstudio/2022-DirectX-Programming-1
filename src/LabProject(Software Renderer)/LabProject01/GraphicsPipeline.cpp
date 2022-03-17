#include "stdafx.h"
#include "GraphicsPipeline.h"

CGameObject* CGraphicsPipeline::m_pGameObject = NULL;
CCamera* CGraphicsPipeline::m_pCamera = NULL;

CPoint3D CGraphicsPipeline::Transform(CPoint3D& f3Model)
{
	CPoint3D f3World = m_pGameObject->WorldTransform(f3Model);
	CPoint3D f3Camera = m_pCamera->CameraTransform(f3World);
	CPoint3D f3Projection = m_pCamera->ProjectionTransform(f3Camera);
	CPoint3D f3Screen = m_pCamera->ScreenTransform(f3Projection);

	return(f3Screen);
}

CPoint3D CGraphicsPipeline::Project(CPoint3D& f3Model)
{
	CPoint3D f3World = m_pGameObject->WorldTransform(f3Model);
	CPoint3D f3Camera = m_pCamera->CameraTransform(f3World);
	CPoint3D f3Projection = m_pCamera->ProjectionTransform(f3Camera);

	return(f3Projection);
}

CPoint3D CGraphicsPipeline::ScreenTransform(CPoint3D& f3Projection)
{
	CPoint3D f3Screen = m_pCamera->ScreenTransform(f3Projection);
	return(f3Screen);
}

