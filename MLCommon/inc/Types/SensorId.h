#ifndef TYPES_SENSORID_H
#define TYPES_SENSORID_H

#include <string>

namespace Types
{

    class SensorId
    {

    public:
        enum Enum
        {
            Unknown = 0,
            Sensor1 = 1,
            Sensor2 = 2
        };

        static ::std::string ToString(const Enum& e)
        {
            ::std::string res;

            switch (e)
            {
                case (Unknown) : res = "Unknown"; break;
                case (Sensor1) : res = "Sensor1"; break;
                case (Sensor2) : res = "Sensor2"; break;

                // No default case since we want to get a warning
            }

            return res;
        }

    private:

        SensorId();
        SensorId(const SensorId& o);
        SensorId& operator=(const SensorId& o);

    };

} // namespace - Types

#endif // ! TYPES_SENSORID_H
