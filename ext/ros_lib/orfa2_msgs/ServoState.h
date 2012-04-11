#ifndef _ROS_orfa2_msgs_ServoState_h
#define _ROS_orfa2_msgs_ServoState_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"

namespace orfa2_msgs
{

  class ServoState : public ros::Msg
  {
    public:
      std_msgs::Header header;
      uint8_t servo_length;
      uint8_t st_servo;
      uint8_t * servo;
      uint8_t position_length;
      uint16_t st_position;
      uint16_t * position;
      uint8_t velocity_length;
      uint16_t st_velocity;
      uint16_t * velocity;

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      *(outbuffer + offset++) = servo_length;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      for( uint8_t i = 0; i < servo_length; i++){
      *(outbuffer + offset + 0) = (this->servo[i] >> (8 * 0)) & 0xFF;
      offset += sizeof(this->servo[i]);
      }
      *(outbuffer + offset++) = position_length;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      for( uint8_t i = 0; i < position_length; i++){
      *(outbuffer + offset + 0) = (this->position[i] >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->position[i] >> (8 * 1)) & 0xFF;
      offset += sizeof(this->position[i]);
      }
      *(outbuffer + offset++) = velocity_length;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      for( uint8_t i = 0; i < velocity_length; i++){
      *(outbuffer + offset + 0) = (this->velocity[i] >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->velocity[i] >> (8 * 1)) & 0xFF;
      offset += sizeof(this->velocity[i]);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      uint8_t servo_lengthT = *(inbuffer + offset++);
      if(servo_lengthT > servo_length)
        this->servo = (uint8_t*)realloc(this->servo, servo_lengthT * sizeof(uint8_t));
      offset += 3;
      servo_length = servo_lengthT;
      for( uint8_t i = 0; i < servo_length; i++){
      this->st_servo =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->st_servo);
        memcpy( &(this->servo[i]), &(this->st_servo), sizeof(uint8_t));
      }
      uint8_t position_lengthT = *(inbuffer + offset++);
      if(position_lengthT > position_length)
        this->position = (uint16_t*)realloc(this->position, position_lengthT * sizeof(uint16_t));
      offset += 3;
      position_length = position_lengthT;
      for( uint8_t i = 0; i < position_length; i++){
      this->st_position =  ((uint16_t) (*(inbuffer + offset)));
      this->st_position |= ((uint16_t) (*(inbuffer + offset + 1))) << (8 * 1);
      offset += sizeof(this->st_position);
        memcpy( &(this->position[i]), &(this->st_position), sizeof(uint16_t));
      }
      uint8_t velocity_lengthT = *(inbuffer + offset++);
      if(velocity_lengthT > velocity_length)
        this->velocity = (uint16_t*)realloc(this->velocity, velocity_lengthT * sizeof(uint16_t));
      offset += 3;
      velocity_length = velocity_lengthT;
      for( uint8_t i = 0; i < velocity_length; i++){
      this->st_velocity =  ((uint16_t) (*(inbuffer + offset)));
      this->st_velocity |= ((uint16_t) (*(inbuffer + offset + 1))) << (8 * 1);
      offset += sizeof(this->st_velocity);
        memcpy( &(this->velocity[i]), &(this->st_velocity), sizeof(uint16_t));
      }
     return offset;
    }

    const char * getType(){ return "orfa2_msgs/ServoState"; };
    const char * getMD5(){ return "1002909847e327566d9332591ea3a23a"; };

  };

}
#endif