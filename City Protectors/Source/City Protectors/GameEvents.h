/*
	GameEvents.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham


	Game specific events
*/

#include <EngineStd.h>
#include <Logger.h>
#include <LuaScriptEvent.h>

/**
	This event is sent when a weapon is fired.
*/
class Event_FireWeapon : public LuaScriptEvent
{
public:
	Event_FireWeapon()
	{
		m_Id = INVALID_GAMEOBJECT_ID;
	}

	Event_FireWeapon(GameObjectId id) :
		m_Id(id)
	{ }

	virtual const EventType& GetEventType() const override
	{
		return sk_EventType;
	}

	virtual void Serialize(std::ostream& out) const override
	{
		out << m_Id << " ";
	}

	virtual void Deserialize(std::istream& in) override
	{
		in >> m_Id;
	}

	virtual IEventPtr Copy() const override
	{
		return IEventPtr(CB_NEW Event_FireWeapon(m_Id));
	}

	virtual const char* GetName() const override
	{
		return "Event_FireWeapon";
	}

	/// Allow this event to be fired from script and received by C++
	virtual bool BuildEventFromScript() override
	{
		if (m_Event.IsTable())
		{
			LuaPlus::LuaObject temp = m_Event.GetByName("id");
			if (temp.IsInteger())
			{
				m_Id = temp.GetInteger();
			}
			else
			{
				CB_ERROR("Invalid id sent to Event_FireWeapon; type = " + std::string(temp.TypeName()));
				return false;
			}
			return true;
		}

		return false;
	}

	GameObjectId GetObjectId() const
	{
		return m_Id;
	}

	EXPORT_FOR_SCRIPT_EVENT(Event_FireWeapon);

public:
	static const EventType sk_EventType;

private:
	GameObjectId m_Id;
};


/**
	This event is sent when there is a gameplay UI update.
*/
class Event_GameplayUIUpdate : public LuaScriptEvent
{
	Event_GameplayUIUpdate()
	{ }

	Event_GameplayUIUpdate(const std::string& gameplayUIString) :
		m_GameplayUIString(gameplayUIString)
	{ }

	virtual const EventType& GetEventType() const override
	{
		return sk_EventType;
	}

	virtual void Serialize(std::ostream& out) const override
	{
		out << m_GameplayUIString << " ";
	}

	virtual void Deserialize(std::istream& in) override
	{
		in >> m_GameplayUIString;
	}

	virtual IEventPtr Copy() const override
	{
		return IEventPtr(CB_NEW Event_GameplayUIUpdate(m_GameplayUIString));
	}

	virtual const char* GetName() const override
	{
		return "Event_GameplayUIUpdate";
	}

	/// Allow this event to be fired from script and received by C++
	virtual bool BuildEventFromScript() override
	{
		if (m_Event.IsString())
		{
			m_GameplayUIString = m_Event.GetString();
			return true;
		}

		return false;
	}

	const std::string& GetUIString() const
	{
		return m_GameplayUIString;
	}

	EXPORT_FOR_SCRIPT_EVENT(Event_GameplayUIUpdate);

public:
	static const EventType sk_EventType;

private:
	std::string m_GameplayUIString;
};


/**
	This event is sent a game object becomes controlled by the player.
*/
class Event_SetControlledObject : public LuaScriptEvent
{
	Event_SetControlledObject()
	{ }

	Event_SetControlledObject(GameObjectId id) :
		m_Id(id)
	{ }

	virtual const EventType& GetEventType() const override
	{
		return sk_EventType;
	}

	virtual void Serialize(std::ostream& out) const override
	{
		out << m_Id;
	}

	virtual void Deserialize(std::istream& in) override
	{
		in >> m_Id;
	}

	virtual IEventPtr Copy() const override
	{
		return IEventPtr(CB_NEW Event_SetControlledObject(m_Id));
	}

	virtual const char* GetName() const override
	{
		return "Event_SetControlledObject";
	}

	/// Allow this event to be fired from script and received by C++
	virtual bool BuildEventFromScript() override
	{
		if (m_Event.IsInteger())
		{
			m_Id = m_Event.GetInteger();
			return true;
		}

		return false;
	}

	const GameObjectId& GetObjectId() const
	{
		return m_Id;
	}

	EXPORT_FOR_SCRIPT_EVENT(Event_SetControlledObject);

public:
	static const EventType sk_EventType;

private:
	GameObjectId m_Id;
};
