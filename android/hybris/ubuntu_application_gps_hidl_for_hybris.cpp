/*
 * Copyright © 2013-2020 Canonical Ltd.
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
 * Authored by: Daniel d'Andrada <daniel.dandrada@canonical.com>
 * Authored by: Erfan Abdi <erfangplus@gmail.com>
 */
#include <ubuntu/hardware/gps.h>

#include <pthread.h>
#include <string.h>
#include <unistd.h>

// android stuff
#include <android/hardware/gnss/1.0/IGnss.h>
#include <android/hardware/gnss/1.1/IGnss.h>

#include <android/hardware/gnss/1.0/IGnssMeasurement.h>
#include <android/hardware/gnss/1.1/IGnssMeasurement.h>

#include <utils/Log.h>
#include <hardware_legacy/power.h>

#define WAKE_LOCK_NAME  "U_HARDWARE_GPS"

using android::OK;
using android::sp;
using android::wp;
using android::status_t;

using android::hardware::Return;
using android::hardware::Void;
using android::hardware::hidl_vec;
using android::hardware::hidl_death_recipient;
using android::hardware::gnss::V1_0::GnssConstellationType;
using android::hardware::gnss::V1_0::GnssLocation;
using android::hardware::gnss::V1_0::GnssLocationFlags;

using android::hardware::gnss::V1_0::IAGnss;
using android::hardware::gnss::V1_0::IAGnssCallback;
using android::hardware::gnss::V1_0::IAGnssCallback;
using android::hardware::gnss::V1_0::IAGnssRil;
using android::hardware::gnss::V1_0::IAGnssRilCallback;
using android::hardware::gnss::V1_0::IGnssBatching;
using android::hardware::gnss::V1_0::IGnssBatchingCallback;
using android::hardware::gnss::V1_0::IGnssDebug;
using android::hardware::gnss::V1_0::IGnssGeofenceCallback;
using android::hardware::gnss::V1_0::IGnssGeofencing;
using android::hardware::gnss::V1_0::IGnssNavigationMessage;
using android::hardware::gnss::V1_0::IGnssNavigationMessageCallback;
using android::hardware::gnss::V1_0::IGnssNi;
using android::hardware::gnss::V1_0::IGnssNiCallback;
using android::hardware::gnss::V1_0::IGnssXtra;
using android::hardware::gnss::V1_0::IGnssXtraCallback;

using android::hardware::gnss::V1_1::IGnssCallback;

using android::hidl::base::V1_0::IBase;

using IGnss_V1_0 = android::hardware::gnss::V1_0::IGnss;
using IGnss_V1_1 = android::hardware::gnss::V1_1::IGnss;
using IGnssConfiguration_V1_0 = android::hardware::gnss::V1_0::IGnssConfiguration;
using IGnssConfiguration_V1_1 = android::hardware::gnss::V1_1::IGnssConfiguration;
using IGnssMeasurement_V1_0 = android::hardware::gnss::V1_0::IGnssMeasurement;
using IGnssMeasurement_V1_1 = android::hardware::gnss::V1_1::IGnssMeasurement;
using IGnssMeasurementCallback_V1_0 = android::hardware::gnss::V1_0::IGnssMeasurementCallback;
using IGnssMeasurementCallback_V1_1 = android::hardware::gnss::V1_1::IGnssMeasurementCallback;

sp<IGnss_V1_0> gnssHal = nullptr;
sp<IGnss_V1_1> gnssHal_V1_1 = nullptr;
sp<IGnssXtra> gnssXtraIface = nullptr;
sp<IAGnssRil> agnssRilIface = nullptr;
sp<IGnssGeofencing> gnssGeofencingIface = nullptr;
sp<IAGnss> agnssIface = nullptr;
sp<IGnssBatching> gnssBatchingIface = nullptr;
sp<IGnssDebug> gnssDebugIface = nullptr;
sp<IGnssConfiguration_V1_0> gnssConfigurationIface = nullptr;
sp<IGnssConfiguration_V1_1> gnssConfigurationIface_V1_1 = nullptr;
sp<IGnssNi> gnssNiIface = nullptr;
sp<IGnssMeasurement_V1_0> gnssMeasurementIface = nullptr;
sp<IGnssMeasurement_V1_1> gnssMeasurementIface_V1_1 = nullptr;
sp<IGnssNavigationMessage> gnssNavigationMessageIface = nullptr;

struct UHardwareGps_
{
    UHardwareGps_(UHardwareGpsParams* params);
    ~UHardwareGps_();

    bool init();

    bool start();
    bool stop();
    void inject_time(int64_t time, int64_t timeReference, int uncertainty);
    void inject_location(double latitude, double longitude, float accuracy);
    void delete_aiding_data(UHardwareGpsAidingData flags);

    void set_server_for_type(UHardwareGpsAGpsType type, const char* hostname, uint16_t port);
    void set_reference_location(UHardwareGpsAGpsRefLocation* location, size_t size_of_struct);

    void notify_connection_is_open(const char* apn);
    void notify_connection_is_closed();
    void notify_connection_not_available();

    bool set_position_mode(uint32_t mode, uint32_t recurrence, uint32_t min_interval,
                           uint32_t preferred_accuracy, uint32_t preferred_time);
    void inject_xtra_data(char* data, int length);

    UHardwareGpsLocationCallback location_cb;
    UHardwareGpsStatusCallback status_cb;
    UHardwareGpsSvStatusCallback sv_status_cb;
    UHardwareGpsNmeaCallback nmea_cb;
    UHardwareGpsSetCapabilities set_capabilities_cb;
    UHardwareGpsRequestUtcTime request_utc_time_cb;

    UHardwareGpsXtraDownloadRequest xtra_download_request_cb;

    UHardwareGpsAGpsStatusCallback agps_status_cb;

    UHardwareGpsNiNotifyCallback gps_ni_notify_cb;

    UHardwareGpsAGpsRilRequestSetId request_setid_cb;
    UHardwareGpsAGpsRilRequestRefLoc request_refloc_cb;

    void* context;
};

namespace
{
UHardwareGps hybris_gps_instance = NULL;

struct GnssCallback : public IGnssCallback {
    Return<void> gnssLocationCb(const GnssLocation& location) override;
    Return<void> gnssStatusCb(const IGnssCallback::GnssStatusValue status) override;
    Return<void> gnssSvStatusCb(const IGnssCallback::GnssSvStatus& svStatus) override;
    Return<void> gnssNmeaCb(int64_t timestamp, const android::hardware::hidl_string& nmea) override;
    Return<void> gnssSetCapabilitesCb(uint32_t capabilities) override;
    Return<void> gnssAcquireWakelockCb() override;
    Return<void> gnssReleaseWakelockCb() override;
    Return<void> gnssRequestTimeCb() override;
    Return<void> gnssRequestLocationCb(const bool independentFromGnss) override;
    Return<void> gnssSetSystemInfoCb(const IGnssCallback::GnssSystemInfo& info) override;

    // New in 1.1
    Return<void> gnssNameCb(const android::hardware::hidl_string& name) override;
};

Return<void> GnssCallback::gnssLocationCb(const GnssLocation& location)
{
    if (hybris_gps_instance && hybris_gps_instance->location_cb){
        UHardwareGpsLocation *Ulocation = nullptr;
        Ulocation->size = sizeof(UHardwareGpsLocation);
        Ulocation->flags = location.gnssLocationFlags;
        Ulocation->latitude = location.latitudeDegrees;
        Ulocation->longitude = location.longitudeDegrees;
        Ulocation->altitude = location.altitudeMeters;
        Ulocation->speed = location.speedMetersPerSec;
        Ulocation->bearing = location.bearingDegrees;
        Ulocation->accuracy = location.horizontalAccuracyMeters;
        Ulocation->timestamp = location.timestamp;

        hybris_gps_instance->location_cb(Ulocation, hybris_gps_instance->context);
    }
    return Void();
}

Return<void> GnssCallback::gnssStatusCb(const IGnssCallback::GnssStatusValue status)
{
    if (hybris_gps_instance && hybris_gps_instance->status_cb){
        uint16_t Ustatus = 0;
        switch (status) {
            case IGnssCallback::GnssStatusValue::NONE:
                Ustatus = 0;
                break;
            case IGnssCallback::GnssStatusValue::SESSION_BEGIN:
                Ustatus = 1;
                break;
            case IGnssCallback::GnssStatusValue::SESSION_END:
                Ustatus = 2;
                break;
            case IGnssCallback::GnssStatusValue::ENGINE_ON:
                Ustatus = 3;
                break;
            case IGnssCallback::GnssStatusValue::ENGINE_OFF:
                Ustatus = 4;
                break;
            default:
                Ustatus = 0;
                break;
        }
        hybris_gps_instance->status_cb(Ustatus, hybris_gps_instance->context);
    }
    return Void();
}

Return<void> GnssCallback::gnssSvStatusCb(const IGnssCallback::GnssSvStatus& svStatus)
{
    if (hybris_gps_instance && hybris_gps_instance->sv_status_cb){
        UHardwareGpsSvStatus *UsvStatus = nullptr;
        int ephemeris_mask = 0;
        int almanac_mask = 0;
        int used_in_fix_mask = 0;
        UsvStatus->size = sizeof(UHardwareGpsSvStatus);
        UsvStatus->num_svs = svStatus.numSvs;
        for (int i = 0; i < UsvStatus->num_svs; i++){
            UsvStatus->sv_list[i].size = sizeof(UHardwareGpsSvInfo);
            int prn = svStatus.gnssSvList[i].svid;
            // From https://github.com/barbeau/gpstest
            // and https://github.com/mvglasow/satstat/wiki/NMEA-IDs
            if (svStatus.gnssSvList[i].constellation == GnssConstellationType::SBAS) {
                prn -= 87;
            } else if (svStatus.gnssSvList[i].constellation == GnssConstellationType::GLONASS) {
                prn += 64;
            } else if (svStatus.gnssSvList[i].constellation == GnssConstellationType::BEIDOU) {
                prn += 200;
            } else if (svStatus.gnssSvList[i].constellation == GnssConstellationType::GALILEO) {
                prn += 300;
            }
            UsvStatus->sv_list[i].prn = prn;
            UsvStatus->sv_list[i].snr = svStatus.gnssSvList[i].cN0Dbhz;
            UsvStatus->sv_list[i].elevation = svStatus.gnssSvList[i].elevationDegrees;
            UsvStatus->sv_list[i].azimuth = svStatus.gnssSvList[i].azimuthDegrees;
            if (svStatus.gnssSvList[i].svFlag & IGnssCallback::GnssSvFlags::HAS_EPHEMERIS_DATA)
                ephemeris_mask |= 1 << i;
            if (svStatus.gnssSvList[i].svFlag & IGnssCallback::GnssSvFlags::HAS_ALMANAC_DATA)
                almanac_mask |= 1 << i;
            if (svStatus.gnssSvList[i].svFlag & IGnssCallback::GnssSvFlags::USED_IN_FIX)
                used_in_fix_mask |= 1 << i;
        }
        UsvStatus->ephemeris_mask = ephemeris_mask;
        UsvStatus->almanac_mask = almanac_mask;
        UsvStatus->used_in_fix_mask = used_in_fix_mask;
        hybris_gps_instance->sv_status_cb(UsvStatus, hybris_gps_instance->context);
    }
    return Void();
}

Return<void> GnssCallback::gnssNmeaCb(
    int64_t timestamp, const ::android::hardware::hidl_string& nmea)
{
    if (hybris_gps_instance && hybris_gps_instance->nmea_cb)
        hybris_gps_instance->nmea_cb(timestamp, nmea.c_str(), nmea.size(), hybris_gps_instance->context);
    return Void();
}

Return<void> GnssCallback::gnssSetCapabilitesCb(uint32_t capabilities)
{
    if (hybris_gps_instance && hybris_gps_instance->set_capabilities_cb)
        hybris_gps_instance->set_capabilities_cb(capabilities, hybris_gps_instance->context);
    return Void();
}

Return<void> GnssCallback::gnssAcquireWakelockCb()
{
    acquire_wake_lock(PARTIAL_WAKE_LOCK, WAKE_LOCK_NAME);
    return Void();
}

Return<void> GnssCallback::gnssReleaseWakelockCb()
{
    release_wake_lock(WAKE_LOCK_NAME);
    return Void();
}

Return<void> GnssCallback::gnssRequestTimeCb()
{
    if (hybris_gps_instance && hybris_gps_instance->request_utc_time_cb)
        hybris_gps_instance->request_utc_time_cb(hybris_gps_instance->context);
    return Void();
}

Return<void> GnssCallback::gnssRequestLocationCb(const bool)
{
    // Empty on purpose.
    return Void();
}

Return<void> GnssCallback::gnssSetSystemInfoCb(const IGnssCallback::GnssSystemInfo&)
{
    // Empty on purpose.
    return Void();
}

Return<void> GnssCallback::gnssNameCb(const android::hardware::hidl_string& name)
{
    // Empty on purpose.
    return Void();
}

class GnssXtraCallback : public IGnssXtraCallback {
    Return<void> downloadRequestCb() override;
};

Return<void> GnssXtraCallback::downloadRequestCb()
{
    if (hybris_gps_instance && hybris_gps_instance->xtra_download_request_cb)
        hybris_gps_instance->xtra_download_request_cb(hybris_gps_instance->context);
    return Void();
}

struct AGnssCallback : public IAGnssCallback {
    // Methods from ::android::hardware::gps::V1_0::IAGnssCallback follow.
    Return<void> agnssStatusIpV6Cb(
      const IAGnssCallback::AGnssStatusIpV6& agps_status) override;
    Return<void> agnssStatusIpV4Cb(
      const IAGnssCallback::AGnssStatusIpV4& agps_status) override;
};

Return<void> AGnssCallback::agnssStatusIpV4Cb(
        const IAGnssCallback::AGnssStatusIpV4& agps_status)
{
    if (hybris_gps_instance && hybris_gps_instance->agps_status_cb){
        UHardwareGpsAGpsStatus *Uagps_status = nullptr;
        Uagps_status->size = sizeof(UHardwareGpsAGpsStatus);
        switch (agps_status.type) {
            case IAGnssCallback::AGnssType::TYPE_SUPL:
                Uagps_status->type = 1;
                break;
            case IAGnssCallback::AGnssType::TYPE_C2K:
                Uagps_status->type = 2;
                break;
            default:
                Uagps_status->type = 1;
                break;
        }
        switch (agps_status.status) {
            case IAGnssCallback::AGnssStatusValue::REQUEST_AGNSS_DATA_CONN:
                Uagps_status->status = 1;
                break;
            case IAGnssCallback::AGnssStatusValue::RELEASE_AGNSS_DATA_CONN:
                Uagps_status->status = 2;
                break;
            case IAGnssCallback::AGnssStatusValue::AGNSS_DATA_CONNECTED:
                Uagps_status->status = 3;
                break;
            case IAGnssCallback::AGnssStatusValue::AGNSS_DATA_CONN_DONE:
                Uagps_status->status = 4;
                break;
            case IAGnssCallback::AGnssStatusValue::AGNSS_DATA_CONN_FAILED:
                Uagps_status->status = 5;
                break;
            default:
                Uagps_status->status = 5;
                break;
        }
        Uagps_status->ipaddr = agps_status.ipV4Addr;

        hybris_gps_instance->agps_status_cb(Uagps_status, hybris_gps_instance->context);
    }
    return Void();
}

Return<void> AGnssCallback::agnssStatusIpV6Cb(
        const IAGnssCallback::AGnssStatusIpV6& agps_status)
{
    ALOGE("AGnssStatusIpV6 is not implemented yet.");
    return Void();
}

struct GnssNiCallback : public IGnssNiCallback {
    Return<void> niNotifyCb(const IGnssNiCallback::GnssNiNotification& notification)
            override;
};

Return<void> GnssNiCallback::niNotifyCb(
        const IGnssNiCallback::GnssNiNotification& notification)
{
    if (hybris_gps_instance && hybris_gps_instance->gps_ni_notify_cb){
        UHardwareGpsNiNotification *Unotification = nullptr;
        Unotification->size = sizeof(UHardwareGpsNiNotification);
        Unotification->notification_id = notification.notificationId;
        switch (notification.niType) {
            case IGnssNiCallback::GnssNiType::VOICE:
                Unotification->ni_type = 1;
                break;
            case IGnssNiCallback::GnssNiType::UMTS_SUPL:
                Unotification->ni_type = 2;
                break;
            case IGnssNiCallback::GnssNiType::UMTS_CTRL_PLANE:
                Unotification->ni_type = 3;
                break;
            case IGnssNiCallback::GnssNiType::EMERGENCY_SUPL:
                Unotification->ni_type = 4;
                break;
            default:
                Unotification->ni_type = 1;
                break;
        }
        Unotification->notify_flags = notification.notifyFlags;
        Unotification->timeout = notification.timeoutSec;
        switch (notification.defaultResponse) {
            case IGnssNiCallback::GnssUserResponseType::RESPONSE_ACCEPT:
                Unotification->default_response = 1;
                break;
            case IGnssNiCallback::GnssUserResponseType::RESPONSE_DENY:
                Unotification->default_response = 2;
                break;
            case IGnssNiCallback::GnssUserResponseType::RESPONSE_NORESP:
                Unotification->default_response = 3;
                break;
            default:
                Unotification->default_response = 1;
                break;
        }
        strcpy(Unotification->requestor_id, notification.requestorId.c_str());
        strcpy(Unotification->text, notification.notificationMessage.c_str());
        switch (notification.requestorIdEncoding) {
            case IGnssNiCallback::GnssNiEncodingType::ENC_NONE:
                Unotification->requestor_id_encoding = 1;
                break;
            case IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_GSM_DEFAULT:
                Unotification->requestor_id_encoding = 2;
                break;
            case IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UTF8:
                Unotification->requestor_id_encoding = 3;
                break;
            case IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UCS2:
                Unotification->requestor_id_encoding = 3;
                break;
            case IGnssNiCallback::GnssNiEncodingType::ENC_UNKNOWN:
                Unotification->requestor_id_encoding = -1;
                break;
            default:
                Unotification->requestor_id_encoding = -1;
                break;
        }
        switch (notification.notificationIdEncoding) {
            case IGnssNiCallback::GnssNiEncodingType::ENC_NONE:
                Unotification->text_encoding = 1;
                break;
            case IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_GSM_DEFAULT:
                Unotification->text_encoding = 2;
                break;
            case IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UTF8:
                Unotification->text_encoding = 3;
                break;
            case IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UCS2:
                Unotification->text_encoding = 3;
                break;
            case IGnssNiCallback::GnssNiEncodingType::ENC_UNKNOWN:
                Unotification->text_encoding = -1;
                break;
            default:
                Unotification->text_encoding = -1;
                break;
        }
        hybris_gps_instance->gps_ni_notify_cb(Unotification, hybris_gps_instance->context);
    }
    return Void();
}

struct AGnssRilCallback : IAGnssRilCallback {
    Return<void> requestSetIdCb(uint32_t setIdFlag) override;
    Return<void> requestRefLocCb() override;
};

Return<void> AGnssRilCallback::requestSetIdCb(uint32_t setIdFlag)
{
    if (hybris_gps_instance && hybris_gps_instance->request_setid_cb)
        hybris_gps_instance->request_setid_cb(setIdFlag, hybris_gps_instance->context);
     return Void();
}

Return<void> AGnssRilCallback::requestRefLocCb()
{
    if (hybris_gps_instance && hybris_gps_instance->request_refloc_cb)
        hybris_gps_instance->request_refloc_cb(0, hybris_gps_instance->context);
    return Void();
}
}

UHardwareGps_::UHardwareGps_(UHardwareGpsParams* params)
    : location_cb(params->location_cb),
      status_cb(params->status_cb),
      sv_status_cb(params->sv_status_cb),
      nmea_cb(params->nmea_cb),
      set_capabilities_cb(params->set_capabilities_cb),
      request_utc_time_cb(params->request_utc_time_cb),
      xtra_download_request_cb(params->xtra_download_request_cb),
      agps_status_cb(params->agps_status_cb),
      gps_ni_notify_cb(params->gps_ni_notify_cb),
      request_setid_cb(params->request_setid_cb),
      request_refloc_cb(params->request_refloc_cb),
      context(params->context)
{

}

UHardwareGps_::~UHardwareGps_()
{
    if (gnssHal != nullptr) {
        gnssHal->cleanup();
    }
}

/* Initializes the GNSS service handle. */
static void set_gps_service_handle() {
    gnssHal_V1_1 = IGnss_V1_1::getService();
    if (gnssHal_V1_1 == nullptr) {
        ALOGD("gnssHal 1.1 was null, trying 1.0");
        gnssHal = IGnss_V1_0::getService();
    } else {
        gnssHal = gnssHal_V1_1;
    }
}

static void set_gps_service_callbacks() {
    /*
     * Fail if the main interface fails to initialize
     */
    if (gnssHal == nullptr) {
        ALOGE("Unable to Initialize GNSS HAL\n");
        return;
    }

    sp<IGnssCallback> gnssCbIface = new GnssCallback();
    Return<bool> result = false;
    if (gnssHal_V1_1 != nullptr) {
        result = gnssHal_V1_1->setCallback_1_1(gnssCbIface);
    } else {
        result = gnssHal->setCallback(gnssCbIface);
    }

    if (!result.isOk() || !result) {
        ALOGE("SetCallback for Gnss Interface fails\n");
        return;
    }

    sp<IGnssXtraCallback> gnssXtraCbIface = new GnssXtraCallback();
    if (gnssXtraIface == nullptr) {
        ALOGE("Unable to initialize GNSS Xtra interface\n");
    } else {
        result = gnssXtraIface->setCallback(gnssXtraCbIface);
        if (!result.isOk() || !result) {
            gnssXtraIface = nullptr;
            ALOGI("SetCallback for Gnss Xtra Interface fails\n");
        }
    }

    sp<IAGnssCallback> aGnssCbIface = new AGnssCallback();
    if (agnssIface != nullptr) {
        agnssIface->setCallback(aGnssCbIface);
    } else {
        ALOGI("Unable to Initialize AGnss interface\n");
    }

    sp<IGnssNiCallback> gnssNiCbIface = new GnssNiCallback();
    if (gnssNiIface != nullptr) {
        gnssNiIface->setCallback(gnssNiCbIface);
    } else {
        ALOGI("Unable to initialize GNSS NI interface\n");
    }

    sp<IAGnssRilCallback> aGnssRilCbIface = new AGnssRilCallback();
    if (agnssRilIface != nullptr) {
        agnssRilIface->setCallback(aGnssRilCbIface);
    } else {
        ALOGI("Unable to Initialize AGnss Ril interface\n");
    }

    /*sp<IGnssGeofenceCallback> gnssGeofencingCbIface = new GnssGeofenceCallback();
    if (gnssGeofencingIface != nullptr) {
        gnssGeofencingIface->setCallback(gnssGeofencingCbIface);
    } else {
        ALOGI("Unable to initialize GNSS Geofencing interface\n");
    }*/
}

bool UHardwareGps_::init()
{
    // Initialize the top level gnss HAL handle.
    set_gps_service_handle();

    if (gnssHal == nullptr) {
        ALOGE("Unable to get GPS service\n");
        return false;
    }

    auto gnssXtra = gnssHal->getExtensionXtra();
    if (!gnssXtra.isOk()) {
        ALOGD("Unable to get a handle to Xtra");
    } else {
        gnssXtraIface = gnssXtra;
    }
    auto gnssRil = gnssHal->getExtensionAGnssRil();
    if (!gnssRil.isOk()) {
        ALOGD("Unable to get a handle to AGnssRil");
    } else {
        agnssRilIface = gnssRil;
    }
    auto gnssAgnss = gnssHal->getExtensionAGnss();
    if (!gnssAgnss.isOk()) {
        ALOGD("Unable to get a handle to AGnss");
    } else {
        agnssIface = gnssAgnss;
    }
    auto gnssDebug = gnssHal->getExtensionGnssDebug();
    if (!gnssDebug.isOk()) {
        ALOGD("Unable to get a handle to GnssDebug");
    } else {
        gnssDebugIface = gnssDebug;
    }
    auto gnssNi = gnssHal->getExtensionGnssNi();
    if (!gnssNi.isOk()) {
        ALOGD("Unable to get a handle to GnssNi");
    } else {
        gnssNiIface = gnssNi;
    }

    /*
    auto gnssNavigationMessage = gnssHal->getExtensionGnssNavigationMessage();
    if (!gnssNavigationMessage.isOk()) {
        ALOGD("Unable to get a handle to GnssNavigationMessage");
    } else {
        gnssNavigationMessageIface = gnssNavigationMessage;
    }
    if (gnssHal_V1_1 != nullptr) {
        auto gnssMeasurement = gnssHal_V1_1->getExtensionGnssMeasurement_1_1();
        if (!gnssMeasurement.isOk()) {
            ALOGD("Unable to get a handle to GnssMeasurement");
        } else {
            gnssMeasurementIface_V1_1 = gnssMeasurement;
            gnssMeasurementIface = gnssMeasurementIface_V1_1;
        }
    } else {
        auto gnssMeasurement_V1_0 = gnssHal->getExtensionGnssMeasurement();
        if (!gnssMeasurement_V1_0.isOk()) {
            ALOGD("Unable to get a handle to GnssMeasurement");
        } else {
            gnssMeasurementIface = gnssMeasurement_V1_0;
        }
    }
    if (gnssHal_V1_1 != nullptr) {
        auto gnssConfiguration = gnssHal_V1_1->getExtensionGnssConfiguration_1_1();
        if (!gnssConfiguration.isOk()) {
            ALOGD("Unable to get a handle to GnssConfiguration");
        } else {
            gnssConfigurationIface_V1_1 = gnssConfiguration;
            gnssConfigurationIface = gnssConfigurationIface_V1_1;
        }
    } else {
        auto gnssConfiguration_V1_0 = gnssHal->getExtensionGnssConfiguration();
        if (!gnssConfiguration_V1_0.isOk()) {
            ALOGD("Unable to get a handle to GnssConfiguration");
        } else {
            gnssConfigurationIface = gnssConfiguration_V1_0;
        }
    }
    auto gnssGeofencing = gnssHal->getExtensionGnssGeofencing();
    if (!gnssGeofencing.isOk()) {
        ALOGD("Unable to get a handle to GnssGeofencing");
    } else {
        gnssGeofencingIface = gnssGeofencing;
    }
    auto gnssBatching = gnssHal->getExtensionGnssBatching();
    if (!gnssBatching.isOk()) {
        ALOGD("Unable to get a handle to gnssBatching");
    } else {
        gnssBatchingIface = gnssBatching;
    }*/

    set_gps_service_callbacks();

    return true;
}

bool UHardwareGps_::start()
{
    if (gnssHal != nullptr) {
        auto result = gnssHal->start();
        if (!result.isOk()) {
            return false;
        } else {
            return result;
        }
    } else {
        return false;
    }
}

bool UHardwareGps_::stop()
{
    if (gnssHal != nullptr) {
        auto result = gnssHal->stop();
        if (!result.isOk()) {
            return false;
        } else {
            return result;
        }
    } else {
        return false;
    }
}

void UHardwareGps_::inject_time(int64_t time, int64_t time_reference, int uncertainty)
{
    if (gnssHal != nullptr) {
        auto result = gnssHal->injectTime(time, time_reference, uncertainty);
        if (!result.isOk() || !result) {
            ALOGE("%s: Gnss injectTime() failed", __func__);
        }
    }
}

void UHardwareGps_::inject_location(double latitude, double longitude, float accuracy)
{
    if (gnssHal != nullptr) {
        auto result = gnssHal->injectLocation(latitude, longitude, accuracy);
        if (!result.isOk() || !result) {
            ALOGE("%s: Gnss injectLocation() failed", __func__);
        }
    }
}

void UHardwareGps_::delete_aiding_data(uint16_t flags)
{
    if (gnssHal != nullptr) {
        auto result = gnssHal->deleteAidingData(static_cast<IGnss_V1_0::GnssAidingData>(flags));
        if (!result.isOk()) {
            ALOGE("Error in deleting aiding data");
        }
    }
}

void UHardwareGps_::set_server_for_type(UHardwareGpsAGpsType type, const char* hostname, uint16_t port)
{
    if (agnssIface == nullptr) {
        ALOGE("no AGPS interface in set_agps_server");
        return;
    }

    auto result = agnssIface->setServer(static_cast<IAGnssCallback::AGnssType>(type),
                                        hostname,
                                        port);

    if (!result.isOk() || !result) {
        ALOGE("%s: Failed to set AGnss host name and port", __func__);
    }
}

void UHardwareGps_::set_reference_location(UHardwareGpsAGpsRefLocation* location, size_t size_of_struct)
{
    IAGnssRil::AGnssRefLocation Alocation;

    if (agnssRilIface == nullptr) {
        ALOGE("No AGPS RIL interface in agps_set_reference_location_cellid");
        return;
    }

    switch (static_cast<IAGnssRil::AGnssRefLocationType>(location->type)) {
        case IAGnssRil::AGnssRefLocationType::GSM_CELLID:
        case IAGnssRil::AGnssRefLocationType::UMTS_CELLID:
            Alocation.type = static_cast<IAGnssRil::AGnssRefLocationType>(location->type);
            Alocation.cellID.mcc = location->u.cellID.mcc;
            Alocation.cellID.mnc = location->u.cellID.mnc;
            Alocation.cellID.lac = location->u.cellID.lac;
            Alocation.cellID.cid = location->u.cellID.cid;
            break;
        default:
            ALOGE("Neither a GSM nor a UMTS cellid (%s:%d).", __FUNCTION__, __LINE__);
            return;
            break;
    }

    agnssRilIface->setRefLocation(Alocation);
}

void UHardwareGps_::notify_connection_is_open(const char* apn)
{
    if (agnssIface == nullptr) {
        ALOGE("no AGPS interface in agps_data_conn_open");
        return;
    }
    if (apn == NULL) {
        return;
    }

    //FIXME: dataConnOpen needs ApnIpType which is IPV4 or 6, but ubuntu doesn't support it and we forced ipv4 on it
    auto result = agnssIface->dataConnOpen(apn, static_cast<IAGnss::ApnIpType>(1));
    if (!result.isOk() || !result){
        ALOGE("%s: Failed to set APN and its IP type", __func__);
    }
}

void UHardwareGps_::notify_connection_is_closed()
{
    if (agnssIface == nullptr) {
        ALOGE("%s: AGPS interface not supported", __func__);
        return;
    }

    auto result = agnssIface->dataConnClosed();
    if (!result.isOk() || !result) {
        ALOGE("%s: Failed to close AGnss data connection", __func__);
    }
}

void UHardwareGps_::notify_connection_not_available()
{
    if (agnssIface == nullptr) {
        ALOGE("%s: AGPS interface not supported", __func__);
        return;
    }

    auto result = agnssIface->dataConnFailed();
    if (!result.isOk() || !result) {
        ALOGE("%s: Failed to notify unavailability of AGnss data connection", __func__);
    }
}

bool UHardwareGps_::set_position_mode(uint32_t mode, uint32_t recurrence, uint32_t min_interval,
                                    uint32_t preferred_accuracy, uint32_t preferred_time)
{
    Return<bool> result = false;
    if (gnssHal_V1_1 != nullptr) {
        result = gnssHal_V1_1->setPositionMode_1_1(static_cast<IGnss_V1_0::GnssPositionMode>(mode),
                 static_cast<IGnss_V1_0::GnssPositionRecurrence>(recurrence),
                 min_interval,
                 preferred_accuracy,
                 preferred_time,
                 false);
    } else if (gnssHal != nullptr) {
        result = gnssHal->setPositionMode(static_cast<IGnss_V1_0::GnssPositionMode>(mode),
                 static_cast<IGnss_V1_0::GnssPositionRecurrence>(recurrence),
                 min_interval,
                 preferred_accuracy,
                 preferred_time);
    }
    if (!result.isOk()) {
        ALOGE("%s: GNSS setPositionMode failed\n", __func__);
        return false;
    } else {
        return result;
    }
}

void UHardwareGps_::inject_xtra_data(char* data, int length)
{
    if (gnssXtraIface == nullptr) {
        ALOGE("XTRA Interface not supported");
        return;
    }
    gnssXtraIface->injectXtraData(std::string((const char*)data, length));
}

/////////////////////////////////////////////////////////////////////
// Implementation of the C API

UHardwareGps u_hardware_gps_new(UHardwareGpsParams* params)
{
    if (hybris_gps_instance != NULL)
        return NULL;

    UHardwareGps u_hardware_gps = new UHardwareGps_(params);
    hybris_gps_instance = u_hardware_gps;

    // Try ten times to initialize the GPS HAL interface,
    // sleeping for 200ms per iteration in case of issues.
    for (unsigned int i = 0; i < 50; i++)
        if (u_hardware_gps->init())
            return hybris_gps_instance = u_hardware_gps;
        else
            // Sleep for some time and leave some time for the system
            // to finish initialization.
            ::usleep(200 * 1000);

    // This is the error case, as we did not succeed in initializing the GPS interface.
    delete u_hardware_gps;
    return hybris_gps_instance;
}

void u_hardware_gps_delete(UHardwareGps handle)
{
    delete handle;
    if (handle == hybris_gps_instance)
        hybris_gps_instance = NULL;
}

bool u_hardware_gps_start(UHardwareGps self)
{
    return self->start();
}

bool u_hardware_gps_stop(UHardwareGps self)
{
    return self->stop();
}

void u_hardware_gps_inject_time(UHardwareGps self, int64_t time, int64_t time_reference,
                            int uncertainty)
{
    self->inject_time(time, time_reference, uncertainty);
}

void u_hardware_gps_inject_location(UHardwareGps self,
                                    UHardwareGpsLocation location)
{
    self->inject_location(location.latitude, location.longitude,
                          location.accuracy);
}

void u_hardware_gps_delete_aiding_data(UHardwareGps self, UHardwareGpsAidingData flags)
{
    self->delete_aiding_data(flags);
}

void u_hardware_gps_agps_set_server_for_type(
        UHardwareGps self,
        UHardwareGpsAGpsType type,
        const char* hostname,
        uint16_t port)
{
    self->set_server_for_type(type, hostname, port);
}

void u_hardware_gps_agps_set_reference_location(
    UHardwareGps self,
    UHardwareGpsAGpsRefLocation *location,
    size_t size_of_struct)
{
    self->set_reference_location(location, size_of_struct);
}

void u_hardware_gps_agps_notify_connection_is_open(
    UHardwareGps self,
    const char *apn)
{
    self->notify_connection_is_open(apn);
}

void u_hardware_gps_agps_notify_connection_is_closed(UHardwareGps self)
{
    self->notify_connection_is_closed();
}

void u_hardware_gps_agps_notify_connection_not_available(UHardwareGps self)
{
    self->notify_connection_not_available();
}


bool u_hardware_gps_set_position_mode(UHardwareGps self, uint32_t mode, uint32_t recurrence,
                                  uint32_t min_interval, uint32_t preferred_accuracy,
                                  uint32_t preferred_time)
{
    return self->set_position_mode(mode, recurrence, min_interval, preferred_accuracy,
                                   preferred_time);
}

void u_hardware_gps_inject_xtra_data(UHardwareGps self, char* data, int length)
{
    self->inject_xtra_data(data, length);
}
