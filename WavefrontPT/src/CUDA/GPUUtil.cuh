#include "helper_math.h"
#include <numbers>
#include "Structs.cuh"

__device__ __inline__ float getLuminance(float3 color) 
{
    return 0.2126f * color.x + 0.7152f * color.y + 0.0722f * color.z;
    // 0.2126R + 0.7152G + 0.0722B
}

template<MaterialTypeID matTypeID>
__device__ float3 evaluateBRDF(float3 normal, float3 outDir, float3 inDir, float3 albedoDiffuse, float3 albedoSpecular, float shininess) 
{
	
    if constexpr (matTypeID == BLINNPHONG)
    {
        inDir *= -1;

        float cosIn = max(dot(normal, inDir), 0.0f);
        float cosOut = max(dot(normal, outDir), 0.0f);

        // If light is behind the surface or the camera is behind the surface
        if (cosIn <= 0.0 || cosOut <= 0.0) return PINK;

        float3 halfVec = normalize(outDir + inDir);
        float cosHalfN = max(dot(normal, halfVec), 0.0f);

        // Diffuse (Lambertian) component
        float3 diffuse = albedoDiffuse / std::numbers::pi;

        // Specular (Blinn-Phong) component with energy conservation
        float normalization = (shininess + 8.0) / (8.0 * std::numbers::pi);
        float3 specular = albedoSpecular * normalization * pow(cosHalfN, shininess);

        //return (diffuse+specular);
        return diffuse;
    }
}

__device__ __inline__ float getDiffusePDF(float3 normal, float3 newRayDir) 
{
    float costh = max(0.0f, dot(normal, newRayDir));
    return costh / std::numbers::pi;
}

__device__ __inline__ float getSpecularPDF(float3 normal, float3 newRayDir)
{

    //TODO SPECULAR - implement 
    //float3 halfVector = sampleSpecularLobe(normal, shininess, randomUV);
            // Reflect the outgoing (view) ray over the half-vector to get light source dir
    //float3 incomingDir = reflect(-outgoingDir, halfVector);
    //return { incomingDir, calculateSpecularPDF(normal, halfVector, outgoingDir) };

    //float costh = dot(normal, newRayDir);
    //return costh / std::numbers::pi;
}

// Pick a new direction based on BRDF 
template<MaterialTypeID matTypeID>
__device__ float3 sampleBRDF(float3 normal, curandState &randState, float3 albedoDiffuse, float3 albedoSpecular, float shininess, float &rPDF)
{
    float3 rDir;

    if constexpr (matTypeID == BLINNPHONG)
    {
		// diffuse probability is based on average luminance of diffuse and specular albedos
		//float diffuseProb = getLuminance(albedoDiffuse) / (getLuminance(albedoDiffuse) + getLuminance(albedoSpecular));

        //if(curand_uniform(&randState) < diffuseProb)
        {   // pick sample from diffuse lobe
            
            float r1 = 2 * std::numbers::pi * curand_uniform(&randState); // azimuth
            float r2 = curand_uniform(&randState); // elevation (how far up from the surface)
            float r2s = sqrtf(r2); // distributes samples towards normal

            float3 w = normal; // new up vector
            float3 u; // new forward vector

            if (fabsf(w.x) > 0.1f)
                u = normalize(cross(make_float3(0, 1, 0), w));
            else
                u = normalize(cross(make_float3(1, 0, 0), w));

            float3 v = cross(w, u); // new right vector

            u *= cosf(r1) * r2s;
            v *= sinf(r1) * r2s;
            w *= sqrtf(1 - r2);
            rDir = normalize(u + v + w); // direction of new sample ray
        }
        //TODO SPECULAR 
        //else 
        //{   // pick sample from specular lobe
        //    float r1 = acos(pow( curand_uniform(&randState), (1 / (shininess + 1)) ));
        //    float r2 = 2 * std::numbers::pi * curand_uniform(&randState);

        //    float3 w = normal; // new up vector
        //    float3 u; // new forward vector

        //    if (fabsf(w.x) > 0.1f)
        //        u = normalize(cross(make_float3(0, 1, 0), w));
        //    else
        //        u = normalize(cross(make_float3(1, 0, 0), w));

        //    float3 v = cross(w, u); // new right vector

        //    u *= cosf(r1) * r2s;
        //    v *= sinf(r1) * r2s;
        //    w *= sqrtf(1 - r2);
        //    rDir = u + v + w; // direction of new sample ray

        //}

        rPDF = getDiffusePDF(normal, rDir); // PDF for picking this direction in uniform hemisphere sampling
    }

    return rDir;

}  

__device__ __inline__ bool planeTriIntersect(float3 rayOgn, float3 rayDir, float3 v0, float3 v1, float3 v2, float3 &rHitPoint, float3 &rNormal) {
    // Compute plane's normal
    rNormal = normalize(cross(v1 - v0, v2 - v0));
    // Check if ray and plane are parallel
    float denom = dot(rNormal, rayDir);
    if (fabsf(denom) < 1e-6f) return false; // Parallel
    // Compute intersection point with the plane
    float t = dot(v0 - rayOgn, rNormal) / denom;
    if (t < 0) return false; // Intersection behind the ray origin
    rHitPoint = rayOgn + t * rayDir;
    // Check if the intersection point is inside the triangle using barycentric coordinates
    float3 edge0 = v1 - v0;
    float3 edge1 = v2 - v1;
    float3 edge2 = v0 - v2;
    float3 c0 = rHitPoint - v0;
    float3 c1 = rHitPoint - v1;
    float3 c2 = rHitPoint - v2;
    if (dot(rNormal, cross(edge0, c0)) >= 0 &&
        dot(rNormal, cross(edge1, c1)) >= 0 &&
        dot(rNormal, cross(edge2, c2)) >= 0) {
        return true; // Inside triangle
    }
    return false; // Outside triangle
}

__device__ __inline__ bool sphereIntersect(float3 rayOgn, float3 rayDir, float3 sphereCenter, float sphereRadius, float3 &rHitPoint, float3 &rNormal) {
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
