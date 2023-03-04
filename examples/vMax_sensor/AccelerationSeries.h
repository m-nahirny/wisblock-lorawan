#ifndef ACCELERATION_SERIES_H_
#define ACCELERATION_SERIES_H_

#include<iostream>
#include<vector>
using namespace std;

class AccelerometerReading
{
    public:
        float accel;
        float seriesTime;
};

class AccelerationSeries
{
    public:
        AccelerationSeries(float logAccelThreshold);
        ~AccelerationSeries();

        void AddEvent(float event, double dt);
        float CalculateSeriesForDisplay(bool logEvents = false, FILE *fp = NULL);
        float CalculateSeriesForLog();
        float GetFinalVelocity();
        float GetFinalDisplacement();
        void Clear(double startTime);

    private:
        float getVb(float Va, float Am, double dt);

        double seriesStartTime;

        vector<AccelerometerReading> events;
        float Vg;   // ground velocity at end of acceleration series
        float Dg;    // ground displacement at end of acceleration series

        float logCheckAThreshold;
};

#endif // ACCELERATION_SERIES_H_