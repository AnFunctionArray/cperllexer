typedef enum _EXCEPTION_DISPOSITION
{
    ExceptionContinueExecution,
    ExceptionContinueSearch,
    ExceptionNestedException,
    ExceptionCollidedUnwind
} EXCEPTION_DISPOSITION;

        struct _EXCEPTION_RECORD;
        struct _CONTEXT;
        struct _DISPATCHER_CONTEXT;

         EXCEPTION_DISPOSITION __cdecl __C_specific_handler(
                 struct _EXCEPTION_RECORD*   ExceptionRecord,
                 void*                       EstablisherFrame,
              struct _CONTEXT*            ContextRecord,
              struct _DISPATCHER_CONTEXT* DispatcherContext
            );
