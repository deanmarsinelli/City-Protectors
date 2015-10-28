/*
	CityProtectorsHumanView.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <D3DRenderer.h>
#include <EventManager.h>
#include <Events.h>
#include <MessageBox.h>
#include <StringUtil.h>
#include <Raycast.h>

#include "CityProtectorsHumanView.h"
#include "CityProtectorsLogic.h"
#include "GameEvents.h"

CityProtectorsHumanView::CityProtectorsHumanView(shared_ptr<IRenderer> renderer) :
HumanView(renderer)
{
	m_ShowUI = true;
	RegisterAllDelegates();
}

CityProtectorsHumanView::~CityProtectorsHumanView()
{
	RemoveAllDelegates();
}

LRESULT CALLBACK CityProtectorsHumanView::OnMsgProc(AppMsg msg)
{
	// a bunch of debug stuff

	if (HumanView::OnMsgProc(msg))
		return 1;

	if (msg.m_uMsg == WM_KEYDOWN)
	{
		if (msg.m_wParam == VK_F1)
		{
			m_ShowUI = !m_ShowUI;
			return 1;
		}
		else if (msg.m_wParam == VK_F2)
		{
			// test the picking API

			POINT ptCursor;
			GetCursorPos(&ptCursor);
			ScreenToClient(g_pApp->GetHwnd(), &ptCursor);

			RayCast rayCast(ptCursor);
			m_pScene->Pick(&rayCast);
			rayCast.Sort();

			if (rayCast.m_NumIntersections)
			{
				// You can iterate through the intersections on the raycast.
				int a = 0;
			}
		}
		else if (msg.m_wParam == VK_F8)
		{
			CityProtectorsLogic* logic = static_cast<CityProtectorsLogic*>(g_pApp->m_pGame);
			logic->ToggleRenderDiagnostics();
		}
		else if (msg.m_wParam == VK_F9)
		{
			m_KeyboardHandler = m_pProtectorController;
			m_PointerHandler = m_pProtectorController;
			m_pCamera->SetTarget(m_pProtector);
			m_pProtector->SetAlpha(0.8f);
			ReleaseCapture();
			return 1;
		}
		else if (msg.m_wParam == VK_F11)
		{
			m_KeyboardHandler = m_pFreeCameraController;
			m_PointerHandler = m_pFreeCameraController;
			m_pCamera->ClearTarget();
			//m_pTeapot->SetAlpha(fOPAQUE);
			SetCapture(g_pApp->GetHwnd());
			return 1;
		}
		else if (msg.m_wParam == 'Q' || msg.m_wParam == VK_ESCAPE)
		{
			if (CBMessageBox::Ask(QUESTION_QUIT_GAME) == IDYES)
			{
				g_pApp->SetQuitting(true);
			}
			return 1;
		}
	}

	return 0;
}

void CityProtectorsHumanView::RenderText()
{
	if (!D3DRenderer::g_pTextHelper)
		return;

	HumanView::RenderText();

	D3DRenderer::g_pTextHelper->Begin();

	// Gameplay UI (with shadow)....
	if (!m_GameplayText.empty())
	{
		D3DRenderer::g_pTextHelper->SetInsertionPos(g_pApp->GetScreenSize().x / 2, 5);
		D3DRenderer::g_pTextHelper->SetForegroundColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
		D3DRenderer::g_pTextHelper->DrawTextLine(m_GameplayText.c_str());
		D3DRenderer::g_pTextHelper->SetInsertionPos(g_pApp->GetScreenSize().x / 2 - 1, 5 - 1);
		D3DRenderer::g_pTextHelper->SetForegroundColor(D3DXCOLOR(0.25f, 1.0f, 0.25f, 1.0f));
		D3DRenderer::g_pTextHelper->DrawTextLine(m_GameplayText.c_str());
	}
	// ...Gameplay UI

	if (m_ShowUI)
	{
		// Output statistics...
		D3DRenderer::g_pTextHelper->SetInsertionPos(5, 5);
		D3DRenderer::g_pTextHelper->SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
		D3DRenderer::g_pTextHelper->DrawTextLine(DXUTGetFrameStats());
		D3DRenderer::g_pTextHelper->DrawTextLine(DXUTGetDeviceStats());
		//...output statistics

		D3DRenderer::g_pTextHelper->SetForegroundColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f));

		//Game State...
		switch (m_BaseGameState)
		{
		case BaseGameState::Initializing:
			D3DRenderer::g_pTextHelper->DrawTextLine(g_pApp->GetString(L"IDS_INITIALIZING").c_str());
			break;

		case BaseGameState::MainMenu:
			D3DRenderer::g_pTextHelper->DrawTextLine(L"Main Menu");
			break;

		case BaseGameState::WaitingForPlayers:
			D3DRenderer::g_pTextHelper->DrawTextLine(g_pApp->GetString(L"IDS_WAITING").c_str());
			break;

		case BaseGameState::LoadingGameEnvironment:
			D3DRenderer::g_pTextHelper->DrawTextLine(g_pApp->GetString(L"IDS_LOADING").c_str());
			break;

		case BaseGameState::Running:
#ifndef DISABLE_PHYSICS
			D3DRenderer::g_pTextHelper->DrawTextLine(g_pApp->GetString(L"IDS_RUNNING").c_str());
#else
			D3DRenderer::g_pTextHelper->DrawTextLine(g_pApp->GetString(L"IDS_NOPHYSICS").c_str());
#endif //!DISABLE_PHYSICS
			break;
		}
		//...Game State

		//Camera...
		TCHAR buffer[256];
		const TCHAR *s = NULL;
		Mat4x4 toWorld;
		Mat4x4 fromWorld;
		if (m_pCamera)
		{
			m_pCamera->Get()->Transform(&toWorld, &fromWorld);
		}
		swprintf(buffer, g_pApp->GetString(L"IDS_CAMERA_LOCATION").c_str(), toWorld.m[3][0], toWorld.m[3][1], toWorld.m[3][2]);
		D3DRenderer::g_pTextHelper->DrawTextLine(buffer);
		//...Camera

		//Help text.  Right justified, lower right of screen.
		RECT helpRect;
		helpRect.left = 0;
		helpRect.right = g_pApp->GetScreenSize().x - 10;
		helpRect.top = g_pApp->GetScreenSize().y - 15 * 8;
		helpRect.bottom = g_pApp->GetScreenSize().y;
		D3DRenderer::g_pTextHelper->SetInsertionPos(helpRect.right, helpRect.top);
		D3DRenderer::g_pTextHelper->SetForegroundColor(D3DXCOLOR(1.0f, 0.75f, 0.0f, 1.0f));
		D3DRenderer::g_pTextHelper->DrawTextLine(helpRect, DT_RIGHT, g_pApp->GetString(L"IDS_CONTROLS_HEADER").c_str());
		helpRect.top = g_pApp->GetScreenSize().y - 15 * 7;
		D3DRenderer::g_pTextHelper->DrawTextLine(helpRect, DT_RIGHT, g_pApp->GetString(L"IDS_CONTROLS").c_str());
		//...Help
	}//end if (m_bShowUI)

	D3DRenderer::g_pTextHelper->End();
}

void CityProtectorsHumanView::OnUpdate(float deltaTime)
{
	HumanView::OnUpdate(deltaTime);

	if (m_pFreeCameraController)
	{
		m_pFreeCameraController->OnUpdate(deltaTime);
	}

	if (m_pProtectorController)
	{
		m_pProtectorController->OnUpdate(deltaTime);
	}

	// set out a tick event to any listeners
	shared_ptr<Event_UpdateTick> pEvent(CB_NEW Event_UpdateTick(deltaTime));
	IEventManager::Get()->TriggerEvent(pEvent);
}

void CityProtectorsHumanView::OnAttach(GameViewId viewId, GameObjectId objectId)
{
	HumanView::OnAttach(viewId, objectId);
}

void CityProtectorsHumanView::SetControlledGameObject(GameObjectId objectId)
{
	m_pProtector = static_pointer_cast<SceneNode>(m_pScene->FindObject(objectId));
	if (!m_pProtector)
	{
		CB_ERROR("Invalid protector");
		return;
	}

	HumanView::SetControlledGameObject(objectId);

	m_pProtectorController.reset(CB_NEW ProtectorController(m_pProtector));
	m_KeyboardHandler = m_pProtectorController;
	m_PointerHandler = m_pProtectorController;
	m_pCamera->SetTarget(m_pProtector);
	m_pProtector->SetAlpha(0.8f);
}

bool CityProtectorsHumanView::LoadGameDelegate(TiXmlElement* pLevelData)
{
	// call base class delegate first
	if (!HumanView::LoadGameDelegate(pLevelData))
	{
		return false;
	}

	// set up standard HUD
	m_StandardHUD.reset(CB_NEW StandardHUD);
	PushElement(m_StandardHUD);

	// a movement controller is going to control the camera, 
	m_pFreeCameraController.reset(CB_NEW MovementController(m_pCamera, 0, 0, false));

	m_pScene->OnRestore();
	return true;
}

void CityProtectorsHumanView::GameplayUiUpdateDelegate(IEventPtr pEvent)
{
	// cast the event to a ui event, and update ui text
	shared_ptr<Event_GameplayUIUpdate> pCastEvent = static_pointer_cast<Event_GameplayUIUpdate>(pEvent);
	if (!pCastEvent->GetUIString().empty())
	{
		m_GameplayText = s2ws(pCastEvent->GetUIString());
	}
	else
	{
		m_GameplayText.clear();
	}
}

void CityProtectorsHumanView::SetControlledObjectDelegate(IEventPtr pEvent)
{
	// cast the event to a set controlled object event, and forward the object ID
	shared_ptr<Event_SetControlledObject> pCastEvent = static_pointer_cast<Event_SetControlledObject>(pEvent);
	SetControlledGameObject(pCastEvent->GetObjectId());
}

void CityProtectorsHumanView::RegisterAllDelegates()
{
	// add event listeners
	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsHumanView::GameplayUiUpdateDelegate), Event_GameplayUIUpdate::sk_EventType);
	pGlobalEventManager->AddListener(fastdelegate::MakeDelegate(this, &CityProtectorsHumanView::SetControlledObjectDelegate), Event_SetControlledObject::sk_EventType);
}

void CityProtectorsHumanView::RemoveAllDelegates()
{
	// remove event listeners
	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsHumanView::GameplayUiUpdateDelegate), Event_GameplayUIUpdate::sk_EventType);
	pGlobalEventManager->RemoveListener(fastdelegate::MakeDelegate(this, &CityProtectorsHumanView::SetControlledObjectDelegate), Event_SetControlledObject::sk_EventType);
}
