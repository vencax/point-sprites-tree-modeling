#pragma once

//constants
#define POINT_SPRITES_CUSTOMVERTEX_FVF	( D3DFVF_XYZ | D3DFVF_DIFFUSE )

// custom vertex format for pointsprites without color( color alpha transparency)
#define CLEAR_POINT_SPRITES_FVF ( D3DFVF_XYZ)

#define CUSTOM_BODY_VERTEX_FVF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

//structures
typedef struct sPSVertex_
{
D3DXVECTOR3	vect;
DWORD	dwColor;
} PS_VERTEX;

typedef struct sBodyVertex_ {
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	float tu, tv;			// texture coords
} BODY_VERTEX;