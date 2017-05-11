#version 150 

in  vec3 pos, norm;

out vec4 fColor;

uniform mat4 view;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

void main()
{
    // Transform light position into eye coordinates
    vec4 lightpos = (view * LightPosition);
	
    vec3 L = normalize( (lightpos).xyz - pos ); // light vector
    vec3 E = normalize( -pos );  // eye (or view) vector
    vec3 H = normalize( L + E ); // halfway vector
    vec3 N = normalize(norm);  // normal vector

    vec3 R = -reflect(L, N); // reflection vector (reflect function
    	     		     // returns assumes L is directed towards surface)

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;

    //float Ks = pow( max(dot(N, H), 0.0), Shininess );  // Blinn modification
    float Ks = pow( max(dot(E, R), 0.0), Shininess ); // Original Phong model
    vec4  specular = Ks * SpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    fColor = ambient + diffuse + specular;
    fColor.a = 1.0;
}
