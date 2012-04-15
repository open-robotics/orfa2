#ifndef _ROS_orfa2_msgs_Motor_h
#define _ROS_orfa2_msgs_Motor_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"

namespace orfa2_msgs
{

  class Motor : public ros::Msg
  {
    public:
      std_msgs::Header header;
      int32_t speed[2];

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      unsigned char * speed_val = (unsigned char *) this->speed;
      for( uint8_t i = 0; i < 2; i++){
      union {
        int32_t real;
        uint32_t base;
      } u_speedi;
      u_speedi.real = this->speed[i];
      *(outbuffer + offset + 0) = (u_speedi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_speedi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_speedi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_speedi.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->speed[i]);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      uint8_t * speed_val = (uint8_t*) this->speed;
      for( uint8_t i = 0; i < 2; i++){
      union {
        int32_t real;
        uint32_t base;
      } u_speedi;
      u_speedi.base = 0;
      u_speedi.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_speedi.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_speedi.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_speedi.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->speed[i] = u_speedi.real;
      offset += sizeof(this->speed[i]);
      }
     return offset;
    }

    const char * getType(){ return "orfa2_msgs/Motor"; };
    const char * getMD5(){ return "d48c73df509ea21703304eafc4917ffa"; };

  };

}
#endif