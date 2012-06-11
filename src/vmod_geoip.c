/**/
#include <stdlib.h>
#include <string.h>
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

int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
    return (0);
}

const char *
vmod_country(struct sess *sp, const char *ip)
{
    const char *country = NULL;
    char *cp;
    int cache_strategy = GEOIP_MMAP_CACHE | GEOIP_CHECK_CACHE;
    GeoIP *gi = NULL;

    if (strchr(ip, ':')) {
      gi = GeoIP_open_type(GEOIP_COUNTRY_EDITION_V6, cache_strategy);
      if (gi) {
        country = GeoIP_country_code_by_addr_v6(gi, ip);
      }
    } else {
      gi = GeoIP_open_type(GEOIP_COUNTRY_EDITION, cache_strategy);
      if (gi) {
        country = GeoIP_country_code_by_addr(gi, ip);
      }
    }

    if (!country) {
      country= unknownCountry;
    }
    cp= WS_Dup(sp->wrk->ws, country);

    if (gi) {
      GeoIP_delete(gi);
    }
    return(cp);
}
