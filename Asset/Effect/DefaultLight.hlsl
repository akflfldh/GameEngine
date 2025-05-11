
struct Light
{
	float3 gLight;
	float3 gDirection;
	float3 gPosW;
};


float3 CaculateFresnelEffect(float3 fresnel, float3 lightVector, float3 normalVector)
{
	return fresnel + (float3(1.0f,1.0f,1.0f) - fresnel) * (pow((1 - dot(lightVector, normalVector)), 5));
}

float CaculateMicrofacetRate(float3 eyeVector, float3 lightVector, float3 normalVector,float shiness)
{

	float3 halfwayVector = normalize(eyeVector + lightVector);
	
	//shiness = 255 * (1.0f - shiness);
	//return float3(1.0f, 1.0f, 1.0f);
	return 	(shiness+8.0f) * pow(dot(halfwayVector, normalVector), shiness) / 8.0f;

}

float4 CaculateLight(float3 eyeVector, float3 lightIntensity,float3 lightVector, float3 normalVector,
	float3 diffuseAlbedo, float3 fresnelR0, float shiness,float3 specularFactor )
{

	float lambertFactor = max(dot(normalVector, lightVector), 0.0f);
	float3 diffuseTerm = diffuseAlbedo.xyz;
	
	float3 specularTerm =  CaculateFresnelEffect(fresnelR0, lightVector, normalVector)*
		CaculateMicrofacetRate(eyeVector,lightVector,normalVector,shiness) * specularFactor ;





	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	return float4(lambertFactor * lightIntensity * (  specularTerm + diffuseTerm),1.0f);









}







