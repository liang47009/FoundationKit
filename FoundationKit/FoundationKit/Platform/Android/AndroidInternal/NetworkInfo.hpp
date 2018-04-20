/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_NETWORKINFO_HPP
#define FOUNDATIONKIT_NETWORKINFO_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID
#include <string>
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaObject.hpp"

NS_FK_BEGIN

namespace android
{
namespace net
{
/**
 * Describes the status of a network interface.
 * <p>Use {@link ConnectivityManager#getActiveNetworkInfo()} to get an instance that represents
 * the current network connection.
 */
class NetworkInfo 
{
public:
    /**
     * Coarse-grained network state. This is probably what most applications should
     * use, rather than {@link android.net.NetworkInfo.DetailedState DetailedState}.
     * The mapping between the two is as follows:
     * <br/><br/>
     * <table>
     * <tr><td><b>Detailed state</b></td><td><b>Coarse-grained state</b></td></tr>
     * <tr><td><code>IDLE</code></td><td><code>DISCONNECTED</code></td></tr>
     * <tr><td><code>SCANNING</code></td><td><code>CONNECTING</code></td></tr>
     * <tr><td><code>CONNECTING</code></td><td><code>CONNECTING</code></td></tr>
     * <tr><td><code>AUTHENTICATING</code></td><td><code>CONNECTING</code></td></tr>
     * <tr><td><code>CONNECTED</code></td><td><code>CONNECTED</code></td></tr>
     * <tr><td><code>DISCONNECTING</code></td><td><code>DISCONNECTING</code></td></tr>
     * <tr><td><code>DISCONNECTED</code></td><td><code>DISCONNECTED</code></td></tr>
     * <tr><td><code>UNAVAILABLE</code></td><td><code>DISCONNECTED</code></td></tr>
     * <tr><td><code>FAILED</code></td><td><code>DISCONNECTED</code></td></tr>
     * </table>
     */
    enum State {
        CONNECTING, CONNECTED, SUSPENDED, DISCONNECTING, DISCONNECTED, UNKNOWN
    };

    NetworkInfo() {}

    NetworkInfo(int type, int subtype, const std::string& typeName, const std::string& subtypeName) {
        NetworkInfoInstance = AndroidJavaObject("android.net.NetworkInfo", type, subtype, typeName, subtypeName);
    }

    NetworkInfo(const NetworkInfo& source) {
        NetworkInfoInstance = source.NetworkInfoInstance;
    }

    NetworkInfo(jobject NetworkInfoObject) {
        NetworkInfoInstance = AndroidJavaObject(NetworkInfoObject);
    }

    NetworkInfo(const AndroidJavaObject& NetworkInfoObject) {
        NetworkInfoInstance = NetworkInfoObject;
    }

    /**
     * Reports the type of network to which the
     * info in this {@code NetworkInfo} pertains.
     * @return one of {@link ConnectivityManager#TYPE_MOBILE}, {@link
     * ConnectivityManager#TYPE_WIFI}, {@link ConnectivityManager#TYPE_WIMAX}, {@link
     * ConnectivityManager#TYPE_ETHERNET},  {@link ConnectivityManager#TYPE_BLUETOOTH}, or other
     * types defined by {@link ConnectivityManager}
     */
    int getType() {
        return NetworkInfoInstance.Call<int>("getType");
    }

    /**
     * @hide
     */
    void setType(int type) {
        NetworkInfoInstance.Call("setType", type);
    }

    /**
     * Return a network-type-specific integer describing the subtype
     * of the network.
     * @return the network subtype
     */
    int getSubtype() {
        return NetworkInfoInstance.Call<int>("getSubtype");
    }

    /**
     * @hide
     */
    void setSubtype(int subtype, const std::string& subtypeName) {
        NetworkInfoInstance.Call("setSubtype", subtype, subtypeName);
    }

    /**
     * Return a human-readable name describe the type of the network,
     * for example "WIFI" or "MOBILE".
     * @return the name of the network type
     */
    std::string getTypeName() {
        return NetworkInfoInstance.Call<std::string>("getTypeName");
    }

    /**
     * Return a human-readable name describing the subtype of the network.
     * @return the name of the network subtype
     */
    std::string getSubtypeName() {
        return NetworkInfoInstance.Call<std::string>("getSubtypeName");
    }

    /**
     * Indicates whether network connectivity exists or is in the process
     * of being established. This is good for applications that need to
     * do anything related to the network other than read or write data.
     * For the latter, call {@link #isConnected()} instead, which guarantees
     * that the network is fully usable.
     * @return {@code true} if network connectivity exists or is in the process
     * of being established, {@code false} otherwise.
     */
    bool isConnectedOrConnecting() {
        return NetworkInfoInstance.Call<bool>("isConnectedOrConnecting");
    }

    /**
     * Indicates whether network connectivity exists and it is possible to establish
     * connections and pass data.
     * <p>Always call this before attempting to perform data transactions.
     * @return {@code true} if network connectivity exists, {@code false} otherwise.
     */
    bool isConnected() {
        return NetworkInfoInstance.Call<bool>("isConnected");
    }

    /**
     * Indicates whether network connectivity is possible. A network is unavailable
     * when a persistent or semi-persistent condition prevents the possibility
     * of connecting to that network. Examples include
     * <ul>
     * <li>The device is out of the coverage area for any network of this type.</li>
     * <li>The device is on a network other than the home network (i.e., roaming), and
     * data roaming has been disabled.</li>
     * <li>The device's radio is turned off, e.g., because airplane mode is enabled.</li>
     * </ul>
     * @return {@code true} if the network is available, {@code false} otherwise
     */
    bool isAvailable() {
        return NetworkInfoInstance.Call<bool>("isAvailable");
    }

    /**
     * Sets if the network is available, ie, if the connectivity is possible.
     * @param isAvailable the new availability value.
     *
     * @hide
     */
    void setIsAvailable(bool isAvailable) {
        NetworkInfoInstance.Call("setIsAvailable", isAvailable);
    }

    /**
     * Indicates whether the current attempt to connect to the network
     * resulted from the ConnectivityManager trying to fail over to this
     * network following a disconnect from another network.
     * @return {@code true} if this is a failover attempt, {@code false}
     * otherwise.
     */
    bool isFailover() {
        return NetworkInfoInstance.Call<bool>("isFailover");
    }

    /**
     * Set the failover boolean.
     * @param isFailover {@code true} to mark the current connection attempt
     * as a failover.
     * @hide
     */
    void setFailover(bool isFailover) {
        NetworkInfoInstance.Call("setFailover", isFailover);
    }

    /**
     * Indicates whether the device is currently roaming on this network.
     * When {@code true}, it suggests that use of data on this network
     * may incur extra costs.
     * @return {@code true} if roaming is in effect, {@code false} otherwise.
     */
    bool isRoaming() {
        return NetworkInfoInstance.Call<bool>("isRoaming");
    }

  
    /**
     * Reports the current coarse-grained state of the network.
     * @return the coarse-grained state
     */
    State getState() {
        jobject StateObject = NetworkInfoInstance.CallWithSig<jobject>("getState","()Landroid/net/NetworkInfo$State;");
        int ordinal = android::Call<int>(StateObject, "ordinal");
        AndroidJNI::GetJNIEnv()->DeleteLocalRef(StateObject);
        return (State)ordinal;
    }


    /**
     * Set the extraInfo field.
     * @param extraInfo an optional {@code String} providing addditional network state
     * information passed up from the lower networking layers.
     * @hide
     */
    void setExtraInfo(const std::string& extraInfo) {
        NetworkInfoInstance.Call("setExtraInfo", extraInfo);
    }

    /**
     * Report the reason an attempt to establish connectivity failed,
     * if one is available.
     * @return the reason for failure, or null if not available
     */
    std::string getReason() {
        return NetworkInfoInstance.Call<std::string>("getReason");
    }

    /**
     * Report the extra information about the network state, if any was
     * provided by the lower networking layers.
     * @return the extra information, or null if not available
     */
    std::string getExtraInfo() {
        return NetworkInfoInstance.Call<std::string>("getExtraInfo");
    }

    std::string toString() {
        return NetworkInfoInstance.Call<std::string>("toString");
    }

    explicit operator bool() const// _NOEXCEPT
    {
        return (!!NetworkInfoInstance);
    }
protected:
    AndroidJavaObject NetworkInfoInstance;

};
} // namespace net
} // namespace android
NS_FK_END
#endif // #if PLATFORM_ANDROID
#endif // END OF FOUNDATIONKIT_NETWORKINFO_HPP
