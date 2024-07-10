#include "mainwindow.h"
#include "./ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tt1 = new QTimer();
    tt2 = new QTimer();
    connect(tt1, SIGNAL(timeout()), this, SLOT(onT1()));
    connect(tt2, SIGNAL(timeout()), this, SLOT(onT2()));

    //tt1->setInterval(5000);
    //tt2->setInterval(8000);

    //tt1->start();
    //tt2->start();

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
    auto system = mavsdk.first_autopilot(3);
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
    const auto set_battery_result = telemetry.set_rate_battery(1.0);

    if (set_rate_result != Telemetry::Result::Success) {
        qDebug() << "Setting rate failed: \n";
        return 1;
    }

    if (set_battery_result != Telemetry::Result::Success) {
        qDebug() << "Setting rate battery failed: \n";
        return 1;
    }


    // Set up callback to monitor altitude while the vehicle is in flight
    telemetry.subscribe_position([](Telemetry::Position position)
    {
        qDebug() << "Altitude: " << position.relative_altitude_m << " m\n";
    });

    telemetry.subscribe_battery([] (Telemetry::Battery bat)
    {
       qDebug() << "Recieved Battery: \n";
       qDebug()<<"Battery Voltage : " << bat.voltage_v << " \n";
    });

    //telemetry.subscribe_flight_mode([](const FlightModeCallback& callback);



    telemetry.subscribe_battery(CallBack_Battery);


    //mavlink_passthrough.subscribe_message(65, CallBack_RC_Channels);

    mavlink_passthrough.subscribe_message(65, [](const mavlink_message_t &msg_raw)
                                                 {

        qDebug() << "Recieved RC Transmission: \n";

        const mavlink_message_t* msg = &msg_raw;

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


    /*

     qDebug() << "Sleeping Now";

    sleep_for(seconds(5));

    const Action::Result arm_result = action.arm();

    if (arm_result != Action::Result::Success) {
        qDebug() << "Arming failed \n";
    }
    else
    {
        qDebug() << "Arming Success \n";
    }

    sleep_for(seconds(3));

    const Action::Result disarm_result = action.disarm();
    if (disarm_result != Action::Result::Success) {
        qDebug() << "DisArming failed \n";
    }
    else
    {
        qDebug() << "DisArming Success \n";
    }

    */

    //while(1);

    return 0;

}


void MainWindow::onT1()
{
}

void MainWindow::onT2()
{

}

void MainWindow::CallBack_Battery(Telemetry::Battery btry)
{
    qDebug() << "Callback :: Battery \n";

    qDebug()<<"Battery Voltage : " << btry.voltage_v << " \n";



    //uint32_t id{0}; /**< @brief Battery ID, for systems with multiple batteries */
    //float temperature_degc{float(NAN)}; /**< @brief Temperature of the battery in degrees
    //                                           Celsius. NAN for unknown temperature */
    //float voltage_v{float(NAN)}; /**< @brief Voltage in volts */
    //float current_battery_a{float(NAN)}; /**< @brief Battery current in Amps, NAN if autopilot
    //                                            does not measure the current */
    //float capacity_consumed_ah{
    //                           float(NAN)}; /**< @brief Consumed charge in Amp hours, NAN if autopilot does not provide
    //                       consumption estimate */
    //float remaining_percent{
    //                        float(NAN)}; /**< @brief Estimated battery remaining (range: 0 to 100) */
}

void MainWindow::CallBack_RC_Channels(const mavlink_message_t msg_raw)
{
    qDebug() << "Callback :: Recieved RC Transmission \n";

    const mavlink_message_t* msg = &msg_raw;
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
