#version 330

// Atributos de entrada
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexCoord;
layout (location = 2) in vec3 vertexNormal;
//in vec4 vertexTangent; // Añadido para el Broken Helmet

//in mat4 instanceTransform;

layout (location = 11) in vec3 instancePos;  // 3 floats
layout (location = 10) in vec4 instanceQuat; // 4 floats

// Uniforms
uniform mat4 mvp;

// Salidas al Fragment Shader
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;
out float visibility;
//out vec4 fragTangent; // Para el Normal Map del casco

// Just copied it, but its likely to work
vec3 rotate_vector(vec3 v, vec4 q) {
    return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void main() {
    // Old Shit of transposing 
    //mat4 modelMat = transpose(instanceTransform);
    //modelMat[3][3] = 1.0;

    // More old shit of getting the angle
    //mat3 normalMatrix = mat3(modelMat); 
    
    // This a pile of steaming old junk
    // mat3 normalMatrix = transpose(inverse(mat3(modelMat)));

    /* MORE OLD CODE
    // Mail boy to the FS
    fragPosition = vec3(modelMat * vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vec4(1.0);
    
    //Doing math to figure out tangents and normals (they seems tasty)
    fragNormal = normalize(normalMatrix * vertexNormal);
    //fragTangent = vec4(normalize(normalMatrix * vertexTangent.xyz), vertexTangent.w);

    // The mail stamp
    gl_Position = mvp * modelMat * vec4(vertexPosition, 1.0);
    */

    // Calculate the normal
    vec3 rotatedNormal = rotate_vector(vertexNormal, instanceQuat);

    // Calculate the vertex
    vec3 rotatedVertex = rotate_vector(vertexPosition, instanceQuat);

    // Calculate the final position
    vec3 worldPos = instancePos + rotatedVertex;

    // New mail boy to the FS
    fragPosition = worldPos;
    fragTexCoord = vertexTexCoord;
    fragNormal = normalize(rotatedNormal);

    fragColor = vec4(1.0, 1.0, 1.0, 1.0);

    // Finally, the mail stamp
    gl_Position = mvp * vec4(worldPos, 1.0 );
}

