#ifndef _ROS_SERVICE_SetupChannel_h
#define _ROS_SERVICE_SetupChannel_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace orfa2_msgs
{

static const char SETUPCHANNEL[] = "orfa2_msgs/SetupChannel";

  class SetupChannelRequest : public ros::Msg
  {
    public:
      uint8_t pin;
      uint8_t type;
      uint16_t rate_ms;
      enum { INPUT =  0 };
      enum { OUTPUT =  1 };
      enum { ANALOG_IN =  2 };

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->pin >> (8 * 0)) & 0xFF;
      offset += sizeof(this->pin);
      *(outbuffer + offset + 0) = (this->type >> (8 * 0)) & 0xFF;
      offset += sizeof(this->type);
      *(outbuffer + offset + 0) = (this->rate_ms >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->rate_ms >> (8 * 1)) & 0xFF;
      offset += sizeof(this->rate_ms);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      this->pin =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->pin);
      this->type =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->type);
      this->rate_ms =  ((uint16_t) (*(inbuffer + offset)));
      this->rate_ms |= ((uint16_t) (*(inbuffer + offset + 1))) << (8 * 1);
      offset += sizeof(this->rate_ms);
     return offset;
    }

    const char * getType(){ return SETUPCHANNEL; };
    const char * getMD5(){ return "adfd864a1ad8f847f0de28bbf46aec10"; };

  };

  class SetupChannelResponse : public ros::Msg
  {
    public:
      bool result;

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      union {
        bool real;
        uint8_t base;
      } u_result;
      u_result.real = this->result;
      *(outbuffer + offset + 0) = (u_result.base >> (8 * 0)) & 0xFF;
      offset += sizeof(this->result);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      union {
        bool real;
        uint8_t base;
      } u_result;
      u_result.base = 0;
      u_result.base |= ((uint8_t) (*(inbuffer + offset + 0))) << (8 * 0);
      this->result = u_result.real;
      offset += sizeof(this->result);
     return offset;
    }

    const char * getType(){ return SETUPCHANNEL; };
    const char * getMD5(){ return "eb13ac1f1354ccecb7941ee8fa2192e8"; };

  };

  class SetupChannel {
    public:
    typedef SetupChannelRequest Request;
    typedef SetupChannelResponse Response;
  };

}
#endif