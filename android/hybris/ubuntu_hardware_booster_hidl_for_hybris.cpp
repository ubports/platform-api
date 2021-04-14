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

#include <utils/Log.h>
#include <utils/RefBase.h>

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

    UbuntuHardwareBooster();
    ~UbuntuHardwareBooster();
};

UbuntuHardwareBooster::UbuntuHardwareBooster()
{
}

UbuntuHardwareBooster::~UbuntuHardwareBooster()
{
}

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
    
    if (powerHal == nullptr) {
        ALOGE("Unable to get Power service\n");
        return false;
    }
    
    return true;
}

void UbuntuHardwareBooster::setInteractive(bool interactive)
{
    if (powerHal == nullptr) {
        ALOGE("Unable to get Power service\n");
        return;
    }

    powerHal->setInteractive(interactive);
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
