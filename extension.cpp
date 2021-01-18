#include "extension.h"
#include "geoip_util.h"

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */
GeoIP_Extension g_GeoIP;
MMDB_s mmdb;

SMEXT_LINK(&g_GeoIP);

bool GeoIP_Extension::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	if (mmdb.filename) // Already loaded.
	{
		return true;
	}

	char m_GeoipDir[PLATFORM_MAX_PATH];
	g_pSM->BuildPath(Path_SM, m_GeoipDir, sizeof(m_GeoipDir), "configs/geoip");

	bool hasEntry = false;

	IDirectory *dir = libsys->OpenDirectory(m_GeoipDir);
	if (dir) 
	{
		while (dir->MoreFiles())
		{
			if (dir->IsEntryFile())
			{
				const char *name = dir->GetEntryName();
				size_t len = strlen(name);
				if (len >= 5 && strcmp(&name[len-5], ".mmdb") == 0)
				{
					char database[PLATFORM_MAX_PATH];
					libsys->PathFormat(database, sizeof(database), "%s/%s", m_GeoipDir, name);

					int status = MMDB_open(database, MMDB_MODE_MMAP, &mmdb);

					if (status != MMDB_SUCCESS)
					{
						ke::SafeStrcpy(error, maxlength, "Failed to open GeoIP2 database.");
						return false;
					}

					hasEntry = true;
					break;
				}
			}
			dir->NextEntry();
		}
		libsys->CloseDirectory(dir);
	}

	if (!hasEntry)
	{
		ke::SafeStrcpy(error, maxlength, "Could not find GeoIP2 database.");
		return false;
	}

	g_pShareSys->AddNatives(myself, geoip_natives);
	g_pShareSys->RegisterLibrary(myself, "GeoIP2");

	char date[40];
	const time_t epoch = (const time_t)mmdb.metadata.build_epoch;
	strftime(date, 40, "%F %T UTC", gmtime(&epoch));

	g_pSM->LogMessage(myself, "GeoIP2 database loaded: %s (%s)", mmdb.metadata.database_type, date);

	char buf[64];
	for (size_t i = 0; i < mmdb.metadata.languages.count; i++)
	{
		if (i == 0)
		{
			strcpy(buf, mmdb.metadata.languages.names[i]);
		}
		else
		{
			strcat(buf, " ");
			strcat(buf, mmdb.metadata.languages.names[i]);
		}
	}

	g_pSM->LogMessage(myself, "GeoIP2 supported languages: %s", buf);

	return true;
}

void GeoIP_Extension::SDK_OnUnload()
{
	MMDB_close(&mmdb);
}

inline void StripPort(char *ip)
{
	char *tmp = strchr(ip, ':');
	if (!tmp)
		return;
	*tmp = '\0';
}

static cell_t sm_Geoip_Code2(IPluginContext *pCtx, const cell_t *params)
{
	char *ip;

	pCtx->LocalToString(params[1], &ip);
	StripPort(ip);

	const char *path[] = {"country", "iso_code", NULL};
	ke::AString str = lookupString(ip, path);
	const char *ccode = str.chars();

	pCtx->StringToLocal(params[2], 3, ccode);

	return (strlen(ccode) != 0) ? 1 : 0;
}

static cell_t sm_Geoip_Code3(IPluginContext *pCtx, const cell_t *params)
{
	char *ip;

	pCtx->LocalToString(params[1], &ip);
	StripPort(ip);

	const char *path[] = {"country", "iso_code", NULL};
	ke::AString str = lookupString(ip, path);
	const char *ccode = str.chars();

	for (size_t i = 0; i < SM_ARRAYSIZE(GeoIPCountryCode); i++)
	{
		if (!strncmp(ccode, GeoIPCountryCode[i], 2))
		{
			ccode = GeoIPCountryCode3[i];
			break;
		}
	}

	pCtx->StringToLocal(params[2], 4, ccode);

	return (strlen(ccode) != 0) ? 1 : 0;
}

static cell_t sm_Geoip_ContinentCode(IPluginContext *pCtx, const cell_t *params)
{
	char *ip;

	pCtx->LocalToString(params[1], &ip);
	StripPort(ip);

	const char *path[] = {"continent", "code", NULL};
	ke::AString str = lookupString(ip, path);
	const char *ccode = str.chars();

	pCtx->StringToLocal(params[2], 3, ccode);

	return (strlen(ccode) != 0) ? 1 : 0;
}

static cell_t sm_Geoip_RegionCode(IPluginContext *pCtx, const cell_t *params)
{
	char *ip;

	pCtx->LocalToString(params[1], &ip);
	StripPort(ip);

	const char *path[] = {"subdivisions", "0", "iso_code", NULL};
	ke::AString str = lookupString(ip, path);
	const char *ccode = str.chars();

	pCtx->StringToLocal(params[2], 4, ccode);

	return (strlen(ccode) != 0) ? 1 : 0;
}

static cell_t sm_Geoip_Timezone(IPluginContext *pCtx, const cell_t *params)
{
	char *ip;

	pCtx->LocalToString(params[1], &ip);
	StripPort(ip);

	const char *path[] = {"location", "time_zone", NULL};
	ke::AString str = lookupString(ip, path);
	const char *ccode = str.chars();

	pCtx->StringToLocal(params[2], params[3], ccode);

	return (strlen(ccode) != 0) ? 1 : 0;
}

static cell_t sm_Geoip_Country(IPluginContext *pCtx, const cell_t *params)
{
	char *ip;
	char *lang;

	pCtx->LocalToString(params[1], &ip);
	pCtx->LocalToString(params[4], &lang);
	StripPort(ip);

	if (strlen(lang) == 0) strcpy(lang, "en");

	const char *path[] = {"country", "names", lang, NULL};
	ke::AString str = lookupString(ip, path);
	const char *ccode = str.chars();

	pCtx->StringToLocal(params[2], params[3], ccode);

	return (strlen(ccode) != 0) ? 1 : 0;
}

static cell_t sm_Geoip_Continent(IPluginContext *pCtx, const cell_t *params)
{
	char *ip;
	char *lang;

	pCtx->LocalToString(params[1], &ip);
	pCtx->LocalToString(params[4], &lang);
	StripPort(ip);

	if (strlen(lang) == 0) strcpy(lang, "en");

	const char *path[] = {"continent", "names", lang, NULL};
	ke::AString str = lookupString(ip, path);
	const char *ccode = str.chars();

	pCtx->StringToLocal(params[2], params[3], ccode);

	return (strlen(ccode) != 0) ? 1 : 0;
}

static cell_t sm_Geoip_Region(IPluginContext *pCtx, const cell_t *params)
{
	char *ip;
	char *lang;

	pCtx->LocalToString(params[1], &ip);
	pCtx->LocalToString(params[4], &lang);
	StripPort(ip);

	if (strlen(lang) == 0) strcpy(lang, "en");

	const char *path[] = {"subdivisions", "0", "names", lang, NULL};
	ke::AString str = lookupString(ip, path);
	const char *ccode = str.chars();

	pCtx->StringToLocal(params[2], params[3], ccode);

	return (strlen(ccode) != 0) ? 1 : 0;
}

static cell_t sm_Geoip_City(IPluginContext *pCtx, const cell_t *params)
{
	char *ip;
	char *lang;

	pCtx->LocalToString(params[1], &ip);
	pCtx->LocalToString(params[4], &lang);
	StripPort(ip);

	if (strlen(lang) == 0) strcpy(lang, "en");

	const char *path[] = {"city", "names", lang, NULL};
	ke::AString str = lookupString(ip, path);
	const char *ccode = str.chars();

	pCtx->StringToLocal(params[2], params[3], ccode);

	return (strlen(ccode) != 0) ? 1 : 0;
}

const sp_nativeinfo_t geoip_natives[] = 
{
	{"GeoipCode2",			sm_Geoip_Code2},
	{"GeoipCode3",			sm_Geoip_Code3},
	{"GeoipContinentCode",		sm_Geoip_ContinentCode},
	{"GeoipRegionCode",		sm_Geoip_RegionCode},
	{"GeoipTimezone",		sm_Geoip_Timezone},
	{"GeoipCountry",		sm_Geoip_Country},
	{"GeoipContinent",		sm_Geoip_Continent},
	{"GeoipRegion",			sm_Geoip_Region},
	{"GeoipCity",			sm_Geoip_City},
	{NULL,					NULL},
};

