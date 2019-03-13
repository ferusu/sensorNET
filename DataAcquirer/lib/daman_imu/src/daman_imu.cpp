/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: DataAquirer                                    */
/*   Author: Ferusu                                             */
/*   Date: 13/03/2019                                           */
/*   Module: daman_imu   File: daman_imu.cpp                    */
/*   Brief:                                                     */
/****************************************************************/


/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/



/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/

#include <daman_imu.h>
#include <dahal_i2c.h>

/******************************************************************/
/*                     Definition of objects                      */
/******************************************************************/



/******************************************************************/
/*            Definition of local symbolic constants              */
/******************************************************************/

// MPU6050 Slave Device Address
#define IMU_ADDRESS                       (0x68)
// MPU6050 few configuration register addresses
#define IMU_REG_SMPLRT_DIV                (0x19)
#define IMU_REG_PWR_MGMT_1                (0x6B)
#define IMU_REG_PWR_MGMT_2                (0x6C)
#define IMU_REG_CONFIG                    (0x1A)
#define IMU_REG_GYRO_CONFIG               (0x1B)
#define IMU_REG_ACCEL_CONFIG              (0x1C)
#define IMU_REG_FIFO_EN                   (0x23)
#define IMU_REG_INT_ENABLE                (0x38)
#define IMU_REG_SIGNAL_PATH_RESET         (0x68)
#define IMU_REG_USER_CTRL                 (0x6A)
#define IMU_REG_ACCEL_XOUT_H              (0x3B)

// MPU6050 few configuration values   
#define IMU_REG_SMPLRT_DIV_VALUE          (0x07)
#define IMU_REG_PWR_MGMT_1_VALUE          (0x01)
#define IMU_REG_PWR_MGMT_2_VALUE          (0x00)
#define IMU_REG_CONFIG_VALUE              (0x00)
#define IMU_REG_GYRO_CONFIG_VALUE         (0x00)
#define IMU_REG_ACCEL_CONFIG_VALUE        (0x00)
#define IMU_REG_FIFO_EN_VALUE             (0x00)
#define IMU_REG_INT_ENABLE_VALUE          (0x01)
#define IMU_REG_SIGNAL_PATH_RESET_VALUE   (0x00)
#define IMU_REG_USER_CTRL_VALUE           (0x00)

/*****************************************************************/
/*            Typedef of structures and enumerations             */
/*****************************************************************/



/*****************************************************************/
/*                Private Constant Declaration                   */
/*****************************************************************/

const uint8_t imuRegisters[11] = 
{
IMU_REG_SMPLRT_DIV,      
IMU_REG_PWR_MGMT_1,      
IMU_REG_PWR_MGMT_2,       
IMU_REG_CONFIG,           
IMU_REG_GYRO_CONFIG,      
IMU_REG_ACCEL_CONFIG,     
IMU_REG_FIFO_EN,          
IMU_REG_INT_ENABLE,       
IMU_REG_SIGNAL_PATH_RESET,
IMU_REG_USER_CTRL,
IMU_REG_ACCEL_XOUT_H    
};
const uint8_t imuConfigValues[10] = 
{
IMU_REG_SMPLRT_DIV_VALUE,
IMU_REG_PWR_MGMT_1_VALUE,
IMU_REG_PWR_MGMT_2_VALUE,
IMU_REG_CONFIG_VALUE,
IMU_REG_GYRO_CONFIG_VALUE,
IMU_REG_ACCEL_CONFIG_VALUE,
IMU_REG_FIFO_EN_VALUE,
IMU_REG_INT_ENABLE_VALUE,
IMU_REG_SIGNAL_PATH_RESET_VALUE,
IMU_REG_USER_CTRL_VALUE 
};

/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/

bool damanImuInitialized = false;

/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/



/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/



/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/

void DamanImuInit(void)
{
    uint8_t configIndex;
    if(!damanImuInitialized)
    {
        delay(150);
        damanImuInitialized = true;
        for (configIndex = 0; configIndex<10; configIndex++)
        {
           DahalI2cWrite(IMU_ADDRESS, imuRegisters[configIndex], imuConfigValues[configIndex]);
        }
    }
}

void DamanImuGet(imuData_t *imuDataOutput)
{
    if(damanImuInitialized)
    {
        DahalI2cRead(IMU_ADDRESS,imuRegisters[10],(uint8_t *)imuDataOutput,sizeof(*imuDataOutput));
    }
}

void DamanImuActivateHardwareFilter (void)
{
    /* Still not implemented */
}

void DamanImuParseData (imuData_t *imuDataOutput)
{
    /* Still not implemented */
}