
#include "./dev.h"


static pwr_dev_t open( const char *initstr )
{
}

static int close( pwr_dev_t dev)
{
}

static int read( pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float reading[], unsigned long long *timestamp)
{
}

static int write( pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float setting[], unsigned long long *timestamp)
{
}

static int time(pwr_dev_t dev, unsigned long long *time)
{
}


static plugin_dev_t dev = {
    open: open, 
    close: close,
    read: read,
    write: write,
    time: time,
};

plugin_dev_t* getDev() {
    return &dev;
}
