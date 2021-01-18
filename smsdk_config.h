#ifndef _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_

/**
 * @file smsdk_config.h
 * @brief Contains macros for configuring basic extension information.
 */

/* Basic information exposed publicly */
#define SMEXT_CONF_NAME			"GeoIP2"
#define SMEXT_CONF_DESCRIPTION		"Geographical IP information"
#define SMEXT_CONF_VERSION		"1.1.3"
#define SMEXT_CONF_AUTHOR		"Accelerator, AlliedModders LLC"
#define SMEXT_CONF_URL			"https://github.com/Accelerator74/GeoIP2"
#define SMEXT_CONF_LOGTAG		"GEOIP2"
#define SMEXT_CONF_LICENSE		"GPL"
#define SMEXT_CONF_DATESTRING		__DATE__

/** 
 * @brief Exposes plugin's main interface.
 */
#define SMEXT_LINK(name) SDKExtension *g_pExtensionIface = name;

/**
 * @brief Sets whether or not this plugin required Metamod.
 * NOTE: Uncomment to enable, comment to disable.
 */
//#define SMEXT_CONF_METAMOD		

/** Enable interfaces you want to use here by uncommenting lines */
//#define SMEXT_ENABLE_FORWARDSYS
//#define SMEXT_ENABLE_HANDLESYS
//#define SMEXT_ENABLE_PLAYERHELPERS
//#define SMEXT_ENABLE_DBMANAGER
//#define SMEXT_ENABLE_GAMECONF

#endif // _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_
