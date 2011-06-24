/**/
#include <stdlib.h>
#include <pthread.h>
#include <GeoIP.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"
#include "include/vct.h"
#include "vcc_if.h"

static const char *unknownCountry= "Unknown";

int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
    return (0);
}

const char *
vmod_country(struct sess *sp, const char *ip)
{
    int ret;
    const char *country = NULL;
    char *cp;
    static GeoIP *gi = NULL;

    gi = GeoIP_new(GEOIP_STANDARD);
    if (gi) {
      country = GeoIP_country_code_by_addr(gi, ip);
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
