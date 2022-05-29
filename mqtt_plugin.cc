#include <time.h>
#include <vector>


//#include "../../trunk-recorder/global_structs.h"
//#include "../../trunk-recorder/recorders/recorder.h"
#include <trunk-recorder/source.h>
//#include "../../trunk-recorder/systems/system.h"
#include <trunk-recorder/plugin_manager/plugin_api.h>
#include <trunk-recorder/gr_blocks/decoder_wrapper.h>
#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS   
#include <boost/property_tree/json_parser.hpp>
#include <boost/log/trivial.hpp>


#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <mqtt/client.h>

typedef struct stat_plugin_t stat_plugin_t;

struct stat_plugin_t {
  std::vector<Source *> sources;
  std::vector<System *> systems;
  std::vector<Call *> calls;
  Config* config;
};


using namespace std;

const int QOS = 1;

const auto TIMEOUT = std::chrono::seconds(10);

class Mqtt_Status : public Plugin_Api, public virtual mqtt::callback, public virtual mqtt::iaction_listener {

  time_t reconnect_time;
  bool m_reconnect;
  bool m_open;
  bool m_done;
  bool m_config_sent;


  std::vector<Source *> sources;
  std::vector<System *> systems;
  std::vector<Call *> calls;
  Config* config;
  std::string mqtt_broker;
  std::string username;
  std::string password;
  std::string topic;
  mqtt::async_client *client;

  protected:
	void on_failure(const mqtt::token& tok) override {
		cout << "\tListener failure for token: "
			<< tok.get_message_id() << endl;
	}

	void on_success(const mqtt::token& tok) override {
		cout << "\tListener success for token: "
			<< tok.get_message_id() << endl;
	}

public:

	void connection_lost(const string& cause) {
		cout << "\nConnection lost" << endl;
		if (!cause.empty())
			cout << "\tcause: " << cause << endl;
	}

	void delivery_complete(mqtt::delivery_token_ptr tok)  {
		cout << "\tDelivery complete for token: "
			<< (tok ? tok->get_message_id() : -1) << endl;
	}


  /**
 * The telemetry client connects to a WebFocket server and sends a message every
 * second containing an integer count. This example can be used as the basis for
 * programs where a client connects and pushes data for logging, stress/load
 * testing, etc.
 */
  int system_rates(std::vector<System *> systems, float timeDiff) {
/*
    boost::property_tree::ptree node;

    
    node.push_back(std::make_pair("", systems.front()->get_stats_current(timeDiff)));


    std::stringstream stats_str;
    boost::property_tree::write_json(stats_str, node);

    try {
          cout << "\nSending message..." << endl;
          mqtt::message_ptr pubmsg = mqtt::make_message(this->topic + "/decode", stats_str.str());
          pubmsg->set_qos(QOS);
          client->publish(pubmsg)->wait_for(TIMEOUT);
          cout << "  ...OK" << endl;

    }
        catch (const mqtt::exception& exc) {
          cerr << exc.what() << endl;

    }*/
    return 0;
  }

  


  int calls_active(std::vector<Call *> calls) {

    int num = calls.size();
    std::string s = std::to_string(num);
    try {
          cout << "\nSending message..." << endl;
          mqtt::message_ptr pubmsg = mqtt::make_message(this->topic + "/calls", s);
          pubmsg->set_qos(QOS);
          client->publish(pubmsg)->wait_for(TIMEOUT);
          cout << "  ...OK" << endl;

    }
        catch (const mqtt::exception& exc) {
          cerr << exc.what() << endl;

    }
    return 0;
  }


  void open_connection() {
        const char* LWT_PAYLOAD = "Last will and testament.";
    // set up access channels to only log interesting things
    client = new mqtt::async_client(this->mqtt_broker , "test", "./store");  

	mqtt::connect_options connOpts;

     if ((this->username != "" ) && (this->password != "")) {
        cout << "\nsetting username and password..." << endl;
         connOpts = mqtt::connect_options_builder().clean_session().user_name(this->username).password(this->password).will(mqtt::message("final", LWT_PAYLOAD, QOS))
		.finalize();;
      } else {
        connOpts = mqtt::connect_options_builder().clean_session().will(mqtt::message("final", LWT_PAYLOAD, QOS))
		.finalize();;
      }
      

mqtt::ssl_options sslopts;
    sslopts.set_verify(false);
    sslopts.set_enable_server_cert_auth(false);
    connOpts.set_ssl(sslopts);
    	try {
          cout << "\nConnecting..." << endl;
          mqtt::token_ptr conntok = client->connect(connOpts);
          cout << "Waiting for the connection..." << endl;
          conntok->wait();
          cout << "  ...OK" << endl;
          m_open = true;
        }
        catch (const mqtt::exception& exc) {
          cerr << exc.what() << endl;

        }
  }

 
  int init(Config *config, std::vector<Source *> sources, std::vector<System *> systems) {

    this->sources = sources;
    this->systems = systems;
    this->config = config;

    return 0;
  }

  int start() {
    open_connection();
    return 0;
  }





    // Factory method
    static boost::shared_ptr<Mqtt_Status> create() {
        return boost::shared_ptr<Mqtt_Status>(
            new Mqtt_Status()
        );
    }

  int parse_config(boost::property_tree::ptree &cfg) {

    this->mqtt_broker = cfg.get<std::string>("broker", "tcp://localhost:1883");
    BOOST_LOG_TRIVIAL(info) << "MQTT Broker: " << this->mqtt_broker;
    this->topic = cfg.get<std::string>("topic", "");
    BOOST_LOG_TRIVIAL(info) << "MQTT Topic: " << this->topic;
    this->username = cfg.get<std::string>("username", "");
    BOOST_LOG_TRIVIAL(info) << "MQTT Broker Username: " << this->username;
    this->password = cfg.get<std::string>("password", "");



    return 0;
  }


};

BOOST_DLL_ALIAS(
   Mqtt_Status::create, // <-- this function is exported with...
    create_plugin                               // <-- ...this alias name
)
