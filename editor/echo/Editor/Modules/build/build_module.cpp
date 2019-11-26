#include "build_module.h"
#include "ios_build_settings.h"
#include "mac_build_settings.h"
#include "android_build_settings.h"
#include "windows_build_settings.h"

namespace Echo
{
    BuildModule::BuildModule()
    {
    }

    BuildModule::~BuildModule()
    {

    }

	BuildModule* BuildModule::instance()
	{
		static BuildModule* inst = EchoNew(BuildModule);
		return inst;
	}

	void BuildModule::bindMethods()
	{

	}

    void BuildModule::registerTypes()
    {
        Class::registerType<BuildSettings>();
        Class::registerType<iOSBuildSettings>();
        Class::registerType<MacBuildSettings>();
        Class::registerType<AndroidBuildSettings>();
        Class::registerType<WindowsBuildSettings>();
    }
}
