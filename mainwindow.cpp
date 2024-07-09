#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <iostream>
#include <future>
#include <memory>
#include <thread>
#include <QDebug>
#include <QString>


using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitMav();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::InitMav()
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    //QString VehicleSerialPath = "serial:///dev/ttyACM0:115200";

    //Make Serial Connection
    ConnectionResult connection_result = mavsdk.add_serial_connection("/dev/ttyACM0", Mavsdk::DEFAULT_SERIAL_BAUDRATE, false, ForwardingOption::ForwardingOff);

    //ConnectionResult connection_result = mavsdk.add_any_connection(VehicleSerialPath);
    if (connection_result != ConnectionResult::Success) {
        qDebug()<< "Adding connectiConnectionResulton failed:'\n";
        return 10;
    }
    qDebug()<< "Vechicle added at /dev/ttyACM0";


    //Detect Autopilot Heartbeat
    auto system = mavsdk.first_autopilot(5);
    if (!system) {
        qDebug()<<"Timed out waiting for system\n";
        return 1;
    }

    qDebug()<<"Autopilot Detected \n";

    // Instantiate plugins.
    auto telemetry = Telemetry{system.value()};
    auto action = Action{system.value()};
    auto mavlink_passthrough = MavlinkPassthrough{system.value()};

    // We want to listen to the altitude of the drone at 1 Hz.
    const auto set_rate_result = telemetry.set_rate_position(1.0);
    if (set_rate_result != Telemetry::Result::Success) {
        qDebug() << "Setting rate failed: \n";
        return 1;
    }


    // Set up callback to monitor altitude while the vehicle is in flight
    telemetry.subscribe_position([](Telemetry::Position position)
    {
        qDebug() << "Altitude: " << position.relative_altitude_m << " m\n";
    });





    mavlink_passthrough.subscribe_message(65, [](const mavlink_message_t &msg_raw)
                                                 {
        const mavlink_message_t* msg = &msg_raw;
        qDebug() << "Recieved RC Transmission: \n";

        mavlink_rc_channels_t rc_channels;
        mavlink_msg_rc_channels_decode(msg, &rc_channels);
        qDebug()<< "Number of channels received: " << int(rc_channels.chancount) << "\n";
        qDebug() << "Channel 1 : " << rc_channels.chan1_raw << "\n";
        qDebug() << "Channel 2 : " << rc_channels.chan2_raw << "\n";
        qDebug() << "Channel 3 : " << rc_channels.chan3_raw << "\n";
        qDebug() << "Channel 4 : " << rc_channels.chan4_raw << "\n";
        qDebug() << "Channel 5 : " << rc_channels.chan5_raw << "\n";
        qDebug() << "Channel 6 : " << rc_channels.chan6_raw << "\n";
        qDebug() << "Channel 7 : " << rc_channels.chan7_raw << "\n";
        qDebug() << "Channel 8 : " << rc_channels.chan8_raw << "\n";

        }
        );

    return 0;
}
