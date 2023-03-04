#include "AccelerationSeries.h"
#include "CalculateVmaxDef.h"
#include  <math.h>

AccelerationSeries::AccelerationSeries(float logAccelThreshold)
{
    Vg = 0;
    Dg = 0;
    logCheckAThreshold = logAccelThreshold;
}

AccelerationSeries::~AccelerationSeries()
{
    events.clear();
    events.shrink_to_fit();
}

void AccelerationSeries::Clear(double startTime)
{
    events.clear();
    events.shrink_to_fit();
    seriesStartTime = startTime;
}

// event is acceleration in mm/sec2
void AccelerationSeries::AddEvent(float event, double eventTime)
{
    // AccelerometerReading *reading = new AccelerometerReading();
    AccelerometerReading reading;
    reading.accel = event;
    reading.seriesTime = eventTime - seriesStartTime;
    events.push_back(reading);
}

// returns the ending velocity for a time series in mm/sec. 
// If no element in the series is over 1 %g or the terminal velocity is less than 15 mm/sec or the ending displacement is less than 2 mm return 0
float AccelerationSeries::CalculateSeriesForDisplay(bool logEvents, FILE *fp)
{
//	double Fb, Am, Dmb, Md, Vmb;
    float Vg_prev = 0;
    float Dg_prev = 0;
//    float Dma = 0;
//    float Vma = 0;
    bool ConformanceCheckA = false;
    bool ConformanceCheckB = false;
    bool ConformanceCheckC = false;
    float prev_time = 0;
    float prev_accel = 0;
    float avg_accel;
    float dt;

    for (auto it = events.begin(); it != events.end(); it++)
    {
        // conformance check A - at least one element in the series is over 1 %g
        // note that the acceleration series is stored in mm/sec2
        if (fabs(it->accel) / AG > 0.01)
            ConformanceCheckA = true;

        dt = it->seriesTime - prev_time;
        avg_accel = (prev_accel + it->accel) / 2;
        Vg = avg_accel * dt + Vg_prev;  // mm / sec
        Dg = Vg * dt + Dg_prev;         // mm
        prev_time = it->seriesTime;
        prev_accel = it->accel;

        // check for accumulated error in very long time series
        if (Dg > 400)
        {
            cout << "Resetting acceleration series - Vg: " << Vg << ", Dg: " << Dg << ", acceleration series is " << it->seriesTime - seriesStartTime << " seconds with " << events.size() << " events" << endl;
            float dt_dump;
            float prev_time_dump = 0;
            float Vg_prev_dump = 0;
            float Dg_prev_dump = 0;
            float prev_accel_dump = 0;
            float Dg_dump, Vg_dump;
            for (auto it_dump = events.begin(); it_dump != events.end(); it_dump++)
            {
                dt_dump = it_dump->seriesTime - prev_time_dump;
                avg_accel = (prev_accel_dump + it_dump->accel) / 2;
                Vg_dump = avg_accel * dt_dump + Vg_prev_dump;
                Dg_dump = Vg_dump * dt_dump + Dg_prev_dump;
                prev_time_dump = it_dump->seriesTime;
                prev_accel_dump = it_dump->accel;
                // cout << "dt: " << dt_dump << ", avg accel: " << avg_accel << ", Vg: " << Vg_dump << ", Dg: " << Dg_dump << endl;
            }

            Dg = 0;
            Vg = 0;
        }

        if (logEvents)
        {
            fprintf(fp, "%f,%f,%f,%f,%f\n", seriesStartTime + it->seriesTime, dt, avg_accel, Vg, Dg);
        }
        Vg_prev = Vg;
        Dg_prev = Dg;
    }

//    if (fabs(Vg) > 100)
    if (fabs(Vg) > 15)  // mm / sec
        ConformanceCheckB = true;
//    if (fabs(Dg) > 5)
    if (fabs(Dg) > 2)   // mm
        ConformanceCheckC = true;

//     if (ConformanceCheckA && fabs(Vg) > 10)
  //       cout << "Vg: " << Vg << ", Dg: " << Dg << ", A: " << ConformanceCheckA << ", B: " << ConformanceCheckB << ", C: " << ConformanceCheckC << endl;

    if (!ConformanceCheckA || !ConformanceCheckB || !ConformanceCheckC)
    {
        Vg = 0;
//        maxMd = 0;
//        maxMdVmb = 0;
    }
    return Vg;
}

float AccelerationSeries::CalculateSeriesForLog()
{
    float Vg_prev = 0;
    float Dg_prev = 0;
    bool ConformanceCheckA = false;
    float prev_time = 0;
    float prev_accel = 0;
    float avg_accel;
    float dt;

    for (auto it = events.begin(); it != events.end(); it++)
    {
        // conformance check A - at least one element in the series is over 1 %g
        // note that the acceleration series is stored in mm/sec2
        // the configuration value for the threshold is in %g so convert to g
        if (fabs(it->accel) / AG > logCheckAThreshold / 100)
            ConformanceCheckA = true;

        dt = it->seriesTime - prev_time;
        avg_accel = (prev_accel + it->accel) / 2;
        Vg = avg_accel * dt + Vg_prev;
        Dg = Vg * dt + Dg_prev;
        prev_time = it->seriesTime;
        prev_accel = it->accel;

        // check for accumulated error in very long time series
        if (Dg > 400)
        {
            Dg = 0;
            Vg = 0;
        }

        Vg_prev = Vg;
        Dg_prev = Dg;
    }

    // if (ConformanceCheckA && fabs(Vg) > 0)
    //     cout << "Vg: " << Vg << ", Dg: " << Dg << endl;

    if (!ConformanceCheckA)
    {
        Vg = 0;
    }

    return Vg;
}

float AccelerationSeries::GetFinalVelocity()
{
    return CalculateSeriesForDisplay();
}

float AccelerationSeries::GetFinalDisplacement()
{
    CalculateSeriesForDisplay();
    return Dg;
}

