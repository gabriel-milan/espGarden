# espGarden - An automatic gardening system

## Introduction

I'm a very big fan of homegrown stuff but, unfortunately, that's kind of difficult for me for I'm hardly ever at home. Thinking about it, I've searched for many automatic gardening systems on the Internet and one that got my attention was the [Ardufarmbot2](https://mjrobot.org/ardufarmbot2/) for its simplicity. At that moment, I realized it can be something much simpler than it looks like.

On this repository, you'll find my own solution for an automatic gardening system, the reasons that led me to the choices I've made and how to replicate every step I've done.

## Premises

I started this with a few premises that I'll list here:

* **Keep it simple**: everything on this project is meant to be easy to understand, to build and to run!
* **No one is rich**: I'll try to spend as little money as I can on this, yet being an elegant solution.
* **Make it scalable**: espGarden is meant to be scalable, in order to be improved every day.

## Decisions made

On Ardufarmbot2, Marcelo used an [ESP8266](https://wikipedia.org/wiki/ESP8266) and the [Blynk](https://blynk.io) app, which was one of the most amazing things I've seen in a while, since I'm not into the [IoT](https://pt.wikipedia.org/wiki/Internet_das_coisas) world too much. Lamentably, the free version of the app provides real serious limitations on how many widgets you can drag and drop.

Thinking about it, I started looking for alternatives for Blynk and then I found out about [ThingsBoard](https://thingsboard.io/). It's a very similar concept, but it's web-based, has few more features and, if you run your own local server, it has no limitations on use. That's exactly what I needed!

Then, I got a VPS on [Ionos](https://www.ionos.com/) for $2/month with Debian Stretch that would attend to my hardware needs. Amazingly, ThingsBoard can be installed using Docker containers, which make things faster and easier to set up.

I already had an [ESP32](https://wikipedia.org/wiki/ESP32) module (the DEVKIT-V1 from DOIT) and decided I would use it for this project, since I would want to start with a small and simple thing and, eventually, scale it to a smart gardening system, maybe even using AI concepts! ESP32 provides very powerful hardware for its low cost and size, so it seems to be a very nice choice.

The sensors I've chosen were all based on the Ardufarmbot2, the amount of each one was arbitrarily decided:

### Sensors list

* 4x Soil moisture sensor (HL-69);
* 2x Waterproof temperature sensor (DS18B20);
* 1x Humidity and temperature sensor (DHT22);

And, obviously, probably the most important part of the gardening system: the watering system! I've chosen to use a simple solenoid valve with a garden hose for it's cheap and easy to install.

I'm still considering making a PCB for espGarden but I think that's something that would limit its scalability.

## Replication steps

### Setting up ThingsBoard

* Set up a server with the OS of your choice (recommend [Ionos](https://www.ionos.com/)  VPS for cost/benefit);
* Install Docker Community Edition on your server: [Tutorial](https://docs.docker.com/install/);
* Install ThingsBoard Community Edition using docker: [Tutorial](https://thingsboard.io/docs/user-guide/install/installation-options/);
* Change the default passwords on the Web UI for security’s sake;
* Add a new dashboard, using this template: [Download](/#);

You're done with ThingsBoard for now!

### Setting up the development environment

This step is very personal, I'll provide the steps I've taken for simplicity. If you already have your own development environment, feel free to use it and skip this step.

* Install Visual Studio Code: [Download](https://code.visualstudio.com/download);
* Install PlatformIO extension: [Tutorial](https://docs.platformio.org/en/latest/ide/vscode.html#installation);
* Create a new PlatformIO project, setting the board to the ESP32 DEVKIT and the framework to Arduino;

You're done!

### Cloning this project

* Open your working directory;
* On terminal, run:
```
git clone https://github.com/gabriel-milan/espGarden
```

### Running code on ESP32

#### TODO