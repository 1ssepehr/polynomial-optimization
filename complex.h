#ifndef _complex
#define _complex

typedef struct complexNumber { double real; double im; } complex;

extern complex Add_Complex( complex A, complex B );

extern complex Subtract_Complex( complex A, complex B );

extern complex Multiply_Complex( complex A, complex B );

extern complex Divide_Complex( complex A, complex B );

extern double Norm_Complex( complex A );

#endif
