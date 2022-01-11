
struct Mqtt_config{
    std::string ClientId;
    std::string ServerIp;
    std::string username;
    std::string password;
    int serverPort;
    std::string keepAliveTopic;
    std::string keepAliveMsg;
    std::string outSubstring;
    std::string outPubstring;
    std::string inSubstring;
    std::string inPubstring;
    std::string ONMsg;
    std::string OFFMsg;
    std::string ONTIMEMsg;
};
