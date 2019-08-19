
class wemos_WiFi{
public:
    void connect();
    void connect(const char* _nodeName);
    void update();
private:
};
extern wemos_WiFi wemosWiFi;