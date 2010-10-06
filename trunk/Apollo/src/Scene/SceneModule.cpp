// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgConfig.h"
#include "Local.h"
#include "SceneModule.h"

Surface* SceneModule::CreateSurface(ApHandle hSurface)
{
  Surface* pSurface = new Surface(hSurface);
  if (pSurface) {
    int ok = pSurface->Create();
    if (ok) {
      surfaces_.Set(hSurface, pSurface);
    } else {
      delete pSurface;
      pSurface = 0;
      throw ApException("SceneModule::CreateSurface " ApHandleFormat " Create() failed", ApHandleType(hSurface));
    }
  }

  return pSurface;
}

void SceneModule::DeleteSurface(ApHandle hSurface)
{
  Surface* pSurface = FindSurface(hSurface);
  if (pSurface) {
    pSurface->Destroy();
    surfaces_.Unset(hSurface);
    delete pSurface;
    pSurface = 0;
  }
}

Surface* SceneModule::FindSurface(ApHandle hSurface)
{
  Surface* pSurface = 0;  

  surfaces_.Get(hSurface, pSurface);
  if (pSurface == 0) { throw ApException("SceneModule::FindSurface no surface=" ApHandleFormat "", ApHandleType(hSurface)); }

  return pSurface;
}

//---------------------------

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Create)
{
  if (surfaces_.Find(pMsg->hScene) != 0) { throw ApException("SceneModule::Scene_Create: scene=" ApHandleFormat " already exists", ApHandleType(pMsg->hScene)); }
  Surface* pSurface = CreateSurface(pMsg->hScene);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Destroy)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  DeleteSurface(pMsg->hScene); 
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Position)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->SetPosition(pMsg->nX, pMsg->nY, pMsg->nW, pMsg->nH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Visibility)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->SetVisibility(pMsg->bVisible);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->CreateElement(pMsg->sPath);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_DeleteElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->DeleteElement(pMsg->sPath);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_TranslateElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->Translate(pMsg->fX, pMsg->fY);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_ScaleElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->Scale(pMsg->fX, pMsg->fY);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_RotateElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->Rotate(pMsg->fAngle);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateRectangle)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->CreateRectangle(pMsg->fX, pMsg->fY, pMsg->fW, pMsg->fH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateImage)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->CreateImage(pMsg->fX, pMsg->fY, pMsg->image);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateText)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->CreateText(pMsg->fX, pMsg->fY, pMsg->sText, pMsg->sFont, pMsg->fSize, pMsg->nFlags);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_MeasureText)
{
  Surface* pSurface = FindSurface(pMsg->hScene);

  TextExtents te;
  pSurface->MeasureText(pMsg->sText, pMsg->sFont, pMsg->fSize, pMsg->nFlags, te);
  pMsg->fBearingX = te.fBearingX_;
  pMsg->fBearingY = te.fBearingY_;
  pMsg->fWidth = te.fWidth_;
  pMsg->fHeight = te.fHeight_;
  pMsg->fAdvanceX = te.fAdvanceX_;
  pMsg->fAdvanceY = te.fAdvanceY_;
 
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetFillColor)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetFillColor(pMsg->fRed, pMsg->fGreen, pMsg->fBlue, pMsg->fAlpha);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetStrokeColor)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetStrokeColor(pMsg->fRed, pMsg->fGreen, pMsg->fBlue, pMsg->fAlpha);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetStrokeWidth)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetStrokeWidth(pMsg->fWidth);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetImageData)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetImageData(pMsg->image);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Draw)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->Draw();
  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

#if defined(AP_TEST)

#include "SceneModuleTester.h"

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Scene", 0)) {
    AP_UNITTEST_REGISTER(SceneModuleTester::Rectangle);
    AP_UNITTEST_REGISTER(SceneModuleTester::FontFlags);
    //AP_UNITTEST_REGISTER(SceneModuleTester::ElementTree);
  }
}

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Scene", 0)) {
    AP_UNITTEST_EXECUTE(SceneModuleTester::Rectangle);
    AP_UNITTEST_EXECUTE(SceneModuleTester::FontFlags);
    //AP_UNITTEST_EXECUTE(SceneModuleTester::ElementTree);
  }
}

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int SceneModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Position, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Visibility, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_DeleteElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_TranslateElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_ScaleElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_RotateElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateRectangle, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateImage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateText, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_MeasureText, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetFillColor, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetStrokeColor, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetStrokeWidth, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetImageData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Draw, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(SceneModule, this);

  return ok;
}

void SceneModule::Exit()
{
  AP_UNITTEST_UNHOOK(SceneModule, this);
  AP_MSG_REGISTRY_FINISH;
}
