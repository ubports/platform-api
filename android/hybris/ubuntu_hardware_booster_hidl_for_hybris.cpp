/*
 * Copyright © 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Erfan Abdi <erfangplus@gmail.com>
 */

#include <ubuntu/hardware/booster.h>

#include <pthread.h>
#include <string.h>
#include <unistd.h>

// android stuff
#include <android/hardware/power/1.3/IPower.h>

#include <cutils/properties.h>

#include <utils/Log.h>
#include <utils/RefBase.h>

#include <dlfcn.h>

using android::OK;
using android::sp;
using android::wp;
using android::status_t;

using android::hardware::Return;
using android::hidl::base::V1_0::IBase;

using IPower_V1_0 = android::hardware::power::V1_0::IPower;
using IPower_V1_1 = android::hardware::power::V1_1::IPower;
using IPower_V1_2 = android::hardware::power::V1_2::IPower;
using IPower_V1_3 = android::hardware::power::V1_3::IPower;

sp<IPower_V1_0> powerHal = nullptr;
sp<IPower_V1_1> powerHal_V1_1 = nullptr;
sp<IPower_V1_2> powerHal_V1_2 = nullptr;
sp<IPower_V1_3> powerHal_V1_3 = nullptr;

struct UbuntuHardwareBooster : public android::RefBase
{
    bool init();
    void setInteractive(bool interactive);

    typedef void (*BoosterEnableScenario)(int);
    typedef void (*BoosterDisableScenario)(int);

    static int translate_ubuntu_scenario(UHardwareBoosterScenario scenario)
    {
        switch (scenario)
        {
            case U_HARDWARE_BOOSTER_SCENARIO_USER_INTERACTION:
                return 5;
        }

        return 0;
    }

    static BoosterEnableScenario load_booster_enable_scenario(void* dl_handle)
    {
        if (!dl_handle)
            return NULL;

        return reinterpret_cast<BoosterEnableScenario>(::dlsym(dl_handle, booster_enable_scenario_from_property()));
    }

    static BoosterDisableScenario load_booster_disable_scenario(void* dl_handle)
    {
        if (!dl_handle)
            return NULL;

        return reinterpret_cast<BoosterDisableScenario>(::dlsym(dl_handle, booster_disable_scenario_from_property()));
    }

    static const char* dl_path_from_property()
    {
        static char value[PROP_VALUE_MAX];
        static int rc = property_get("ubuntu.booster.dl", value, NULL);

        return rc == 0 ? NULL : &value[0];
    }

    static const char* booster_enable_scenario_from_property()
    {
        static char value[PROP_VALUE_MAX];
        static int rc = property_get("ubuntu.booster.enable", value, NULL);

        return rc == 0 ? NULL : &value[0];
    }

    static const char* booster_disable_scenario_from_property()
    {
        static char value[PROP_VALUE_MAX];
        static int rc = property_get("ubuntu.booster.disable", value, NULL);

        return rc == 0 ? NULL : &value[0];
    }

    UbuntuHardwareBooster()
    {
        const char* dl_path = dl_path_from_property();

        if (dl_path)
            dl_handle = dlopen(dl_path, RTLD_NOW);

        if (!dl_handle)
            return;

        booster_enable_scenario = load_booster_enable_scenario(dl_handle);
        booster_disable_scenario = load_booster_disable_scenario(dl_handle);
    }

    ~UbuntuHardwareBooster()
    {
        if (dl_handle)
            dlclose(dl_handle);
    }

    void enable_scenario(UHardwareBoosterScenario scenario)
    {
        if (booster_enable_scenario)
            booster_enable_scenario(translate_ubuntu_scenario(scenario));
    }

    void disable_scenario(UHardwareBoosterScenario scenario)
    {
        if (booster_disable_scenario)
            booster_disable_scenario(translate_ubuntu_scenario(scenario));
    }

    void* dl_handle = nullptr;
    BoosterEnableScenario booster_enable_scenario = nullptr;
    BoosterDisableScenario booster_disable_scenario = nullptr;
};

static void set_power_service_handle() {
    powerHal_V1_3 = IPower_V1_3::getService();
    if (powerHal_V1_3 != nullptr) {
        powerHal = powerHal_V1_3;
        powerHal_V1_1 = powerHal_V1_3;
        powerHal_V1_2 = powerHal_V1_3;
        return;
    }

    ALOGD("powerHal 1.3 was null, trying 1.2");
    powerHal_V1_2 = IPower_V1_2::getService();
    if (powerHal_V1_2 != nullptr) {
        powerHal = powerHal_V1_2;
        powerHal_V1_1 = powerHal_V1_2;
        return;
    }

    ALOGD("powerHal 1.2 was null, trying 1.1");
    powerHal_V1_1 = IPower_V1_1::getService();
    if (powerHal_V1_1 != nullptr) {
        powerHal = powerHal_V1_1;
        return;
    }
    ALOGD("powerHal 1.1 was null, trying 1.0");
    powerHal = IPower_V1_0::getService();
}

bool UbuntuHardwareBooster::init()
{
    /* Initializes the Power service handle. */
    set_power_service_handle();
    
    if (powerHal)
        return true;

    ALOGE("Unable to get Power service\n");
    return false;
}

void UbuntuHardwareBooster::setInteractive(bool interactive)
{
    if (powerHal)
        powerHal->setInteractive(interactive);

    if (dl_handle) {
        if (interactive)
            enable_scenario(U_HARDWARE_BOOSTER_SCENARIO_USER_INTERACTION);
        else
            disable_scenario(U_HARDWARE_BOOSTER_SCENARIO_USER_INTERACTION);
    }
}

UHardwareBooster*
u_hardware_booster_new()
{
    UHardwareBooster* u_hardware_booster = new UbuntuHardwareBooster();

    // Try ten times to initialize the Power HAL interface,
    // sleeping for 200ms per iteration in case of issues.
    for (unsigned int i = 0; i < 50; i++)
        if (u_hardware_booster->init())
            return u_hardware_booster;
        else
            // Sleep for some time and leave some time for the system
            // to finish initialization.
            ::usleep(200 * 1000);
    
    // This is the error case, as we did not succeed in initializing the Power interface.
    return u_hardware_booster;
}

void
u_hardware_booster_ref(UHardwareBooster* booster)
{
    if (booster)
        booster->incStrong(NULL);
}

void
u_hardware_booster_unref(UHardwareBooster* booster)
{
    if (booster)
        booster->decStrong(NULL);
}

void
u_hardware_booster_enable_scenario(UHardwareBooster* booster, UHardwareBoosterScenario scenario)
{
    if (booster)
        if (scenario == U_HARDWARE_BOOSTER_SCENARIO_USER_INTERACTION)
            booster->setInteractive(true);
}

void
u_hardware_booster_disable_scenario(UHardwareBooster* booster, UHardwareBoosterScenario scenario)
{
    if (booster)
        if (scenario == U_HARDWARE_BOOSTER_SCENARIO_USER_INTERACTION)
            booster->setInteractive(false);
}
