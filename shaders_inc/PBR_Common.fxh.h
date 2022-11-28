"#ifndef _PBR_COMMON_FXH_\n"
"#define _PBR_COMMON_FXH_\n"
"\n"
"#ifndef PI\n"
"#    define PI 3.141592653589793\n"
"#endif\n"
"\n"
"// Lambertian diffuse\n"
"// see https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/\n"
"float3 LambertianDiffuse(float3 DiffuseColor)\n"
"{\n"
"    return DiffuseColor / PI;\n"
"}\n"
"\n"
"// The following equation models the Fresnel reflectance term of the spec equation (aka F())\n"
"// Implementation of fresnel term from \"An Inexpensive BRDF Model for Physically based Rendering\" by Christophe Schlick\n"
"// https://en.wikipedia.org/wiki/Schlick%27s_approximation\n"
"//\n"
"//      Rf(Theta) = Rf(0) + (1 - Rf(0)) * (1 - cos(Theta))^5\n"
"//\n"
"//\n"
"//           \'.       |       .\'\n"
"//             \'.     |Theta.\'\n"
"//               \'.   |   .\'\n"
"//                 \'. | .\'\n"
"//        ___________\'.\'___________\n"
"//                   \'|\n"
"//                  \' |\n"
"//                 \'  |\n"
"//                \'   |\n"
"//               \' Phi|\n"
"//\n"
"// Note that precise relfectance term is given by the following expression:\n"
"//\n"
"//      Rf(Theta) = 0.5 * (sin^2(Theta - Phi) / sin^2(Theta + Phi) + tan^2(Theta - Phi) / tan^2(Theta + Phi))\n"
"//\n"
"#define SCHLICK_REFLECTION(VdotH, Reflectance0, Reflectance90) ((Reflectance0) + ((Reflectance90) - (Reflectance0)) * pow(clamp(1.0 - (VdotH), 0.0, 1.0), 5.0))\n"
"float SchlickReflection(float VdotH, float Reflectance0, float Reflectance90)\n"
"{\n"
"    return SCHLICK_REFLECTION(VdotH, Reflectance0, Reflectance90);\n"
"}\n"
"float3 SchlickReflection(float VdotH, float3 Reflectance0, float3 Reflectance90)\n"
"{\n"
"    return SCHLICK_REFLECTION(VdotH, Reflectance0, Reflectance90);\n"
"}\n"
"\n"
"// Visibility = G(v,l,a) / (4 * (n,v) * (n,l))\n"
"// see https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf\n"
"float SmithGGXVisibilityCorrelated(float NdotL, float NdotV, float AlphaRoughness)\n"
"{\n"
"    float a2 = AlphaRoughness * AlphaRoughness;\n"
"\n"
"    float GGXV = NdotL * sqrt(max(NdotV * NdotV * (1.0 - a2) + a2, 1e-7));\n"
"    float GGXL = NdotV * sqrt(max(NdotL * NdotL * (1.0 - a2) + a2, 1e-7));\n"
"\n"
"    return 0.5 / (GGXV + GGXL);\n"
"}\n"
"\n"
"// Smith GGX shadow-masking function G(v,l,a) (aka G2)\n"
"float SmithGGXShadowMasking(float NdotL, float NdotV, float AlphaRoughness)\n"
"{\n"
"    return 4.0 * NdotL * NdotV * SmithGGXVisibilityCorrelated(NdotL, NdotV, AlphaRoughness);\n"
"}\n"
"\n"
"// Smith GGX masking function G1\n"
"// [1] \"Sampling the GGX Distribution of Visible Normals\" (2018) by Eric Heitz - eq. (2)\n"
"// https://jcgt.org/published/0007/04/01/\n"
"float SmithGGXMasking(float NdotV, float AlphaRoughness)\n"
"{\n"
"    float a2 = AlphaRoughness * AlphaRoughness;\n"
"\n"
"    // In [1], eq. (2) is defined for the tangent-space view direction V:\n"
"    //\n"
"    //                                        1\n"
"    //      G1(V) = -----------------------------------------------------------\n"
"    //                                    {      (ax*V.x)^2 + (ay*V.y)^2)  }\n"
"    //               1 + 0.5 * ( -1 + sqrt{ 1 + -------------------------- } )\n"
"    //                                    {              V.z^2             }\n"
"    //\n"
"    // Note that [1] uses notation N for the micronormal, but in our case N is the macronormal,\n"
"    // while micronormal is H (aka the halfway vector).\n"
"    //\n"
"    // After multiplying both nominator and denominator by 2*V.z and given that in our\n"
"    // case ax = ay = a, we get:\n"
"    //\n"
"    //                                2 * V.z                                        2 * V.z\n"
"    //      G1(V) = ------------------------------------------- =  ----------------------------------------\n"
"    //               V.z + sqrt{ V.z^2 + a2 * (V.x^2 + V.y^2) }     V.z + sqrt{ V.z^2 + a2 * (1 - V.z^2) }\n"
"    //\n"
"    // Since V.z = NdotV, we finally get:\n"
"\n"
"    float Denom = NdotV + sqrt(a2 + (1.0 - a2) * NdotV * NdotV);\n"
"    return 2.0 * max(NdotV, 0.0) / max(Denom, 1e-6);\n"
"}\n"
"\n"
"\n"
"// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())\n"
"// Implementation from \"Average Irregularity Representation of a Roughened Surface for Ray Reflection\" by T. S. Trowbridge, and K. P. Reitz\n"
"// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.\n"
"float NormalDistribution_GGX(float NdotH, float AlphaRoughness)\n"
"{\n"
"    // [1] \"Sampling the GGX Distribution of Visible Normals\" (2018) by Eric Heitz - eq. (1)\n"
"    // https://jcgt.org/published/0007/04/01/\n"
"\n"
"    // Make sure we reasonably handle AlphaRoughness == 0\n"
"    // (which corresponds to delta function)\n"
"    AlphaRoughness = max(AlphaRoughness, 1e-3);\n"
"\n"
"    float a2  = AlphaRoughness * AlphaRoughness;\n"
"    float nh2 = NdotH * NdotH;\n"
"    float f   = nh2 * a2 + (1.0 - nh2);\n"
"    return a2 / (PI * f * f);\n"
"}\n"
"\n"
"\n"
"// Samples a normal from Visible Normal Distribution as described in\n"
"// [1] \"A Simpler and Exact Sampling Routine for the GGX Distribution of Visible Normals\" (2017) by Eric Heitz\n"
"//     https://hal.archives-ouvertes.fr/hal-01509746/document\n"
"// [2] \"Sampling the GGX Distribution of Visible Normals\" (2018) by Eric Heitz\n"
"//     https://jcgt.org/published/0007/04/01/\n"
"//\n"
"// Notes:\n"
"//      - View direction must be pointing away from the surface.\n"
"//      - Returned normal is in tangent space with Z up.\n"
"//      - Returned normal should be used to reflect the view direction and obtain\n"
"//        the sampling direction.\n"
"float3 SmithGGXSampleVisibleNormal(float3 View, // View direction in tangent space\n"
"                                   float  ax,   // X roughness\n"
"                                   float  ay,   // Y roughness\n"
"                                   float  u1,   // Uniform random variable in [0, 1]\n"
"                                   float  u2    // Uniform random variable in [0, 1]\n"
")\n"
"{\n"
"    // Stretch the view vector so we are sampling as if roughness==1\n"
"    float3 V = normalize(View * float3(ax, ay, 1.0));\n"
"\n"
"#if 1\n"
"    // Technique from [1]\n"
"    // Note: while [2] claims to provide a better parameterization, it produces\n"
"    //       subjectively noisier images, so we will stick with method from [1].\n"
"\n"
"    // Build an orthonormal basis with V, T1, and T2\n"
"    float3 T1 = (V.z < 0.999) ? normalize(cross(V, float3(0.0, 0.0, 1.0))) : float3(1.0, 0.0, 0.0);\n"
"    float3 T2 = cross(T1, V);\n"
"\n"
"    // Choose a point on a disk with each half of the disk weighted\n"
"    // proportionally to its projection onto direction V\n"
"    float a = 1.0 / (1.0 + V.z);\n"
"    float r = sqrt(u1);\n"
"    float phi = (u2 < a) ? (u2 / a) * PI : PI + (u2 - a) / (1.0 - a) * PI;\n"
"    float p1 = r * cos(phi);\n"
"    float p2 = r * sin(phi) * ((u2 < a) ? 1.0 : V.z);\n"
"#else\n"
"    // Technique from [2]\n"
"    // Note: [1] uses earlier parameterization that cannot be used with view directions located in the lower\n"
"    //       hemisphere (View.z < 0). Though this is not a problem for classic microfacet BSDF models,\n"
"    //       [2] provides a better approximation that is not subject to this limitation.\n"
"\n"
"    // Build orthonormal basis (with special case if cross product is zero) (Section 4.1)\n"
"    float lensq = dot(V.xy, V.xy);\n"
"    float3 T1 = lensq > 0.0 ? float3(-V.y, V.x, 0.0) / sqrt(lensq) : float3(1.0, 0.0, 0.0);\n"
"    float3 T2 = cross(V, T1);\n"
"\n"
"    // Sample the projected area (Section 4.2)\n"
"    float r   = sqrt(u1);\n"
"    float phi = 2.0 * PI * u2;\n"
"    float p1 = r * cos(phi);\n"
"    float p2 = r * sin(phi);\n"
"    float s  = 0.5 * (1.0 + V.z);\n"
"    p2 = (1.0 - s) * sqrt(1.0 - p1 * p1) + s * p2;\n"
"#endif\n"
"\n"
"    // Calculate the normal in the stretched tangent space\n"
"    float3 N = p1 * T1 + p2 * T2 + sqrt(max(0.0, 1.0 - p1 * p1 - p2 * p2)) * V;\n"
"\n"
"    // Transform the normal back to the ellipsoid configuration\n"
"    return normalize(float3(ax * N.x, ay * N.y, max(0.0, N.z)));\n"
"}\n"
"\n"
"// Returns the probability of sampling direction L for the view direction V and normal N\n"
"// using the visible normals distribution.\n"
"// [1] \"Sampling the GGX Distribution of Visible Normals\" (2018) by Eric Heitz\n"
"// https://jcgt.org/published/0007/04/01/\n"
"float SmithGGXSampleDirectionPDF(float3 V, float3 N, float3 L, float AlphaRoughness)\n"
"{\n"
"    // Micronormal is the halfway vector\n"
"    float3 H = normalize(V + L);\n"
"\n"
"    float NdotH = dot(H, N);\n"
"    float NdotV = dot(N, V);\n"
"    float NdotL = dot(N, L);\n"
"    //float VdotH = dot(V, H);\n"
"    if (NdotH <= 0.0 || NdotV <= 0.0 || NdotL <= 0.0)\n"
"        return 0.0;\n"
"\n"
"    // Note that [1] uses notation N for the micronormal, but in our case N is the macronormal,\n"
"    // while micronormal is H (aka the halfway vector).\n"
"    float NDF = NormalDistribution_GGX(NdotH, AlphaRoughness); // (1) - D(N)\n"
"    float G1  = SmithGGXMasking(NdotV, AlphaRoughness);        // (2) - G1(V)\n"
"\n"
"    float VNDF = G1 /* * VdotH */ * NDF / NdotV; // (3) - Dv(N)\n"
"    return  VNDF / (4.0 /* * VdotH */); // (17) - VdotH cancels out\n"
"}\n"
"\n"
"struct AngularInfo\n"
"{\n"
"    float NdotL; // cos angle between normal and light direction\n"
"    float NdotV; // cos angle between normal and view direction\n"
"    float NdotH; // cos angle between normal and half vector\n"
"    float LdotH; // cos angle between light direction and half vector\n"
"    float VdotH; // cos angle between view direction and half vector\n"
"};\n"
"\n"
"AngularInfo GetAngularInfo(float3 PointToLight, float3 Normal, float3 View)\n"
"{\n"
"    float3 n = normalize(Normal);       // Outward direction of surface point\n"
"    float3 v = normalize(View);         // Direction from surface point to camera\n"
"    float3 l = normalize(PointToLight); // Direction from surface point to light\n"
"    float3 h = normalize(l + v);        // Direction of the vector between l and v\n"
"\n"
"    AngularInfo info;\n"
"    info.NdotL = clamp(dot(n, l), 0.0, 1.0);\n"
"    info.NdotV = clamp(dot(n, v), 0.0, 1.0);\n"
"    info.NdotH = clamp(dot(n, h), 0.0, 1.0);\n"
"    info.LdotH = clamp(dot(l, h), 0.0, 1.0);\n"
"    info.VdotH = clamp(dot(v, h), 0.0, 1.0);\n"
"\n"
"    return info;\n"
"}\n"
"\n"
"struct SurfaceReflectanceInfo\n"
"{\n"
"    float  PerceptualRoughness;\n"
"    float3 Reflectance0;\n"
"    float3 Reflectance90;\n"
"    float3 DiffuseColor;\n"
"};\n"
"\n"
"// BRDF with Lambertian diffuse term and Smith-GGX specular term.\n"
"void SmithGGX_BRDF(in float3                 PointToLight,\n"
"                   in float3                 Normal,\n"
"                   in float3                 View,\n"
"                   in SurfaceReflectanceInfo SrfInfo,\n"
"                   out float3                DiffuseContrib,\n"
"                   out float3                SpecContrib,\n"
"                   out float                 NdotL)\n"
"{\n"
"    AngularInfo angularInfo = GetAngularInfo(PointToLight, Normal, View);\n"
"\n"
"    DiffuseContrib = float3(0.0, 0.0, 0.0);\n"
"    SpecContrib    = float3(0.0, 0.0, 0.0);\n"
"    NdotL          = angularInfo.NdotL;\n"
"    // If one of the dot products is larger than zero, no division by zero can happen. Avoids black borders.\n"
"    if (angularInfo.NdotL > 0.0 || angularInfo.NdotV > 0.0)\n"
"    {\n"
"        //           D(h,a) * G(v,l,a) * F(v,h,f0)\n"
"        // f(v,l) = -------------------------------- = D(h,a) * Vis(v,l,a) * F(v,h,f0)\n"
"        //               4 * (n,v) * (n,l)\n"
"        // where\n"
"        //\n"
"        // Vis(v,l,a) = G(v,l,a) / (4 * (n,v) * (n,l))\n"
"\n"
"        // It is not a mistake that AlphaRoughness = PerceptualRoughness ^ 2 and that\n"
"        // SmithGGXVisibilityCorrelated and NormalDistribution_GGX then use a2 = AlphaRoughness ^ 2.\n"
"        // See eq. 3 in https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf\n"
"        float  AlphaRoughness = SrfInfo.PerceptualRoughness * SrfInfo.PerceptualRoughness;\n"
"        float  D   = NormalDistribution_GGX(angularInfo.NdotH, AlphaRoughness);\n"
"        float  Vis = SmithGGXVisibilityCorrelated(angularInfo.NdotL, angularInfo.NdotV, AlphaRoughness);\n"
"        float3 F   = SchlickReflection(angularInfo.VdotH, SrfInfo.Reflectance0, SrfInfo.Reflectance90);\n"
"\n"
"        DiffuseContrib = (1.0 - F) * LambertianDiffuse(SrfInfo.DiffuseColor);\n"
"        SpecContrib    = F * Vis * D;\n"
"    }\n"
"}\n"
"\n"
"#endif // _PBR_COMMON_FXH_\n"
