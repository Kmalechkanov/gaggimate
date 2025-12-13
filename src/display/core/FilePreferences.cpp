#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

class FilePreferences {
public:
    FilePreferences(const char* filename = "/settings.json") : filename(filename) {}

    bool begin(const char* key = nullptr, bool readOnly = false) {
        readOnlyMode = readOnly;
        load();      // load existing JSON into memory
        dirty = false;
        return true;
    }

    void end() {
        save();      // flush changes on end
    }

    void clear() {
        if (!readOnlyMode) {
            json.clear();
            dirty = true;
        }
    }

    void doSave() {
        if (dirty && !readOnlyMode) {
            File f = LittleFS.open(filename, "w");
            if (f) {
                serializeJson(json, f);
                f.close();
                dirty = false;
            }
        }
    }

    // ---- Integers ----
    void putInt(const String& key, int value) { json[key] = value; dirty = true; }
    int getInt(const String& key, int def = 0) { return json.containsKey(key) ? json[key].as<int>() : def; }

    // ---- Floats ----
    void putFloat(const String& key, float value) { json[key] = value; dirty = true; }
    float getFloat(const String& key, float def = 0.0) { return json.containsKey(key) ? json[key].as<float>() : def; }

    // ---- Doubles ----
    void putDouble(const String& key, double value) { json[key] = value; dirty = true; }
    double getDouble(const String& key, double def = 0.0) { return json.containsKey(key) ? json[key].as<double>() : def; }

    // ---- Booleans ----
    void putBool(const String& key, bool value) { json[key] = value; dirty = true; }
    bool getBool(const String& key, bool def = false) { return json.containsKey(key) ? json[key].as<bool>() : def; }

    // ---- Strings ----
    void putString(const String& key, const String& value) { json[key] = value; dirty = true; }
    String getString(const String& key, const String& def = "") { return json.containsKey(key) ? json[key].as<String>() : def; }

private:
    const char* filename;
    bool readOnlyMode = false;
    bool dirty = false;
    DynamicJsonDocument json{8 * 1024};

    void load() {
        if (LittleFS.exists(filename)) {
            File f = LittleFS.open(filename, "r");
            DeserializationError err = deserializeJson(json, f);
            f.close();
            if (err) {
                Serial.println("Failed to parse JSON, starting with empty");
                json.clear();
            }
        }
    }

    void save() {
        if (dirty && !readOnlyMode) {
            File f = LittleFS.open(filename, "w");
            if (f) {
                serializeJson(json, f);
                f.close();
                dirty = false;
            }
        }
    }
};
