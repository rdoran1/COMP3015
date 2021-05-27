#version 460

in vec3 LightIntensity;
in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform struct LightInfo 
{
  vec4 Position; // Light position in eye coords.
  vec3 Intensity;
  vec3 La;       
  vec3 L;
} Light;

uniform struct MaterialInfo 
{
	vec3 Ka; // Ambient reflectivity
	vec3 Kd;
	vec3 Ks; // Specular reflectivity
	float Shininess;
} Material;

uniform struct SpotLightInfo 
{
	vec3 Position; // Position in cam coords
	vec3 L; // Diffuse/spec intensity
	vec3 La; // Amb intensity
	vec3 Direction; // Direction of the spotlight in cam coords.
	float Exponent; // Angular attenuation exponent
	float Cutoff; // Cutoff angle (between 0 and pi/2)
} Spot;

layout( location = 1 ) out vec4 SkyBoxColor;

const int levels = 4;
const float scaleFactor = 1.0 / levels;

layout (location = 0) out vec4 FragColor;
layout(binding=0) uniform sampler2D Tex1;

vec3 phongModel( vec3 position, vec3 n )
{
	vec3 texColor = texture(Tex1, TexCoord).rgb;

	vec3 ambient = Light.La * texColor;
	vec3 s = normalize(vec3(Light.Position));
	float sDotN = max( dot(s,n), 0.0 );
	vec3 diffuse = Material.Kd * sDotN;
	vec3 spec = vec3(0.0);
	if( sDotN > 0.0 ) 
	{
		vec3 v = normalize(-position.xyz);
		vec3 r = reflect( -s, n );
		spec = Material.Ks * pow( max( dot(r,v), 0.0 ), 
		Material.Shininess );
	}	

 return ambient + Light.L * (diffuse + spec);
}

vec3 blinnPhong( vec3 position, vec3 n )
{
	vec3 texColor = texture(Tex1, TexCoord).rgb;

	vec3 ambient = Light.La * texColor;
	vec3 s = normalize(vec3(Light.Position));
	float sDotN = max( dot(s,n), 0.0 );
	vec3 diffuse = Material.Kd * floor( sDotN * levels ) * 
scaleFactor;
	vec3 spec = vec3(0.0);
	if( sDotN > 0.0 ) 
	{
		vec3 v = normalize(-position.xyz);
		vec3 h = normalize(v + s);
		spec = Material.Ks * pow( max( dot(h,n), 0.0 ), 
		Material.Shininess );
	}	

 return ambient + Light.L * (diffuse + spec);
}

vec3 spotlight( vec3 position, vec3 n )
{
	vec3 texColor = texture(Tex1, TexCoord).rgb;

	vec3 ambient = Spot.La * Material.Ka * texColor;
	vec3 s = normalize(vec3(Spot.Position) - position);

	float cosAng = dot(-s, normalize(Spot.Direction));
	
	float angle = acos( cosAng );
	float spotScale = 0.0;
		
	vec3 diffuse;
	vec3 spec = vec3(0.0);

	if(angle < Spot.Cutoff )
	{
		spotScale = pow( cosAng, Spot.Exponent );
		float sDotN = max( dot(s,n), 0.0 ); 
		diffuse = Material.Kd * sDotN * Spot.L;

		if( sDotN > 0.0 ) 
		{
			vec3 v = normalize(-position.xyz);
			vec3 h = normalize(v + s);
			spec = Material.Ks * pow( max( dot(h,n), 0.0 ), Material.Shininess );
		}
	}

 return ambient + spotScale * Spot.L * (diffuse + spec);
}

void main()
{
	//FragColor = vec4(phongModel(Position, normalize(Normal)), 1);
	FragColor = vec4(blinnPhong(Position.xyz, normalize(Normal)), 1);
	//FragColor = vec4(spotlight(Position, normalize(Normal)), 1);
}
