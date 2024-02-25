

//*****************************************************************************
// �萔�o�b�t�@
//*****************************************************************************

// �}�g���N�X�o�b�t�@
cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}

cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}

cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}

// �}�e���A���o�b�t�@
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte���E�p
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ���C�g�p�o�b�t�@
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte���E�p
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
	float		Dummy[3];//16byte���E�p
};

// �t�H�O�p�o�b�t�@
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// �����p�o�b�t�@
cbuffer Fuchi : register(b6)
{
	int			fuchi;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

// dissolve
struct DISSOLVE
{
	int		Enable;
	float	threshold;
	int		fill[2];
};

cbuffer DissolveBuffer : register(b8)
{
	DISSOLVE dissolve;
}

// �Z�s�A�p�o�b�t�@
cbuffer Sepia : register(b9)
{
	int			sepia;
	int			fake[3];
};

// ���]�F�p�o�b�t�@
cbuffer Reversal : register(b10)
{
	int			   reversal;
	int				fake1[3];
};

// ���m�N��
cbuffer Monochrome : register(b11)
{
	int			   monochrome;
	int				 fake2[3];
};

//=============================================================================
// ���_�V�F�[�_
//=============================================================================
void VertexShaderPolygon( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float4 outNormal		: NORMAL0,
						  out float2 outTexCoord	: TEXCOORD0,
						  out float4 outDiffuse		: COLOR0,
						  out float4 outWorldPos    : POSITION0)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);

	outDiffuse = inDiffuse;
}



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
// dissolve
Texture2D		g_Fractalnoise : register(t1);

SamplerState	g_SamplerState : register( s0 );


//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
void PixelShaderPolygon( in  float4 inPosition		: SV_POSITION,
						 in  float4 inNormal		: NORMAL0,
						 in  float2 inTexCoord		: TEXCOORD0,
						 in  float4 inDiffuse		: COLOR0,
						 in  float4 inWorldPos      : POSITION0,

						 out float4 outDiffuse		: SV_Target )
{
	float4 color;

	if (Material.noTexSampling == 0)
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else
	{
		color = inDiffuse;
	}

	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 5; i++)
		{
			float3 lightDir;
			float light;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light = dot(lightDir, inNormal.xyz);

					light = 0.5 - 0.5 * light;
					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
				}
				else if (Light.Flags[i].x == 2)
				{
					lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
					light = dot(lightDir, inNormal.xyz);

					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

					float distance = length(inWorldPos - Light.Position[i]);

					float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
					tempColor *= att;
				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}

				outColor += tempColor;
			}
		}

		color = outColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}

	//�t�H�O
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}

	//�����
	if (fuchi == 1)
	{
		float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));


		//if ((angle < 0.5f)&&(angle > -0.5f))
		if (angle > -0.9f)
		{
			outDiffuse.rb  = 1.0f;
			outDiffuse.g = 0.0f;			
		}
	}

	// dissolve(�n����)
	{
		// �摜����UV���W���Ƃ��Ă��ĂP�s�N�Z�����Ƃ̎w�肳�ꂽ�F���Ƃ�
		// g_SamplerState ���F�̎��� ����̂������Ń��[�v���ĂƂ��

		// �m�C�Y�e�N�X�`������I�u�W�F�N�g�̃e�N�X�`�����W�ɑΉ�����
		// �m�C�Y�l���T���v�����O���Ă���
		float noise = g_Fractalnoise.Sample(g_SamplerState, inTexCoord);

		//dissolve
		if (dissolve.Enable == 1)
		{
			// �F��RGBa���邩��
			float4 noise = g_Fractalnoise.Sample(g_SamplerState, inTexCoord);

			// �m�C�Y�l���Ⴂ�ꍇ�����ɂ��Ă���
			if (noise.r < dissolve.threshold)
			{
				outDiffuse.a = 0.0f;
			}
			else if (noise.r < dissolve.threshold + 0.1f)
			{

				// dissolve�̋��E����
				// �m�C�Y�l��dissolve�̕ϐ��̍���10�{�ɂ��āA������Ă���
				// ���E�����̐F��ݒ肷��ϐ�

				float temp = (noise.r - dissolve.threshold) * 10.0f;

				// �F�����߂Ă���
				outDiffuse.rbg = temp;
				outDiffuse.r = 1.0f;
			}
		}
	}

	// �Z�s�A
	{
		if (sepia == 1)
		{
			float gray = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;

			color.r = 0.8 * gray;
			color.g = 0.6 * gray;
			color.b = 0.4 * gray;

			outDiffuse = color;
		}
	}

	// ���]�F
	{
		if (reversal == 1)
		{
			//color = g_Texture.Sample(g_SamplerState, inTexCoord);
				float4 inverted_color = 1 - color;
				inverted_color.a = color.a;
				inverted_color.rgb *= inverted_color.a;

				outDiffuse = inverted_color;
		}
	}

	// ���m�N��
	{
		if (monochrome == 1)
		{
			color = g_Texture.Sample(g_SamplerState, inTexCoord);
			float4 gray = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;
			gray.a = 1.0;

			color = gray;

			outDiffuse = color;
		}
	}


}


