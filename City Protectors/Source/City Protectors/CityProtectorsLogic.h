
#pragma once

#include <BaseGameLogic.h>

class CityProtectorsLogic : public BaseGameLogic
{
public:
	CityProtectorsLogic();
	virtual ~CityProtectorsLogic();


protected:

private:

protected:
	std::list<NetworkEventForwarder*> m_NetworkEventForwarders;
};