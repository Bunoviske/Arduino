#include <ros.h>
#include <geometry_msgs/Twist.h>


ros::NodeHandle nh;

geometry_msgs::Twist vel;

ros::Publisher p("/turtle1/cmd_vel", &vel);

void setup()
{ 
  nh.initNode();
  nh.advertise(p);
}

void loop()
{  

  vel.linear.x = 10;
  vel.linear.y = 0;
  vel.linear.z = 10;

  vel.angular.x = 0;
  vel.angular.y = 0;
  vel.angular.z = 16;
 
  p.publish(&vel);
  nh.spinOnce();
  delay(10);
}

