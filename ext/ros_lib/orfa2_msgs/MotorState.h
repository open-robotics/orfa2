#ifndef _ROS_orfa2_msgs_MotorState_h
#define _ROS_orfa2_msgs_MotorState_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"

namespace orfa2_msgs
{

  class MotorState : public ros::Msg
  {
    public:
      std_msgs::Header header;
      int16_t pwm[2];
      int32_t veltocity[2];
      int64_t encoder[2];

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      unsigned char * pwm_val = (unsigned char *) this->pwm;
      for( uint8_t i = 0; i < 2; i++){
      union {
        int16_t real;
        uint16_t base;
      } u_pwmi;
      u_pwmi.real = this->pwm[i];
      *(outbuffer + offset + 0) = (u_pwmi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_pwmi.base >> (8 * 1)) & 0xFF;
      offset += sizeof(this->pwm[i]);
      }
      unsigned char * veltocity_val = (unsigned char *) this->veltocity;
      for( uint8_t i = 0; i < 2; i++){
      union {
        int32_t real;
        uint32_t base;
      } u_veltocityi;
      u_veltocityi.real = this->veltocity[i];
      *(outbuffer + offset + 0) = (u_veltocityi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_veltocityi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_veltocityi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_veltocityi.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->veltocity[i]);
      }
      unsigned char * encoder_val = (unsigned char *) this->encoder;
      for( uint8_t i = 0; i < 2; i++){
      union {
        int64_t real;
        uint32_t base;
      } u_encoderi;
      u_encoderi.real = this->encoder[i];
      *(outbuffer + offset + 0) = (u_encoderi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_encoderi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_encoderi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_encoderi.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->encoder[i]);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      uint8_t * pwm_val = (uint8_t*) this->pwm;
      for( uint8_t i = 0; i < 2; i++){
      union {
        int16_t real;
        uint16_t base;
      } u_pwmi;
      u_pwmi.base = 0;
      u_pwmi.base |= ((uint16_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_pwmi.base |= ((uint16_t) (*(inbuffer + offset + 1))) << (8 * 1);
      this->pwm[i] = u_pwmi.real;
      offset += sizeof(this->pwm[i]);
      }
      uint8_t * veltocity_val = (uint8_t*) this->veltocity;
      for( uint8_t i = 0; i < 2; i++){
      union {
        int32_t real;
        uint32_t base;
      } u_veltocityi;
      u_veltocityi.base = 0;
      u_veltocityi.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_veltocityi.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_veltocityi.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_veltocityi.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->veltocity[i] = u_veltocityi.real;
      offset += sizeof(this->veltocity[i]);
      }
      uint8_t * encoder_val = (uint8_t*) this->encoder;
      for( uint8_t i = 0; i < 2; i++){
      union {
        int64_t real;
        uint32_t base;
      } u_encoderi;
      u_encoderi.base = 0;
      u_encoderi.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_encoderi.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_encoderi.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_encoderi.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->encoder[i] = u_encoderi.real;
      offset += sizeof(this->encoder[i]);
      }
     return offset;
    }

    const char * getType(){ return "orfa2_msgs/MotorState"; };
    const char * getMD5(){ return "654f3567416d0d05b8a39239dc48ad8e"; };

  };

}
#endif