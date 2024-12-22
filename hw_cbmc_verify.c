#include <assert.h>
#include <math.h>
#include <stdint.h>

struct module_format9_MADD_mult {
  unsigned A8;
  unsigned B8;
  unsigned PAB;
};

extern struct module_format9_MADD_mult format9_MADD_mult;

void set_inputs(void);


float cast_mpfloat8_v9_nearest(float origin) {

  uint32_t bits;

  bits = *(uint32_t *)(&origin);
  
  int32_t exp = ((bits >> 23) & 0xFF) - 127;  //unbiased exponent
  uint32_t sign = (bits >> 31) & 1;  //sign bit
  uint32_t mant = (bits & 0x7FFFFF); //mantissa
  
  uint32_t outbits;
  
  float out;
  
  if(exp == 128){ //infinity or NaN, 128 is inf/nan, 2^(8-1) -1 = 127 is emax for IEEE 754
      //infinity case, propagate infinities
      if(mant == 0){
          outbits = (sign << 31| 0xFF << 23| 0);  //infinity
          uint32_t *tempout = &outbits;
          out = *((float *)tempout);
          return out;
      } else {
      //NaN case
          outbits = (sign << 31| 0xFF << 23| mant);  //NaNs are propagated
          uint32_t *tempout = &outbits;
          out = *((float *)tempout);
          return out;
      }
  }
  
  // 2^11 - 2^21 case, 0 bit mantissa
  if(exp >= 11){ 

    //get the odd_bit (O.xxx_xxxx_xxxx_xxxx_xxxx_xxxx) for ties to even
    //uint32_t odd_bit = 1; //odd bit is the implied 1 so no need for odd bit

    mant = mant + (1 << (23 - 1 - 0)); //round to nearest, ties to even
    
    
    if((mant >> 23) == 1) {//if overflow through rounding
      exp = exp + 1; //add bias
      mant = 0;
      
    }

    if(exp > 21) {
      outbits = (sign << 31| 0xFF << 23| 0); //overflow to infinity
      uint32_t *tempout = &outbits;
      out = *((float *)tempout);
      return out;
    }

    mant = 0; 
    exp = exp + 127; //add bias
    outbits = (sign << 31| exp << 23| mant);
    uint32_t *tempout = &outbits;
    out = *((float *)tempout);
    return out;
  }

  // 2^7 - 2^10 case, 1 bit mantissa

  if (exp >= 7) {

    //get the odd_bit (1.Oxx_xxxx_xxxx_xxxx_xxxx_xxxx) for ties to even
    uint32_t odd_bit = (mant >> 22) & 1;

    mant = mant + ((1 << (23 - 1 - 1)) - 1 + odd_bit); //round to nearest, ties to even
    
    if((mant >> 23) == 1) {//if overflow through rounding
      exp = exp + 1; //add bias
      mant = 0;
    }

    mant = mant & 0x400000; //truncate mantissa, 400000 is top 1 bits of mantissa
    exp = exp + 127; //add bias
    outbits = (sign << 31| exp << 23| mant);
    uint32_t *tempout = &outbits;
    out = *((float *)tempout);
    return out;
  }

  // 2^4 - 2^6 case, 2 bit mantissa

  if (exp >= 4) {

    //get the odd_bit (1.xOx_xxxx_xxxx_xxxx_xxxx_xxxx) for ties to even
    uint32_t odd_bit = (mant >> 21) & 1;

    mant = mant + ((1 << (23 - 1 - 2)) - 1 + odd_bit); //round to nearest, ties to even
    
    if((mant >> 23) == 1) {//if overflow through rounding
      exp = exp + 1; //add bias
      mant = 0;
    }

    mant = mant & 0x600000; //truncate mantissa, 600000 is top 2 bits of mantissa
    exp = exp + 127; //add bias
    outbits = (sign << 31| exp << 23| mant);
    uint32_t *tempout = &outbits;
    out = *((float *)tempout);
    return out;
  }

  // 2^-4 - 2^3 case, 3 bit mantissa

  if (exp >= -4) {

    //get the odd_bit (1.xxO_xxxx_xxxx_xxxx_xxxx_xxxx) for ties to even
    uint32_t odd_bit = (mant >> 20) & 1;

    mant = mant + ((1 << (23 - 1 - 3)) - 1 + odd_bit); //round to nearest, ties to even
    
    if((mant >> 23) == 1) {//if overflow through rounding
      exp = exp + 1; //add bias
      mant = 0;
    }

    mant = mant & 0x700000; 
    exp = exp + 127; //add bias
    outbits = (sign << 31| exp << 23| mant);
    uint32_t *tempout = &outbits;
    out = *((float *)tempout);
    return out;
  }

  // 2^-7 - 2^-5 case, 2 bit mantissa

  if (exp >= -7) {
    //get the odd_bit (1.xOx_xxxx_xxxx_xxxx_xxxx_xxxx) for ties to even
    uint32_t odd_bit = (mant >> 21) & 1;

    mant = mant + ((1 << (23 - 1 - 2)) - 1 + odd_bit); //round to nearest, ties to even
    
    if((mant >> 23) == 1) {//if overflow through rounding
      exp = exp + 1; //add bias
      mant = 0;
    }

    mant = mant & 0x600000; //truncate mantissa, 600000 is top 2 bits of mantissa
    exp = exp + 127; //add bias
    outbits = (sign << 31| exp << 23| mant);
    uint32_t *tempout = &outbits;
    out = *((float *)tempout);
    return out;
    
  }

  // 2^-11 - 2^-8 case, 1 bit mantissa
  if (exp >= -11) {
    //get the odd_bit (1.Oxx_xxxx_xxxx_xxxx_xxxx_xxxx) for ties to even
    uint32_t odd_bit = (mant >> 22) & 1;

    mant = mant + ((1 << (23 - 1 - 1)) - 1 + odd_bit); //round to nearest, ties to even
    
    if((mant >> 23) == 1) {//if overflow through rounding
      exp = exp + 1; //add bias
      mant = 0;
    }

    mant = mant & 0x400000; //truncate mantissa, 400000 is top 1 bits of mantissa
    exp = exp + 127; //add bias
    outbits = (sign << 31| exp << 23| mant);
    uint32_t *tempout = &outbits;
    out = *((float *)tempout);
    return out;
  }

  // 2^-22 - 2^-12 case, 0 bit mantissa

  //get the odd_bit (O.xxx_xxxx_xxxx_xxxx_xxxx_xxxx) for ties to even
  //uint32_t odd_bit = 1; //odd bit is the implied 1 so no need for odd bit

  mant = mant + (1 << (23 - 1 - 0)); //round to nearest, ties to even
  
  
  if((mant >> 23) == 1) {//if overflow through rounding
    exp = exp + 1; //add bias
    mant = 0;
    
  }

  if(exp < -22){ //underflow to 0
    outbits = 0;//(0 << 31| 0 << 23| 0);  //there is no negative 0
    uint32_t *tempout = &outbits;
    out = *((float *)tempout);
    return out;
  }

  mant = 0; 
  exp = exp + 127; //add bias
  outbits = (sign << 31| exp << 23| mant);
  uint32_t *tempout = &outbits;
  out = *((float *)tempout);
  return out;

}

int power(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}


float cast_format9_to_real(uint8_t origin) {
    // Handle special cases
    if (origin == 0b00000000) {
        return 0.0;
    }
    if (origin == 0b10000000) {
        return NAN;
    }
    if (origin == 0b01111111) {
        return INFINITY;
    }
    if (origin == 0b11111111) {
        return -INFINITY;
    }

    uint8_t sign = (origin >> 7) & 1;
    uint8_t exp, mant;
    uint8_t extracted_exp = origin & 0b01111111;
    int bias;

    if (extracted_exp >= 0b1110100) {
        bias = 105;
        exp = origin & 0b01111111;
        mant = 0;
    } else if (extracted_exp >= 0b1101100) {
        bias = 47;
        exp = (origin >> 1) & 0b00111111;
        mant = (origin & 1) << 2;
    } else if (extracted_exp >= 0b1100000) {
        bias  = 20;
        exp = (origin >> 2) & 0b00011111;
        mant = (origin & 3) << 1;
    } else if (extracted_exp >= 0b0100000) {
        bias = 8;
        exp = (origin >> 3) & 0b00001111;
        mant = origin & 7;
    } else if (extracted_exp >= 0b0010100) {
        bias = 12;
        exp = (origin >> 2) & 0b00011111;
        mant = (origin & 3) << 1;
    } else if (extracted_exp >= 0b0001100) {
        bias = 17;
        exp = (origin >> 1) & 0b00111111;
        mant = (origin & 1) << 2;
    } else if (extracted_exp >= 0b0000000) {
        bias = 23;
        exp = origin & 0b01111111;
        mant = 0;
    }


    int adjusted_exponent = (int)exp - bias;

    // Calculate the real value
    float normalized_mantissa = 1.0 + mant / 8.0;
    // float real = std::ldexp(normalized_mantissa, adjusted_exponent);
    // float real = ldexpf(normalized_mantissa, adjusted_exponent);
    float real = normalized_mantissa * power(2, adjusted_exponent);

    if (sign == 1) {
        real = -real;
    }

    return real;
}

unsigned convert_float_to_bfloat16(float f) {
  unsigned bits = *(unsigned *)&f;

  // truncate to 16 bits
  unsigned result = (bits >> 16) & 0xFFFF;

  return result;
}

int main()
{
  for(int i = 0; i < 256; i++) {
    //for(int j = 0; j < 256; j++) {
      unsigned a = i;
      unsigned b = 255;
      unsigned pab;

      //sync the inputs
      format9_MADD_mult.A8 = a;
      format9_MADD_mult.B8 = b;

      //partition constraint
      __CPROVER_assume(format9_MADD_mult.A8 == a && format9_MADD_mult.B8 == b);

      set_inputs();

      //compute verilog result
      pab = format9_MADD_mult.PAB;

      //compute c result
      float a_f = cast_mpfloat8_v9_nearest(cast_format9_to_real(a));
      float b_f = cast_mpfloat8_v9_nearest(cast_format9_to_real(b));
      float pab_f = a_f * b_f;

      unsigned pab_bf16 = convert_float_to_bfloat16(pab_f);

      //check the output
      assert(pab == pab_bf16);//0x3F80 is 1.0 in bfloat16
    //}
  }

}