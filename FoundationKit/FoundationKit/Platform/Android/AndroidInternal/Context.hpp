/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_CONTEXT_HPP
#define FOUNDATIONKIT_CONTEXT_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID
NS_FK_BEGIN

namespace android
{
namespace content
{

/**
 * Interface to global information about an application environment.  This is
 * an abstract class whose implementation is provided by
 * the Android system.  It
 * allows access to application-specific resources and classes, as well as
 * up-calls for application-level operations such as launching activities,
 * broadcasting and receiving intents, etc.
 */
class Context 
{
public:
    /**
     * File creation mode: the default mode, where the created file can only
     * be accessed by the calling application (or all applications sharing the
     * same user ID).
     * @see #MODE_WORLD_READABLE
     * @see #MODE_WORLD_WRITEABLE
     */
    static const int MODE_PRIVATE = 0x0000;
    /**
     * @deprecated Creating world-readable files is very dangerous, and likely
     * to cause security holes in applications.  It is strongly discouraged;
     * instead, applications should use more formal mechanism for interactions
     * such as {@link ContentProvider}, {@link BroadcastReceiver}, and
     * {@link android.app.Service}.  There are no guarantees that this
     * access mode will remain on a file, such as when it goes through a
     * backup and restore.
     * File creation mode: allow all other applications to have read access
     * to the created file.
     * @see #MODE_PRIVATE
     * @see #MODE_WORLD_WRITEABLE
     */
    [[deprecated]]
    static const int MODE_WORLD_READABLE = 0x0001;
    /**
     * @deprecated Creating world-writable files is very dangerous, and likely
     * to cause security holes in applications.  It is strongly discouraged;
     * instead, applications should use more formal mechanism for interactions
     * such as {@link ContentProvider}, {@link BroadcastReceiver}, and
     * {@link android.app.Service}.  There are no guarantees that this
     * access mode will remain on a file, such as when it goes through a
     * backup and restore.
     * File creation mode: allow all other applications to have write access
     * to the created file.
     * @see #MODE_PRIVATE
     * @see #MODE_WORLD_READABLE
     */
    [[deprecated]]
    static const int MODE_WORLD_WRITEABLE = 0x0002;
    /**
     * File creation mode: for use with {@link #openFileOutput}, if the file
     * already exists then write data to the end of the existing file
     * instead of erasing it.
     * @see #openFileOutput
     */
    static const int MODE_APPEND = 0x8000;

    /**
     * SharedPreference loading flag: when set, the file on disk will
     * be checked for modification even if the shared preferences
     * instance is already loaded in this process.  This behavior is
     * sometimes desired in cases where the application has multiple
     * processes, all writing to the same SharedPreferences file.
     * Generally there are better forms of communication between
     * processes, though.
     *
     * <p>This was the legacy (but undocumented) behavior in and
     * before Gingerbread (Android 2.3) and this flag is implied when
     * targetting such releases.  For applications targetting SDK
     * versions <em>greater than</em> Android 2.3, this flag must be
     * explicitly set if desired.
     *
     * @see #getSharedPreferences
     */
    static const int MODE_MULTI_PROCESS = 0x0004;

    /**
     * Database open flag: when set, the database is opened with write-ahead
     * logging enabled by default.
     *
     * @see #openOrCreateDatabase(String, int, CursorFactory)
     * @see #openOrCreateDatabase(String, int, CursorFactory, DatabaseErrorHandler)
     * @see SQLiteDatabase#enableWriteAheadLogging
     */
    static const int MODE_ENABLE_WRITE_AHEAD_LOGGING = 0x0008;

    /**
     * Flag for {@link #bindService}: automatically create the service as long
     * as the binding exists.  Note that while this will create the service,
     * its {@link android.app.Service#onStartCommand}
     * method will still only be called due to an
     * explicit call to {@link #startService}.  Even without that, though,
     * this still provides you with access to the service object while the
     * service is created.
     *
     * <p>Note that prior to {@link android.os.Build.VERSION_CODES#ICE_CREAM_SANDWICH},
     * not supplying this flag would also impact how important the system
     * consider's the target service's process to be.  When set, the only way
     * for it to be raised was by binding from a service in which case it will
     * only be important when that activity is in the foreground.  Now to
     * achieve this behavior you must explicitly supply the new flag
     * {@link #BIND_ADJUST_WITH_ACTIVITY}.  For compatibility, old applications
     * that don't specify {@link #BIND_AUTO_CREATE} will automatically have
     * the flags {@link #BIND_WAIVE_PRIORITY} and
     * {@link #BIND_ADJUST_WITH_ACTIVITY} set for them in order to achieve
     * the same result.
     */
    static const int BIND_AUTO_CREATE = 0x0001;

    /**
     * Flag for {@link #bindService}: include debugging help for mismatched
     * calls to unbind.  When this flag is set, the callstack of the following
     * {@link #unbindService} call is retained, to be printed if a later
     * incorrect unbind call is made.  Note that doing this requires retaining
     * information about the binding that was made for the lifetime of the app,
     * resulting in a leak -- this should only be used for debugging.
     */
    static const int BIND_DEBUG_UNBIND = 0x0002;

    /**
     * Flag for {@link #bindService}: don't allow this binding to raise
     * the target service's process to the foreground scheduling priority.
     * It will still be raised to at least the same memory priority
     * as the client (so that its process will not be killable in any
     * situation where the client is not killable), but for CPU scheduling
     * purposes it may be left in the background.  This only has an impact
     * in the situation where the binding client is a foreground process
     * and the target service is in a background process.
     */
    static const int BIND_NOT_FOREGROUND = 0x0004;

    /**
     * Flag for {@link #bindService}: indicates that the client application
     * binding to this service considers the service to be more important than
     * the app itself.  When set, the platform will try to have the out of
     * memory killer kill the app before it kills the service it is bound to, though
     * this is not guaranteed to be the case.
     */
    static const int BIND_ABOVE_CLIENT = 0x0008;

    /**
     * Flag for {@link #bindService}: allow the process hosting the bound
     * service to go through its normal memory management.  It will be
     * treated more like a running service, allowing the system to
     * (temporarily) expunge the process if low on memory or for some other
     * whim it may have, and being more aggressive about making it a candidate
     * to be killed (and restarted) if running for a long time.
     */
    static const int BIND_ALLOW_OOM_MANAGEMENT = 0x0010;

    /**
     * Flag for {@link #bindService}: don't impact the scheduling or
     * memory management priority of the target service's hosting process.
     * Allows the service's process to be managed on the background LRU list
     * just like a regular application process in the background.
     */
    static const int BIND_WAIVE_PRIORITY = 0x0020;

    /**
     * Flag for {@link #bindService}: this service is very important to
     * the client, so should be brought to the foreground process level
     * when the client is.  Normally a process can only be raised to the
     * visibility level by a client, even if that client is in the foreground.
     */
    static const int BIND_IMPORTANT = 0x0040;

    /**
     * Flag for {@link #bindService}: If binding from an activity, allow the
     * target service's process importance to be raised based on whether the
     * activity is visible to the user, regardless whether another flag is
     * used to reduce the amount that the client process's overall importance
     * is used to impact it.
     */
    static const int BIND_ADJUST_WITH_ACTIVITY = 0x0080;

    /**
     * @hide Flag for {@link #bindService}: Treat the binding as hosting
     * an activity, an unbinding as the activity going in the background.
     * That is, when unbinding, the process when empty will go on the activity
     * LRU list instead of the regular one, keeping it around more aggressively
     * than it otherwise would be.  This is intended for use with IMEs to try
     * to keep IME processes around for faster keyboard switching.
     */
    static const int BIND_TREAT_LIKE_ACTIVITY = 0x08000000;

    /**
     * @hide An idea that is not yet implemented.
     * Flag for {@link #bindService}: If binding from an activity, consider
     * this service to be visible like the binding activity is.  That is,
     * it will be treated as something more important to keep around than
     * invisible background activities.  This will impact the number of
     * recent activities the user can switch between without having them
     * restart.  There is no guarantee this will be respected, as the system
     * tries to balance such requests from one app vs. the importantance of
     * keeping other apps around.
     */
    static const int BIND_VISIBLE = 0x10000000;

    /**
     * @hide
     * Flag for {@link #bindService}: Consider this binding to be causing the target
     * process to be showing UI, so it will be do a UI_HIDDEN memory trim when it goes
     * away.
     */
    static const int BIND_SHOWING_UI = 0x20000000;

    /**
     * Flag for {@link #bindService}: Don't consider the bound service to be
     * visible, even if the caller is visible.
     * @hide
     */
    static const int BIND_NOT_VISIBLE = 0x40000000;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.os.PowerManager} for controlling power management,
     * including "wake locks," which let you keep the device on while
     * you're running long tasks.
     */
    static const char* POWER_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.view.WindowManager} for accessing the system's window
     * manager.
     *
     * @see #getSystemService
     * @see android.view.WindowManager
     */
    static const char* WINDOW_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.view.LayoutInflater} for inflating layout resources in this
     * context.
     *
     * @see #getSystemService
     * @see android.view.LayoutInflater
     */
    static const char* LAYOUT_INFLATER_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.accounts.AccountManager} for receiving intents at a
     * time of your choosing.
     *
     * @see #getSystemService
     * @see android.accounts.AccountManager
     */
    static const char* ACCOUNT_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.app.ActivityManager} for interacting with the global
     * system state.
     *
     * @see #getSystemService
     * @see android.app.ActivityManager
     */
    static const char* ACTIVITY_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.app.AlarmManager} for receiving intents at a
     * time of your choosing.
     *
     * @see #getSystemService
     * @see android.app.AlarmManager
     */
    static const char* ALARM_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.app.NotificationManager} for informing the user of
     * background events.
     *
     * @see #getSystemService
     * @see android.app.NotificationManager
     */
    static const char* NOTIFICATION_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.view.accessibility.AccessibilityManager} for giving the user
     * feedback for UI events through the registered event listeners.
     *
     * @see #getSystemService
     * @see android.view.accessibility.AccessibilityManager
     */
    static const char* ACCESSIBILITY_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.view.accessibility.CaptioningManager} for obtaining
     * captioning properties and listening for changes in captioning
     * preferences.
     *
     * @see #getSystemService
     * @see android.view.accessibility.CaptioningManager
     */
    static const char* CAPTIONING_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.app.NotificationManager} for controlling keyguard.
     *
     * @see #getSystemService
     * @see android.app.KeyguardManager
     */
    static const char* KEYGUARD_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.location.LocationManager} for controlling location
     * updates.
     *
     * @see #getSystemService
     * @see android.location.LocationManager
     */
    static const char* LOCATION_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.location.CountryDetector} for detecting the country that
     * the user is in.
     *
     * @hide
     */
    static const char* COUNTRY_DETECTOR;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.app.SearchManager} for handling searches.
     *
     * @see #getSystemService
     * @see android.app.SearchManager
     */
    static const char* SEARCH_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.hardware.SensorManager} for accessing sensors.
     *
     * @see #getSystemService
     * @see android.hardware.SensorManager
     */
    static const char* SENSOR_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.os.storage.StorageManager} for accessing system storage
     * functions.
     *
     * @see #getSystemService
     * @see android.os.storage.StorageManager
     */
    static const char* STORAGE_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * com.android.server.WallpaperService for accessing wallpapers.
     *
     * @see #getSystemService
     */
    static const char* WALLPAPER_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.os.Vibrator} for interacting with the vibration hardware.
     *
     * @see #getSystemService
     * @see android.os.Vibrator
     */
    static const char* VIBRATOR_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.app.StatusBarManager} for interacting with the status bar.
     *
     * @see #getSystemService
     * @see android.app.StatusBarManager
     * @hide
     */
    static const char* STATUS_BAR_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.net.ConnectivityManager} for handling management of
     * network connections.
     *
     * @see #getSystemService
     * @see android.net.ConnectivityManager
     */
    static const char* CONNECTIVITY_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.os.IUpdateLock} for managing runtime sequences that
     * must not be interrupted by headless OTA application or similar.
     *
     * @hide
     * @see #getSystemService
     * @see android.os.UpdateLock
     */
    static const char* UPDATE_LOCK_SERVICE;

    /**
     * Constant for the internal network management service, not really a Context service.
     * @hide
     */
    static const char* NETWORKMANAGEMENT_SERVICE;

    /** {@hide} */
    static const char* NETWORK_STATS_SERVICE;
    /** {@hide} */
    static const char* NETWORK_POLICY_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.net.wifi.WifiManager} for handling management of
     * Wi-Fi access.
     *
     * @see #getSystemService
     * @see android.net.wifi.WifiManager
     */
    static const char* WIFI_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.net.wifi.passpoint.WifiPasspointManager} for handling management of
     * Wi-Fi passpoint access.
     *
     * @see #getSystemService
     * @see android.net.wifi.passpoint.WifiPasspointManager
     * @hide
     */
    static const char* WIFI_PASSPOINT_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.net.wifi.p2p.WifiP2pManager} for handling management of
     * Wi-Fi peer-to-peer connections.
     *
     * @see #getSystemService
     * @see android.net.wifi.p2p.WifiP2pManager
     */
    static const char* WIFI_P2P_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.net.wifi.WifiScanner} for scanning the wifi universe
     *
     * @see #getSystemService
     * @see android.net.wifi.WifiScanner
     * @hide
     */
    static const char* WIFI_SCANNING_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.net.wifi.RttManager} for ranging devices with wifi
     *
     * @see #getSystemService
     * @see android.net.wifi.RttManager
     * @hide
     */
    static const char* WIFI_RTT_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.net.EthernetManager} for handling management of
     * Ethernet access.
     *
     * @see #getSystemService
     * @see android.net.EthernetManager
     *
     * @hide
     */
    static const char* ETHERNET_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.net.nsd.NsdManager} for handling management of network service
     * discovery
     *
     * @see #getSystemService
     * @see android.net.nsd.NsdManager
     */
    static const char* NSD_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.media.AudioManager} for handling management of volume,
     * ringer modes and audio routing.
     *
     * @see #getSystemService
     * @see android.media.AudioManager
     */
    static const char* AUDIO_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.service.fingerprint.FingerprintManager} for handling management
     * of fingerprints.
     *
     * @see #getSystemService
     * @see android.app.FingerprintManager
     * @hide
     */
    static const char* FINGERPRINT_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.media.MediaRouter} for controlling and managing
     * routing of media.
     *
     * @see #getSystemService
     * @see android.media.MediaRouter
     */
    static const char* MEDIA_ROUTER_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.media.session.MediaSessionManager} for managing media Sessions.
     *
     * @see #getSystemService
     * @see android.media.session.MediaSessionManager
     */
    static const char* MEDIA_SESSION_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.telephony.TelephonyManager} for handling management the
     * telephony features of the device.
     *
     * @see #getSystemService
     * @see android.telephony.TelephonyManager
     */
    static const char* TELEPHONY_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.telephony.SubscriptionManager} for handling management the
     * telephony subscriptions of the device.
     *
     * @see #getSystemService
     * @see android.telephony.SubscriptionManager
     */
    static const char* TELEPHONY_SUBSCRIPTION_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.telecom.TelecomManager} to manage telecom-related features
     * of the device.
     *
     * @see #getSystemService
     * @see android.telecom.TelecomManager
     */
    static const char* TELECOM_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.text.ClipboardManager} for accessing and modifying
     * the contents of the global clipboard.
     *
     * @see #getSystemService
     * @see android.text.ClipboardManager
     */
    static const char* CLIPBOARD_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.view.inputmethod.InputMethodManager} for accessing input
     * methods.
     *
     * @see #getSystemService
     */
    static const char* INPUT_METHOD_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.view.textservice.TextServicesManager} for accessing
     * text services.
     *
     * @see #getSystemService
     */
    static const char* TEXT_SERVICES_MANAGER_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.appwidget.AppWidgetManager} for accessing AppWidgets.
     *
     * @see #getSystemService
     */
    static const char* APPWIDGET_SERVICE;

    /**
     * Official published name of the (internal) voice interaction manager service.
     *
     * @hide
     * @see #getSystemService
     */
    static const char* VOICE_INTERACTION_MANAGER_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve an
     * {@link android.app.backup.IBackupManager IBackupManager} for communicating
     * with the backup mechanism.
     * @hide
     *
     * @see #getSystemService
     */
    static const char* BACKUP_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.os.DropBoxManager} instance for recording
     * diagnostic logs.
     * @see #getSystemService
     */
    static const char* DROPBOX_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.app.admin.DevicePolicyManager} for working with global
     * device policy management.
     *
     * @see #getSystemService
     */
    static const char* DEVICE_POLICY_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.app.UiModeManager} for controlling UI modes.
     *
     * @see #getSystemService
     */
    static const char* UI_MODE_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.app.DownloadManager} for requesting HTTP downloads.
     *
     * @see #getSystemService
     */
    static const char* DOWNLOAD_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.os.BatteryManager} for managing battery state.
     *
     * @see #getSystemService
     */
    static const char* BATTERY_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.nfc.NfcManager} for using NFC.
     *
     * @see #getSystemService
     */
    static const char* NFC_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.bluetooth.BluetoothAdapter} for using Bluetooth.
     *
     * @see #getSystemService
     */
    static const char* BLUETOOTH_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.net.sip.SipManager} for accessing the SIP related service.
     *
     * @see #getSystemService
     */
    /** @hide */
    static const char* SIP_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.hardware.usb.UsbManager} for access to USB devices (as a USB host)
     * and for controlling this device's behavior as a USB device.
     *
     * @see #getSystemService
     * @see android.hardware.usb.UsbManager
     */
    static const char* USB_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.hardware.SerialManager} for access to serial ports.
     *
     * @see #getSystemService
     * @see android.hardware.SerialManager
     *
     * @hide
     */
    static const char* SERIAL_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.hardware.hdmi.HdmiControlManager} for controlling and managing
     * HDMI-CEC protocol.
     *
     * @see #getSystemService
     * @see android.hardware.hdmi.HdmiControlManager
     * @hide
     */
    static const char* HDMI_CONTROL_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.hardware.input.InputManager} for interacting with input devices.
     *
     * @see #getSystemService
     * @see android.hardware.input.InputManager
     */
    static const char* INPUT_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.hardware.display.DisplayManager} for interacting with display devices.
     *
     * @see #getSystemService
     * @see android.hardware.display.DisplayManager
     */
    static const char* DISPLAY_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.os.UserManager} for managing users on devices that support multiple users.
     *
     * @see #getSystemService
     * @see android.os.UserManager
     */
    static const char* USER_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.content.pm.LauncherApps} for querying and monitoring launchable apps across
     * profiles of a user.
     *
     * @see #getSystemService
     * @see android.content.pm.LauncherApps
     */
    static const char* LAUNCHER_APPS_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.content.RestrictionsManager} for retrieving application restrictions
     * and requesting permissions for restricted operations.
     * @see #getSystemService
     * @see android.content.RestrictionsManager
     */
    static const char* RESTRICTIONS_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.app.AppOpsManager} for tracking application operations
     * on the device.
     *
     * @see #getSystemService
     * @see android.app.AppOpsManager
     */
    static const char* APP_OPS_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.hardware.camera2.CameraManager} for interacting with
     * camera devices.
     *
     * @see #getSystemService
     * @see android.hardware.camera2.CameraManager
     */
    static const char* CAMERA_SERVICE;

    /**
     * {@link android.print.PrintManager} for printing and managing
     * printers and print tasks.
     *
     * @see #getSystemService
     * @see android.print.PrintManager
     */
    static const char* PRINT_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.hardware.ConsumerIrManager} for transmitting infrared
     * signals from the device.
     *
     * @see #getSystemService
     * @see android.hardware.ConsumerIrManager
     */
    static const char* CONSUMER_IR_SERVICE;

    /**
     * {@link android.app.trust.TrustManager} for managing trust agents.
     * @see #getSystemService
     * @see android.app.trust.TrustManager
     * @hide
     */
    static const char* TRUST_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a
     * {@link android.media.tv.TvInputManager} for interacting with TV inputs
     * on the device.
     *
     * @see #getSystemService
     * @see android.media.tv.TvInputManager
     */
    static const char* TV_INPUT_SERVICE;

    /**
     * {@link android.net.NetworkScoreManager} for managing network scoring.
     * @see #getSystemService
     * @see android.net.NetworkScoreManager
     * @hide
     */
    static const char* NETWORK_SCORE_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.app.usage.UsageStatsManager} for querying device usage stats.
     *
     * @see #getSystemService
     * @see android.app.usage.UsageStatsManager
     */
    static const char* USAGE_STATS_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.app.job.JobScheduler} instance for managing occasional
     * background tasks.
     * @see #getSystemService
     * @see android.app.job.JobScheduler
     */
    static const char* JOB_SCHEDULER_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.service.persistentdata.PersistentDataBlockManager} instance
     * for interacting with a storage device that lives across factory resets.
     *
     * @see #getSystemService
     * @see android.service.persistentdata.PersistentDataBlockManager
     * @hide
     */
    static const char* PERSISTENT_DATA_BLOCK_SERVICE;

    /**
     * Use with {@link #getSystemService} to retrieve a {@link
     * android.media.projection.MediaProjectionManager} instance for managing
     * media projection sessions.
     * @see #getSystemService
     * @see android.media.projection.ProjectionManager
     */
    static const char* MEDIA_PROJECTION_SERVICE;

    /**
     * Flag for use with {@link #createPackageContext}: include the application
     * code with the context.  This means loading code into the caller's
     * process, so that {@link #getClassLoader()} can be used to instantiate
     * the application's classes.  Setting this flags imposes security
     * restrictions on what application context you can access; if the
     * requested application can not be safely loaded into your process,
     * java.lang.SecurityException will be thrown.  If this flag is not set,
     * there will be no restrictions on the packages that can be loaded,
     * but {@link #getClassLoader} will always return the default system
     * class loader.
     */
    static const int CONTEXT_INCLUDE_CODE = 0x00000001;

    /**
     * Flag for use with {@link #createPackageContext}: ignore any security
     * restrictions on the Context being requested, allowing it to always
     * be loaded.  For use with {@link #CONTEXT_INCLUDE_CODE} to allow code
     * to be loaded into a process even when it isn't safe to do so.  Use
     * with extreme care!
     */
    static const int CONTEXT_IGNORE_SECURITY = 0x00000002;

    /**
     * Flag for use with {@link #createPackageContext}: a restricted context may
     * disable specific features. For instance, a View associated with a restricted
     * context would ignore particular XML attributes.
     */
    static const int CONTEXT_RESTRICTED = 0x00000004;

    /**
     * @hide Used to indicate we should tell the activity manager about the process
     * loading this code.
     */
    static const int CONTEXT_REGISTER_PACKAGE = 0x40000000;
};




/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.os.PowerManager} for controlling power management,
 * including "wake locks," which let you keep the device on while
 * you're running long tasks.
 */
const char* Context::POWER_SERVICE = "power";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.view.WindowManager} for accessing the system's window
 * manager.
 *
 * @see #getSystemService
 * @see android.view.WindowManager
 */
const char* Context::WINDOW_SERVICE = "window";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.view.LayoutInflater} for inflating layout resources in this
 * context.
 *
 * @see #getSystemService
 * @see android.view.LayoutInflater
 */
const char* Context::LAYOUT_INFLATER_SERVICE = "layout_inflater";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.accounts.AccountManager} for receiving intents at a
 * time of your choosing.
 *
 * @see #getSystemService
 * @see android.accounts.AccountManager
 */
const char* Context::ACCOUNT_SERVICE = "account";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.app.ActivityManager} for interacting with the global
 * system state.
 *
 * @see #getSystemService
 * @see android.app.ActivityManager
 */
const char* Context::ACTIVITY_SERVICE = "activity";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.app.AlarmManager} for receiving intents at a
 * time of your choosing.
 *
 * @see #getSystemService
 * @see android.app.AlarmManager
 */
const char* Context::ALARM_SERVICE = "alarm";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.app.NotificationManager} for informing the user of
 * background events.
 *
 * @see #getSystemService
 * @see android.app.NotificationManager
 */
const char* Context::NOTIFICATION_SERVICE = "notification";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.view.accessibility.AccessibilityManager} for giving the user
 * feedback for UI events through the registered event listeners.
 *
 * @see #getSystemService
 * @see android.view.accessibility.AccessibilityManager
 */
const char* Context::ACCESSIBILITY_SERVICE = "accessibility";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.view.accessibility.CaptioningManager} for obtaining
 * captioning properties and listening for changes in captioning
 * preferences.
 *
 * @see #getSystemService
 * @see android.view.accessibility.CaptioningManager
 */
const char* Context::CAPTIONING_SERVICE = "captioning";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.app.NotificationManager} for controlling keyguard.
 *
 * @see #getSystemService
 * @see android.app.KeyguardManager
 */
const char* Context::KEYGUARD_SERVICE = "keyguard";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.location.LocationManager} for controlling location
 * updates.
 *
 * @see #getSystemService
 * @see android.location.LocationManager
 */
const char* Context::LOCATION_SERVICE = "location";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.location.CountryDetector} for detecting the country that
 * the user is in.
 *
 * @hide
 */
const char* Context::COUNTRY_DETECTOR = "country_detector";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.app.SearchManager} for handling searches.
 *
 * @see #getSystemService
 * @see android.app.SearchManager
 */
const char* Context::SEARCH_SERVICE = "search";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.hardware.SensorManager} for accessing sensors.
 *
 * @see #getSystemService
 * @see android.hardware.SensorManager
 */
const char* Context::SENSOR_SERVICE = "sensor";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.os.storage.StorageManager} for accessing system storage
 * functions.
 *
 * @see #getSystemService
 * @see android.os.storage.StorageManager
 */
const char* Context::STORAGE_SERVICE = "storage";

/**
 * Use with {@link #getSystemService} to retrieve a
 * com.android.server.WallpaperService for accessing wallpapers.
 *
 * @see #getSystemService
 */
const char* Context::WALLPAPER_SERVICE = "wallpaper";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.os.Vibrator} for interacting with the vibration hardware.
 *
 * @see #getSystemService
 * @see android.os.Vibrator
 */
const char* Context::VIBRATOR_SERVICE = "vibrator";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.app.StatusBarManager} for interacting with the status bar.
 *
 * @see #getSystemService
 * @see android.app.StatusBarManager
 * @hide
 */
const char* Context::STATUS_BAR_SERVICE = "statusbar";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.net.ConnectivityManager} for handling management of
 * network connections.
 *
 * @see #getSystemService
 * @see android.net.ConnectivityManager
 */
const char* Context::CONNECTIVITY_SERVICE = "connectivity";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.os.IUpdateLock} for managing runtime sequences that
 * must not be interrupted by headless OTA application or similar.
 *
 * @hide
 * @see #getSystemService
 * @see android.os.UpdateLock
 */
const char* Context::UPDATE_LOCK_SERVICE = "updatelock";

/**
 * Constant for the internal network management service, not really a Context service.
 * @hide
 */
const char* Context::NETWORKMANAGEMENT_SERVICE = "network_management";

/** {@hide} */
const char* Context::NETWORK_STATS_SERVICE = "netstats";
/** {@hide} */
const char* Context::NETWORK_POLICY_SERVICE = "netpolicy";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.net.wifi.WifiManager} for handling management of
 * Wi-Fi access.
 *
 * @see #getSystemService
 * @see android.net.wifi.WifiManager
 */
const char* Context::WIFI_SERVICE = "wifi";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.net.wifi.passpoint.WifiPasspointManager} for handling management of
 * Wi-Fi passpoint access.
 *
 * @see #getSystemService
 * @see android.net.wifi.passpoint.WifiPasspointManager
 * @hide
 */
const char* Context::WIFI_PASSPOINT_SERVICE = "wifipasspoint";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.net.wifi.p2p.WifiP2pManager} for handling management of
 * Wi-Fi peer-to-peer connections.
 *
 * @see #getSystemService
 * @see android.net.wifi.p2p.WifiP2pManager
 */
const char* Context::WIFI_P2P_SERVICE = "wifip2p";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.net.wifi.WifiScanner} for scanning the wifi universe
 *
 * @see #getSystemService
 * @see android.net.wifi.WifiScanner
 * @hide
 */
const char* Context::WIFI_SCANNING_SERVICE = "wifiscanner";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.net.wifi.RttManager} for ranging devices with wifi
 *
 * @see #getSystemService
 * @see android.net.wifi.RttManager
 * @hide
 */
const char* Context::WIFI_RTT_SERVICE = "rttmanager";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.net.EthernetManager} for handling management of
 * Ethernet access.
 *
 * @see #getSystemService
 * @see android.net.EthernetManager
 *
 * @hide
 */
const char* Context::ETHERNET_SERVICE = "ethernet";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.net.nsd.NsdManager} for handling management of network service
 * discovery
 *
 * @see #getSystemService
 * @see android.net.nsd.NsdManager
 */
const char* Context::NSD_SERVICE = "servicediscovery";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.media.AudioManager} for handling management of volume,
 * ringer modes and audio routing.
 *
 * @see #getSystemService
 * @see android.media.AudioManager
 */
const char* Context::AUDIO_SERVICE = "audio";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.service.fingerprint.FingerprintManager} for handling management
 * of fingerprints.
 *
 * @see #getSystemService
 * @see android.app.FingerprintManager
 * @hide
 */
const char* Context::FINGERPRINT_SERVICE = "fingerprint";

/**
* Use with {@link #getSystemService} to retrieve a
* {@link android.media.MediaRouter} for controlling and managing
* routing of media.
*
* @see #getSystemService
* @see android.media.MediaRouter
*/
const char* Context::MEDIA_ROUTER_SERVICE = "media_router";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.media.session.MediaSessionManager} for managing media Sessions.
 *
 * @see #getSystemService
 * @see android.media.session.MediaSessionManager
 */
const char* Context::MEDIA_SESSION_SERVICE = "media_session";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.telephony.TelephonyManager} for handling management the
 * telephony features of the device.
 *
 * @see #getSystemService
 * @see android.telephony.TelephonyManager
 */
const char* Context::TELEPHONY_SERVICE = "phone";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.telephony.SubscriptionManager} for handling management the
 * telephony subscriptions of the device.
 *
 * @see #getSystemService
 * @see android.telephony.SubscriptionManager
 */
const char* Context::TELEPHONY_SUBSCRIPTION_SERVICE = "telephony_subscription_service";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.telecom.TelecomManager} to manage telecom-related features
 * of the device.
 *
 * @see #getSystemService
 * @see android.telecom.TelecomManager
 */
const char* Context::TELECOM_SERVICE = "telecom";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.text.ClipboardManager} for accessing and modifying
 * the contents of the global clipboard.
 *
 * @see #getSystemService
 * @see android.text.ClipboardManager
 */
const char* Context::CLIPBOARD_SERVICE = "clipboard";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.view.inputmethod.InputMethodManager} for accessing input
 * methods.
 *
 * @see #getSystemService
 */
const char* Context::INPUT_METHOD_SERVICE = "input_method";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.view.textservice.TextServicesManager} for accessing
 * text services.
 *
 * @see #getSystemService
 */
const char* Context::TEXT_SERVICES_MANAGER_SERVICE = "textservices";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.appwidget.AppWidgetManager} for accessing AppWidgets.
 *
 * @see #getSystemService
 */
const char* Context::APPWIDGET_SERVICE = "appwidget";

/**
 * Official published name of the (internal) voice interaction manager service.
 *
 * @hide
 * @see #getSystemService
 */
const char* Context::VOICE_INTERACTION_MANAGER_SERVICE = "voiceinteraction";

/**
 * Use with {@link #getSystemService} to retrieve an
 * {@link android.app.backup.IBackupManager IBackupManager} for communicating
 * with the backup mechanism.
 * @hide
 *
 * @see #getSystemService
 */
const char* Context::BACKUP_SERVICE = "backup";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.os.DropBoxManager} instance for recording
 * diagnostic logs.
 * @see #getSystemService
 */
const char* Context::DROPBOX_SERVICE = "dropbox";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.app.admin.DevicePolicyManager} for working with global
 * device policy management.
 *
 * @see #getSystemService
 */
const char* Context::DEVICE_POLICY_SERVICE = "device_policy";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.app.UiModeManager} for controlling UI modes.
 *
 * @see #getSystemService
 */
const char* Context::UI_MODE_SERVICE = "uimode";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.app.DownloadManager} for requesting HTTP downloads.
 *
 * @see #getSystemService
 */
const char* Context::DOWNLOAD_SERVICE = "download";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.os.BatteryManager} for managing battery state.
 *
 * @see #getSystemService
 */
const char* Context::BATTERY_SERVICE = "batterymanager";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.nfc.NfcManager} for using NFC.
 *
 * @see #getSystemService
 */
const char* Context::NFC_SERVICE = "nfc";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.bluetooth.BluetoothAdapter} for using Bluetooth.
 *
 * @see #getSystemService
 */
const char* Context::BLUETOOTH_SERVICE = "bluetooth";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.net.sip.SipManager} for accessing the SIP related service.
 *
 * @see #getSystemService
 */
 /** @hide */
const char* Context::SIP_SERVICE = "sip";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.hardware.usb.UsbManager} for access to USB devices (as a USB host)
 * and for controlling this device's behavior as a USB device.
 *
 * @see #getSystemService
 * @see android.hardware.usb.UsbManager
 */
const char* Context::USB_SERVICE = "usb";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.hardware.SerialManager} for access to serial ports.
 *
 * @see #getSystemService
 * @see android.hardware.SerialManager
 *
 * @hide
 */
const char* Context::SERIAL_SERVICE = "serial";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.hardware.hdmi.HdmiControlManager} for controlling and managing
 * HDMI-CEC protocol.
 *
 * @see #getSystemService
 * @see android.hardware.hdmi.HdmiControlManager
 * @hide
 */
const char* Context::HDMI_CONTROL_SERVICE = "hdmi_control";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.hardware.input.InputManager} for interacting with input devices.
 *
 * @see #getSystemService
 * @see android.hardware.input.InputManager
 */
const char* Context::INPUT_SERVICE = "input";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.hardware.display.DisplayManager} for interacting with display devices.
 *
 * @see #getSystemService
 * @see android.hardware.display.DisplayManager
 */
const char* Context::DISPLAY_SERVICE = "display";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.os.UserManager} for managing users on devices that support multiple users.
 *
 * @see #getSystemService
 * @see android.os.UserManager
 */
const char* Context::USER_SERVICE = "user";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.content.pm.LauncherApps} for querying and monitoring launchable apps across
 * profiles of a user.
 *
 * @see #getSystemService
 * @see android.content.pm.LauncherApps
 */
const char* Context::LAUNCHER_APPS_SERVICE = "launcherapps";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.content.RestrictionsManager} for retrieving application restrictions
 * and requesting permissions for restricted operations.
 * @see #getSystemService
 * @see android.content.RestrictionsManager
 */
const char* Context::RESTRICTIONS_SERVICE = "restrictions";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.app.AppOpsManager} for tracking application operations
 * on the device.
 *
 * @see #getSystemService
 * @see android.app.AppOpsManager
 */
const char* Context::APP_OPS_SERVICE = "appops";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.hardware.camera2.CameraManager} for interacting with
 * camera devices.
 *
 * @see #getSystemService
 * @see android.hardware.camera2.CameraManager
 */
const char* Context::CAMERA_SERVICE = "camera";

/**
 * {@link android.print.PrintManager} for printing and managing
 * printers and print tasks.
 *
 * @see #getSystemService
 * @see android.print.PrintManager
 */
const char* Context::PRINT_SERVICE = "print";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.hardware.ConsumerIrManager} for transmitting infrared
 * signals from the device.
 *
 * @see #getSystemService
 * @see android.hardware.ConsumerIrManager
 */
const char* Context::CONSUMER_IR_SERVICE = "consumer_ir";

/**
 * {@link android.app.trust.TrustManager} for managing trust agents.
 * @see #getSystemService
 * @see android.app.trust.TrustManager
 * @hide
 */
const char* Context::TRUST_SERVICE = "trust";

/**
 * Use with {@link #getSystemService} to retrieve a
 * {@link android.media.tv.TvInputManager} for interacting with TV inputs
 * on the device.
 *
 * @see #getSystemService
 * @see android.media.tv.TvInputManager
 */
const char* Context::TV_INPUT_SERVICE = "tv_input";

/**
 * {@link android.net.NetworkScoreManager} for managing network scoring.
 * @see #getSystemService
 * @see android.net.NetworkScoreManager
 * @hide
 */
const char* Context::NETWORK_SCORE_SERVICE = "network_score";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.app.usage.UsageStatsManager} for querying device usage stats.
 *
 * @see #getSystemService
 * @see android.app.usage.UsageStatsManager
 */
const char* Context::USAGE_STATS_SERVICE = "usagestats";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.app.job.JobScheduler} instance for managing occasional
 * background tasks.
 * @see #getSystemService
 * @see android.app.job.JobScheduler
 */
const char* Context::JOB_SCHEDULER_SERVICE = "jobscheduler";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.service.persistentdata.PersistentDataBlockManager} instance
 * for interacting with a storage device that lives across factory resets.
 *
 * @see #getSystemService
 * @see android.service.persistentdata.PersistentDataBlockManager
 * @hide
 */
const char* Context::PERSISTENT_DATA_BLOCK_SERVICE = "persistent_data_block";

/**
 * Use with {@link #getSystemService} to retrieve a {@link
 * android.media.projection.MediaProjectionManager} instance for managing
 * media projection sessions.
 * @see #getSystemService
 * @see android.media.projection.ProjectionManager
 */
const char* Context::MEDIA_PROJECTION_SERVICE = "media_projection";

} // namespace content
} // namespace android

NS_FK_END

#endif // #if PLATFORM_ANDROID

#endif // END OF FOUNDATIONKIT_CONTEXT_HPP
