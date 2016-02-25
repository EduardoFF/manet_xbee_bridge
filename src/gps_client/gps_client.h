#include <libgpsmm.h>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>

class GPSDClient {
  public:
    GPSDClient();
    float m_time, m_latitude,
      m_longitude, m_altitude;

    bool start();
    void step();
    void stop();
    float getLat()  const;
    float getLong() const;
    float getAlt()  const;
    float getTime() const;

  private:

    gpsmm *m_gps;
    bool m_use_gps_time;
    bool m_check_fix_by_variance;

    void process_data(struct gps_data_t* p);

    void process_data_gps(struct gps_data_t* p);

};

