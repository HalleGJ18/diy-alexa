#include <Arduino.h>
#include "IntentProcessor.h"
#include "Speaker.h"
#include <dotstar_wing.h>
#include <HTTPClient.h>
#include <string>
#include <ArduinoJson.h>

using namespace std;

IntentProcessor::IntentProcessor(Speaker *speaker)
{
    m_speaker = speaker;
}

IntentResult IntentProcessor::turnOnDevice(const Intent &intent)
{
    Serial.printf(
      "turnOnDevice: confidence=%.f%%; device_name=%s; trait_value=%s; trait_confidence=%.f%%\n",
      100 * intent.intent_confidence, intent.device_name.c_str(),
      intent.trait_value.c_str(), 100 * intent.trait_confidence
    );

    if (intent.intent_confidence < 0.4)
    {
        Serial.printf("Only %.f%% certain on intent\n", 100 * intent.intent_confidence);
        return FAILED;
    }
    if (intent.device_name.empty())
    {
        Serial.println("No device found");
        return FAILED;
    }
    if (intent.device_confidence < 0.4)
    {
        Serial.printf("Only %.f%% certain on device\n", 100 * intent.device_confidence);
        return FAILED;
    }
    if (intent.trait_value.empty())
    {
        Serial.println("Can't work out the intent action");
        return FAILED;
    }
    if (intent.trait_confidence < 0.4)
    {
        Serial.printf("Only %.f%% certain on trait\n", 100 * intent.trait_confidence);
        return FAILED;
    }
    bool is_turn_on = intent.trait_value == "on";

    // global device name "lights"
    if (intent.device_name == "lights")
    {
        for (const auto &dev_pin : m_device_to_pin)
        {
            digitalWrite(dev_pin.second, is_turn_on);
        }
    }
    else if (intent.device_name == "matrix") {
      if (is_turn_on)
        dotstar_wing_on();
      else
        dotstar_wing_off();
    }
    else
    {
        // see if the device name is something we know about
        if (m_device_to_pin.find(intent.device_name) == m_device_to_pin.end())
        {
            Serial.printf("Don't recognise the device '%s'\n", intent.device_name.c_str());
            return FAILED;
        }
        digitalWrite(m_device_to_pin[intent.device_name], is_turn_on);
    }
    // success!
    return SUCCESS;
}

IntentResult IntentProcessor::tellJoke()
{
    m_speaker->playRandomJoke();
    return SILENT_SUCCESS;
}

IntentResult IntentProcessor::life()
{
    m_speaker->playLife();
    return SILENT_SUCCESS;
}

IntentResult IntentProcessor::weather(const Intent &intent)
{

    // construct API url
    string coords = std::to_string(intent.location_lat); 
    coords += ",";
    coords += std::to_string(intent.location_long);

    string endpoint = "http://api.weatherapi.com/v1/current.json?key=18b95064517e4cc3934143406232105&q=" + coords + "&aqi=no";

    HTTPClient http;

    String url = String(endpoint.c_str());

    http.begin(url);
    String payload;

    // query weather API
    int httpCode = http.GET();
    if (httpCode > 0) {
 
        payload = http.getString();
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources

    // filter relevant information
    StaticJsonDocument<500> filter;
    filter["current"]["temp_c"] = true;
    filter["current"]["condition"]["text"] = true;

    StaticJsonDocument<500> weatherJson;
    deserializeJson(weatherJson, payload, DeserializationOption::Filter(filter));


    // output information

    string temp = weatherJson["current"]["temp_c"];
    string condition = weatherJson["current"]["condition"]["text"];

    Serial.printf("\n");
    Serial.printf("The weather in %s is %s C and %s\n", intent.location_name.c_str(), temp.c_str(), condition.c_str());
    Serial.printf("\n");

    m_speaker->playWeather(condition);

    return SILENT_SUCCESS;
}

IntentResult IntentProcessor::changeColour(const Intent &intent)
{
    Serial.printf(
      "changeColour: confidence=%.f%%; device_name=%s; trait_value=%s; trait_confidence=%.f%%\n",
      100 * intent.intent_confidence, intent.device_name.c_str(),
      intent.trait_value.c_str(), 100 * intent.trait_confidence
    );
    return SUCCESS;
}

IntentResult IntentProcessor::processIntent(const Intent &intent)
{
    Serial.printf(
      "processIntent: name=%s; confidence=%.f%%; trait_value=%s; trait_confidence=%.f%%\n",
      intent.intent_name.c_str(), 100 * intent.intent_confidence,
      intent.trait_value.c_str(), 100 * intent.trait_confidence
    );

    if (intent.text.empty())
    {
        Serial.println("No text recognised");
        return FAILED;
    }
    Serial.printf("I heard \"%s\"\n", intent.text.c_str());
    if (intent.intent_name.empty())
    {
        Serial.println("Can't work out what you want to do with the device...");
        return FAILED;
    }
    Serial.printf("Intent is %s\n", intent.intent_name.c_str());
    if (intent.intent_name == "Turn_on_device")
    {
        return turnOnDevice(intent);
    }
    if (intent.intent_name == "Tell_joke")
    {
        return tellJoke();
    }
    if (intent.intent_name == "Life")
    {
        return life();
    }
    if (intent.intent_name == "Change_colour")
    {
        return changeColour(intent);
    }
    if (intent.intent_name == "Weather")
    {
        return weather(intent);
    }

    return FAILED;
}

void IntentProcessor::addDevice(const std::string &name, int gpio_pin)
{
    m_device_to_pin.insert(std::make_pair(name, gpio_pin));
    pinMode(gpio_pin, OUTPUT);
}
