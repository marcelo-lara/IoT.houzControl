# Setup
- link core libraries
## server
mklink /J 00_Server\src\wemos_WiFi src\wemos_WiFi
mklink /J 00_Server\src\HouzCore src\HouzCore

## office
mklink /J 01_Office\src\wemos_WiFi src\wemos_WiFi
mklink /J 01_Office\src\HouzCore src\HouzCore


# Core 
## Abstract



### 

### Device
the core element is a device. 

    - everything is a device.
    - a device could have nested devices
    - the node itself is a device


 

## Link
### RF | nrf24l01+

### API | AsyncWeb

# Nodes
A node is a hardware board with one or more devices attached to it

## 01 Office
- ceiling light
- air conditioner
    - status
    - temperature
    - fan
- enviroment sensor
    - bme280
    - lightLevel
- wall switch
    - switch
    - status indicator

## 02 Suite
- ceiling light
- fan
    - status
    - fanSpeed
- air conditioner
    - status
    - temperature
    - fan
- enviroment sensor
    - bme280
    - lightLevel
- wall switch
    - switch
    - status indicator

## 03 Living
- light array
    - main light [1]
    - dicro rail [8]
    - spotlight  [2]
    - fx light   [2]
- air conditioner
    - status
    - temperature
    - fan
- wall switch
    - switch
    - status indicator

## 04 Kitchen
- ceiling light [2]
- wall switch
    - switch
    - status indicator

## 05 Workspace
- desktop light [2]
- wall switch
    - switch
    - status indicator
