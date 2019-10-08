#include "ros/ros.h"
#include "ros/ros.h"
#include "darknet_ros_msgs/BoundingBoxes.h"
#include "darknet_ros_msgs/BoundingBox.h"
#include "sensor_msgs/LaserScan.h"
#include "acc_logic_ros/MsgACC.h" 
#include <pthread.h>

#define RAD2DEG(x) ((x)*180./M_PI)

using namespace std;

string target = "cup";
bool detect_target = false;
int req_stop = 0;
acc_logic_ros::MsgACC msg;
ros::Publisher req_pub;

void msgCallback(const darknet_ros_msgs::BoundingBoxes::ConstPtr& msg)
{
    cout<<"Bouding Boxes (Class):" << msg->bounding_boxes[0].Class <<endl;
    if(msg->bounding_boxes[0].Class.compare(target) == 0) {
	cout<<">>>>TARGET (Class):" << msg->bounding_boxes[0].Class <<endl;
        detect_target = true;
    } else {
        detect_target = false;
    }
}

void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{
    int count = scan->scan_time / scan->time_increment;
   
    static float detect_degree;
    static float detect_range;
    req_stop = false;

    for(int i = 0; i < count; i++) {
        float degree = RAD2DEG(scan->angle_min + scan->angle_increment * i);
	if (scan->ranges[i] < 1 && (degree > -100 && degree < -90)) {
		if(detect_target == true) {
                     req_stop = 1;
                }
        }
    }
    if( req_stop == 1) {
         ROS_INFO(">>>>>>>>>>>>>>>>>>>>");
         ROS_INFO(">>>>>>STOP: [%f, %f]", detect_degree, detect_range);
         ROS_INFO(">>>>>>>>>>>>>>>>>>>>");

    }
}

void *test(void *data)
{
    ros::Rate loop_rate(20);
  while (ros::ok())
  {
    msg.stamp = ros::Time::now();        
    msg.acc_cmd  = req_stop;                 

    req_pub.publish(msg);        
	
    loop_rate.sleep();                  

  }
}


int main(int argc, char **argv)
{
    pthread_t thread_t;
    int status;

    ros::init(argc, argv, "acc_logic_node2");
    ros::NodeHandle nh;
    ros::Subscriber obj_sub = nh.subscribe("/darknet_ros/bounding_boxes",100,msgCallback);
    ros::Subscriber scan_sub = nh.subscribe<sensor_msgs::LaserScan>("/scan", 1000, scanCallback);

	
    req_pub = nh.advertise<acc_logic_ros::MsgACC>("acc_logic_msg", 100);

	
    if (pthread_create(&thread_t, NULL, test, 0) < 0)
    {
        printf("thread create error:");
        exit(0);
    }


    ros::spin();

    pthread_join(thread_t, (void **)&status);
    printf("Thread End %d\n", status);


	return 0;
}

