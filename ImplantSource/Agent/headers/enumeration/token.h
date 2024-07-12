#pragma once

#include "../agent/agent.h"
#include <ntstatus.h>


HANDLE TokenCurrentHandle(
);
BOOL TokenElevated(
    IN HANDLE Token
);

