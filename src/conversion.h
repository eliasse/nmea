#ifndef _CONVERSION_
#define _CONVERSION_

//static const float PI = 3.141592653589793; 

inline float toRad(float x){
  return (x * (PI/180.f));
}

inline float toDeg(float x){
  return (x * (180.f/PI));
}

#endif
