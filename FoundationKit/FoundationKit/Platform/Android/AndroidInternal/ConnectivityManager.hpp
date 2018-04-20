/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_CONNECTIVITYMANAGER_HPP
#define FOUNDATIONKIT_CONNECTIVITYMANAGER_HPP


#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID
#include <jni.h>
#include <string>
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaObject.hpp"
#include "FoundationKit/Platform/Android/AndroidInternal/Context.hpp"
#include "FoundationKit/Platform/Android/AndroidInternal/NetworkInfo.hpp"
NS_FK_BEGIN

namespace android
{
namespace net
{
class ConnectivityManager
{
public:
    /**
    * The absence of a connection type.
    * @hide
    */
    static const int TYPE_NONE = -1;

    /**
    * The Mobile data connection.  When active, all data traffic
    * will use this network type's interface by default
    * (it has a default route)
    */
    static const int TYPE_MOBILE = 0;
    /**
    * The WIFI data connection.  When active, all data traffic
    * will use this network type's interface by default
    * (it has a default route).
    */
    static const int TYPE_WIFI = 1;
    /**
    * An MMS-specific Mobile data connection.  This network type may use the
    * same network interface as {@link #TYPE_MOBILE} or it may use a different
    * one.  This is used by applications needing to talk to the carrier's
    * Multimedia Messaging Service servers.
    */
    static const int TYPE_MOBILE_MMS = 2;
    /**
    * A SUPL-specific Mobile data connection.  This network type may use the
    * same network interface as {@link #TYPE_MOBILE} or it may use a different
    * one.  This is used by applications needing to talk to the carrier's
    * Secure User Plane Location servers for help locating the device.
    */
    static const int TYPE_MOBILE_SUPL = 3;
    /**
    * A DUN-specific Mobile data connection.  This network type may use the
    * same network interface as {@link #TYPE_MOBILE} or it may use a different
    * one.  This is sometimes by the system when setting up an upstream connection
    * for tethering so that the carrier is aware of DUN traffic.
    */
    static const int TYPE_MOBILE_DUN = 4;
    /**
    * A High Priority Mobile data connection.  This network type uses the
    * same network interface as {@link #TYPE_MOBILE} but the routing setup
    * is different.  Only requesting processes will have access to the
    * Mobile DNS servers and only IP's explicitly requested via {@link #requestRouteToHost}
    * will route over this interface if no default route exists.
    */
    static const int TYPE_MOBILE_HIPRI = 5;
    /**
    * The WiMAX data connection.  When active, all data traffic
    * will use this network type's interface by default
    * (it has a default route).
    */
    static const int TYPE_WIMAX = 6;

    /**
    * The Bluetooth data connection.  When active, all data traffic
    * will use this network type's interface by default
    * (it has a default route).
    */
    static const int TYPE_BLUETOOTH = 7;

    /**
    * Dummy data connection.  This should not be used on shipping devices.
    */
    static const int TYPE_DUMMY = 8;

    /**
    * The Ethernet data connection.  When active, all data traffic
    * will use this network type's interface by default
    * (it has a default route).
    */
    static const int TYPE_ETHERNET = 9;

    /**
    * Over the air Administration.
    * {@hide}
    */
    static const int TYPE_MOBILE_FOTA = 10;

    /**
    * IP Multimedia Subsystem.
    * {@hide}
    */
    static const int TYPE_MOBILE_IMS = 11;

    /**
    * Carrier Branded Services.
    * {@hide}
    */
    static const int TYPE_MOBILE_CBS = 12;

    /**
    * A Wi-Fi p2p connection. Only requesting processes will have access to
    * the peers connected.
    * {@hide}
    */
    static const int TYPE_WIFI_P2P = 13;

    /**
    * The network to use for initially attaching to the network
    * {@hide}
    */
    static const int TYPE_MOBILE_IA = 14;

    /**
    * Emergency PDN connection for emergency calls
    * {@hide}
    */
    static const int TYPE_MOBILE_EMERGENCY = 15;

    /**
    * The network that uses proxy to achieve connectivity.
    * {@hide}
    */
    static const int TYPE_PROXY = 16;

    /**
     * A virtual network using one or more native bearers.
     * It may or may not be providing security services.
    */
    static const int TYPE_VPN = 17;

    /** {@hide} */
    static const int MAX_RADIO_TYPE = TYPE_VPN;

    /** {@hide} */
    static const int MAX_NETWORK_TYPE = TYPE_VPN;

    ConnectivityManager()
    {
        jobject MainActivity = AndroidJNI::GetMainActivity();
        AndroidJavaObject  MainActivityObject(MainActivity);
        jobject cm = MainActivityObject.Call<jobject>("getSystemService", content::Context::CONNECTIVITY_SERVICE);
        ConnectivityManagerInstance = AndroidJavaObject(cm);
    }

    ConnectivityManager(jobject cm)
    {
        ConnectivityManagerInstance = AndroidJavaObject(cm);
    }


    /**
     * Returns details about the currently active default data network. When
     * connected, this network is the default route for outgoing connections.
     * You should always check {@link NetworkInfo#isConnected()} before initiating
     * network traffic. This may return {@code null} when there is no default
     * network.
     *
     * @return a {@link NetworkInfo} object for the current default network
     *        or {@code null} if no network default network is currently active
     *
     * <p>This method requires the call to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     */
    NetworkInfo getActiveNetworkInfo() {
        ScopeJavaObjectRef NetworkInfoObject = ConnectivityManagerInstance.CallWithSig<jobject>("getActiveNetworkInfo", "()Landroid/net/NetworkInfo;");
        if (!NetworkInfoObject)
            return NetworkInfo();
        return NetworkInfo(NetworkInfoObject.Get());
    }

    /**
     * Returns connection status information about a particular
     * network type.
     *
     * @param networkType integer specifying which networkType in
     *        which you're interested.
     * @return a {@link NetworkInfo} object for the requested
     *        network type or {@code null} if the type is not
     *        supported by the device.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     */
    NetworkInfo getNetworkInfo(int networkType) {
        ScopeJavaObjectRef NetworkInfoObject = ConnectivityManagerInstance.CallWithSig<jobject>("getNetworkInfo", "(I)Landroid/net/NetworkInfo;",networkType);
        if (!NetworkInfoObject)
            return NetworkInfo();
        return NetworkInfo(NetworkInfoObject.Get());
    }

    /**
     * Returns connection status information about all network
     * types supported by the device.
     *
     * @return an array of {@link NetworkInfo} objects.  Check each
     * {@link NetworkInfo#getType} for which type each applies.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#ACCESS_NETWORK_STATE}.
     */
    std::vector<NetworkInfo> getAllNetworkInfo() {
        std::vector<jobject> NetworkInfoVec = ConnectivityManagerInstance.CallWithSig<std::vector<jobject>>("getAllNetworkInfo", "()[Landroid/net/NetworkInfo;");
        std::vector<NetworkInfo> NetworkInfoArray;
        for (auto obj : NetworkInfoVec)
        {
            NetworkInfoArray.push_back(obj);
            AndroidJNI::GetJNIEnv()->DeleteLocalRef(obj);
        }
        return NetworkInfoArray;
    }

    explicit operator bool() const// _NOEXCEPT
    {
        return (!!ConnectivityManagerInstance);
    }
protected:
    AndroidJavaObject ConnectivityManagerInstance;
};

} // namespace net
} // namespace android


NS_FK_END

#endif // #if PLATFORM_ANDROID


#endif // END OF FOUNDATIONKIT_CONNECTIVITYMANAGER_HPP
