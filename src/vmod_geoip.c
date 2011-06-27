/**/
#include <stdlib.h>
#include <GeoIP.h>
#include <pthread.h>

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
static pthread_mutex_t critSec_mtx;
int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
    int ret;
    ret = pthread_mutex_init(&critSec_mtx, NULL);
    assert (ret == 0);
    return (0);
}


const char *
critical_section(struct sess *sp, const char *ip)
{
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




const char *
vmod_country(struct sess *sp, const char *ip)
{
    int ret;
    const char *cp;
    
    ret = pthread_mutex_lock(&critSec_mtx);
    assert (ret == 0);
    cp= critical_section(sp, ip);
    pthread_mutex_unlock(&critSec_mtx);
    return(cp);   
}
