/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_TELEPHONYMANAGER_HPP
#define FOUNDATIONKIT_TELEPHONYMANAGER_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID
#include <string>
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaObject.hpp"
#include "FoundationKit/Platform/Android/AndroidInternal/Context.hpp"
NS_FK_BEGIN
namespace android
{
namespace telephony
{


class TelephonyManager
{
public:

    TelephonyManager()
    {
        jobject MainActivity = AndroidJNI::GetMainActivity();
        AndroidJavaObject  MainActivityObject(MainActivity);
        jobject tm = MainActivityObject.Call<jobject>("getSystemService", content::Context::TELEPHONY_SERVICE);
        TelephonyManagerInstance = AndroidJavaObject(tm);
    }

    TelephonyManager(jobject tm)
    {
        TelephonyManagerInstance = AndroidJavaObject(tm);
    }

    /**
     * Returns the unique device ID, for example, the IMEI for GSM and the MEID
     * or ESN for CDMA phones. Return null if device ID is not available.
     *
     * <p>Requires Permission:
     *   {@link android.Manifest.permission#READ_PHONE_STATE READ_PHONE_STATE}
     */
    std::string getDeviceId() {
        return TelephonyManagerInstance.Call<std::string>("getDeviceId");
    }


    /**
     * Returns the alphabetic name of current registered operator.
     * <p>
     * Availability: Only when user is registered to a network. Result may be
     * unreliable on CDMA networks (use {@link #getPhoneType()} to determine if
     * on a CDMA network).
     */
    std::string getNetworkOperatorName() {
        return TelephonyManagerInstance.Call<std::string>("getNetworkOperatorName");
    }

    /**
     * Returns true if the device is considered roaming on the current
     * network, for GSM purposes.
     * <p>
     * Availability: Only when user registered to a network.
     */
    bool isNetworkRoaming() {
        return TelephonyManagerInstance.Call<bool>("isNetworkRoaming");
    }

    /**
     * Returns the ISO country code equivalent of the current registered
     * operator's MCC (Mobile Country Code).
     * <p>
     * Availability: Only when user is registered to a network. Result may be
     * unreliable on CDMA networks (use {@link #getPhoneType()} to determine if
     * on a CDMA network).
     */
    std::string getNetworkCountryIso() {
        return TelephonyManagerInstance.Call<std::string>("getNetworkCountryIso");
    }

    /** Network type is unknown */
    static const int NETWORK_TYPE_UNKNOWN = 0;
    /** Current network is GPRS */
    static const int NETWORK_TYPE_GPRS = 1;
    /** Current network is EDGE */
    static const int NETWORK_TYPE_EDGE = 2;
    /** Current network is UMTS */
    static const int NETWORK_TYPE_UMTS = 3;
    /** Current network is CDMA: Either IS95A or IS95B*/
    static const int NETWORK_TYPE_CDMA = 4;
    /** Current network is EVDO revision 0*/
    static const int NETWORK_TYPE_EVDO_0 = 5;
    /** Current network is EVDO revision A*/
    static const int NETWORK_TYPE_EVDO_A = 6;
    /** Current network is 1xRTT*/
    static const int NETWORK_TYPE_1xRTT = 7;
    /** Current network is HSDPA */
    static const int NETWORK_TYPE_HSDPA = 8;
    /** Current network is HSUPA */
    static const int NETWORK_TYPE_HSUPA = 9;
    /** Current network is HSPA */
    static const int NETWORK_TYPE_HSPA = 10;
    /** Current network is iDen */
    static const int NETWORK_TYPE_IDEN = 11;
    /** Current network is EVDO revision B*/
    static const int NETWORK_TYPE_EVDO_B = 12;
    /** Current network is LTE */
    static const int NETWORK_TYPE_LTE = 13;
    /** Current network is eHRPD */
    static const int NETWORK_TYPE_EHRPD = 14;
    /** Current network is HSPA+ */
    static const int NETWORK_TYPE_HSPAP = 15;
    /** Current network is GSM {@hide} */
    static const int NETWORK_TYPE_GSM = 16;
    /**
     * @return the NETWORK_TYPE_xxxx for current data connection.
     */
    int getNetworkType() {
        return TelephonyManagerInstance.Call<int>("getNetworkType");
    }
    /**
     * Returns a constant indicating the radio technology (network type)
     * currently in use on the device for a subscription.
     * @return the network type
     *
     * @param subId for which network type is returned
     *
     * @see #NETWORK_TYPE_UNKNOWN
     * @see #NETWORK_TYPE_GPRS
     * @see #NETWORK_TYPE_EDGE
     * @see #NETWORK_TYPE_UMTS
     * @see #NETWORK_TYPE_HSDPA
     * @see #NETWORK_TYPE_HSUPA
     * @see #NETWORK_TYPE_HSPA
     * @see #NETWORK_TYPE_CDMA
     * @see #NETWORK_TYPE_EVDO_0
     * @see #NETWORK_TYPE_EVDO_A
     * @see #NETWORK_TYPE_EVDO_B
     * @see #NETWORK_TYPE_1xRTT
     * @see #NETWORK_TYPE_IDEN
     * @see #NETWORK_TYPE_LTE
     * @see #NETWORK_TYPE_EHRPD
     * @see #NETWORK_TYPE_HSPAP
     */
     /** {@hide} */
    int getNetworkType(int subId) {
        return TelephonyManagerInstance.Call<int>("getNetworkType", subId);
    }

    /** Unknown network class. {@hide} */
    static const int NETWORK_CLASS_UNKNOWN = 0;
    /** Class of broadly defined "2G" networks. {@hide} */
    static const int NETWORK_CLASS_2_G = 1;
    /** Class of broadly defined "3G" networks. {@hide} */
    static const int NETWORK_CLASS_3_G = 2;
    /** Class of broadly defined "4G" networks. {@hide} */
    static const int NETWORK_CLASS_4_G = 3;

    /**
     * Return general class of network type, such as "3G" or "4G". In cases
     * where classification is contentious, this method is conservative.
     *
     * @hide
     */
    static int getNetworkClass(int networkType) {
        switch (networkType) {
        case NETWORK_TYPE_GPRS:
        case NETWORK_TYPE_GSM:
        case NETWORK_TYPE_EDGE:
        case NETWORK_TYPE_CDMA:
        case NETWORK_TYPE_1xRTT:
        case NETWORK_TYPE_IDEN:
            return NETWORK_CLASS_2_G;
        case NETWORK_TYPE_UMTS:
        case NETWORK_TYPE_EVDO_0:
        case NETWORK_TYPE_EVDO_A:
        case NETWORK_TYPE_HSDPA:
        case NETWORK_TYPE_HSUPA:
        case NETWORK_TYPE_HSPA:
        case NETWORK_TYPE_EVDO_B:
        case NETWORK_TYPE_EHRPD:
        case NETWORK_TYPE_HSPAP:
            return NETWORK_CLASS_3_G;
        case NETWORK_TYPE_LTE:
            return NETWORK_CLASS_4_G;
        default:
            return NETWORK_CLASS_UNKNOWN;
        }
        return NETWORK_CLASS_UNKNOWN;
    }
    /**
     * Returns a string representation of the radio technology (network type)
     * currently in use on the device.
     * @return the name of the radio technology
     *
     * @hide pending API council review
     */
    std::string getNetworkTypeName() {
        return TelephonyManagerInstance.Call<std::string>("getNetworkTypeName");
    }

    /**
     * Returns a string representation of the radio technology (network type)
     * currently in use on the device.
     * @param subId for which network type is returned
     * @return the name of the radio technology
     *
     */
     /** {@hide} */
    static std::string getNetworkTypeName(int type) {
        switch (type) {
        case NETWORK_TYPE_GPRS:
            return "GPRS";
        case NETWORK_TYPE_EDGE:
            return "EDGE";
        case NETWORK_TYPE_UMTS:
            return "UMTS";
        case NETWORK_TYPE_HSDPA:
            return "HSDPA";
        case NETWORK_TYPE_HSUPA:
            return "HSUPA";
        case NETWORK_TYPE_HSPA:
            return "HSPA";
        case NETWORK_TYPE_CDMA:
            return "CDMA";
        case NETWORK_TYPE_EVDO_0:
            return "CDMA - EvDo rev. 0";
        case NETWORK_TYPE_EVDO_A:
            return "CDMA - EvDo rev. A";
        case NETWORK_TYPE_EVDO_B:
            return "CDMA - EvDo rev. B";
        case NETWORK_TYPE_1xRTT:
            return "CDMA - 1xRTT";
        case NETWORK_TYPE_LTE:
            return "LTE";
        case NETWORK_TYPE_EHRPD:
            return "CDMA - eHRPD";
        case NETWORK_TYPE_IDEN:
            return "iDEN";
        case NETWORK_TYPE_HSPAP:
            return "HSPA+";
        case NETWORK_TYPE_GSM:
            return "GSM";
        default:
            return "UNKNOWN";
        }
    }

    /**
     * Returns the phone number string for line 1, for example, the MSISDN
     * for a GSM phone. Return null if it is unavailable.
     * <p>
     * Requires Permission:
     *   {@link android.Manifest.permission#READ_PHONE_STATE READ_PHONE_STATE}
     */
    std::string getLine1Number() {
        return TelephonyManagerInstance.Call<std::string>("getLine1Number");
    }

    explicit operator bool() const// _NOEXCEPT
    {
        return (!!TelephonyManagerInstance);
    }
protected:
    AndroidJavaObject TelephonyManagerInstance;
};

} // namespace telephony
} // namespace android
NS_FK_END
#endif // #if PLATFORM_ANDROID


#endif // END OF FOUNDATIONKIT_TELEPHONYMANAGER_HPP
