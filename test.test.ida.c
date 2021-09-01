typedef enum _EXCEPTION_DISPOSITION
{
    ExceptionContinueExecution,
    ExceptionContinueSearch,
    ExceptionNestedException,
    ExceptionCollidedUnwind
} EXCEPTION_DISPOSITION;








    EXCEPTION_DISPOSITION __cdecl _except_handler(
          struct _EXCEPTION_RECORD* _ExceptionRecord,
          void*                     _EstablisherFrame,
          struct _CONTEXT*       _ContextRecord,
          void*                  _DispatcherContext
        );

int printf(const char*pfmt, ...);

int main() {
    return printf("%d\n", ExceptionNestedException);
}