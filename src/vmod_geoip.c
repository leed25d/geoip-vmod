/**/
#include <stdlib.h>
#include <GeoIP.h>

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

static const char *unknownCountry= "AA";

struct GeoIP_databases {
    GeoIP* country;
};

static void free_databases(void* ptr)
{
    struct GeoIP_databases* db = (struct GeoIP_databases*)ptr;
    if (db->country)
        GeoIP_delete(db->country);

    free(ptr);
}

int
init_function(struct vmod_priv *pp, const struct VCL_conf *conf)
{
    pp->priv = malloc(sizeof(struct GeoIP_databases));
    if (!pp->priv)
        return 1;
    struct GeoIP_databases* db = (struct GeoIP_databases*)pp->priv;

    db->country = GeoIP_new(GEOIP_STANDARD);
    pp->free = &free_databases;
    return (0);
}

VCL_STRING
vmod_country(const struct vrt_ctx *ctx, struct vmod_priv *pp, const char *ip)
{
    const char *country = NULL;
    char *cp;
    if (pp->priv)
    {
        struct GeoIP_databases* db = (struct GeoIP_databases*)pp->priv;
        if (db && db->country)
            country = GeoIP_country_code_by_addr(db->country, ip);
    }
    if (!country)
        country = unknownCountry;
    cp = WS_Copy(ctx->ws, country, strlen(country));

    return(cp);
}
