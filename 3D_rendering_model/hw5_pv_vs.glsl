#version 150


in  vec3 vPos;
in  vec3 vNormal;
out vec4 color;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 view;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

void main()
{
    // Transform vertex  and light position into eye coordinates
    vec3 pos = (modelview * vec4(vPos, 1.0)).xyz;
    vec4 lightpos = (view * LightPosition);
    
    vec3 L = normalize( (lightpos).xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E ); // halfway vector
    
    // Transform vertex normal into eye coordinates
    vec3 N = normalize( modelview*vec4(vNormal, 0.0) ).xyz;
    
    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;
    
    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;
    
    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
        specular = vec4(0.0, 0.0, 0.0, 1.0);
    }
    
    gl_Position = projection * modelview * vec4(vPos,1.0);
    gl_PointSize = 5.0;
    
    color = ambient + diffuse + specular;
    color.a = 1.0;
}


