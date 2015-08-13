/*
	Console.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Console.h"

#include "D3DRenderer.h"
#include "StringUtil.h"
#include "WindowsApp.h"

// constants used in the console
const float kCursorBlinkTimeMS = 500.0f;
const char const* kExitString = "exit";
const char const* kClearString = "clear";

Console::Console() :
m_IsActive(false),
m_ExecutesStringOnUpdate(false),
m_CursorOn(true)
{
	m_ConsoleInputSize = 48;
	m_CurrentInputString = std::string("");

	// input and output are both white
	m_InputColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_OutputColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	m_CursorBlinkTimer = kCursorBlinkTimeMS;
}

void Console::AddDisplayText(const std::string& text)
{
	// add a new line of text to the output
	m_CurrentOutputString += text;
	m_CurrentOutputString += '\n';
}

void Console::SetDisplayText(const std::string& text)
{
	// overwrite the output string
	m_CurrentOutputString = text;
}

void Console::SetActive(const bool isActive)
{
	m_IsActive = isActive;
}

bool Console::IsActive() const
{
	return m_IsActive;
}

void Console::HandleKeyboardInput(const unsigned int keyVal, const unsigned int oemKeyVal, const bool keyDown)
{
	if (m_ExecutesStringOnUpdate == true)
	{
		// already have a string to execute, ignore
		return;
	}

	switch (oemKeyVal)
	{
	case VK_BACK:
	{
		// if backspace is pressed and theres more than 0 characters, erase one
		const size_t strSize = m_CurrentInputString.size();
		if (strSize > 0)
		{
			m_CurrentInputString.erase((strSize - 1), 1);
		}
		break;
	}
	case VK_RETURN:
		// execute the input string
		m_ExecutesStringOnUpdate = true;
		break;
	
	default:
		// append the current key to the input string
		m_CurrentInputString += (char)keyVal;
		break;
	}
}

void Console::Update(const float deltaTime)
{
	// early return if console is not active
	if (!m_IsActive)
		return;

	// is there a string command to execute?
	if (m_ExecutesStringOnUpdate)
	{
		const std::string inputString = std::string(">") + m_CurrentInputString;
		if (m_CurrentInputString.compare(kExitString) == 0)
		{
			// if the input string is "exit"
			SetActive(false);
			m_CurrentInputString.clear();
		}
		else if (m_CurrentInputString.compare(kClearString) == 0)
		{
			// if the input string is "clear"
			m_CurrentOutputString = inputString;
			m_CurrentInputString.clear();
		}
		else
		{
			// add the input string to the output window
			AddDisplayText(inputString);

			const int retVal = true;
			// attempt to execute the input string if its not empty
			if (!m_CurrentInputString.empty())
			{
				LuaStateManager::Get()->ExecuteString(m_CurrentInputString.c_str());
			}

			// clear the input string
			m_CurrentInputString.clear();
		}
		m_ExecutesStringOnUpdate = false;
	}

	// blink the cursor if necessary
	m_CursorBlinkTimer -= deltaTime;
	if (m_CursorBlinkTimer < 0.0f)
	{
		m_CursorBlinkTimer = 0.0f;
		m_CursorOn = !m_CursorOn;
		m_CursorBlinkTimer = kCursorBlinkTimeMS;
	}
}

void Console::Render()
{
	// early return if console is not active
	if (!m_IsActive)
		return;

	// start the text helper
	D3DRenderer::g_pTextHelper->Begin();
	const D3DXCOLOR white(1.0f, 1.0f, 1.0f, 1.0f);
	const D3DXCOLOR black(0.0f, 0.0f, 0.0f, 0.0f);
	RECT inputTextRect, outputTextRect, shadowRect;

	// display the console text at the top of the screen along with the cursor
	const std::string finalInputString = std::string(">") + m_CurrentInputString + (m_CursorOn ? '_' : '\xa0');
	inputTextRect.left = 10;
	inputTextRect.right = g_pApp->GetScreenSize().x - 10;
	inputTextRect.top = 100;
	inputTextRect.bottom = g_pApp->GetScreenSize().y - 10;

	// conver the input string to wide char
	const int kNumWideChars = 4096;
	WCHAR wideBuffer[kNumWideChars];
	AnsiToWideCch(wideBuffer, finalInputString.c_str(), kNumWideChars);

	// draw the text
	D3DRenderer::g_pTextHelper->DrawTextLine(inputTextRect, DT_LEFT | DT_TOP | DT_CALCRECT, wideBuffer);

	// draw with shadow
	shadowRect = inputTextRect;
	shadowRect.left++;
	shadowRect.top++;
	D3DRenderer::g_pTextHelper->SetForegroundColor(black);
	D3DRenderer::g_pTextHelper->DrawTextLine(shadowRect, DT_LEFT | DT_TOP, wideBuffer);

	// draw with bright text
	D3DRenderer::g_pTextHelper->SetForegroundColor(white);
	D3DRenderer::g_pTextHelper->DrawFormattedTextLine(inputTextRect, DT_LEFT | DT_TOP, wideBuffer);

	// display output text below the input text
	outputTextRect.left = inputTextRect.left + 15;
	outputTextRect.top = inputTextRect.bottom + 15;
	outputTextRect.right = g_pApp->GetScreenSize().x - 10;
	outputTextRect.bottom = g_pApp->GetScreenSize().y - 10;
	AnsiToWideCch(wideBuffer, m_CurrentInputString.c_str(), kNumWideChars);

	// draw output with shadow
	shadowRect = outputTextRect;
	shadowRect.left++;
	shadowRect.top++;
	D3DRenderer::g_pTextHelper->SetForegroundColor(black);
	D3DRenderer::g_pTextHelper->DrawTextLine(shadowRect, DT_LEFT | DT_TOP, wideBuffer);

	// draw output bright 
	D3DRenderer::g_pTextHelper->SetForegroundColor(white);
	D3DRenderer::g_pTextHelper->DrawTextLine(outputTextRect, DT_LEFT | DT_TOP, wideBuffer);

	D3DRenderer::g_pTextHelper->End();
}