#include "debounce.h"
#include "pushbutton.h"

#define FALSE 0
#define TRUE 1

extern unsigned int g1msTimer;

void InitializeSwitch(SwitchDefine *Switch,char *SwitchPort,unsigned char SwitchBit,
        unsigned char HoldTime,unsigned char ReleaseTime)
{
    Switch->CurrentState = DbExpectHigh;
    Switch->SwitchPort = SwitchPort;
    Switch->SwitchPortBit = SwitchBit;

    Switch->HoldTime = HoldTime; // units equal milliseconds
    Switch->ReleaseTime = ReleaseTime; // units equal milliseconds
    Switch->EventTime = 0;
}

SwitchStatus GetSwitch(SwitchDefine *Switch)
{
    if (*Switch->SwitchPort & Switch->SwitchPortBit) {
            return Low;
        }
        else {
            return High;
        }
}

SwitchStatus Debouncer(SwitchDefine *Switch)
{
    SwitchStatus CurrentSwitchReading;
    SwitchStatus DebouncedSwitchStatus = Low;
    unsigned char X1;
    unsigned char X0;
    DbState NextState;
    int ElapsedTime = 0;

    // First, determine the current inputs, X1 and X0.
    CurrentSwitchReading = GetSwitch(Switch);

    if (CurrentSwitchReading == Low) {
        X0 = FALSE;
    }
    else {
        X0 = TRUE;
    }

    ElapsedTime = (unsigned int) (g1msTimer - Switch->EventTime); //Compare with event time and current time

    if (Switch->CurrentState == DbValidateHigh) {
        if (ElapsedTime >= Switch->HoldTime) {
            X1 = TRUE;
        }
        else {
            X1 = FALSE;
        }
    }
    else {
        if (ElapsedTime >= Switch->ReleaseTime) {
            X1 = TRUE;
        }
        else {
            X1 = FALSE;
        }
    }

    // Next, based on the input values and the current state, determine the next state.
    switch (Switch->CurrentState) {
        case DbExpectHigh:
            if (X0 == TRUE) {
                NextState = DbValidateHigh;
            }
            else {
                NextState = DbExpectHigh;
            }
        break;
        case DbValidateHigh:
            if (X0 == FALSE) {
                NextState = DbExpectHigh;
            }
            else if (X1 == FALSE) {
                NextState = DbValidateHigh;
            }
            else {
                NextState = DbExpectLow;
            }
        break;
        case DbExpectLow:
            if (X0 == FALSE) {
                NextState = DbValidateLow;
            }
            else {
                NextState = DbExpectLow;
            }
        break;
        case DbValidateLow:
            if (X0 == TRUE) {
                NextState = DbExpectLow;
            }
            else if (X1 == TRUE) {
                NextState = DbExpectHigh;
            }
            else {
                NextState = DbValidateLow;
            }
        break;
        default: NextState = DbExpectHigh;
    }

    // Perform the output function based on the inputs and current state.
    switch (Switch->CurrentState) {
        case DbExpectHigh:
            DebouncedSwitchStatus = Low;
            if (X0 == TRUE) {
                Switch->EventTime = g1msTimer;
            }
        break;
        case DbValidateHigh:
            if ((X1 == TRUE) && (X0 == TRUE)) {
                DebouncedSwitchStatus = High;
            }
            else {
                DebouncedSwitchStatus = Low;
            }
        break;
        case DbExpectLow:
            DebouncedSwitchStatus = High;
            if (X0 == FALSE) {
                Switch->EventTime = g1msTimer;
            }
        break;
        case DbValidateLow:
            if ((X1 == TRUE) && (X0 == FALSE)) {
                DebouncedSwitchStatus = Low;
            }
            else {
                DebouncedSwitchStatus = High;
            }
        break;
    }

    // Finally, update the current state.
    Switch->CurrentState = NextState;

    return DebouncedSwitchStatus;
}

