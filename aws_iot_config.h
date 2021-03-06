/*
 * Copyright 2010-2016 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef config_usr_h
#define config_usr_h

// Copy and paste your configuration into this file
//===============================================================
#define AWS_IOT_MQTT_HOST "a1x2pkiln6cviy.iot.us-east-1.amazonaws.com"   // your endpoint e.g. 12345abcd.iot.us-east-1.amazonaws.com
#define AWS_IOT_MQTT_PORT 8883                   // your port
#define AWS_IOT_CLIENT_ID "YunSolarHomeController"            // your client ID
#define AWS_IOT_MY_THING_NAME "SolarHomeController"           // your thing name e.g. "Light1"
#define AWS_IOT_ROOT_CA_FILENAME "VerisignCert.pem" // your root-CA filename
#define AWS_IOT_CERTIFICATE_FILENAME "SolarHomeController.cert.pem"           // your certificate filename
#define AWS_IOT_PRIVATE_KEY_FILENAME "SolarHomeController.private.key"              // your private key filename
#define AWS_IOT_DEBUG                   // Enable debug logging. Remove it if you want to disable it.
//===============================================================
// SDK config, DO NOT modify it
#define AWS_IOT_PATH_PREFIX "../certs/"
#define AWS_IOT_ROOT_CA_PATH AWS_IOT_PATH_PREFIX AWS_IOT_ROOT_CA_FILENAME     // use this in config call
#define AWS_IOT_CERTIFICATE_PATH AWS_IOT_PATH_PREFIX AWS_IOT_CERTIFICATE_FILENAME // use this in config call
#define AWS_IOT_PRIVATE_KEY_PATH AWS_IOT_PATH_PREFIX AWS_IOT_PRIVATE_KEY_FILENAME // use this in config call

#endif
