#include "helper_math.h"
#include <numbers>
#include "Structs.cuh"


template<MaterialTypeID matTypeID>
__device__ float3 evaluateBRDF(float3 normal, float3 outDir, float3 inDir, float3 albedoDiffuse, float3 albedoSpecular, float shininess) 
{
	
    if constexpr (matTypeID == BLINNPHONG)
    {
        return normal;

        float cosIn = max(dot(normal, inDir), 0.0);
        float cosOut = max(dot(normal, outDir), 0.0);

        // If light is behind the surface or the camera is behind the surface
        if (cosIn <= 0.0 || cosOut <= 0.0) return float3(0.0);

        float3 halfVec = normalize(outDir + inDir);
        float cosHalfN = max(dot(normal, halfVec), 0.0);

        // Diffuse (Lambertian) component
        float3 diffuse = albedoDiffuse / std::numbers::pi;

        // Specular (Blinn-Phong) component with energy conservation
        float normalization = (shininess + 8.0) / (8.0 * std::numbers::pi);
        float3 specular = albedoSpecular * normalization * pow(cosHalfN, shininess);

        return (diffuse + specular);
    }
}

// Pick a new direction based on BRDF 
template<MaterialTypeID matTypeID>
__device__ float3 sampleBRDF(float3 normal, float3 outDir, curandState &randState)
{



}  

__device__ static bool sphereIntersect(float3 rayOgn, float3 rayDir, float3 sphereCenter, float sphereRadius, float3 &rHitPoint, float3 &rNormal) {
    float t0, t1; // Solutions for t if the ray intersects the sphere
    
    float3 L = sphereCenter - rayOgn;
    float tca = dot(L, rayDir);
    if (tca < 0) return false;
    float d2 = dot(L, L) - tca * tca;
    if (d2 > sphereRadius * sphereRadius) return false;
    float thc = sqrt(sphereRadius * sphereRadius - d2);
    t0 = tca - thc;
    t1 = tca + thc;

    if (t0 > t1) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    if (t0 < 0) {
        t0 = t1;
        if (t0 < 0) return false;
    }

    rHitPoint = rayOgn + t0 * rayDir;
    rNormal = (rHitPoint - sphereCenter) / sphereRadius;
    return true;
}
