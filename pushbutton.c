#include "pushbutton.h"

void InitializePushButtonPortPin (void)
{
    ENABLE_PULL_UP_PULL_DOWN_RESISTORS;
    SELECT_PULL_UP_RESISTORS;
    SET_PUSHBUTTON_TO_AN_INPUT;
}
