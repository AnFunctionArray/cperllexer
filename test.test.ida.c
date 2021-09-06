
typedef struct __crt_locale_pointers
{
    void*    locinfo;
    void* mbcinfo;
} __crt_locale_pointers;

typedef __crt_locale_pointers* _locale_t;

__inline char* __cdecl __acrt_get_locale_data_prefix(void const volatile* const _LocalePointers)
{
    _locale_t const _TypedLocalePointers = (_locale_t)_LocalePointers;
    return (char*)_TypedLocalePointers->locinfo;
}
int main() {
    a a;

    a = 'C';

    typedef int f;

    (f)(a);

    return printf("%c f\n", (a)a);
}