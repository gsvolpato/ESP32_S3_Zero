#ifndef COMPASS_H
#define COMPASS_H

struct CompassData {
    float heading;
    float x;
    float y;
    float z;
};

bool compassSetup();
bool compassGetData(CompassData *data);

#endif // COMPASS_H

