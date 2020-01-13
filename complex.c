#include "complex.h"
#include <math.h>

complex Add_Complex( complex A, complex B ){
    complex C;
    C.real = A.real + B.real;
    C.im   = A.im   + B.im  ;
    return C;
}

complex Subtract_Complex( complex A, complex B ){
    complex C;
    C.real = A.real - B.real;
    C.im   = A.im   - B.im  ;
    return C;
}

complex Multiply_Complex( complex A, complex B ){
    complex C;
    C.real = ( A.real * B.real ) - ( A.im * B.im   );
    C.im   = ( A.real * B.im   ) + ( A.im * B.real );
    return C;
}

complex Divide_Complex( complex A, complex B ){
    complex C;
    double denom;    
    denom = ( B.real * B.real ) + ( B.im * B.im );
    C.real = (( A.real * B.real ) + ( A.im   * B.im )) / denom;
    C.im   = (( A.im   * B.real ) - ( A.real * B.im )) / denom;
    return C;
}

double Norm_Complex( complex A ){
    return sqrt( A.real * A.real + A.im * A.im );
}
