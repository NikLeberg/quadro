/*
  BNO055.h v0.6 - Bibliothek um den Bosch Sensortec BNO055 9-Achsen-Sensor anzusprechen.
  Erstellt von Niklaus R. Leuenberger, Juli 16, 2018.
*/

#ifndef BNO055_h
#define BNO055_h

#include "Arduino.h"
#include <Wire.h>
#include <functional>

/* REGISTER ADDRESS DEFINITION */
/* Page id register definition */
#define BNO055_PAGE_ID_ADDR                 (0x07)

/* PAGE0 REGISTER DEFINITION START */
#define BNO055_CHIP_ID_ADDR                 (0x00)
#define BNO055_ACCEL_REV_ID_ADDR            (0x01)
#define BNO055_MAG_REV_ID_ADDR              (0x02)
#define BNO055_GYRO_REV_ID_ADDR             (0x03)
#define BNO055_SW_REV_ID_LSB_ADDR           (0x04)
#define BNO055_SW_REV_ID_MSB_ADDR           (0x05)
#define BNO055_BL_REV_ID_ADDR               (0X06)

/* Accel data register */
#define BNO055_ACCEL_DATA_X_LSB_ADDR        (0x08)
#define BNO055_ACCEL_DATA_X_MSB_ADDR        (0x09)
#define BNO055_ACCEL_DATA_Y_LSB_ADDR        (0x0A)
#define BNO055_ACCEL_DATA_Y_MSB_ADDR        (0x0B)
#define BNO055_ACCEL_DATA_Z_LSB_ADDR        (0x0C)
#define BNO055_ACCEL_DATA_Z_MSB_ADDR        (0x0D)

/* Mag data register */
#define BNO055_MAG_DATA_X_LSB_ADDR          (0x0E)
#define BNO055_MAG_DATA_X_MSB_ADDR          (0x0F)
#define BNO055_MAG_DATA_Y_LSB_ADDR          (0x10)
#define BNO055_MAG_DATA_Y_MSB_ADDR          (0x11)
#define BNO055_MAG_DATA_Z_LSB_ADDR          (0x12)
#define BNO055_MAG_DATA_Z_MSB_ADDR          (0x13)

/* Gyro data registers */
#define BNO055_GYRO_DATA_X_LSB_ADDR         (0x14)
#define BNO055_GYRO_DATA_X_MSB_ADDR         (0x15)
#define BNO055_GYRO_DATA_Y_LSB_ADDR         (0x16)
#define BNO055_GYRO_DATA_Y_MSB_ADDR         (0x17)
#define BNO055_GYRO_DATA_Z_LSB_ADDR         (0x18)
#define BNO055_GYRO_DATA_Z_MSB_ADDR         (0x19)

/* Euler data registers */
#define BNO055_EULER_H_LSB_ADDR             (0x1A)
#define BNO055_EULER_H_MSB_ADDR             (0x1B)

#define BNO055_EULER_R_LSB_ADDR             (0x1C)
#define BNO055_EULER_R_MSB_ADDR             (0x1D)

#define BNO055_EULER_P_LSB_ADDR             (0x1E)
#define BNO055_EULER_P_MSB_ADDR             (0x1F)

/* Quaternion data registers */
#define BNO055_QUATERNION_DATA_W_LSB_ADDR   (0x20)
#define BNO055_QUATERNION_DATA_W_MSB_ADDR   (0x21)
#define BNO055_QUATERNION_DATA_X_LSB_ADDR   (0x22)
#define BNO055_QUATERNION_DATA_X_MSB_ADDR   (0x23)
#define BNO055_QUATERNION_DATA_Y_LSB_ADDR   (0x24)
#define BNO055_QUATERNION_DATA_Y_MSB_ADDR   (0x25)
#define BNO055_QUATERNION_DATA_Z_LSB_ADDR   (0x26)
#define BNO055_QUATERNION_DATA_Z_MSB_ADDR   (0x27)

/* Linear acceleration data registers */
#define BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR (0X28)
#define BNO055_LINEAR_ACCEL_DATA_X_MSB_ADDR (0X29)
#define BNO055_LINEAR_ACCEL_DATA_Y_LSB_ADDR (0x2A)
#define BNO055_LINEAR_ACCEL_DATA_Y_MSB_ADDR (0x2B)
#define BNO055_LINEAR_ACCEL_DATA_Z_LSB_ADDR (0x2C)
#define BNO055_LINEAR_ACCEL_DATA_Z_MSB_ADDR (0x2D)

/* Gravity data registers */
#define BNO055_GRAVITY_DATA_X_LSB_ADDR      (0x2E)
#define BNO055_GRAVITY_DATA_X_MSB_ADDR      (0x2F)
#define BNO055_GRAVITY_DATA_Y_LSB_ADDR      (0x30)
#define BNO055_GRAVITY_DATA_Y_MSB_ADDR      (0x31)
#define BNO055_GRAVITY_DATA_Z_LSB_ADDR      (0x32)
#define BNO055_GRAVITY_DATA_Z_MSB_ADDR      (0x33)

/* Temperature data register */
#define BNO055_TEMP_ADDR                    (0x34)

/* Status registers */
#define BNO055_CALIB_STAT_ADDR              (0x35)
#define BNO055_SELFTEST_RESULT_ADDR         (0x36)
#define BNO055_INTR_STAT_ADDR               (0x37)
#define BNO055_SYS_CLK_STAT_ADDR            (0x38)
#define BNO055_SYS_STAT_ADDR                (0x39)
#define BNO055_SYS_ERR_ADDR                 (0x3A)

/* Unit selection register */
#define BNO055_UNIT_SEL_ADDR                (0x3B)
#define BNO055_DATA_SELECT_ADDR             (0x3C)

/* Mode registers */
#define BNO055_OPR_MODE_ADDR                (0x3D)
#define BNO055_PWR_MODE_ADDR                (0x3E)

#define BNO055_SYS_TRIGGER_ADDR             (0x3F)
#define BNO055_TEMP_SOURCE_ADDR             (0x40)
/* Axis remap registers */
#define BNO055_AXIS_MAP_CONFIG_ADDR         (0x41)
#define BNO055_AXIS_MAP_SIGN_ADDR           (0x42)

/* SIC registers */
#define BNO055_SIC_MATRIX_0_LSB_ADDR        (0x43)
#define BNO055_SIC_MATRIX_0_MSB_ADDR        (0x44)
#define BNO055_SIC_MATRIX_1_LSB_ADDR        (0x45)
#define BNO055_SIC_MATRIX_1_MSB_ADDR        (0x46)
#define BNO055_SIC_MATRIX_2_LSB_ADDR        (0x47)
#define BNO055_SIC_MATRIX_2_MSB_ADDR        (0x48)
#define BNO055_SIC_MATRIX_3_LSB_ADDR        (0x49)
#define BNO055_SIC_MATRIX_3_MSB_ADDR        (0x4A)
#define BNO055_SIC_MATRIX_4_LSB_ADDR        (0x4B)
#define BNO055_SIC_MATRIX_4_MSB_ADDR        (0x4C)
#define BNO055_SIC_MATRIX_5_LSB_ADDR        (0x4D)
#define BNO055_SIC_MATRIX_5_MSB_ADDR        (0x4E)
#define BNO055_SIC_MATRIX_6_LSB_ADDR        (0x4F)
#define BNO055_SIC_MATRIX_6_MSB_ADDR        (0x50)
#define BNO055_SIC_MATRIX_7_LSB_ADDR        (0x51)
#define BNO055_SIC_MATRIX_7_MSB_ADDR        (0x52)
#define BNO055_SIC_MATRIX_8_LSB_ADDR        (0x53)
#define BNO055_SIC_MATRIX_8_MSB_ADDR        (0x54)

/* Accelerometer Offset registers */
#define BNO055_ACCEL_OFFSET_X_LSB_ADDR      (0x55)
#define BNO055_ACCEL_OFFSET_X_MSB_ADDR      (0x56)
#define BNO055_ACCEL_OFFSET_Y_LSB_ADDR      (0x57)
#define BNO055_ACCEL_OFFSET_Y_MSB_ADDR      (0x58)
#define BNO055_ACCEL_OFFSET_Z_LSB_ADDR      (0x59)
#define BNO055_ACCEL_OFFSET_Z_MSB_ADDR      (0x5A)

/* Magnetometer Offset registers */
#define BNO055_MAG_OFFSET_X_LSB_ADDR        (0x5B)
#define BNO055_MAG_OFFSET_X_MSB_ADDR        (0x5C)
#define BNO055_MAG_OFFSET_Y_LSB_ADDR        (0x5D)
#define BNO055_MAG_OFFSET_Y_MSB_ADDR        (0x5E)
#define BNO055_MAG_OFFSET_Z_LSB_ADDR        (0x5F)
#define BNO055_MAG_OFFSET_Z_MSB_ADDR        (0x60)

/* Gyroscope Offset registers */
#define BNO055_GYRO_OFFSET_X_LSB_ADDR       (0x61)
#define BNO055_GYRO_OFFSET_X_MSB_ADDR       (0x62)
#define BNO055_GYRO_OFFSET_Y_LSB_ADDR       (0x63)
#define BNO055_GYRO_OFFSET_Y_MSB_ADDR       (0x64)
#define BNO055_GYRO_OFFSET_Z_LSB_ADDR       (0x65)
#define BNO055_GYRO_OFFSET_Z_MSB_ADDR       (0x66)

/* Radius registers */
#define BNO055_ACCEL_RADIUS_LSB_ADDR        (0x67)
#define BNO055_ACCEL_RADIUS_MSB_ADDR        (0x68)
#define BNO055_MAG_RADIUS_LSB_ADDR          (0x69)
#define BNO055_MAG_RADIUS_MSB_ADDR          (0x6A)
/* PAGE0 REGISTERS DEFINITION END */

/* PAGE1 REGISTERS DEFINITION START */
/* Configuration registers */
#define BNO055_ACCEL_CONFIG_ADDR            (0x08)
#define BNO055_MAG_CONFIG_ADDR              (0x09)
#define BNO055_GYRO_CONFIG_ADDR             (0x0A)
#define BNO055_GYRO_MODE_CONFIG_ADDR        (0x0B)
#define BNO055_ACCEL_SLEEP_CONFIG_ADDR      (0x0C)
#define BNO055_GYRO_SLEEP_CONFIG_ADDR       (0x0D)
#define BNO055_MAG_SLEEP_CONFIG_ADDR        (0x0E)

/* Interrupt registers */
#define BNO055_INT_MASK_ADDR                (0x0F)
#define BNO055_INT_ADDR                     (0x10)
#define BNO055_ACCEL_ANY_MOTION_THRES_ADDR  (0x11)
#define BNO055_ACCEL_INTR_SETTINGS_ADDR     (0x12)
#define BNO055_ACCEL_HIGH_G_DURN_ADDR       (0x13)
#define BNO055_ACCEL_HIGH_G_THRES_ADDR      (0x14)
#define BNO055_ACCEL_NO_MOTION_THRES_ADDR   (0x15)
#define BNO055_ACCEL_NO_MOTION_SET_ADDR     (0x16)
#define BNO055_GYRO_INTR_SETING_ADDR        (0x17)
#define BNO055_GYRO_HIGHRATE_X_SET_ADDR     (0x18)
#define BNO055_GYRO_DURN_X_ADDR             (0x19)
#define BNO055_GYRO_HIGHRATE_Y_SET_ADDR     (0x1A)
#define BNO055_GYRO_DURN_Y_ADDR             (0x1B)
#define BNO055_GYRO_HIGHRATE_Z_SET_ADDR     (0x1C)
#define BNO055_GYRO_DURN_Z_ADDR             (0x1D)
#define BNO055_GYRO_ANY_MOTION_THRES_ADDR   (0x1E)
#define BNO055_GYRO_ANY_MOTION_SET_ADDR     (0x1F)
/* PAGE1 REGISTERS DEFINITION END */

/* Sensor Register-Seiten */
#define BNO055_PAGE_ZERO                    (0x00)
#define BNO055_PAGE_ONE                     (0x01)

/* Sensor Betriebsmodi */
#define BNO055_MODE_CONFIGMODE              (0x00)
#define BNO055_MODE_ACCONLY                 (0x01)
#define BNO055_MODE_MAGONLY                 (0x02)
#define BNO055_MODE_GYROONLY                (0x03)
#define BNO055_MODE_ACCMAG                  (0x04)
#define BNO055_MODE_ACCGYRO                 (0x05)
#define BNO055_MODE_MAGGYRO                 (0x06)
#define BNO055_MODE_AMG                     (0x07)
#define BNO055_MODE_IMU                     (0x08)
#define BNO055_MODE_COMPASS                 (0x09)
#define BNO055_MODE_M4G                     (0x0A)
#define BNO055_MODE_NDOF_FMC_OFF            (0x0B)
#define BNO055_MODE_NDOF                    (0x0C)

/* Sensor-ID */
#define BNO055_CHIP_ID                      (0xA0)

class BNO055
{
  public:
    BNO055(uint8_t addr,
      std::function<uint8_t(uint8_t, uint8_t)> readByte,
      std::function<bool(uint8_t, uint8_t, uint8_t*, uint8_t)> readBytes,
      std::function<bool(uint8_t, uint8_t, uint8_t)> writeByte) :
        _addr(addr),
        _readByte(readByte),
        _readBytes(readBytes),
        _writeByte(writeByte) {};

    uint8_t setup();
    uint8_t getMode();
    bool setMode(uint8_t bno_mode);
    bool getEuler(int16_t &heading, int16_t &roll, int16_t &pitch);
    bool getEuler(float &heading, float &roll, float &pitch);
    bool getLinAcc(int16_t &x, int16_t &y, int16_t &z);
    bool getLinAcc(float &x, float &y, float &z);
    uint8_t getCalibStat();
    bool getCalibData(int16_t data[11]);
    bool setCalibData(int16_t data[11]);
  private:
    uint8_t _addr;
    std::function<uint8_t(uint8_t, uint8_t)> _readByte;
    std::function<bool(uint8_t, uint8_t, uint8_t*, uint8_t)> _readBytes;
    std::function<bool(uint8_t, uint8_t, uint8_t)> _writeByte;
    uint8_t _mode = B11111111;
};

#endif
