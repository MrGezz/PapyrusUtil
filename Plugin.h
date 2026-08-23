#pragma once

#define PAPYRUSUTIL_VERSION 47

#include "skse64/PluginAPI.h"
#include "skse64/GameAPI.h"


struct StaticFunctionTag;


namespace Plugin {
	void InitPlugin();
	bool RegisterFuncs(VMClassRegistry* registry);
	void HandleSKSEMessage(SKSEMessagingInterface::Message * msg);
	UInt32 GetVersion(StaticFunctionTag* base);

}
