#ifndef CALCULATE_VMAX_DEF_H
#define CALCULATE_VMAX_DEF_H

#include <time.h>

class Configuration {
	public:
        char instrument[10];
        char location[50];
        char network[50];
        int display_refresh;    // refresh interval minutes
        float logCheckAThreshold;
        float logSendThreshold;
        int disable_display;
};

class DNS_Name {
	public:
        char dns_name[100];
	    int port;
};

class IP_Address {
    public:
	    int ip1;
	    int ip2;
	    int ip3;
	    int ip4;
	    int port;
};

class EventStatus {
    public:
        float MaxEvent;
        unsigned long MaxEventTime;
        struct tm MaxEventDateTime;
        float LastEventX;
        unsigned long LastEventXTime;
        struct tm LastEventXDateTime;
        float LastEventY;
        unsigned long LastEventYTime;
        struct tm LastEventYDateTime;
        int clear_events;
};

class ScheduledEvent {
    public:
        unsigned long eventStartTime;
        unsigned long eventEndTime;
        float eventVmax;
};

class PPRThresholds {
    public:
        float yellow;
        float red;
};

const int AG = 9810;    // acceleration due to gravity mm/sec2

#define MAX_EVENTFILE_LINE_LENGTH 80

#endif //CALCULATE_VMAX_DEF_H