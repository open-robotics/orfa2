#ifndef _ROS_orfa2_msgs_Analog_h
#define _ROS_orfa2_msgs_Analog_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"

namespace orfa2_msgs
{

  class Analog : public ros::Msg
  {
    public:
      std_msgs::Header header;
      uint8_t pin_length;
      uint8_t st_pin;
      uint8_t * pin;
      uint8_t value_length;
      uint16_t st_value;
      uint16_t * value;

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      *(outbuffer + offset++) = pin_length;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      for( uint8_t i = 0; i < pin_length; i++){
      *(outbuffer + offset + 0) = (this->pin[i] >> (8 * 0)) & 0xFF;
      offset += sizeof(this->pin[i]);
      }
      *(outbuffer + offset++) = value_length;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      for( uint8_t i = 0; i < value_length; i++){
      *(outbuffer + offset + 0) = (this->value[i] >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->value[i] >> (8 * 1)) & 0xFF;
      offset += sizeof(this->value[i]);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      uint8_t pin_lengthT = *(inbuffer + offset++);
      if(pin_lengthT > pin_length)
        this->pin = (uint8_t*)realloc(this->pin, pin_lengthT * sizeof(uint8_t));
      offset += 3;
      pin_length = pin_lengthT;
      for( uint8_t i = 0; i < pin_length; i++){
      this->st_pin =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->st_pin);
        memcpy( &(this->pin[i]), &(this->st_pin), sizeof(uint8_t));
      }
      uint8_t value_lengthT = *(inbuffer + offset++);
      if(value_lengthT > value_length)
        this->value = (uint16_t*)realloc(this->value, value_lengthT * sizeof(uint16_t));
      offset += 3;
      value_length = value_lengthT;
      for( uint8_t i = 0; i < value_length; i++){
      this->st_value =  ((uint16_t) (*(inbuffer + offset)));
      this->st_value |= ((uint16_t) (*(inbuffer + offset + 1))) << (8 * 1);
      offset += sizeof(this->st_value);
        memcpy( &(this->value[i]), &(this->st_value), sizeof(uint16_t));
      }
     return offset;
    }

    const char * getType(){ return "orfa2_msgs/Analog"; };
    const char * getMD5(){ return "6799252f8eb6ff1d3523cddf9d8558b9"; };

  };

}
#endif