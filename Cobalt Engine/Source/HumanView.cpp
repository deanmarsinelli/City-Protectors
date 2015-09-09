/*
	HumanView.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "HumanView.h"

#include "Audio.h"
#include "DirectSoundAudio.h"
#include "CameraNode.h"
#include "EngineStd.h"
#include "Frustrum.h"
#include "Logger.h"

const GameViewId CB_INVALID_GAMEVIEW_ID = 0xffffffff;

const unsigned int SCREEN_REFRESH_RATE(1000 / 60);

HumanView::HumanView(shared_ptr<IRenderer> renderer)
{
	InitAudio();

	m_pProcessManager = CB_NEW ProcessManager;

	m_PointerRadius = 1;
	m_ViewId = CB_INVALID_GAMEVIEW_ID;

	RegisterAllDelegates();
	m_BaseGameState = BaseGameState::Initializing;

	if (renderer)
	{
		m_pScene.reset(CB_NEW ScreenElementScene(renderer));
		
		Frustrum frustrum;
		
	}

	m_LastDraw = 0;
}

HumanView::~HumanView()
{
	RemoveAllDelegates();

	// remove all UI elements
	while (!m_ScreenElements.empty())
	{
		m_ScreenElements.pop_front();
	}

	CB_SAFE_DELETE(m_pProcessManager);
	CB_SAFE_DELETE(g_pAudio);
}

bool HumanView::LoadGame(TiXmlElement* pLevelData)
{
	// call the deletgate load method
	return LoadGameDelegate(pLevelData);
}

bool HumanView::InitAudio()
{
	// create and initalize the global audio system
	if (!g_pAudio)
	{
#ifdef DIRECTX
		g_pAudio = CB_NEW DirectSoundAudio();
#endif
	}

	if (!g_pAudio)
		return false;
	
	if (!g_pAudio->Initialize(g_pApp->GetHwnd()))
		return false;

	return true;
}

HRESULT HumanView::OnRestore()
{
	// recursively call OnRestore for everything attached to this view
	HRESULT hr = S_OK;

	for (ScreenElementList::iterator it = m_ScreenElements.begin(); it != m_ScreenElements.end(); ++it)
	{
		V_RETURN((*it)->OnRestore());
	}

	return hr;
}

void HumanView::OnRender(float time, float deltaTime)
{
	m_CurrTick = timeGetTime();
	if (m_CurrTick = m_LastDraw)
		return;

	// Is it time to draw?
	if (m_RunFullSpeed || (m_CurrTick - m_LastDraw) > SCREEN_REFRESH_RATE)
	{
		// pre-render the scene
		if (g_pApp->m_Renderer->PreRender())
		{
			// sort UI screen elements 
			m_ScreenElements.sort(SortBy_SharedPtr_Content<IScreenElement>());
			
			// render screen elements
			for (ScreenElementList::iterator it = m_ScreenElements.begin(); it != m_ScreenElements.end(); ++it)
			{
				if ((*it)->IsVisible())
				{
					(*it)->OnRender(time, deltaTime);
				}
			}

			RenderText();

			// render the console
			m_Console.Render();

			// record the last successful draw
			m_LastDraw = m_CurrTick;
		}

		g_pApp->m_Renderer->PostRender();
	}
}

HRESULT HumanView::OnLostDevice()
{
	// recursively call OnLostDevice for everything attached to this view
	HRESULT hr;

	for (ScreenElementList::iterator it = m_ScreenElements.begin(); it != m_ScreenElements.end(); ++it)
	{
		V_RETURN((*it)->OnLostDevice());
	}

	return S_OK;
}

GameViewType HumanViewGetType()
{
	return GameViewType::GameView_Human;
}

GameViewId HumanView::GetId() const
{
	return m_ViewId;
}

void HumanView::OnAttach(GameViewId viewId, GameObjectId objectId)
{
	m_ViewId = viewId;
	m_GameObjectId = objectId;
}

LRESULT CALLBACK HumanView::OnMsgProc(AppMsg msg)
{
	// iterate the screen layers in reverse order to send input messages to the screen on top
	for (ScreenElementList::reverse_iterator it = m_ScreenElements.rbegin(); it != m_ScreenElements.rend(); ++it)
	{
		if ((*it)->IsVisible())
		{
			if ((*it)->OnMsgProc(msg))
			{
				return 1;
			}
		}
	}

	LRESULT result = 0;
	switch (msg.m_uMsg)
	{
	case WM_KEYDOWN:
		if (m_Console.IsActive())
		{
			// let the console eat this key stroke
			break;
		}
		else if (m_KeyboardHandler)
		{
			result = m_KeyboardHandler->OnKeyDown(static_cast<const BYTE>(msg.m_wParam));
		}
		break;

	case WM_KEYUP:
		if (m_Console.IsActive())
		{
			// let the console eat this key stroke
			break;
		}
		else if (m_KeyboardHandler)
		{
			result = m_KeyboardHandler->OnKeyUp(static_cast<const BYTE>(msg.m_wParam));
		}
		break;

	case WM_MOUSEMOVE:
		if (m_PointerHandler)
		{
			result = m_PointerHandler->OnPointerMove(Point(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam)), m_PointerRadius);
		}
		break;

	case WM_LBUTTONDOWN:
		if (m_PointerHandler)
		{
			SetCapture(msg.m_hWnd);
			result = m_PointerHandler->OnPointerButtonDown(Point(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam)), m_PointerRadius, "PointerLeft");
		}
		break;

	case WM_LBUTTONUP:
		if (m_PointerHandler)
		{
			SetCapture(nullptr);
			result = m_PointerHandler->OnPointerButtonUp(Point(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam)), m_PointerRadius, "PointerLeft");
		}
		break;

	case WM_RBUTTONDOWN:
		if (m_PointerHandler)
		{
			SetCapture(msg.m_hWnd);
			result = m_PointerHandler->OnPointerButtonDown(Point(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam)), m_PointerRadius, "PointerRight");
		}
		break;

	case WM_RBUTTONUP:
		if (m_PointerHandler)
		{
			SetCapture(nullptr);
			result = m_PointerHandler->OnPointerButtonUp(Point(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam)), m_PointerRadius, "PointerRight");
		}
		break;

	case WM_CHAR:
		if (m_Console.IsActive())
		{
			const unsigned int oemScan = (int)(msg.m_lParam & (0xff << 16)) >> 16;
			m_Console.HandleKeyboardInput(msg.m_wParam, MapVirtualKey(oemScan, 1), true);
		}
		else
		{
			// if the console is not active, and the console key is pressed activate it
			if (msg.m_wParam == '~' || msg.m_wParam == '`')
			{
				m_Console.SetActive(true);
			}
		}
		break;

	default:
		return 0;
	}

	return 0;
}

void HumanView::OnUpdate(float deltaTime)
{
	// update any processes 
	m_pProcessManager->UpdateProcesses(deltaTime);

	// update the console
	m_Console.Update(deltaTime);

	// update every screen element
	for (ScreenElementList::iterator it = m_ScreenElements.begin(); it != m_ScreenElements.end(); ++it)
	{
		(*it)->OnUpdate(deltaTime);
	}
}

void HumanView::PushElement(shared_ptr<IScreenElement> pElement)
{
	m_ScreenElements.push_front(pElement);
}

void HumanView::RemoveElement(shared_ptr<IScreenElement> pElement)
{
	m_ScreenElements.remove(pElement);
}

void HumanView::TogglePause(bool active)
{
	if (active)
	{
		if (g_pAudio)
			g_pAudio->PauseAllSounds();
	}
	else
	{
		if (g_pAudio)
			g_pAudio->ResumeAllSounds();
	}
}

ProcessManager* HumanView::GetProcessManager()
{
	return m_pProcessManager;
}

void HumanView::SetControlledGameObject(GameObjectId objectId)
{
	m_GameObjectId = objectId;
}

void HumanView::SetCameraOffset(const Vec4& offset)
{
	CB_ASSERT(m_pCamera);
	if (m_pCamera)
	{
		m_pCamera->SetCameraOffset(offset);
	}
}

void HumanView::PlaySoundDelegate(IEventPtr pEventData)
{
	// TODO
}

void HumanView::GameStateDelegate(IEventPtr pEventData)
{
	// TODO
}

Console& HumanView::GetConsole()
{
	return m_Console;
}

bool HumanView::LoadGameDelegate(TiXmlElement* pLevelData)
{
	PushElement(m_pScene);

	return true;
}

void HumanView::RegisterAllDelegates()
{
	// TODO
}

void HumanView::RemoveAllDelegates()
{
	// TODO
}