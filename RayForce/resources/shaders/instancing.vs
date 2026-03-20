#version 330

// Atributos de entrada
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
//in vec4 vertexTangent; // Añadido para el Broken Helmet

in mat4 instanceTransform;

// Uniforms
uniform mat4 mvp;

// Salidas al Fragment Shader
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;
//out vec4 fragTangent; // Para el Normal Map del casco

void main()
{
    // 1. Transponemos la matriz de PhysX
    mat4 modelMat = transpose(instanceTransform);
    modelMat[3][3] = 1.0;

    // 2. Extraemos la matriz de rotación para las normales (3x3)
    // Esto sustituye al 'uniform mat4 matNormal'
    mat3 normalMatrix = mat3(modelMat); 
    
    // Si escalas los cascos de forma no uniforme, usa: 
    // mat3 normalMatrix = transpose(inverse(mat3(modelMat)));

    // 3. Enviamos datos al Fragment Shader
    fragPosition = vec3(modelMat * vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vec4(1.0);
    
    // Calculamos normal y tangente usando la matriz de ESTA instancia
    fragNormal = normalize(normalMatrix * vertexNormal);
    //fragTangent = vec4(normalize(normalMatrix * vertexTangent.xyz), vertexTangent.w);

    // 4. Posición final
    gl_Position = mvp * modelMat * vec4(vertexPosition, 1.0);
}