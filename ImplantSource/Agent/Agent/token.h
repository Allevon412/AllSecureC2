#include "agent.h"
#include <ntstatus.h>


HANDLE TokenCurrentHandle(
    pAgent agent
);
BOOL TokenElevated(
    IN HANDLE Token, pAgent agent
);

