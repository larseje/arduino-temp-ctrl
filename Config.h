
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CFG_ADDR 0

typedef struct {
  float ferm_temp_set_point;
} sConfig;

class Config {
  public:
    void load ();
    void save ();
    float getFermentationTempSetPoint ();
    void setFermentationTempSetPoint (float val);
  private:
    sConfig cfg;
};

extern Config cfg;

#endif
