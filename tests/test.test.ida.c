typedef struct __crt_locale_pointers
{
    char*    locinfo;
    char* mbcinfo;
} __crt_locale_pointers;

typedef __crt_locale_pointers* _locale_t;

    __inline char* __cdecl __acrt_get_locale_data_prefix(void const volatile* const _LocalePointers)
    {
        _locale_t const _TypedLocalePointers;
        return (char*)_TypedLocalePointers->locinfo;
    }