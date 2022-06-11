#ifndef WINVEH_H
#define WINVEH_H

#include <windows.h>
#include <vector>

class WinVeh
{
public:
    /// Add a windows veh. Experimental Function
    /// \details Google breakpad do not catch exception after qt app enters main event loop, so add a veh to catch exceptions.
    /// Test environment: qt5.6.2_mingw, qt5.6.2_msvc2015-32bit, qt5.9.9_mingw, qt5.9.9_msvc2015-32bit
    static void AddVeh();
    /// Veh may catch some unnecessary exception, such as 0x000006ba(print server err). Set the extra exception codes that you want to ignore
    static void SetExtraIgnoredExceptionCodes(const std::vector<unsigned long> &codes);
private:
    static PVOID previous_filter_veh;
    static LONG WINAPI HandleException_Veh(PEXCEPTION_POINTERS exinfo);
    static std::vector<unsigned long> extraIgnoredExceptionCodes;
};


#endif // WINVEH_H
