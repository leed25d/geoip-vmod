/**/
#include <stdlib.h>
#include <GeoIP.h>
#include <GeoIPCity.h>

#include "vrt.h"
#include "cache/cache.h"
#include "vcc_if.h"

//  Are there any code elements available which I myself can
//  assign in my own application of ISO 3166-1?  Yes. There are
//  series of codes which the ISO 3166/MA will never use in the
//  updating process of // ISO 3166-1 and which are freely
//  available for users.
//
//  To quote from ISO 3166-1:2006, clause 8.1.3 User-assigned code
//  elements:
//  ,----
//  | If users need code elements to represent country names not
//  | included in this part of ISO 3166, the series of letters
//  | AA, QM to QZ, XA to XZ, and ZZ, and the series AAA to AAZ,
//  | QMA to QZZ, XAA to XZZ, and ZZA to ZZZ respectively and the
//  | series of numbers 900 to 999 are available. These users
//  | should inform the ISO 3166/MA of such use.
//  `----

static const char *unknownCountry = "AA";
static const char *unknownOrg = "Unknown organization";
static const char *unknownRegion = "Unknown region";
static const char *unknownCity = "Unknown City";

/**
 * Keep a pointer to each GeoIP database.
 */
struct GeoIP_databases {
    GeoIP* country;
    GeoIP* org;
    GeoIP* region;
    GeoIP* city;
};

/**
 * Manage the GeoIP databases at startup and exit.
 *
 * We try to open each database one by one. Failing to open one of them is
 * not fatal because some of them may just not be installed on the system
 * (for example the Organization db). In that case, all request for an
 * organization name will return the unknownCountry code.
 */
static void free_databases(void* ptr)
{
    struct GeoIP_databases* db = (struct GeoIP_databases*)ptr;
    if (db->country)
        GeoIP_delete(db->country);
    if (db->org)
        GeoIP_delete(db->org);
    if (db->region)
        GeoIP_delete(db->region);

    free(ptr);
}

int
init_function(struct vmod_priv *pp, const struct VCL_conf *conf)
{
    pp->priv = malloc(sizeof(struct GeoIP_databases));
    if (!pp->priv)
        return 1;
    struct GeoIP_databases* db = (struct GeoIP_databases*)pp->priv;

    db->country = GeoIP_new(GEOIP_MMAP_CACHE);
    db->org = GeoIP_open(GeoIPDBFileName[GEOIP_ORG_EDITION], GEOIP_MMAP_CACHE);
    db->region = GeoIP_open(GeoIPDBFileName[GEOIP_REGION_EDITION_REV1], GEOIP_MMAP_CACHE);
    db->city = GeoIP_open(GeoIPDBFileName[GEOIP_CITY_EDITION_REV1], GEOIP_MMAP_CACHE);

    pp->free = &free_databases;
    return (0);
}

/**
 * Country code.
 */
VCL_STRING
vmod_country(const struct vrt_ctx *ctx, struct vmod_priv *pp, const char *ip)
{
    const char *country = NULL;
    if (pp->priv)
    {
        struct GeoIP_databases* db = (struct GeoIP_databases*)pp->priv;
        if (db && db->country)
            country = GeoIP_country_code_by_addr(db->country, ip);
    }
    if (!country)
        country = unknownCountry;
    return WS_Copy(ctx->ws, country, -1);
}

VCL_STRING
vmod_country_code(const struct vrt_ctx *ctx, struct vmod_priv *pp, const char *ip)
{
    return vmod_country(ctx, pp, ip);
}

VCL_STRING
vmod_country_code_from_ip(const struct vrt_ctx* ctx, struct vmod_priv* pp, const struct suckaddr* ip)
{
    return vmod_country(ctx, pp, VRT_IP_string(ctx, ip));
}

/**
 * Organization
 */
VCL_STRING
vmod_organization(const struct vrt_ctx *ctx, struct vmod_priv *pp, const char *ip)
{
    const char *org = NULL;
    if (pp->priv)
    {
        struct GeoIP_databases* db = (struct GeoIP_databases*)pp->priv;
        if (db && db->org)
            org = GeoIP_org_by_addr(db->org, ip);
    }
    if (!org)
        org = unknownOrg;
    return WS_Copy(ctx->ws, org, -1);
}

VCL_STRING
vmod_organization_from_ip(const struct vrt_ctx* ctx, struct vmod_priv* pp, const struct suckaddr* ip)
{
    return vmod_organization(ctx, pp, VRT_IP_string(ctx, ip));
}

/**
 * Region
 */
VCL_STRING
vmod_region(const struct vrt_ctx *ctx, struct vmod_priv *pp, const char *ip)
{
    const char *region = NULL;

    if (pp->priv)
    {
        struct GeoIP_databases* db = (struct GeoIP_databases*)pp->priv;
        if (db && db->region)
        {
            GeoIPRegion *gir;
            if ((gir = GeoIP_region_by_addr(db->region, ip)) != NULL)
            {
                region = GeoIP_region_name_by_code(gir->country_code, gir->region);
                GeoIPRegion_delete(gir);
            }
        }

    }
    if (!region)
        region = unknownRegion;
    return WS_Copy(ctx->ws, region, -1);
}

VCL_STRING
vmod_region_from_ip(const struct vrt_ctx* ctx, struct vmod_priv* pp, const struct suckaddr* ip)
{
    return vmod_region(ctx, pp, VRT_IP_string(ctx, ip));
}

/**
 * City
 */
VCL_STRING
vmod_city(const struct vrt_ctx *ctx, struct vmod_priv *pp, const char *ip)
{
    const char *city = NULL;

    if (pp->priv)
    {
        struct GeoIP_databases* db = (struct GeoIP_databases*)pp->priv;
        if (db && db->city)
        {
            GeoIPRecord *gir;
            if ((gir = GeoIP_record_by_addr(db->city, ip)) != NULL)
            {
                if (gir->city)
                {
                    city = WS_Copy(ctx->ws, gir->city, -1);
                }
                else
                {
                    city = WS_Copy(ctx->ws, unknownCity, -1);
                }
                GeoIPRecord_delete(gir);
            }
        }

    }

    return city;
}

VCL_STRING
vmod_city_from_ip(const struct vrt_ctx* ctx, struct vmod_priv* pp, const struct suckaddr* ip)
{
    return vmod_city(ctx, pp, VRT_IP_string(ctx, ip));
}

