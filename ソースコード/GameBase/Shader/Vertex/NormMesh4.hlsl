struct VertexInput {
	float3 pos : POSITION0;//座標
	float3 norm : NORMAL;//法線
	float4 dif : COLOR0;//ディフューズカラー
	float4 spec : COLOR1;//スペキュラカラー
	float4 uv0 : TEXCOORD0;//テクスチャ座標
	float4 uv1 : TEXCOORD1;//サブテクスチャ座標
	// バンプマップ
	float3 tan:TANGENT;//接ベクトル
	float3 binorm:BINORMAL;//従法線
	// スキニングメッシュ
	int4 BlendIndices0:BLENDINDICES0;//ボーン初期用float型定数配列インデックス0
	float4 BlendWeight0:BLENDWEIGHT0;//ボーン処理用ウエイト値0
};
struct VSOutput {
	float4 svpos:SV_POSITION;
	float3 pos:POSITION;
	float3 norm:NORMAL;
	float2 uv:TECOORD;
	float4 diff:COLOR0;
	float4 spec:COLOR1;
	float3 tan:TANGENT;
	float3 bin:BINORMAL;
};
// 基本パラメータ
struct DX_D3D11_VS_CONST_BUFFER_BASE
{
	float4		AntiViewportM[4];		// アンチビューポート行列
	float4		ProjectionM[4];		// ビュー　→　プロジェクション行列
	float4		viewM[3];				// ワールド　→　ビュー行列
	float4		localM[3];		// ローカル　→　ワールド行列

	float4		ToonOutLineSize;			// トゥーンの輪郭線の大きさ
	float		DiffuseSource;				// ディフューズカラー( 0.0f:マテリアル  1.0f:頂点 )
	float		SpecularSource;				// スペキュラカラー(   0.0f:マテリアル  1.0f:頂点 )
	float		MulSpecularColor;			// スペキュラカラー値に乗算する値( スペキュラ無効処理で使用 )
	float		Padding;
};

// スキニングメッシュ用の　ローカル　→　ワールド行列
struct DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX
{
	// ローカル　→　ワールド行列
	float4		Matrix[54 * 3];
};

// その他の行列
struct DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX
{
	float4		ShadowMapLightViewProjectionMatrix[3][4];			// シャドウマップ用のライトビュー行列とライト射影行列を乗算したもの
	float4		TextureMatrix[3][2];								// テクスチャ座標操作用行列
};

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_VS_BASE			: register(b1)
{
	DX_D3D11_VS_CONST_BUFFER_BASE				g_Base;
};

// その他の行列
cbuffer cbD3D11_CONST_BUFFER_VS_OTHERMATRIX		: register(b2)
{
	DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX		g_OtherMatrix;
};

// スキニングメッシュ用の　ローカル　→　ワールド行列
cbuffer cbD3D11_CONST_BUFFER_VS_LOCALWORLDMATRIX	: register(b3)
{
	DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX	g_LocalWorldMatrix;
};

VSOutput main(VertexInput input)
{
	VSOutput output;
	float4 pos = float4(input.pos, 1);
	float4 ILocalWorldMatrix[3];
	// 複数のフレームのブレンド行列の作成
	ILocalWorldMatrix[0] = g_LocalWorldMatrix.Matrix[input.BlendIndices0.x + 0] * input.BlendWeight0.x;
	ILocalWorldMatrix[1] = g_LocalWorldMatrix.Matrix[input.BlendIndices0.x + 1] * input.BlendWeight0.x;
	ILocalWorldMatrix[2] = g_LocalWorldMatrix.Matrix[input.BlendIndices0.x + 2] * input.BlendWeight0.x;

	ILocalWorldMatrix[0] += g_LocalWorldMatrix.Matrix[input.BlendIndices0.y + 0] * input.BlendWeight0.y;
	ILocalWorldMatrix[1] += g_LocalWorldMatrix.Matrix[input.BlendIndices0.y + 1] * input.BlendWeight0.y;
	ILocalWorldMatrix[2] += g_LocalWorldMatrix.Matrix[input.BlendIndices0.y + 2] * input.BlendWeight0.y;

	ILocalWorldMatrix[0] += g_LocalWorldMatrix.Matrix[input.BlendIndices0.z + 0] * input.BlendWeight0.z;
	ILocalWorldMatrix[1] += g_LocalWorldMatrix.Matrix[input.BlendIndices0.z + 1] * input.BlendWeight0.z;
	ILocalWorldMatrix[2] += g_LocalWorldMatrix.Matrix[input.BlendIndices0.z + 2] * input.BlendWeight0.z;

	ILocalWorldMatrix[0] += g_LocalWorldMatrix.Matrix[input.BlendIndices0.w + 0] * input.BlendWeight0.w;
	ILocalWorldMatrix[1] += g_LocalWorldMatrix.Matrix[input.BlendIndices0.w + 1] * input.BlendWeight0.w;
	ILocalWorldMatrix[2] += g_LocalWorldMatrix.Matrix[input.BlendIndices0.w + 2] * input.BlendWeight0.w;

	// ローカル座標をワールド座標に変換
	float4 IWorldPos;
	IWorldPos.x = dot(pos, ILocalWorldMatrix[0]);
	IWorldPos.y = dot(pos, ILocalWorldMatrix[1]);
	IWorldPos.z = dot(pos, ILocalWorldMatrix[2]);
	IWorldPos.w = 1.0f;
	// ワールド座標をビュー座標に変換
	float4 IViewPos;
	IViewPos.x = dot(IWorldPos, g_Base.viewM[0]);
	IViewPos.y = dot(IWorldPos, g_Base.viewM[1]);
	IViewPos.z = dot(IWorldPos, g_Base.viewM[2]);
	IViewPos.w = 1.0f;
	//ビュー座標を射影座標に変換
	output.svpos.x = dot(IViewPos, g_Base.ProjectionM[0]);
	output.svpos.y = dot(IViewPos, g_Base.ProjectionM[1]);
	output.svpos.z = dot(IViewPos, g_Base.ProjectionM[2]);
	output.svpos.w = dot(IViewPos, g_Base.ProjectionM[3]);

	// 従法線、接線、法線をビューベクトルに変換
	float3 IWorldNrm;
	IWorldNrm.x = dot(input.norm, ILocalWorldMatrix[0].xyz);
	IWorldNrm.y = dot(input.norm, ILocalWorldMatrix[1].xyz);
	IWorldNrm.z = dot(input.norm, ILocalWorldMatrix[2].xyz);
	float3 IWorldBin;
	IWorldBin.x = dot(input.binorm, ILocalWorldMatrix[0].xyz);
	IWorldBin.y = dot(input.binorm, ILocalWorldMatrix[1].xyz);
	IWorldBin.z = dot(input.binorm, ILocalWorldMatrix[2].xyz);
	float3 IWorldTan;
	IWorldTan.x = dot(input.tan, ILocalWorldMatrix[0].xyz);
	IWorldTan.y = dot(input.tan, ILocalWorldMatrix[1].xyz);
	IWorldTan.z = dot(input.tan, ILocalWorldMatrix[2].xyz);

	float3 IViewNrm;
	IViewNrm.x = dot(IWorldNrm, g_Base.viewM[0].xyz);
	IViewNrm.y = dot(IWorldNrm, g_Base.viewM[1].xyz);
	IViewNrm.z = dot(IWorldNrm, g_Base.viewM[2].xyz);
	float3 IViewBin;
	IViewBin.x = dot(IWorldBin, g_Base.viewM[0].xyz);
	IViewBin.y = dot(IWorldBin, g_Base.viewM[1].xyz);
	IViewBin.z = dot(IWorldBin, g_Base.viewM[2].xyz);
	float3 IViewTan;
	IViewTan.x = dot(IWorldTan, g_Base.viewM[0].xyz);
	IViewTan.y = dot(IWorldTan, g_Base.viewM[1].xyz);
	IViewTan.z = dot(IWorldTan, g_Base.viewM[2].xyz);

	output.uv.x = dot(input.uv0, g_OtherMatrix.TextureMatrix[0][0]);
	output.uv.y = dot(input.uv0, g_OtherMatrix.TextureMatrix[0][1]);

	output.pos = IViewPos.xyz;
	output.norm = IViewNrm;

	output.tan = input.tan;
	output.bin = input.binorm;
	return output;
}