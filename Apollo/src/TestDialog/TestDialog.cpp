// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "TestDialog.h"
#include "MsgDialog.h"

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

#define MODULE_NAME "TestDialog"

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME, // szName
  "", // szServices
  "TestDialog Module", // szLongName
  "1", // szVersion
  "Shows a dialog with test controls", // szDescription
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

TESTDIALOG_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

class TestDialogModule
{
public:
  TestDialogModule() {}
  virtual ~TestDialogModule() {}

  void Init();
  void Exit();
  void Open();

  void On_TestDialog_Control(ApSRPCMessage* pMsg);

protected:
  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<TestDialogModule> TestDialogModuleInstance;

//----------------------------------------------------------

void TestDialogModule::Init()
{
  if (Apollo::getModuleConfig(MODULE_NAME, "OpenOnStartup", 0)) {
    Open();
  }
}

void TestDialogModule::Exit()
{
}

void TestDialogModule::Open()
{
  Msg_Dialog_Create msg;
  msg.hDialog = Apollo::newHandle();
  msg.nLeft = Apollo::getModuleConfig(MODULE_NAME, "Left", 200);
  msg.nTop = Apollo::getModuleConfig(MODULE_NAME, "Top", 200);
  msg.nWidth = Apollo::getModuleConfig(MODULE_NAME, "Width", 500);
  msg.nHeight = Apollo::getModuleConfig(MODULE_NAME, "Height", 300);
  msg.bVisible = 1;
  msg.sCaption = "Test Controls";
  msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "icon.png";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "index.html";
  if (!msg.Request()) { throw ApException("Msg_Dialog_Create failed: %s", StringType(msg.sComment)); }
}

//----------------------------------------------------------

AP_SRPC_HANDLER_METHOD(TestDialogModule, TestDialog_Control, ApSRPCMessage)
{
  String sMethod = pMsg->srpc.getString("Method");
  if (0) {
  } else if (sMethod == "Open") {
    Open();
  }

  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

TESTDIALOG_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  TestDialogModuleInstance::Delete();
  TestDialogModuleInstance::Get()->Init();
 
  { ApSRPCMessage msg("TestDialog_Control"); msg.Hook(MODULE_NAME, AP_REFINSTANCE_SRPC_CALLBACK(TestDialogModule, TestDialog_Control), TestDialogModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }

  return 1;
}

TESTDIALOG_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { ApSRPCMessage msg("TestDialog_Control"); msg.Unhook(MODULE_NAME, AP_REFINSTANCE_SRPC_CALLBACK(TestDialogModule, TestDialog_Control), TestDialogModuleInstance::Get()); }

  TestDialogModuleInstance::Get()->Exit();
  TestDialogModuleInstance::Delete();

  return 1;
}