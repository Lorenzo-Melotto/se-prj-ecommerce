#include "time_utils.h"

int millisec_sleep(long millisec){
    if(millisec < 0){
        // return -1 if the time to wait is negative
        errno = EINVAL;
        return -1;
    }
    struct timespec ts;
    int res;

    ts.tv_sec = millisec / 1000; // millisecond to second conversion
    ts.tv_nsec = (millisec % 1000) * 1000000; // millisecond to nanosecond conversion

    do {
        res = nanosleep(&ts, &ts);
    } while(res && errno == EINTR);

    return res;
}

int long get_nanoseconds() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((long) (ts.tv_sec * 1000000000L + ts.tv_nsec));
}

long int get_day_nanos(char* buf) {
   struct tm *info;
   struct timespec ts;

   timespec_get(&ts, TIME_UTC);
    
   info = localtime(&(ts.tv_sec));
   // ISO	ISO 8601, SQL standard	1997-12-17 07:37:16-08
   sprintf(buf, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
	       info -> tm_year + 1900,
	       info -> tm_mon + 1,
	       info -> tm_mday,
	       info -> tm_hour,
	       info -> tm_min,
	       info -> tm_sec);

    return ts.tv_nsec;
}

void update_time() {
    nanos = get_nanoseconds();
    timeadvance = ((double) (nanos - last_nanos))/((double) (1000000000L));
    last_nanos = nanos;
}

void init_time() {
    last_nanos = get_nanoseconds();
}

chrono::system_clock::time_point chrono_start(){
    return chrono::high_resolution_clock::now();
}

int chrono_end(chrono::system_clock::time_point start) {
    chrono::system_clock::time_point end = chrono::high_resolution_clock::now();
    chrono::nanoseconds duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
    int time_passed = duration.count();
    return time_passed;
}

