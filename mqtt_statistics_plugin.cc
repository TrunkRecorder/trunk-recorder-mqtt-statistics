#include <time.h>
#include <vector>

#include <trunk-recorder/source.h>
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

struct stat_plugin_t
{
  std::vector<Source *> sources;
  std::vector<System *> systems;
  std::vector<Call *> calls;
  Config *config;
};

using namespace std;

const int QOS = 1;

const auto TIMEOUT = std::chrono::seconds(10);

class Mqtt_Statistics : public Plugin_Api, public virtual mqtt::callback, public virtual mqtt::iaction_listener
{

  time_t reconnect_time;
  bool m_reconnect;
  bool m_open;
  bool m_done;
  bool m_config_sent;

  std::vector<Source *> sources;
  std::vector<System *> systems;
  std::vector<Call *> calls;
  Config *config;
  std::string mqtt_broker;
  std::string username;
  std::string password;
  std::string topic;
  mqtt::async_client *client;

protected:
  void on_failure(const mqtt::token &tok) override
  {
    BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin - \tListener failure for token: "
         << tok.get_message_id() << endl;
  }

  void on_success(const mqtt::token &tok) override
  {
    BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin - \tListener success for token: "
         << tok.get_message_id() << endl;
  }

public:
  void connection_lost(const string &cause) override
  {
    BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin - \tConnection lost" << endl;
    if (!cause.empty())
      BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin - \tcause: " << cause << endl;
  }

  void delivery_complete(mqtt::delivery_token_ptr tok) override
  {
    BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin - \tDelivery complete for token: "
         << (tok ? tok->get_message_id() : -1) << endl;
  }

  /**
   * The telemetry client connects to a WebFocket server and sends a message every
   * second containing an integer count. This example can be used as the basis for
   * programs where a client connects and pushes data for logging, stress/load
   * testing, etc.
   */
  int system_rates(std::vector<System *> systems, float timeDiff) override
  {
        boost::property_tree::ptree node;
        node.push_back(std::make_pair("", systems.front()->get_stats_current(timeDiff)));
        std::stringstream stats_str;
        boost::property_tree::write_json(stats_str, node);

        try {
              mqtt::message_ptr pubmsg = mqtt::make_message(this->topic + "/decode", stats_str.str());
              pubmsg->set_qos(QOS);
              client->publish(pubmsg); //->wait_for(TIMEOUT);
        }
            catch (const mqtt::exception& exc) {
              BOOST_LOG_TRIVIAL(error) << "MQTT Statistics Plugin - " <<exc.what() << endl;
        }
    return 0;
  }

  int calls_active(std::vector<Call *> calls) override
  {

    int num = calls.size();
    std::string s = std::to_string(num);
    try
    {
      mqtt::message_ptr pubmsg = mqtt::make_message(this->topic + "/calls", s);
      pubmsg->set_qos(QOS);
      client->publish(pubmsg); //->wait_for(TIMEOUT);
    }
    catch (const mqtt::exception &exc)
    {
      BOOST_LOG_TRIVIAL(error) << "MQTT Statistics Plugin - " <<exc.what() << endl;
    }
    return 0;
  }

  void open_connection()
  {
    const char *LWT_PAYLOAD = "Last will and testament.";
    // set up access channels to only log interesting things
    client = new mqtt::async_client(this->mqtt_broker, "tr-statistics", "./store");

    mqtt::connect_options connOpts;

    if ((this->username != "") && (this->password != ""))
    {
      BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin - \tsetting username and password..." << endl;
      connOpts = mqtt::connect_options_builder().clean_session().user_name(this->username).password(this->password).will(mqtt::message("final", LWT_PAYLOAD, QOS)).finalize();
      ;
    }
    else
    {
      connOpts = mqtt::connect_options_builder().clean_session().will(mqtt::message("final", LWT_PAYLOAD, QOS)).finalize();
      ;
    }

    mqtt::ssl_options sslopts;
    sslopts.set_verify(false);
    sslopts.set_enable_server_cert_auth(false);
    connOpts.set_ssl(sslopts);
    connOpts.set_automatic_reconnect(10, 40); //This seems to be a block reconnect
    try
    {
      BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin - \tConnecting...";
      mqtt::token_ptr conntok = client->connect(connOpts);
      BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin - \tWaiting for the connection...";
      conntok->wait();
      BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin - \t ...OK";
      m_open = true;
    }
    catch (const mqtt::exception &exc)
    {
      BOOST_LOG_TRIVIAL(error) << "MQTT Statistics Plugin - " <<exc.what() << endl;
    }
  }

  int start() override
  {
    open_connection();
    return 0;
  }

  // Factory method
  static boost::shared_ptr<Mqtt_Statistics> create()
  {
    return boost::shared_ptr<Mqtt_Statistics>(
        new Mqtt_Statistics());
  }

  int parse_config(boost::property_tree::ptree &cfg) override
  {

    this->mqtt_broker = cfg.get<std::string>("broker", "tcp://localhost:1883");
    BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin Broker: " << this->mqtt_broker;
    this->topic = cfg.get<std::string>("topic", "");
    if (this->topic.back() == '/') {
      this->topic.erase(this->topic.size() - 1);
    }
    BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin Topic: " << this->topic;
    this->username = cfg.get<std::string>("username", "");
    BOOST_LOG_TRIVIAL(info) << " MQTT Statistics Plugin Broker Username: " << this->username;
    this->password = cfg.get<std::string>("password", "");

    return 0;
  }
};

BOOST_DLL_ALIAS(
    Mqtt_Statistics::create, // <-- this function is exported with...
    create_plugin            // <-- ...this alias name
)
