/*
	MessageBox.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <DXUTGui.h>
#include <string>

#include "UserInterface.h"

enum MessageBox_Questions {
	QUESTION_WHERES_THE_CD,
	QUESTION_QUIT_GAME,
};

/**
	A message box UI element that can contain a title, message text, and buttons.
*/
class MessageBox : public BaseUI
{
public:
	// static methods
	/// Callback for GUIEvents involving this message box
	static void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);
	static int Ask(MessageBox_Questions question);

public:
	/// Constructor to set up the box with a message, title, and buttons
	MessageBox(std::wstring message, std::wstring title, int buttonFlags = MB_OK);
	
	/// Destructor
	~MessageBox();

	// IScreenElement implementation
	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc)
	virtual HRESULT OnRestore();

	/// Render the message box
	virtual HRESULT OnRender(double time, float deltaTime);

	/// Return max z-order -- message boxes are always the top layer
	virtual int GetZOrder() const;

	/// Set z order is disabled
	virtual void SetZOrder(const int zOrder) { }

	/// Msg callback from the view to the message box
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg);

protected:
	/// DirectX dialog box
	CDXUTDialog m_UI;

	/// Id of the button
	int m_ButtonId;
};