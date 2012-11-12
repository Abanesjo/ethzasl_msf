/*
 * msf_masterROS.hpp
 *
 *  Created on: Nov 12, 2012
 *      Author: slynen
 */

#ifndef MSF_MEASUREMENTROS_HPP_
#define MSF_MEASUREMENTROS_HPP_

#include <ros/ros.h>
#include <msf_core/MSF_CoreConfig.h>
#include <msf_core/msf_sensormanager.hpp>

#include <ros/ros.h>
#include <dynamic_reconfigure/server.h>

namespace msf_core{

typedef dynamic_reconfigure::Server<msf_core::MSF_CoreConfig> ReconfigureServer;

//abstract class defining user configurable calculations for the msf_core with ROS interfaces
struct MSF_SensorManagerROS:public msf_core::MSF_SensorManager{
protected:
	/// dynamic reconfigure config
	msf_core::MSF_CoreConfig config_;
private:
	// dynamic reconfigure
	ReconfigureServer *reconfServer_;
	typedef boost::function<void(msf_core::MSF_CoreConfig& config, uint32_t level)> CallbackType;
	std::vector<CallbackType> callbacks_;

public:

	/// registers dynamic reconfigure callbacks
	template<class T>
	void registerCallback(void(T::*cb_func)(msf_core::MSF_CoreConfig& config, uint32_t level), T* p_obj)
	{
		callbacks_.push_back(boost::bind(cb_func, p_obj, _1, _2));
	}

	MSF_SensorManagerROS(){
		reconfServer_ = new ReconfigureServer(ros::NodeHandle("~"));
		ReconfigureServer::CallbackType f = boost::bind(&MSF_SensorManagerROS::Config, this, _1, _2);
		reconfServer_->setCallback(f);
		//register dyn config list
		registerCallback(&MSF_SensorManagerROS::DynConfig, this);
	}

	~MSF_SensorManagerROS(){
		delete reconfServer_;
	}

	/// gets called by dynamic reconfigure and calls all registered callbacks in callbacks_
	void Config(msf_core::MSF_CoreConfig &config, uint32_t level)
	{
		for (std::vector<CallbackType>::iterator it = callbacks_.begin(); it != callbacks_.end(); it++)
			(*it)(config, level);
	}

	/// handles the dynamic reconfigure for ssf_core
	virtual void DynConfig(msf_core::MSF_CoreConfig &config, uint32_t level)
	{
		config_ = config;
	}


	virtual bool getParam_fixed_bias(){
		return config_.fixed_bias;
	}
	virtual double getParam_noise_acc(){
		return config_.noise_acc;
	}
	virtual double getParam_noise_accbias(){
		return config_.noise_accbias;
	}
	virtual double getParam_noise_gyr(){
		return config_.noise_gyr;
	}
	virtual double getParam_noise_gyrbias(){
		return config_.noise_gyrbias;
	}


};

}
#endif /* MSF_MEASUREMENTROS_HPP_ */