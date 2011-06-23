/**/
#include <stdlib.h>
#include <pthread.h>
#include <GeoIP.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"
#include "include/vct.h"
#include "vcc_if.h"

static GeoIP *gi = NULL;
static pthread_mutex_t gi_mutex;
static const char *unknownCountry= "Unknown";
static char wspace[8192];

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
    int ret;
    const char *country = NULL;
    char *cp;
    if (!gi) return(unknownCountry);

    ret = pthread_mutex_lock(&gi_mutex);
    assert(ret == 0);

    country = GeoIP_country_code_by_addr(gi, ip);
    if (country == NULL) {
        return(unknownCountry);
    }

    cp= WS_Dup(sp->wrk->ws, country);
    pthread_mutex_unlock(&gi_mutex);

    return(cp);
}
