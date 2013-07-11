/**/
#include <stdlib.h>
#include <GeoIP.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"
#include "include/vct.h"
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
static GeoIP *gi;
int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
    if (gi) {
      GeoIP_delete(gi);
    }
    gi = GeoIP_new(GEOIP_STANDARD);
    return (0);
}

const char *
vmod_country(struct sess *sp, const char *ip)
{
    const char *country = NULL;
    char *cp;

    if (!gi) {
        gi = GeoIP_new(GEOIP_STANDARD);
        if (gi) {
            WSP(sp, SLT_VCL_Log, "GeoIP database was loaded on request.");
        } else {
            WSP(sp, SLT_VCL_Log, "GeoIP database was not loaded on request.");
        }
    }
    if (gi) {
      country = GeoIP_country_code_by_addr(gi, ip);
    }
    if (!country) {
      country= unknownCountry;
    }
    cp= WS_Dup(sp->wrk->ws, country);

    return(cp);
}
