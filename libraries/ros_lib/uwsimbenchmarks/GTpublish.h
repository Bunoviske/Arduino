#ifndef _ROS_SERVICE_GTpublish_h
#define _ROS_SERVICE_GTpublish_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace uwsimbenchmarks
{

static const char GTPUBLISH[] = "uwsimbenchmarks/GTpublish";

  class GTpublishRequest : public ros::Msg
  {
    public:
      uint8_t groundTruth_length;
      float st_groundTruth;
      float * groundTruth;

    GTpublishRequest():
      groundTruth_length(0), groundTruth(NULL)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      *(outbuffer + offset++) = groundTruth_length;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      for( uint8_t i = 0; i < groundTruth_length; i++){
      union {
        float real;
        uint32_t base;
      } u_groundTruthi;
      u_groundTruthi.real = this->groundTruth[i];
      *(outbuffer + offset + 0) = (u_groundTruthi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_groundTruthi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_groundTruthi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_groundTruthi.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->groundTruth[i]);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      uint8_t groundTruth_lengthT = *(inbuffer + offset++);
      if(groundTruth_lengthT > groundTruth_length)
        this->groundTruth = (float*)realloc(this->groundTruth, groundTruth_lengthT * sizeof(float));
      offset += 3;
      groundTruth_length = groundTruth_lengthT;
      for( uint8_t i = 0; i < groundTruth_length; i++){
      union {
        float real;
        uint32_t base;
      } u_st_groundTruth;
      u_st_groundTruth.base = 0;
      u_st_groundTruth.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_groundTruth.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_groundTruth.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_groundTruth.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->st_groundTruth = u_st_groundTruth.real;
      offset += sizeof(this->st_groundTruth);
        memcpy( &(this->groundTruth[i]), &(this->st_groundTruth), sizeof(float));
      }
     return offset;
    }

    const char * getType(){ return GTPUBLISH; };
    const char * getMD5(){ return "0b412ada447d72a7ae228bb70b64a2cb"; };

  };

  class GTpublishResponse : public ros::Msg
  {
    public:

    GTpublishResponse()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
     return offset;
    }

    const char * getType(){ return GTPUBLISH; };
    const char * getMD5(){ return "d41d8cd98f00b204e9800998ecf8427e"; };

  };

  class GTpublish {
    public:
    typedef GTpublishRequest Request;
    typedef GTpublishResponse Response;
  };

}
#endif
