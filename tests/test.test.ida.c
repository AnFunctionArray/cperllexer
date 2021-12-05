    typedef  int LONG64;


__forceinline
LONG64
UnsignedMultiplyExtract128 (
      LONG64 Multiplier,
      LONG64 Multiplicand,
      LONG64  Shift
    )

{

    LONG64 extractedProduct;
    LONG64 highProduct;
    LONG64 lowProduct;

    lowProduct = _umul128(Multiplier, Multiplicand, &highProduct);
    extractedProduct = __shiftright128(lowProduct, highProduct, Shift);
    return extractedProduct;
}