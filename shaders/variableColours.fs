//////////////////////////////////////////////////////////////////////////////////////////
//
// variable colours example
//

// inputs

uniform float progress;
uniform vec4 toColour;
uniform vec4 fromColour;

void main() 
{ 
  gl_FragColor = mix(fromColour, toColour, progress);
}
