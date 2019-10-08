#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"

#define RAD2DEG(x) ((x)*180./M_PI)

using namespace std;

void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{
    int count = scan->scan_time / scan->time_increment;
    ROS_INFO("I heard a laser scan %s[%d]:", scan->header.frame_id.c_str(), count);
    ROS_INFO("angle_range, %f, %f", RAD2DEG(scan->angle_min), RAD2DEG(scan->angle_max));
  
    for(int i = 0; i < count; i++) {
        float degree = RAD2DEG(scan->angle_min + scan->angle_increment * i);
		//if(degree > 100 && degree < 150)
	if (scan->ranges[i] < 1 && (degree > -100 && degree < -90))
		ROS_INFO("0: [%f, %f]", degree, scan->ranges[i]);

    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "scan_sub");
    ROS_INFO("SCAN SUB !!!!!!!!!!!!!!!!!!!!");
    ros::NodeHandle nb;

    ros::Subscriber sub = nb.subscribe<sensor_msgs::LaserScan>("/scan", 1000, scanCallback);

    ros::spin();

	return 0;
}

