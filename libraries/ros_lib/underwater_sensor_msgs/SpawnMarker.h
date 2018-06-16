#ifndef _ROS_SERVICE_SpawnMarker_h
#define _ROS_SERVICE_SpawnMarker_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "visualization_msgs/Marker.h"

namespace underwater_sensor_msgs
{

static const char SPAWNMARKER[] = "underwater_sensor_msgs/SpawnMarker";

  class SpawnMarkerRequest : public ros::Msg
  {
    public:
      visualization_msgs::Marker marker;

    SpawnMarkerRequest():
      marker()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->marker.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->marker.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return SPAWNMARKER; };
    const char * getMD5(){ return "227a89e3d0c778686f83f2dd7b0caac4"; };

  };

  class SpawnMarkerResponse : public ros::Msg
  {
    public:
      bool success;
      const char* status_message;

    SpawnMarkerResponse():
      success(0),
      status_message("")
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      union {
        bool real;
        uint8_t base;
      } u_success;
      u_success.real = this->success;
      *(outbuffer + offset + 0) = (u_success.base >> (8 * 0)) & 0xFF;
      offset += sizeof(this->success);
      uint32_t length_status_message = strlen(this->status_message);
      memcpy(outbuffer + offset, &length_status_message, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->status_message, length_status_message);
      offset += length_status_message;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      union {
        bool real;
        uint8_t base;
      } u_success;
      u_success.base = 0;
      u_success.base |= ((uint8_t) (*(inbuffer + offset + 0))) << (8 * 0);
      this->success = u_success.real;
      offset += sizeof(this->success);
      uint32_t length_status_message;
      memcpy(&length_status_message, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_status_message; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_status_message-1]=0;
      this->status_message = (char *)(inbuffer + offset-1);
      offset += length_status_message;
     return offset;
    }

    const char * getType(){ return SPAWNMARKER; };
    const char * getMD5(){ return "2ec6f3eff0161f4257b808b12bc830c2"; };

  };

  class SpawnMarker {
    public:
    typedef SpawnMarkerRequest Request;
    typedef SpawnMarkerResponse Response;
  };

}
#endif
