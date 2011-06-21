#include <stdlib.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"
#include <GeoIP.h>
#include "vcc_if.h"

static GeoIP *gi = NULL;

int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
    if(!gi) { gi = GeoIP_new(GEOIP_STANDARD); }
    return (0);
}

const char *
vmod_country(struct sess *sp, const char *ip)
{

  const char *country = NULL;
  if (!gi) return "Unknown";
  country = GeoIP_country_code_by_addr(gi, ip);
  return country ? country : "Unknown";

}
