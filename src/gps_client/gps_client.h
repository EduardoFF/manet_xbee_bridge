#include <libgpsmm.h>
#include <string>
#include <cmath>

class GPSDClient {
  public:
    GPSDClient(gpsmm *gps, bool use_gps_time, bool check_fix_by_variance);

    bool start();
    void step();
    void stop();

  private:
    gpsmm *m_gps;
    bool m_use_gps_time;
    bool m_check_fix_by_variance;

    void process_data(struct gps_data_t* p);

    void process_data_gps(struct gps_data_t* p);

    void process_data_navsat(struct gps_data_t* p);
};

