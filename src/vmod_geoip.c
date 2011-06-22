#include <stdlib.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"
#include <GeoIP.h>
#include "vcc_if.h"

static GeoIP *gi = NULL;
static pthread_mutex_t gi_mutex;
static const char *unknownCountry= "Unknown";

int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
    int ret;
    ret= pthread_mutex_init(&gi_mutex, NULL);
    assert(ret == 0);

    if(!gi) { gi = GeoIP_new(GEOIP_STANDARD); }
    return (0);
}

const char *
vmod_country(struct sess *sp, const char *ip)
{
    int ret, w, v;
    const char *country = NULL;
    char *cp;
    if (!gi) return unknownCountry;

    ret = pthread_mutex_lock(&gi_mutex);
    assert(ret == 0);

    country = GeoIP_country_code_by_addr(gi, ip);

    w = WS_Reserve(sp->wrk->ws, 0);  /* Reserve some work space (w= # bytes reserved) */
    cp = sp->wrk->ws->f;             /* Front of workspace area */
    *cp= 0;
    v= snprintf(cp, w, "%s", country ? country : unknownCountry);

    pthread_mutex_unlock(&gi_mutex);

    if (v > w) {
        /* Insufficient reserved space, reset and leave */
        WS_Release(sp->wrk->ws, 0);
        return (unknownCountry);
    }
    return(cp);
}
