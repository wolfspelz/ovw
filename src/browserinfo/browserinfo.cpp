// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "browserinfo.h"
#include "MsgBrowserInfo.h"
#include "MsgUnitTest.h"
#include "Local.h"
#include "BrowserInfoModule.h"

#if defined(WIN32)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  AP_MEMORY_CHECK(dwReason);
  AP_UNUSED_ARG(hModule);
  AP_UNUSED_ARG(dwReason);
  AP_UNUSED_ARG(lpReserved);
  return TRUE;
}
#endif // defined(WIN32)

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "BrowserInfo",
  "Brower window info module",
  "1",
  "Brower window tracker. Tracks native browser windows and tells position, size, visibility.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

BROWSERINFO_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

BROWSERINFO_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 0;

  BrowserInfoModuleInstance::Delete();
  if (BrowserInfoModuleInstance::Get() != 0) { ok = BrowserInfoModuleInstance::Get()->Init(); }

  return ok;
}

BROWSERINFO_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 1;

  if (BrowserInfoModuleInstance::Get() != 0) {
    BrowserInfoModuleInstance::Get()->Exit();
    BrowserInfoModuleInstance::Delete();
  }

  return ok;
}
