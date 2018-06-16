#ifndef _ROS_underwater_sensor_msgs_DVL_h
#define _ROS_underwater_sensor_msgs_DVL_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"

namespace underwater_sensor_msgs
{

  class DVL : public ros::Msg
  {
    public:
      std_msgs::Header header;
      const char* header_dvl;
      const char* date;
      float salinity;
      float temperature;
      float depth;
      float sound_speed;
      int32_t test;
      float wi_x_axis;
      float wi_y_axis;
      float wi_z_axis;
      float wi_error;
      const char* wi_status;
      float bi_x_axis;
      float bi_y_axis;
      float bi_z_axis;
      float bi_error;
      const char* bi_status;
      float ws_transverse;
      float ws_longitudinal;
      float ws_normal;
      const char* ws_status;
      float bs_transverse;
      float bs_longitudinal;
      float bs_normal;
      const char* bs_status;
      float we_east;
      float we_north;
      float we_upwards;
      const char* we_status;
      float be_east;
      float be_north;
      float be_upwards;
      const char* be_status;
      float wd_east;
      float wd_north;
      float wd_upwards;
      float wd_range;
      float wd_time;
      float bd_east;
      float bd_north;
      float bd_upwards;
      float bd_range;
      float bd_time;
      const char* raw_data;

    DVL():
      header(),
      header_dvl(""),
      date(""),
      salinity(0),
      temperature(0),
      depth(0),
      sound_speed(0),
      test(0),
      wi_x_axis(0),
      wi_y_axis(0),
      wi_z_axis(0),
      wi_error(0),
      wi_status(""),
      bi_x_axis(0),
      bi_y_axis(0),
      bi_z_axis(0),
      bi_error(0),
      bi_status(""),
      ws_transverse(0),
      ws_longitudinal(0),
      ws_normal(0),
      ws_status(""),
      bs_transverse(0),
      bs_longitudinal(0),
      bs_normal(0),
      bs_status(""),
      we_east(0),
      we_north(0),
      we_upwards(0),
      we_status(""),
      be_east(0),
      be_north(0),
      be_upwards(0),
      be_status(""),
      wd_east(0),
      wd_north(0),
      wd_upwards(0),
      wd_range(0),
      wd_time(0),
      bd_east(0),
      bd_north(0),
      bd_upwards(0),
      bd_range(0),
      bd_time(0),
      raw_data("")
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      uint32_t length_header_dvl = strlen(this->header_dvl);
      memcpy(outbuffer + offset, &length_header_dvl, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->header_dvl, length_header_dvl);
      offset += length_header_dvl;
      uint32_t length_date = strlen(this->date);
      memcpy(outbuffer + offset, &length_date, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->date, length_date);
      offset += length_date;
      offset += serializeAvrFloat64(outbuffer + offset, this->salinity);
      offset += serializeAvrFloat64(outbuffer + offset, this->temperature);
      offset += serializeAvrFloat64(outbuffer + offset, this->depth);
      offset += serializeAvrFloat64(outbuffer + offset, this->sound_speed);
      union {
        int32_t real;
        uint32_t base;
      } u_test;
      u_test.real = this->test;
      *(outbuffer + offset + 0) = (u_test.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_test.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_test.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_test.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->test);
      offset += serializeAvrFloat64(outbuffer + offset, this->wi_x_axis);
      offset += serializeAvrFloat64(outbuffer + offset, this->wi_y_axis);
      offset += serializeAvrFloat64(outbuffer + offset, this->wi_z_axis);
      offset += serializeAvrFloat64(outbuffer + offset, this->wi_error);
      uint32_t length_wi_status = strlen(this->wi_status);
      memcpy(outbuffer + offset, &length_wi_status, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->wi_status, length_wi_status);
      offset += length_wi_status;
      offset += serializeAvrFloat64(outbuffer + offset, this->bi_x_axis);
      offset += serializeAvrFloat64(outbuffer + offset, this->bi_y_axis);
      offset += serializeAvrFloat64(outbuffer + offset, this->bi_z_axis);
      offset += serializeAvrFloat64(outbuffer + offset, this->bi_error);
      uint32_t length_bi_status = strlen(this->bi_status);
      memcpy(outbuffer + offset, &length_bi_status, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->bi_status, length_bi_status);
      offset += length_bi_status;
      offset += serializeAvrFloat64(outbuffer + offset, this->ws_transverse);
      offset += serializeAvrFloat64(outbuffer + offset, this->ws_longitudinal);
      offset += serializeAvrFloat64(outbuffer + offset, this->ws_normal);
      uint32_t length_ws_status = strlen(this->ws_status);
      memcpy(outbuffer + offset, &length_ws_status, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->ws_status, length_ws_status);
      offset += length_ws_status;
      offset += serializeAvrFloat64(outbuffer + offset, this->bs_transverse);
      offset += serializeAvrFloat64(outbuffer + offset, this->bs_longitudinal);
      offset += serializeAvrFloat64(outbuffer + offset, this->bs_normal);
      uint32_t length_bs_status = strlen(this->bs_status);
      memcpy(outbuffer + offset, &length_bs_status, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->bs_status, length_bs_status);
      offset += length_bs_status;
      offset += serializeAvrFloat64(outbuffer + offset, this->we_east);
      offset += serializeAvrFloat64(outbuffer + offset, this->we_north);
      offset += serializeAvrFloat64(outbuffer + offset, this->we_upwards);
      uint32_t length_we_status = strlen(this->we_status);
      memcpy(outbuffer + offset, &length_we_status, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->we_status, length_we_status);
      offset += length_we_status;
      offset += serializeAvrFloat64(outbuffer + offset, this->be_east);
      offset += serializeAvrFloat64(outbuffer + offset, this->be_north);
      offset += serializeAvrFloat64(outbuffer + offset, this->be_upwards);
      uint32_t length_be_status = strlen(this->be_status);
      memcpy(outbuffer + offset, &length_be_status, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->be_status, length_be_status);
      offset += length_be_status;
      offset += serializeAvrFloat64(outbuffer + offset, this->wd_east);
      offset += serializeAvrFloat64(outbuffer + offset, this->wd_north);
      offset += serializeAvrFloat64(outbuffer + offset, this->wd_upwards);
      offset += serializeAvrFloat64(outbuffer + offset, this->wd_range);
      offset += serializeAvrFloat64(outbuffer + offset, this->wd_time);
      offset += serializeAvrFloat64(outbuffer + offset, this->bd_east);
      offset += serializeAvrFloat64(outbuffer + offset, this->bd_north);
      offset += serializeAvrFloat64(outbuffer + offset, this->bd_upwards);
      offset += serializeAvrFloat64(outbuffer + offset, this->bd_range);
      offset += serializeAvrFloat64(outbuffer + offset, this->bd_time);
      uint32_t length_raw_data = strlen(this->raw_data);
      memcpy(outbuffer + offset, &length_raw_data, sizeof(uint32_t));
      offset += 4;
      memcpy(outbuffer + offset, this->raw_data, length_raw_data);
      offset += length_raw_data;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      uint32_t length_header_dvl;
      memcpy(&length_header_dvl, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_header_dvl; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_header_dvl-1]=0;
      this->header_dvl = (char *)(inbuffer + offset-1);
      offset += length_header_dvl;
      uint32_t length_date;
      memcpy(&length_date, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_date; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_date-1]=0;
      this->date = (char *)(inbuffer + offset-1);
      offset += length_date;
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->salinity));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->temperature));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->depth));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->sound_speed));
      union {
        int32_t real;
        uint32_t base;
      } u_test;
      u_test.base = 0;
      u_test.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_test.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_test.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_test.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->test = u_test.real;
      offset += sizeof(this->test);
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->wi_x_axis));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->wi_y_axis));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->wi_z_axis));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->wi_error));
      uint32_t length_wi_status;
      memcpy(&length_wi_status, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_wi_status; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_wi_status-1]=0;
      this->wi_status = (char *)(inbuffer + offset-1);
      offset += length_wi_status;
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bi_x_axis));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bi_y_axis));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bi_z_axis));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bi_error));
      uint32_t length_bi_status;
      memcpy(&length_bi_status, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_bi_status; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_bi_status-1]=0;
      this->bi_status = (char *)(inbuffer + offset-1);
      offset += length_bi_status;
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->ws_transverse));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->ws_longitudinal));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->ws_normal));
      uint32_t length_ws_status;
      memcpy(&length_ws_status, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_ws_status; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_ws_status-1]=0;
      this->ws_status = (char *)(inbuffer + offset-1);
      offset += length_ws_status;
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bs_transverse));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bs_longitudinal));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bs_normal));
      uint32_t length_bs_status;
      memcpy(&length_bs_status, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_bs_status; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_bs_status-1]=0;
      this->bs_status = (char *)(inbuffer + offset-1);
      offset += length_bs_status;
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->we_east));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->we_north));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->we_upwards));
      uint32_t length_we_status;
      memcpy(&length_we_status, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_we_status; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_we_status-1]=0;
      this->we_status = (char *)(inbuffer + offset-1);
      offset += length_we_status;
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->be_east));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->be_north));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->be_upwards));
      uint32_t length_be_status;
      memcpy(&length_be_status, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_be_status; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_be_status-1]=0;
      this->be_status = (char *)(inbuffer + offset-1);
      offset += length_be_status;
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->wd_east));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->wd_north));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->wd_upwards));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->wd_range));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->wd_time));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bd_east));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bd_north));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bd_upwards));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bd_range));
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bd_time));
      uint32_t length_raw_data;
      memcpy(&length_raw_data, (inbuffer + offset), sizeof(uint32_t));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_raw_data; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_raw_data-1]=0;
      this->raw_data = (char *)(inbuffer + offset-1);
      offset += length_raw_data;
     return offset;
    }

    const char * getType(){ return "underwater_sensor_msgs/DVL"; };
    const char * getMD5(){ return "c455f331872552e620e26fc7caad335d"; };

  };

}
#endif