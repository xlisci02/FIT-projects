/*!
 * @file 
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include<math.h>
#include<assert.h>

#include"student/student_shader.h"
#include"student/gpu.h"
#include"student/uniforms.h"

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(
    GPUVertexShaderOutput     *const output,
    GPUVertexShaderInput const*const input ,
    GPU                        const gpu   ){
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3) ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující view a projekční matici.
  /// View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici čtěte z uniformní proměnné "projectionMatrix".
  /// Zachovejte jména uniformních proměnných a pozice vstupních a výstupních atributů.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Pro získání dat atributů použijte příslušné funkce vs_interpret* definované v souboru program.h.
  /// Pro získání dat uniformních proměnných použijte příslušné funkce shader_interpretUniform* definované v souboru program.h.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní struktuře.<br>
  /// <b>Seznam funkcí, které jistě použijete</b>:
  ///  - gpu_getUniformsHandle()
  ///  - getUniformLocation()
  ///  - shader_interpretUniformAsMat4()
  ///  - vs_interpretInputVertexAttributeAsVec3()
  ///  - vs_interpretOutputVertexAttributeAsVec3()

  
  // get handle to all uniforms
  Uniforms const uniformsHandle = gpu_getUniformsHandle(gpu);

  // get uniform location of view matrix
  UniformLocation const viewMatrixLocation = getUniformLocation(gpu,"viewMatrix");

  // get pointer to view matrix
  Mat4 const*const view = shader_interpretUniformAsMat4(uniformsHandle, viewMatrixLocation);

  // get uniform location of projection matrix
  UniformLocation const projectionMatrixLocation = getUniformLocation(gpu, "projectionMatrix");

  // get pointer to projection matrix
  Mat4 const*const proj = shader_interpretUniformAsMat4(uniformsHandle, projectionMatrixLocation);

  // interpret zero vertex attribute of input vertex as vec3
  Vec3 const*const position = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);
   
  // interpret zero vertex attribute of input vertex as vec3  
  Vec3 const*const normala = vs_interpretInputVertexAttributeAsVec3(gpu, input, 1);

  Mat4 mvp;
  multiply_Mat4_Mat4(&mvp,proj,view);

  Vec4 pos4;
  copy_Vec3Float_To_Vec4(&pos4,position,1.f); //position of vertex in world-space

  multiply_Mat4_Vec4(&output->gl_Position, &mvp, &pos4);

  Vec3 * const pos = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 0);
  
  init_Vec3(
      pos              , //pointer to zero vertex attribute
      position->data[0], 
      position->data[1], 
      position->data[2]);

  Vec3 *const n = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1); 
  
  init_Vec3(
      n            ,  //pointer to first vertex attribute 
      normala->data[0], 
      normala->data[1], 
      normala->data[2]);
}

void phong_fragmentShader(
    GPUFragmentShaderOutput     *const output,
    GPUFragmentShaderInput const*const input ,
    GPU                          const gpu   ){
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br> 
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnný.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// Difuzní barvu materiálu nastavte na čistou zelenou.
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()
  
  // get handle to all uniforms
  Uniforms const handle = gpu_getUniformsHandle(gpu);
  
  // interpret zero attibute of input fragment of fragment shader as vec3
  Vec3 const * position =fs_interpretInputAttributeAsVec3(gpu, input, 0);
  
  // interpret first attibute of input fragment of fragment shader as vec3
  Vec3 const * normala = fs_interpretInputAttributeAsVec3(gpu, input, 1);

  // interpret uniform values as vec3
  Vec3 const * const cameraPos = shader_interpretUniformAsVec3(handle,getUniformLocation(gpu,"cameraPosition"));
  Vec3 const * const lightPos = shader_interpretUniformAsVec3(handle,getUniformLocation(gpu,"lightPosition"));
  
  // Green color for diffuse light
  Vec3 colorG={.data[0]=0.f,.data[1]=1.f,.data[2]=0.f};
  
  // Green color for specular light
  Vec3 colorW={.data[0]=1.f,.data[1]=1.f,.data[2]=1.f};
  
  Vec3 L; // vector from position to lightPos 
  Vec3 diff; // diffuse light
  Vec3 N; // normal
  Vec3 R; // reflect 
  Vec3 specular; // specular light
  Vec3 V; // vector from position to cameraPos
  Vec3 result; // result (I_D + I_S)
  
  /* EQUATIONS */

  sub_Vec3(&L,lightPos,position); // 
  
  normalize_Vec3(&L,&L);  // normalize vector L
  normalize_Vec3(&N,normala);  // normalize normal vector

  float LN = dot_Vec3(&L,&N); // dot product of vectors L,N
  
  if(LN < 0) // negative dot product will be zero
    LN = 0.f;
  if(LN > 1)
    LN = 1.f;
  
  multiply_Vec3_Float(&diff,&colorG,LN); // multiply dot product with colorG = I_D (diff variable in my case)

  sub_Vec3(&V,cameraPos,position); // vector from position to cameraPos
  
  normalize_Vec3(&V,&V); // normalize vector V
  
  L.data[0]=-L.data[0]; // vector L in opposite direction
  L.data[1]=-L.data[1];
  L.data[2]=-L.data[2];
  
  // R vector is reflected vector  
  reflect(&R,&L,&N);

  // dot product of vectors V,R
  float VR = dot_Vec3(&V,&R);
  
  if(VR < 0) // negative dot product will be zero
    VR = 0;
  if(VR > 1)
    VR = 1.f;

  VR=(float)pow(VR,40.f);  // Shininess factor is in the exponent 
  
  multiply_Vec3_Float(&specular,&colorW,VR); // multiply 
  
  add_Vec3(&result,&diff,&specular); // I_P = I_D + I_S = (result)

  copy_Vec3Float_To_Vec4(&output->color,&result,1.f);
  
}

/// @}
