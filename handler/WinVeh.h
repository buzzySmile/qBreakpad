#ifndef WINVEH_H
#define WINVEH_H

#include <windows.h>

class WinVeh
{
public:
    /// Add A Windows VEH
    static void AddVeh();
private:
    static PVOID previous_filter_veh;
    static LONG WINAPI HandleException_Veh(PEXCEPTION_POINTERS exinfo);
};


#endif // WINVEH_H
